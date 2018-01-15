/**
 * @file models.hpp
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

#ifndef CC_MODELS_HPP
#define CC_MODELS_HPP

#include <artis-star/common/Value.hpp>
#include <artis-star/common/time/DoubleTime.hpp>
#include <artis-star/common/utils/Trace.hpp>
#include <artis-star/kernel/pdevs/Dynamics.hpp>
#include <artis-star/common/observer/View.hpp>

#include <utils/rand.hpp>

#include <ostream>
#include <boost/format.hpp>

namespace cc {

typedef artis::common::Value Value;
typedef artis::common::Bag < artis::common::DoubleTime > Bag;
typedef artis::common::ExternalEvent <
    artis::common::DoubleTime > ExternalEvent;

template < class Dynamics, class Parameters = artis::common::NoParameters >
using Context = typename artis::pdevs::Context < artis::common::DoubleTime,
                                                 Dynamics, Parameters >;

// template < class Dyn, class Parameters = artis::common::NoParameters >
// using Dynamics = typename artis::pdevs::Dynamics < artis::common::DoubleTime,
//                                                    Dyn, Parameters >::type;

typedef typename artis::common::DoubleTime::type Time;

static constexpr Time infinity = artis::common::DoubleTime::infinity;

typedef artis::observer::View < artis::common::DoubleTime > View;

static cc::utils::Rand _rand;

struct Slab
{
    double length;
    double width;
    unsigned int destination;
    unsigned int index;
    unsigned int cc_number;
    unsigned int table_number;
    double max_date;

    std::string to_string() const;
};

class Slabs : public std::vector < Slab >
{
public:
    Slabs()
    {}

    std::string to_string() const;
};

} // namespace cc

std::ostream& operator<<(std::ostream& o, const cc::Slab& slab);
std::ostream& operator<<(std::ostream& o, const cc::Slabs& slabs);

typedef artis::common::Trace < artis::common::DoubleTime > Trace;

typedef artis::common::TraceElement <
    artis::common::DoubleTime > TraceElement;

// template
// artis::common::Trace < artis::common::DoubleTime >& operator<<(
//     artis::common::Trace < artis::common::DoubleTime >& trace,
//     const artis::common::TraceElement < artis::common::DoubleTime >& e);

// template
// artis::common::Trace < artis::common::DoubleTime >& operator<<(
//     artis::common::Trace < artis::common::DoubleTime >& trace,
//     const std::string& str);

// template
// artis::common::Trace < artis::common::DoubleTime >& operator<<(
//     artis::common::Trace < artis::common::DoubleTime >& trace,
//     typename artis::common::DoubleTime::type t);

#endif
