/**
 * @file crane.hpp
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

#ifndef CC_CRANE_HPP
#define CC_CRANE_HPP

#include <models.hpp>
#include <stack.hpp>

namespace cc {

struct CraneParameters
{
    unsigned int stack_number;
    std::vector < std::vector < unsigned int > > stack_index_by_cluster;
    unsigned int destination_number;
    double move_duration;
};

class Crane : public artis::pdevs::Dynamics < artis::common::DoubleTime,
                                              Crane, CraneParameters >
{
public:
    enum inputs { IN, FULL, EMPTY };
    enum outputs { TAKE = 1, OUT = 100 };
    enum vars { MOVE_NUMBER, SLAB_NUMBER };

    Crane(const std::string& name,
          const Context < Crane, CraneParameters >& context);

    virtual ~Crane();

    void dint(Time t);
    void dext(Time t, Time e, const Bag& msgs);
    Time start(Time t);
    Time ta(Time t) const;
    artis::common::Bag < artis::common::DoubleTime > lambda(
        Time t) const;

    Value observe(const Time& /* t */, unsigned int /* index */) const;

private:
    void select_stack();
    void remove_slabs();

    enum Phase { MOVE_TO_STACK, MOVE_TO_STOCK, SEND_DELIVER, SEND_TAKE, WAIT };

    // state
    Phase _phase;
    unsigned int                                 _full_cluster_index;
    std::vector < unsigned int >                 _full_stack_numbers;
    Slabs                                        _current_slabs;
    std::vector < std::vector < FullData > >     _datas;
    unsigned int                                 _taken_slab_number;
    Time                                         _sigma;

    // parameters
    unsigned int                                 _stack_number;
    std::vector < std::vector < unsigned int > > _stack_index_by_cluster;
    unsigned int                                 _destination_number;
    double                                       _move_duration;

    // observables
    unsigned int                                 _move_number;
    unsigned int                                 _slab_number;
};

} // namespace cc

#endif
