/**
 * @file Node.hpp
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

#ifndef COMMON_NODE
#define COMMON_NODE

#include <artis-star/common/Model.hpp>

#include <string>

namespace artis { namespace common {

template < class Time >
class Model;

template < class Time >
class Node
{
public :
    Node(Model < Time >* model, unsigned int port_index)
      : _model(model), _port_index(port_index)
    {
        _id = std::hash<std::string>()(model->get_name() +
                                       std::to_string(port_index));
    }

    Node(const Node < Time >& other)
        : _model(other._model), _port_index(other._port_index), _id(other._id)
    { }

    virtual ~Node()
    { }

    bool operator<(const Node < Time >& o) const
    {
      return _id < o._id;
    }

    bool operator==(const Node < Time >& o) const
    {
      return _id == o._id;
    }

    unsigned int get_port_index() const
    { return _port_index; }

    Model < Time >* get_model() const
    { return _model; }

private :
    Model < Time >* _model;
    unsigned int    _port_index;
    int             _id;
};

} } // namespace artis common

#endif
