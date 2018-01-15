/**
 * @file stack.hpp
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

#ifndef CC_STACK_HPP
#define CC_STACK_HPP

#include <models.hpp>

namespace cc {

struct StackParameters
{
    unsigned int number;
};

struct FullData
{
    unsigned int stack_index;
    unsigned int cluster_index;
    unsigned int destination[5];
    double width[5];
};

class Stack : public artis::pdevs::Dynamics <
    artis::common::DoubleTime, Stack, StackParameters >
{
public:
    enum inputs { IN, TAKE, FAIL };
    enum outputs { OUT_FULL, EMPTY, OUT };
    enum vars { HEIGHT };

    Stack(const std::string& name,
          const Context < Stack, StackParameters >& context);
    virtual ~Stack();

    void dint(Time t);
    void dext(Time /* t */, Time /* e */, const Bag& /* msgs */);
    Time start(Time t);
    Time ta(Time t) const;
    Bag lambda(Time /* t */) const;
    Value observe(const Time& t, unsigned int index) const;

private:
    enum Phase { WAIT, SEND_FULL, SEND_DELIVER, SEND_EMPTY };
    enum State { NO_FULL, FULL };

    Phase        _phase;
    State        _state;
    Slabs        _slabs;
    unsigned int _number;
    unsigned int _taken_slab_number;
};

} // namespace cc

#endif
