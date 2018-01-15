/**
 * @file gantry_crane.cpp
 * See the AUTHORS or Authors.txt file
 */

/*
 * Copyright (C) 2017-2018 ULCO http://www.univ-litoral.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/format.hpp>

#include <gantry_crane.hpp>
#include <stack.hpp>

namespace cc {

GantryCrane::GantryCrane(const std::string& name,
                         const Context < GantryCrane,
                         GantryCraneParameters>& context) :
    artis::pdevs::Dynamics < artis::common::DoubleTime,
                             GantryCrane,
                             GantryCraneParameters >(name, context),
    _slab(nullptr), _stack_number(context.parameters().stack_number),
    _cluster_number(context.parameters().cluster_number),
    _preferences(context.parameters().preferences)
{
    input_ports({ { ARRIVED, "arrived" }, { NEW, "new" },
                  { FULL, "full" }, { EMPTY, "empty" } });
    output_ports({ { TAKE, "take" }, { OUT_FAIL + 1, "fail_1" },
                   { OUT_FAIL + 2, "fail_2" } });
    for (unsigned int i = 1; i <= _stack_number; ++i) {
        output_port({ OUT + i, (boost::format("out_%1%") % i).str() });
    }
}

GantryCrane::~GantryCrane()
{
    if (_slab) {
        delete _slab;
    }
}

bool GantryCrane::all_stacks_are_full()
{
    unsigned int n = 0;

    for (unsigned int i = 0; i < _stack_number; ++i) {
        if (_stacked_slabs[i].size() == 5) ++n;
    }
    return n == _stack_number;
}

bool GantryCrane::select_stack(Time t)
{
    if (not all_stacks_are_full()) {
        // number of destination = 5
        const unsigned int n_destination = 8; 
        // maximum height (size) of stacks = 5
        const unsigned int maximum_size = 5;
        // _preferences[(i * number_of_destination + j) * number_of_destination + k] 
        // scores for:
        //   - stack of size i, destination j+1 of slab on the top of stack, and destination k+1 for the new slab to stack 
    
        bool width_check  = true;
        unsigned int size = 0;

        double best_score = 0;
        unsigned int n_best     = 0;
        std::vector<double> bests(_stack_number);
        double current_score;

        for (unsigned int i = 0; i < _stack_number; ++i) {
            // check the constraints of the stack : size and width
            size = _stacked_slabs[i].size();
            if (size == 0) {
                width_check = true;
            } else {
                width_check = _stacked_slabs[i][size - 1].width >=
                    _slab->width or std::abs(_stacked_slabs[i][size - 1].width -
                                       _slab->width) <= 100;
            }

            // update the highest score taking care of equality
            if ((size < 5) && width_check) {
                if (size == 0) {
                    current_score = _preferences[ _slab->destination ];
                    //std::cout << "(" << current_score << ")" ;
                }
                else {
                    /*
                    unsigned tmp = n_destination
                        + (size * n_destination + (_stacked_slabs[i][size - 1].destination - 1)) * (n_destination - 1) 
                        + (_slab->destination - 1);
                    std::cout << "(" << size << " " << _stacked_slabs[i][size - 1].destination << " " << _slab->destination << " " << tmp << ", " << _preferences[tmp] << ")" ;
                    */
                    current_score = _preferences[
                        n_destination
                        + (size * n_destination + (_stacked_slabs[i][size - 1].destination - 1)) * (n_destination - 1) 
                        + (_slab->destination - 1) ];
                    // bonus for close widths
                    current_score += 0.5 - std::abs(_stacked_slabs[i][size - 1].width - _slab->width) / 2000. ;
                }
                if (n_best == 0 || best_score < current_score) {
                    best_score = current_score;
                    bests[0]   = i;
                    n_best     = 1;
                } else {
                    if (best_score == current_score) {
                        bests[n_best] = i;
                        n_best++;
                    }
                }
            }
        };

//        std::cout << "=" << best_score << " "; 
        if (n_best == 0) {
            // it is not possible to stack...
            return false;
        } else {
            // select one with the highest score
            if (n_best == 1)
                _selected_stack_index = 1 + bests[0];
            else {
                // random one from the highest
                _selected_stack_index = 1 + bests[_rand.getInt(0, n_best - 1)];
            }

            _stacked_slabs[_selected_stack_index - 1].push_back(*_slab);

#ifdef WITH_TRACE_MODEL
            Trace::trace() << TraceElement(get_name(), t,
                                           artis::common::DELTA_INT)
                           << "select -> ";
            for (std::vector < Slabs >::const_iterator it =
                     _stacked_slabs.begin(); it != _stacked_slabs.end(); ++it) {
                Trace::trace() << it->to_string() << " ";
            }
            Trace::trace().flush();
#endif

            return true;
        }
    } else {
        return false;
    }
}

