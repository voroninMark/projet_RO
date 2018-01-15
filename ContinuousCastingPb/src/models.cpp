/**
 * @file models.cpp
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

#include <models.hpp>

namespace cc {

std::string Slab::to_string() const
{
    return (boost::format("[%1%;%2%;%3%;%4%;%5%;%6%;%7%]") % index %
            cc_number % length % width % destination % table_number %
            max_date).str();
}

std::string Slabs::to_string() const
{
    std::string str = "{ ";

    for (const_iterator it = begin(); it != end(); ++it) {
        str += it->to_string();
        str += " ";
    }
    str += "}";
    return str;
}

} // namespace cc

std::ostream& operator<<(std::ostream& o, const cc::Slab& slab)
{
    o << slab.to_string();
    return o;
}

std::ostream& operator<<(std::ostream& o, const cc::Slabs& slabs)
{
    o << slabs.to_string();
    return o;
}
