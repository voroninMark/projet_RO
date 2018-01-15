/**
 * @file run_out_table.hpp
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

#ifndef CC_RUN_OUT_TABLE_HPP
#define CC_RUN_OUT_TABLE_HPP

#include <models.hpp>

namespace cc {

struct RunOutTableParameters
{
    unsigned int number;
};

class RunOutTable : public artis::pdevs::Dynamics <
    artis::common::DoubleTime, RunOutTable, RunOutTableParameters >
{
public:
    enum inputs { IN, TAKE };
    enum outputs { OUT, ARRIVED };

    RunOutTable(const std::string& name,
                const Context < RunOutTable, RunOutTableParameters >& context);
    virtual ~RunOutTable();

    void dint(Time t);
    void dext(Time t, Time /* e */, const Bag& msgs);
    Time start(Time /* t */);
    Time ta(Time t) const;
    Bag lambda(Time /* t */) const;

    Value observe(const Time& /* t */, unsigned int /* index */) const
    { return artis::common::Value(); }

private:
    enum Phase { WAIT, SEND_ARRIVED, SEND_OUT, FAIL };

    Phase        _phase;
    Slab*        _slab;
    unsigned int _number;
};

} // namespace cc

#endif
