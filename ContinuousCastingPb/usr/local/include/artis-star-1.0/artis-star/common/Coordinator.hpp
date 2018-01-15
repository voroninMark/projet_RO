/**
 * @file Coordinator.hpp
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

#ifndef COMMON_COORDINATOR
#define COMMON_COORDINATOR

#include <artis-star/common/Bag.hpp>
#include <artis-star/common/ExternalEvent.hpp>
#include <artis-star/common/Links.hpp>
#include <artis-star/common/Model.hpp>
#include <artis-star/common/Node.hpp>

#include <sstream>

namespace artis { namespace common {

template < class Time >
class Model;

template < class Time >
class GraphManager;

template < class Time >
class Coordinator : public virtual Model < Time >
{
public :
    Coordinator(const std::string& name) : Model < Time >(name)
    { }

    virtual ~Coordinator()
    { }

    virtual const GraphManager < Time >& get_graph_manager() const = 0;

    virtual const Model < Time >* get_submodel(unsigned int index) const
    {
        const GraphManager < Time >& graph_manager =
            get_graph_manager();
        typename common::ModelMap < Time >::const_iterator it =
            graph_manager.child_map().find(index);

        if (it != graph_manager.child_map().end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    virtual const Model < Time >* get_submodel(unsigned int index,
                                               unsigned int rank) const
    {
        const GraphManager < Time >& graph_manager =
            get_graph_manager();
        typename common::ModelsMap < Time >::const_iterator it =
            graph_manager.children_map().find(index);

        if (it != graph_manager.children_map().end() and
            it->second.size() > rank) {
            return it->second.at(rank);
        } else {
            return nullptr;
        }
    }

    virtual unsigned int get_submodel_number(unsigned int index) const
    {
        const GraphManager < Time >& graph_manager =
            get_graph_manager();
        typename common::ModelsMap < Time >::const_iterator it =
            graph_manager.children_map().find(index);

        if (it != graph_manager.children_map().end()) {
            return it->second.size();
        } else {
            return 0;
        }
    }

    virtual bool is_atomic() const
    { return false; }

    virtual std::string to_string(int /* level */) const
    {
        std::ostringstream ss;

        ss << "Coordinator "
           << Coordinator < Time >::get_name();
        return ss.str();
    }

// DEVS methods
    virtual common::Value observe(const typename Time::type& t,
                                  unsigned int index) const =0;
    virtual void output(const typename Time::type& t) =0;
    virtual void post_event(const typename Time::type& t,
                            const common::ExternalEvent < Time >& event) =0;
    virtual typename Time::type dispatch_events(const common::Bag < Time >&
                                                bag,
                                                const typename Time::type& t) =0;
    virtual typename Time::type start(const typename Time::type& t) =0;
    virtual typename Time::type transition(const typename Time::type& t) =0;
};

} } // namespace artis common

#endif
