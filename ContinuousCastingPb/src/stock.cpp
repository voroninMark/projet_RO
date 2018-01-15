/**
 * @file stock.cpp
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

#include <stock.hpp>

namespace cc {

Stock::Stock(const std::string& name,
             const Context < Stock, StockParameters >& context) :
    artis::pdevs::Dynamics < artis::common::DoubleTime,
                             Stock, StockParameters >(name, context)
{
    input_port({ IN, "in" });
}

Stock::~Stock()
{ }

void Stock::dint(Time /* t */)
{
}

void Stock::dext(Time t, Time /* e */, const Bag& msgs)
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
            }
        });
}

Time Stock::start(Time /* t */)
{
    _phase = WAIT;
    return infinity;
}

Time Stock::ta(Time /* t */) const
{
    return infinity;
}

Bag Stock::lambda(Time /* t */) const
{
    Bag msgs;

    return msgs;
}

} // namespace cc
