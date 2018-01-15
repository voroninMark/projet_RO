/**
 * @file cluster.cpp
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

#include <cluster.hpp>

namespace cc {

Cluster::Cluster(const std::string& name,
                 const Context < Cluster, ClusterParameters >& context) :
    artis::pdevs::Dynamics < artis::common::DoubleTime, Cluster,
                             ClusterParameters >(name, context),
    _number(context.parameters().number),
    _stack_indexes(context.parameters().stack_indexes)
{
    input_ports({ { EMPTY, "empty" }, { FULL, "full" } });
    output_ports({ { OUT_FULL, "full" }, { OUT_EMPTY, "empty" } });
}

Cluster::~Cluster()
{ }

void Cluster::dint(Time /* t */)
{
    if (_phase == SEND_FULL) {
        _phase = WAIT;
    } else if (_phase == SEND_EMPTY) {
        _phase = WAIT;
        _data.clear();
    }
}

void Cluster::dext(Time t, Time /* e */, const Bag& msgs)
{
    std::for_each(msgs.begin(), msgs.end(), [this, t](const ExternalEvent &e) {
            if (e.on_port(FULL)) {
                FullData data;

                e.data()(data);
                if (not _full) {
                    data.cluster_index = _number;
                    _data.push_back(data);

#ifdef WITH_TRACE_MODEL
                    Trace::trace() << TraceElement(get_name(), t,
                                                   artis::common::DELTA_EXT)
                                   << "full: " << data.stack_index << " [ ";
                    for (unsigned int i = 0; i < 5; ++i) {
                        Trace::trace() << data.destination[i] << " ";
                    }
                    Trace::trace() << "] - " << data.cluster_index;
                    Trace::trace().flush();
#endif

                    ++_full_stack_number;
                    if (_full_stack_number == _stack_indexes.size()) {
                        _phase = SEND_FULL;
                        _full = true;
                    }
                }
            } else if (e.on_port(EMPTY)) {
                FullData data;

                e.data()(data);

#ifdef WITH_TRACE_MODEL
                Trace::trace() << TraceElement(get_name(), t,
                                               artis::common::DELTA_EXT)
                               << "empty: " << data.stack_index;
                Trace::trace().flush();
#endif

                --_full_stack_number;
                if (_full_stack_number == 0) {
                    _phase = SEND_EMPTY;
                    _full = false;
                }
            }
        });
}

Time Cluster::start(Time /* t */)
{
    _phase = WAIT;
    _full_stack_number = 0;
    _full = false;
    return infinity;
}

Time Cluster::ta(Time /* t */) const
{
    if (_phase == SEND_FULL or _phase == SEND_EMPTY) {
        return 0;
    } else {
        return infinity;
    }
}

Bag Cluster::lambda(Time /* t */) const
{
    Bag msgs;

    if (_phase == SEND_FULL) {
        for (std::vector < FullData >::const_iterator it = _data.begin();
             it != _data.end(); ++it) {
            msgs.push_back(ExternalEvent(OUT_FULL, *it));
        }
    } else if (_phase == SEND_EMPTY) {
        for (std::vector < FullData >::const_iterator it = _data.begin();
             it != _data.end(); ++it) {
            msgs.push_back(ExternalEvent(OUT_EMPTY, *it));
        }
    }
    return msgs;
}

} // namespace cc
