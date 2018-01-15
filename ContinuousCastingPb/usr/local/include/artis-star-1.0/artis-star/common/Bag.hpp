/**
 * @file Bag.hpp
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

#ifndef COMMON_BAG
#define COMMON_BAG 1

#include <artis-star/common/ExternalEvent.hpp>

#include <boost/serialization/vector.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace artis { namespace common {

template < class Time >
class ExternalEvent;

template < class Time >
class Bag : public std::vector < ExternalEvent < Time > >
{
public:
    Bag()
    { }
    virtual ~Bag()
    { }

    std::string to_string() const
    {
        std::ostringstream ss;

        ss << "{ ";
        for (typename Bag < Time >::const_iterator it =
                 Bag < Time >::begin();
             it != Bag < Time >::end(); ++it) {
            ss << it->to_string() << " ";
        }
        ss << "}";
        return ss.str();
    }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        (void) version;

        ar & *dynamic_cast < const std::vector < ExternalEvent < Time > >* >(
            this);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        (void) version;

        ar & *dynamic_cast < std::vector < ExternalEvent < Time > >* >(this);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

} } // namespace artis common

#endif
