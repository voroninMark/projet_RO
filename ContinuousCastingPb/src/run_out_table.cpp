/**
 * @file run_out_table.cpp
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

#include <run_out_table.hpp>

namespace cc {

RunOutTable::RunOutTable(const std::string& name,
                         const Context < RunOutTable,
                         RunOutTableParameters >& context) :
    artis::pdevs::Dynamics < artis::common::DoubleTime, RunOutTable,
                             RunOutTableParameters >(name, context),
    _slab(nullptr), _number(context.parameters().number)
{
    input_ports({ { IN, "in" }, { TAKE, "take" } });
    output_ports({ { OUT, "out" }, { ARRIVED, "arrived" } });
}

RunOutTable::~RunOutTable()
{
    if (_slab) {
        delete _slab;
    }
}

void RunOutTable::dint(Time t)
{
    if (_phase == WAIT) {

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t,
                                       artis::common::DELTA_INT)
                       << "FAILED";
        Trace::trace().flush();
#endif

        _phase = FAIL;
    } else if (_phase == SEND_OUT) {
        delete _slab;
        _slab = nullptr;
        _phase = WAIT;
    } else if (_phase == SEND_ARRIVED) {
        _phase = WAIT;
    }
}

void RunOutTable::dext(Time t, Time /* e */, const Bag& msgs)
{
    std::for_each(msgs.begin(), msgs.end(), [this, t](const ExternalEvent &e) {
            if (e.on_port(IN)) {
                if (_phase != FAIL) {
                    if (_slab == nullptr) {
                        _slab = new Slab;
                        e.data()(*_slab);
                        _slab->table_number = _number;
                        _slab->max_date = t + _rand.normal(2.5, 0.1);
                        _phase = SEND_ARRIVED;

#ifdef WITH_TRACE_MODEL
                        Trace::trace() << TraceElement(get_name(), t,
                                                       artis::common::DELTA_EXT)
                                       << "in -> " << _slab->to_string();
                        Trace::trace().flush();
#endif

                    }
                }
            } else if (e.on_port(TAKE)) {
                unsigned int table_number;

                e.data()(table_number);
                if (table_number == _number) {

#ifdef WITH_TRACE_MODEL
                    Trace::trace() << TraceElement(get_name(), t,
                                                   artis::common::DELTA_EXT)
                                   << "take";
                    Trace::trace().flush();
#endif

                    _phase = SEND_OUT;
                }
            }
        });
}

Time RunOutTable::start(Time /* t */)
{
    _phase = WAIT;
    _slab = nullptr;
    return infinity;
}

Time RunOutTable::ta(Time t) const
{
    if (_phase == WAIT) {
        if (_slab) {
            return _slab->max_date - t;
        } else {
            return infinity;
        }
    } else if (_phase == FAIL) {
        return infinity;
    } else {
        return 0;
    }
}

Bag RunOutTable::lambda(Time /* t */) const
{
    Bag msgs;

    if (_phase == SEND_ARRIVED) {
        msgs.push_back(ExternalEvent(ARRIVED, *_slab));
    } else if (_phase == SEND_OUT) {
        msgs.push_back(ExternalEvent(OUT, *_slab));
    }
    return msgs;
}

} // namespace cc
