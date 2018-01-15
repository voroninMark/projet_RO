/**
 * @file kernel/dtss/GraphManager.hpp
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

#ifndef DTSS_GRAPH_MANAGER
#define DTSS_GRAPH_MANAGER

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/GraphManager.hpp>
#include <artis-star/common/Links.hpp>
#include <artis-star/common/Model.hpp>
#include <artis-star/common/Parameters.hpp>

namespace artis { namespace dtss {

template < class Time,
           class GraphParameters = common::NoParameters >
class GraphManager : public common::GraphManager < Time >
{
public:
    GraphManager(common::Coordinator < Time >* coordinator,
                 const GraphParameters& /* parameters */) :
        common::GraphParameters < Time >(coordinator)
    { }

    virtual ~GraphManager()
    { }

    void add_link(common::Model < Time >* src_model,
                  const std::string& src_port_name,
                  common::Model < Time >* dst_model,
                  const std::string& dst_port_name)
    {
        assert((src_model != _coordinator and
                dst_model != _coordinator and
                src_model->exist_out_port(src_port_name) and
                dst_model->exist_in_port(dst_port_name)) or
               (src_model == _coordinator and
                dst_model != _coordinator and
                src_model->exist_in_port(src_port_name) and
                dst_model->exist_in_port(dst_port_name)) or
               (src_model != _coordinator and
                dst_model == _coordinator and
                src_model->exist_out_port(src_port_name) and
                dst_model->exist_out_port(dst_port_name)));

        _link_list.add(src_model, src_port_name, dst_model, dst_port_name);
    }

    void dispatch_events(common::Bag < Time > bag,
                         typename Time::type t)
    {
        for (auto & ymsg : bag) {
            typename common::Links < Time >::Result result_model =
                _link_list.find(ymsg.get_model(),
                                ymsg.get_port_name());

            for (typename common::Links < Time >::const_iterator it =
                     result_model.first; it != result_model.second; ++it) {
                // event on output port of coupled model
                if (it->second.get_model() == _coordinator) {
                    common::Bag < Time > ymessages;

                    ymessages.push_back(
                        common::ExternalEvent < Time >(
                            it->second, ymsg.data()));
                    dynamic_cast < common::Coordinator <Time >* >(
                        _coordinator->get_parent())
                        ->dispatch_events(ymessages, t);
                } else { // event on input port of internal model
                    it->second.get_model()->post_event(
                        t, common::ExternalEvent < Time >(
                            it->second, ymsg.data()));
                }
            }
        }
    }

    void post_event(typename Time::type t,
                    const common::ExternalEvent < Time >& event)
    {
        typename common::Links < Time >::Result result =
            _link_list.find(_coordinator, event.get_port_name());

        for (typename common::Links < Time >::const_iterator it_r =
                 result.first; it_r != result.second; ++it_r) {
            it_r->second.get_model()->post_event(
                t, common::ExternalEvent < Time >(it_r->second,
                                         event.data()));
        }
    }

private:
    common::Links < Time > _link_list;
};

} } // namespace artis dtss

#endif
