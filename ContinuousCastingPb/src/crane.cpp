/**
 * @file crane.cpp
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

#include <crane.hpp>

namespace cc {

Crane::Crane(const std::string& name,
             const Context < Crane, CraneParameters >& context) :
    artis::pdevs::Dynamics < artis::common::DoubleTime, Crane,
                             CraneParameters >(name, context),
    _stack_number(context.parameters().stack_number),
    _stack_index_by_cluster(context.parameters().stack_index_by_cluster),
    _destination_number(context.parameters().destination_number),
    _move_duration(context.parameters().move_duration)
{
    for (unsigned int i = 0;
         i < context.parameters().stack_index_by_cluster.size(); ++i) {
        _datas.push_back(std::vector < FullData >());
    }

    input_ports({{ IN, "in" }, { FULL, "full" }, { EMPTY, "empty" }});
    for (unsigned int i = 1; i <= _stack_number; ++i) {
        output_port({ TAKE + i, (boost::format("take_%1%") % i).str() });
    }
    for (unsigned int i = 1; i <= _destination_number; ++i) {
        output_port({ OUT + i, (boost::format("out_%1%") % i).str() });
    }
    observables({ { MOVE_NUMBER, "move_number" },
            { SLAB_NUMBER, "slab_number" } });
}

Crane::~Crane()
{
}

void Crane::remove_slabs()
{
    FullData& data = _datas[_full_cluster_index - 1].back();
    unsigned int i = 0;
    unsigned int first = 0;

    while (data.destination[i] == 0) {
        ++i;
    }
    first = i;

    while (i < _destination_number - 1 and data.destination[i] > 0 and
           data.destination[i + 1] > 0 and
           data.width[i] <= data.width[i + 1] and
           data.destination[i] == data.destination[i + 1]) {
        ++i;
    }

    unsigned int j = i + 1;

    while (j < _destination_number and data.destination[j] == 0) {
        ++j;
    }

    if (i == _destination_number - 1 or j == _destination_number) {
        _datas[_full_cluster_index - 1].pop_back();
    } else {
        for (unsigned int k = first; k < first + _taken_slab_number; ++k) {
            data.destination[k] = 0;
        }
    }
}

void Crane::select_stack()
{
    FullData& data = _datas[_full_cluster_index - 1].back();
    unsigned int i = 0;

    while (data.destination[i] == 0) {
        ++i;
    }
    _taken_slab_number = 1;
    while (i < _destination_number - 1 and data.destination[i] > 0 and
           data.destination[i + 1] > 0 and
           data.width[i] <= data.width[i + 1] and
           data.destination[i] == data.destination[i + 1]) {
        ++i;
        ++_taken_slab_number;
    }
}

void Crane::dint(Time /* t */)
{
    if (_phase == SEND_TAKE) {
        _phase = WAIT;
        _sigma = infinity;
    } else if (_phase == SEND_DELIVER) {
        ++_move_number;
        _slab_number += _taken_slab_number;
        remove_slabs();
        _current_slabs.clear();
        if (_datas[_full_cluster_index - 1].empty()) {
            unsigned int cluster_index =
                _full_cluster_index == 1 ? 2 : 1;

            if (_full_stack_numbers[cluster_index - 1] ==
                _stack_index_by_cluster[cluster_index - 1].size()) {
                _full_cluster_index = cluster_index;
                _phase = MOVE_TO_STACK;
                _sigma = _move_duration;
            } else {
                _full_cluster_index = 0;
                _phase = WAIT;
                _sigma = infinity;
            }
        } else {
            _phase = MOVE_TO_STACK;
            _sigma = _move_duration;
        }
    } else if (_phase == MOVE_TO_STACK) {
        select_stack();
        _phase = SEND_TAKE;
        _sigma = 0;
    } else if (_phase == MOVE_TO_STOCK) {
        _phase = SEND_DELIVER;
        _sigma = 0;
    }
}

void Crane::dext(Time t, Time e, const Bag& msgs)
{
    std::for_each(msgs.begin(), msgs.end(), [this, t, e](
                      const ExternalEvent &event) {
            if (event.on_port(FULL)) {
                FullData data;

                event.data()(data);
                _datas[data.cluster_index - 1].push_back(data);

#ifdef WITH_TRACE_MODEL
                Trace::trace() << TraceElement(get_name(), t,
                                               artis::common::DELTA_EXT)
                               << "full: " << data.stack_index << " [ ";
                for (unsigned int i = 0; i < _destination_number; ++i) {
                    Trace::trace() << data.destination[i] << " ";
                }
                Trace::trace() << "] - " << data.cluster_index << " " << _phase;
                Trace::trace().flush();
#endif

                ++_full_stack_numbers[data.cluster_index - 1];
                if (_full_stack_numbers[data.cluster_index - 1] ==
                    _stack_index_by_cluster[
                        data.cluster_index - 1].size()) {
                    if (_phase == WAIT) {
                        _full_cluster_index = data.cluster_index;
                        _phase = MOVE_TO_STACK;
                        _sigma = _move_duration;
                    } else {
                        _sigma -= e;
                    }
                }
            } else if (event.on_port(IN)) {
                Slab slab;

                event.data()(slab);
                _current_slabs.push_back(slab);

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t,
                                       artis::common::DELTA_EXT)
                       << "in -> " << slab.index << " " << _phase;
        Trace::trace().flush();
#endif

                _phase = MOVE_TO_STOCK;
                _sigma = _move_duration;
            } else if (event.on_port(EMPTY)) {
                FullData data;

                event.data()(data);

#ifdef WITH_TRACE_MODEL
                Trace::trace() << TraceElement(get_name(), t,
                                               artis::common::DELTA_EXT)
                               << "empty -> " << data.stack_index << " "
                               << _phase;
                Trace::trace().flush();
#endif

                --_full_stack_numbers[_full_cluster_index - 1];
            }
        });
}

Time Crane::start(Time /* t */)
{
    _phase = WAIT;
    _sigma = infinity;
    _full_cluster_index = 0;
    _move_number = 0;
    _slab_number = 0;
    for (unsigned int i = 0; i < _stack_index_by_cluster.size(); ++i) {
        _full_stack_numbers.push_back(0);
    }
    return _sigma;
}

Time Crane::ta(Time /* t */) const
{
    return _sigma;
}

Bag Crane::lambda(Time t) const
{
    Bag msgs;

    if (_phase == SEND_TAKE) {
        const FullData& data = _datas[_full_cluster_index - 1].back();

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t, artis::common::LAMBDA)
                       << "take: stack_" << data.stack_index << " "
                       << _taken_slab_number << " slabs";
        Trace::trace().flush();
#endif

        msgs.push_back(ExternalEvent(TAKE + data.stack_index,
                                     _taken_slab_number));
    } else if (_phase == SEND_DELIVER) {
        for (Slabs::const_iterator it = _current_slabs.begin();
             it != _current_slabs.end(); ++it) {

#ifdef WITH_TRACE_MODEL
            Trace::trace() << TraceElement(get_name(), t, artis::common::LAMBDA)
                           << "out: " << it->destination;
            Trace::trace().flush();
#endif

            msgs.push_back(ExternalEvent(OUT + it->destination, *it));
        }
    }
    return msgs;
}

Value Crane::observe(const Time& /* t */, unsigned int index) const
{
    switch (index) {
    case MOVE_NUMBER: return (int)_move_number;
    case SLAB_NUMBER: return (int)_slab_number;
    }
    return Value();
}

} // namespace cc
