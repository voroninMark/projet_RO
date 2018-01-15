/**
 * @file Links.hpp
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

#ifndef COMMON_LINKS
#define COMMON_LINKS 1

#include <artis-star/common/Node.hpp>
#include <artis-star/common/utils/String.hpp>

#include <map>
#include <sstream>

namespace artis { namespace common {

template < class Time >
class Node;

template < class Time >
class Links : public std::multimap < Node < Time >,
                                     Node < Time > >
{
public:

    typedef std::pair <
        typename Links < Time >::const_iterator,
        typename Links < Time >::const_iterator
    > Result;

    Links()
    { }
    virtual ~Links()
    { }

    void add(Model < Time >* out_model,
             unsigned int out_port_index,
             Model < Time >* in_model,
             unsigned int in_port_index)
    {
        std::multimap < Node < Time >,
                        Node < Time > >::insert(
                            std::pair < Node < Time >,
                                        Node < Time > >(
                                            Node < Time >(
                                                out_model, out_port_index),
                                            Node < Time >(
                                                in_model, in_port_index)));
    }

    bool exist(Model < Time >* out_model,
               unsigned int out_port_index,
               Model < Time >* in_model,
               unsigned int in_port_index) const
    {
      std::pair < typename Links < Time >::const_iterator,
		  typename Links < Time >::const_iterator > it =
	std::multimap < Node < Time >,
			Node < Time > >::equal_range(
                            Node < Time >(out_model, out_port_index));
      typename Links < Time >::const_iterator it2 = it.first;
        bool found = false;

        while (not found and it2 != it.second) {
            found = it2->second == Node < Time >(
                in_model, in_port_index);
            ++it2;
        }
        return found;
    }

    Links::Result find(Model < Time >* out_model,
                       unsigned int out_port_index) const
    {
        return std::multimap < Node < Time >,
                               Node < Time > >::equal_range(
                                   common::Node < Time >(
                                       out_model, out_port_index));
    }

    std::string to_string(int level = 0) const
    {
        std::stringstream ss;

        ss << common::String::make_spaces(level * 2) << "Links:" << std::endl;
        for (typename Links < Time >::const_iterator it =
                 Links < Time >::begin();
             it != Links < Time >::end(); ++it) {
            ss << common::String::make_spaces((level + 1) * 2)
               << it->first.get_model()->get_name() << "::"
               << it->first.get_model()->get_out_port_name(
                   it->first.get_port_index())
               << " -> "
               << it->second.get_model()->get_name() << "::"
               << it->second.get_model()->get_out_port_name(
                   it->second.get_port_index()) << std::endl;
        }
        return ss.str();
    }
};

} } // namespace artis common

#endif
