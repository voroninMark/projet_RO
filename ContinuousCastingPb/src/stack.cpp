/**
 * @file stack.cpp
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

#include <stack.hpp>

namespace cc {

Stack::Stack(const std::string& name,
             const Context < Stack, StackParameters >& context) :
    artis::pdevs::Dynamics < artis::common::DoubleTime,
                             Stack, StackParameters >(name, context),
    _number(context.parameters().number)
{
    input_ports({ { IN, "in" }, { TAKE, "take" }, { FAIL, "fail" } });
    output_ports({ { OUT_FULL, "full" }, { EMPTY, "empty" }, { OUT, "out" } });
    observables({ { HEIGHT, "height" } });
}

Stack::~Stack()
{ }

void Stack::dint(Time /* t */)
{
    if (_phase == SEND_FULL or _phase == SEND_EMPTY) {
        _phase = WAIT;
    } else if (_phase == SEND_DELIVER) {
        _state = NO_FULL;
        for (unsigned int i = 0; i < _taken_slab_number; ++i) {
            _slabs.pop_back();
        }
        if (_slabs.empty()) {
            _phase = SEND_EMPTY;
        } else {
            _phase = WAIT;
        }
    }
}

void Stack::dext(Time t, Time /* e */, const Bag& msgs)
{
    std::for_each(msgs.begin(), msgs.end(), [this, t](const ExternalEvent &e) {
            if (e.on_port(IN)) {
                Slab slab;

                e.data()(slab);

#ifdef WITH_TRACE_MODEL
                Trace::trace() << TraceElement(get_name(), t,
                                               artis::common::DELTA_EXT)
                               << "in -> " << slab.to_string();
                Trace::trace().flush();
#endif

                _slabs.push_back(slab);
                _state = _slabs.size() >= 5 ? FULL : NO_FULL;
                if (_state == FULL) {
                    _phase = SEND_FULL;
                }
            } else if (e.on_port(TAKE)) {
                e.data()(_taken_slab_number);

#ifdef WITH_TRACE_MODEL
                Trace::trace() << TraceElement(get_name(), t,
                                               artis::common::DELTA_EXT)
                               << "take: " << _taken_slab_number;
                Trace::trace().flush();
#endif

                _phase = SEND_DELIVER;
            } else if (e.on_port(FAIL)) {

#ifdef WITH_TRACE_MODEL
                Trace::trace() << TraceElement(get_name(), t,
                                               artis::common::DELTA_EXT)
                               << "fail";
                Trace::trace().flush();
#endif

                if (_state != FULL) {
                    _phase = SEND_FULL;
                }
            }
        });
}

Time Stack::start(Time /* t */)
{
    _phase = WAIT;
    _state = NO_FULL;
    return infinity;
}

Time Stack::ta(Time /* t */) const
{
    if (_phase == SEND_FULL or _phase == SEND_DELIVER or _phase == SEND_EMPTY) {
        return 0;
    } else {
        return infinity;
    }
}

Bag Stack::lambda(Time t) const
{
    Bag msgs;

    if (_phase == SEND_FULL) {
        FullData data;
        unsigned int i = 0;

        data.stack_index = _number;
        for (Slabs::const_iterator it = _slabs.begin();
             it != _slabs.end(); ++it, ++i) {
            data.destination[i] = it->destination;
            data.width[i] = it->width;
        }
        for (unsigned int j = i; j < 5; ++j) {
            data.destination[j] = 0;
        }

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t,
                                       artis::common::LAMBDA)
                       << "full";
        Trace::trace().flush();
#endif

        msgs.push_back(ExternalEvent(OUT_FULL, data));
    } else if (_phase == SEND_DELIVER) {
        for (unsigned int i = 0; i < _taken_slab_number; ++i) {
            msgs.push_back(ExternalEvent(OUT, _slabs[_slabs.size() - i - 1]));
        }
    } else if (_phase == SEND_EMPTY) {
        FullData data;

        data.stack_index = _number;

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t,
                                       artis::common::LAMBDA)
                       << "empty";
        Trace::trace().flush();
#endif

        msgs.push_back(ExternalEvent(EMPTY, data));
    }
    return msgs;
}

Value Stack::observe(const Time& /* t */, unsigned int index) const
{
    switch (index) {
    case HEIGHT: return (int)_slabs.size();
    }
    return Value();
}

} // namespace cc
