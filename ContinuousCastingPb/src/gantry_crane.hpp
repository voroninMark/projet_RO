/**
 * @file gantry_crane.hpp
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

#ifndef CC_GANTRY_CRANE_HPP
#define CC_GANTRY_CRANE_HPP

#include <models.hpp>

namespace cc {

struct GantryCraneParameters
{
    unsigned int                 cluster_number;
    unsigned int                 stack_number;
    std::vector < unsigned int > preferences;
};

class GantryCrane : public artis::pdevs::Dynamics < artis::common::DoubleTime,
                                                    GantryCrane,
                                                    GantryCraneParameters >
{
public:
    enum inputs { ARRIVED, NEW, FULL, EMPTY };
    enum outputs { TAKE, OUT_FAIL = 10, OUT = 100 };

    GantryCrane(const std::string& name,
                const Context < GantryCrane, GantryCraneParameters>& context);

    virtual ~GantryCrane();

    void dint(Time /* t */);
    void dext(Time t, Time /* e */, const Bag& msgs);
    Time start(Time /* t */);
    Time ta(Time /* t */) const;
    artis::common::Bag < artis::common::DoubleTime > lambda(
        Time /* t */) const;

    Value observe(const Time& /* t */, unsigned int /* index */) const
    { return artis::common::Value(); }

private:
    bool select_stack(Time t);
    bool all_stacks_are_full();

    enum Phase { SLAB_ARRIVED, DELIVER, FAIL, SEND_FAIL, SEND_OUT, SEND_TAKE,
                 WAIT };

    // state
    Phase                _phase;
    Time                 _sigma;
    Slab*                _slab; // current slab processing by gantry crane
    std::vector < Slab > _next_slabs; // list of arrived slabs to
                                      // runout table
    std::vector < bool > _full_clusters; // full flags of cluster
    unsigned int         _selected_stack_index; // index of selected
                                                // stack
    std::vector < Slabs > _stacked_slabs;
    unsigned int          _fail_cluster_index;

    // parameters
    unsigned int         _stack_number; // number of stacks
    unsigned int         _cluster_number; // number of clusters
    std::vector < unsigned int > _preferences; // preferences to select stack
};

} // namespace cc

#endif
