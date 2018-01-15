/**
 * @file cluster.hpp
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

#ifndef CC_CLUSTER_HPP
#define CC_CLUSTER_HPP

#include <models.hpp>
#include <stack.hpp>

namespace cc {

struct ClusterParameters
{
    unsigned int number;
    std::vector < unsigned int > stack_indexes;
};

class Cluster : public artis::pdevs::Dynamics < artis::common::DoubleTime,
                                                Cluster, ClusterParameters >
{
public:
    enum inputs { EMPTY, FULL };
    enum outputs { OUT_FULL, OUT_EMPTY };

    Cluster(const std::string& name,
            const Context < Cluster, ClusterParameters >& context);
    virtual ~Cluster();

    void dint(Time t);
    void dext(Time t, Time e, const Bag& msgs);
    Time start(Time t);
    Time ta(Time t) const;
    Bag lambda(Time t) const;

    Value observe(const Time& /* t */, unsigned int /* index */) const
    { return artis::common::Value(); }

private:
    enum Phase { WAIT, SEND_EMPTY, SEND_FULL };

    // state
    Phase                        _phase;
    unsigned int                 _full_stack_number;
    bool                         _full;
    std::vector < FullData >     _data;

    // parameters
    unsigned int                 _number;
    std::vector < unsigned int > _stack_indexes;
};

} // namespace cc

#endif
