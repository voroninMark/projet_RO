/**
 * @file common/time/Time.hpp
 * @author The ARTIS Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * ARTIS - the multimodeling and simulation environment
 * This file is a part of the ARTIS environment
 *
 * Copyright (C) 2013-2018 ULCO http://www.univ-littoral.fr
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

#ifndef COMMON_TIME_TIME
#define COMMON_TIME_TIME 1

namespace artis { namespace common {

template < typename Type, typename Limits >
struct Time
{
    static constexpr Type negative_infinity = Limits::negative_infinity;
    static constexpr Type infinity = Limits::positive_infinity;
    static constexpr Type null = Limits::null;
    typedef Type type;
};

} } // namespace artis common

#endif
