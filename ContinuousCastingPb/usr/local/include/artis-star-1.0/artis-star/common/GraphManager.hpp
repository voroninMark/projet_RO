/**
 * @file common/GraphManager.hpp
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

#ifndef COMMON_GRAPH_MANANGER
#define COMMON_GRAPH_MANANGER

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/Links.hpp>
#include <artis-star/common/Model.hpp>
#include <artis-star/common/Parameters.hpp>
#include <artis-star/common/utils/String.hpp>

#include <sstream>

namespace artis { namespace common {

template < class Time >
class GraphManager
{
public:
    GraphManager(common::Coordinator < Time >* coordinator) :
        _coordinator(coordinator)
    { }

    virtual ~GraphManager()
    { }

    void add_child(unsigned int index, common::Model < Time >* child)
    {
        _children.push_back(child);
        _child_map[index] = child;
        child->set_parent(_coordinator);
    }

    void add_children(unsigned int index, common::Model < Time >* child)
    {
        _children.push_back(child);
        if (_children_map.find(index) == _children_map.end()) {
            _children_map[index] = Models < Time >();
        }
        _children_map[index].push_back(child);
        child->set_parent(_coordinator);
    }

    const common::Models < Time >& children() const
    { return _children; }

    const common::ModelMap < Time >& child_map() const
    { return _child_map; }

    const common::ModelsMap < Time >& children_map() const
    { return _children_map; }

    common::Coordinator < Time >* get_coordinator() const
    { return _coordinator; }

protected:
    common::Models < Time >        _children;
    common::ModelMap < Time >      _child_map;
    common::ModelsMap < Time >     _children_map;
    common::Coordinator < Time >*  _coordinator;
};

} } // namespace artis common

#endif