void GantryCrane::dint(Time t)
{
    if (_phase == SLAB_ARRIVED) {
        _phase = SEND_TAKE;
        _sigma = 0;
    } else if (_phase == SEND_TAKE) {
        _next_slabs.pop_back();
        if (_next_slabs.empty()) {
            _phase = WAIT;
            _sigma = infinity;
        } else {
            _phase = SLAB_ARRIVED;
            _sigma = 0.5;
        }
    } else if (_phase == DELIVER) {
        if (select_stack(t)) {
            _phase = SEND_OUT;
            _sigma = 0;
        } else {

#ifdef WITH_TRACE_MODEL
            Trace::trace() << TraceElement(get_name(), t,
                                           artis::common::DELTA_INT)
                           << "FAILED";
            Trace::trace().flush();
#endif

            // TODO: make a choice between 1 or 2
            _fail_cluster_index = _rand.getInt(1, 2);
            _phase = SEND_FAIL;
            _sigma = 0;
        }
    } else if (_phase == SEND_OUT) {
        delete _slab;
        _slab = nullptr;
        if (_next_slabs.empty()) {
            _phase = WAIT;
            _sigma = infinity;
        } else {
            _phase = SLAB_ARRIVED;
            _sigma = 0.5;
        }
    } else if (_phase == SEND_FAIL) {
            _phase = FAIL;
            _sigma = infinity;
    }
}

void GantryCrane::dext(Time t, Time e, const Bag& msgs)
{
    std::for_each(msgs.begin(), msgs.end(), [this, t, e](
                      const ExternalEvent &event) {
            if (event.on_port(NEW)) {
                if (_phase != FAIL) {
                    _slab = new Slab;
                    event.data()(*_slab);

#ifdef WITH_TRACE_MODEL
                    Trace::trace() << TraceElement(get_name(), t,
                                                   artis::common::DELTA_EXT)
                                   << "new -> " << _slab->to_string();
                    Trace::trace().flush();
#endif

                    _phase = DELIVER;
                    _sigma = 0.5;
                }
            } else if (event.on_port(ARRIVED)) {
                if (_phase != FAIL) {
                    Slab next_slab;

                    event.data()(next_slab);
                    _next_slabs.push_back(next_slab);

#ifdef WITH_TRACE_MODEL
                    Trace::trace() << TraceElement(get_name(), t,
                                                   artis::common::DELTA_EXT)
                                   << "arrived -> " << next_slab.to_string();
                    Trace::trace().flush();
#endif

                    if (_phase == WAIT) {
                        _phase = SLAB_ARRIVED;
                        _sigma = 0.5;
                    } else {
                        _sigma -= e;
                    }
                }
            } else if (event.on_port(FULL)) {
                FullData data;

                event.data()(data);

#ifdef WITH_TRACE_MODEL
                Trace::trace() << TraceElement(get_name(), t,
                                               artis::common::DELTA_EXT)
                               << "full -> " << data.stack_index;
                Trace::trace().flush();
#endif

                _full_clusters[data.cluster_index - 1] = true;
            } else if (event.on_port(EMPTY)) {
                FullData data;

                event.data()(data);

#ifdef WITH_TRACE_MODEL
                Trace::trace() << TraceElement(get_name(), t,
                                               artis::common::DELTA_EXT)
                               << "empty -> " << data.stack_index;
                Trace::trace().flush();
#endif

                _full_clusters[data.cluster_index - 1] = false;
                _stacked_slabs[data.stack_index - 1].clear();
                if (_phase == FAIL) {
                    _phase = DELIVER;
                    _sigma = 0;
                }
            }
        });
}

Time GantryCrane::start(Time /* t */)
{
    _phase = WAIT;
    _sigma = infinity;
    _full_clusters.clear();
    for (unsigned int i = 0; i < _cluster_number; ++i) {
        _full_clusters.push_back(false);
    }
    for (unsigned int i = 0; i < _stack_number; ++i) {
        _stacked_slabs.push_back(Slabs());
    }
    return infinity;
}

Time GantryCrane::ta(Time /* t */) const
{
    return _sigma;
}

Bag GantryCrane::lambda(Time t) const
{
    Bag msgs;

    if (_phase == SEND_TAKE) {
        Slab next_slab = _next_slabs[_next_slabs.size() - 1];

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t, artis::common::LAMBDA)
                       << "take: " << next_slab.table_number;
        Trace::trace().flush();
#endif

        msgs.push_back(ExternalEvent(TAKE, next_slab.table_number));
    } else if (_phase == SEND_OUT) {

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t, artis::common::LAMBDA)
                       << "out: " << _slab->to_string();
        Trace::trace().flush();
#endif

        msgs.push_back(ExternalEvent(OUT + _selected_stack_index, *_slab));
    } else if (_phase == SEND_FAIL) {

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t, artis::common::LAMBDA)
                       << "fail";
        Trace::trace().flush();
#endif

        msgs.push_back(ExternalEvent(OUT_FAIL + _fail_cluster_index, 0));
    }
    return msgs;
}

} // namespace cc
