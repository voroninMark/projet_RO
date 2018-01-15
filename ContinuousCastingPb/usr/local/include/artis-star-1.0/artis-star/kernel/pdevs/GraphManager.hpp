/**
 * @file kernel/pdevs/GraphManager.hpp
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

#ifndef PDEVS_GRAPH_MANANGER
#define PDEVS_GRAPH_MANANGER

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/GraphManager.hpp>
#include <artis-star/common/Links.hpp>
#include <artis-star/common/Model.hpp>
#include <artis-star/common/Parameters.hpp>
#include <artis-star/common/utils/String.hpp>

#include <sstream>

namespace artis { namespace pdevs {

template < class Time,
           class Parameters = common::NoParameters,
           class GraphParameters = common::NoParameters >
class GraphManager : public common::GraphManager < Time >
{
public:
    typedef GraphManager < Time, Parameters, GraphParameters > type;

    struct ModelPort
    {
        common::Model < Time >* model;
        unsigned int port_index;
        type* graph_manager;

        ModelPort(common::Model < Time >* model,
                  unsigned int port_index) : model(model),
                                             port_index(port_index),
                                             graph_manager(nullptr)
        {}

        void operator>>(const ModelPort& dst)
        {
            graph_manager->add_link(model, port_index,
                                    dst.model, dst.port_index);
        }
    };

    GraphManager(common::Coordinator < Time >* coordinator,
                 const Parameters& /* parameters */,
                 const GraphParameters& /* graph_parameters */) :
        common::GraphManager < Time >(coordinator)
    { }

    virtual ~GraphManager()
    { }

    ModelPort in(ModelPort p)
    { p.graph_manager = this; return p; }

    ModelPort out(ModelPort p)
    { p.graph_manager = this; return p; }

    void dispatch_events(common::Bag < Time > bag,
                         typename Time::type t)
    {
        for (auto & ymsg : bag) {
            typename common::Links < Time >::Result result_model =
                _link_list.find(ymsg.get_model(),
                                ymsg.get_port_index());

            for (typename common::Links < Time >::
                     const_iterator it = result_model.first;
                     it != result_model.second; ++it) {
                // event on output port of coupled Model
                if (it->second.get_model() ==
                    common::GraphManager < Time >::_coordinator) {
                    dispatch_events_to_parent(it->second, ymsg.data(),
                                              t);
                } else { // event on input port of internal model
                    it->second.get_model()->post_event(
                        t, common::ExternalEvent < Time >(
                            it->second, ymsg.data()));
                }
            }
        }
    }

    virtual void dispatch_events_to_parent(common::Node < Time > node,
                                           const common::Value& content,
                                           typename Time::type t)
    {
        common::Bag < Time > ymessages;

        ymessages.push_back(
            common::ExternalEvent <Time >(node, content));

        dynamic_cast < common::Coordinator < Time >* >(
            common::GraphManager < Time >::_coordinator->get_parent())
            ->dispatch_events(ymessages, t);
    }

    bool exist_link(common::Model < Time >* src_model,
                    unsigned int src_port_index,
                    common::Model < Time >* dst_model,
                    unsigned int dst_port_index) const
    {
        return _link_list.exist(src_model, src_port_index, dst_model,
                                dst_port_index);
    }

    void post_event(typename Time::type t,
                    const common::ExternalEvent < Time >& event)
    {
        typename common::Links < Time >::Result result =
            _link_list.find(common::GraphManager < Time >::_coordinator,
                            event.get_port_index());

        for (typename common::Links < Time >::const_iterator it_r =
                 result.first; it_r != result.second; ++it_r) {
                 it_r->second.get_model()->post_event(
                     t, common::ExternalEvent < Time >(it_r->second,
                                                       event.data()));
        }
    }

    virtual std::string to_string(int level) const
    {
    	std::ostringstream ss;

    	ss << common::String::make_spaces(level * 2) << "Childs :" << std::endl;
        for (auto & child : common::GraphManager < Time >::_children) {
            ss << child->to_string(level + 1);
        }
        ss << _link_list.to_string(level);
        return ss.str();
    }

private:
    void add_link(common::Model < Time >* src_model,
                  unsigned int src_port_index,
                  common::Model < Time >* dst_model,
                  unsigned int dst_port_index)
    {
        assert((src_model != common::GraphManager < Time >::_coordinator and
                dst_model != common::GraphManager < Time >::_coordinator and
                src_model->exist_out_port(src_port_index) and
                dst_model->exist_in_port(dst_port_index)) or
               (src_model == common::GraphManager < Time >::_coordinator and
                dst_model != common::GraphManager < Time >::_coordinator and
                src_model->exist_in_port(src_port_index) and
                dst_model->exist_in_port(dst_port_index)) or
               (src_model != common::GraphManager < Time >::_coordinator and
                dst_model == common::GraphManager < Time >::_coordinator and
                src_model->exist_out_port(src_port_index) and
                dst_model->exist_out_port(dst_port_index)));

        _link_list.add(src_model, src_port_index, dst_model, dst_port_index);
    }

    common::Links < Time > _link_list;
};

} } // namespace artis pdevs

#endif
