/**
 * @file kernel/pdevs/Coordinator.hpp
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

#ifndef PDEVS_COORDINATOR
#define PDEVS_COORDINATOR 1

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/Parameters.hpp>
#include <artis-star/common/Scheduler.hpp>
#include <artis-star/common/utils/String.hpp>
#include <artis-star/common/utils/Trace.hpp>

#include <cassert>
#include <iostream>

namespace artis { namespace pdevs {

template < class Time,
           class GraphManager,
           class Parameters = common::NoParameters,
           class GraphParameters = common::NoParameters >
class Coordinator : public common::Coordinator < Time >
{
    typedef Coordinator < Time, GraphManager,
                          Parameters, GraphParameters > type;

public:
    typedef Parameters parameters_type;
    typedef GraphParameters graph_parameters_type;

    Coordinator(const std::string& name,
                const Parameters& parameters,
                const GraphParameters& graph_parameters) :
        common::Model < Time >(name),
        common::Coordinator < Time >(name),
        _graph_manager(this, parameters, graph_parameters)
    { }

    virtual ~Coordinator()
    { }

    const common::GraphManager < Time >& get_graph_manager() const
    { return _graph_manager; }

    virtual std::string to_string(int level) const
    {
        std::ostringstream ss;

        ss << common::String::make_spaces(level * 2) << "p-devs coordinator \""
           << type::get_name() << "\":" << std::endl;
        ss << _graph_manager.to_string(level + 1);
        return ss.str();
    }

    typename Time::type start(const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::I_MESSAGE)
            << ": BEFORE => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

        assert(_graph_manager.children().size() > 0);

        for (auto & child : _graph_manager.children()) {
            _event_table.init(child->start(t), child);
        }
        type::_tl = t;
        type::_tn = _event_table.get_current_time();

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::I_MESSAGE)
            << ": AFTER => " << "tl = " << type::_tl
            << " ; tn = " << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

        return type::_tn;
    }

/**************************************************
 * when *-message(t)
 *   calculate IMM (models with tn = t in scheduler
 *   calculate INF from IMM
 *   for each e in IMM U INF
 *     calculate influencer
 *   ...
 *  send done to parent
 **************************************************/
    void output(const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::OUTPUT)
            << ": BEFORE => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn << " ; scheduler = " << _event_table.to_string();
        common::Trace < Time >::trace().flush();
#endif

        assert(t == type::_tn);

        common::Models < Time > IMM =
            _event_table.get_current_models(t);

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::OUTPUT)
            << ": IMM = " << IMM.to_string();
        common::Trace < Time >::trace().flush();
#endif

        for (auto & model : IMM) {
            model->output(t);
        }

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::OUTPUT)
            << ": AFTER => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn << " ; scheduler = " << _event_table.to_string();
        common::Trace < Time >::trace().flush();
#endif

    }

/*******************************************************************
 * when x-message(t)
 *   receivers = { r | r in children, N in Ir, Z(N,r)(x) isn't empty
 *   for each r in receivers
 *     send x-message(Z(N,r)(x), t) with input value Z(N,r)(x) to r
 *   for each r in IMM and not in receivers
 *     send x-message(empty, t) to r
 *   sort event list acocrding to tn
 *   tl = t
 *   tn = min(tn_d | d in D)
 *******************************************************************/
    typename Time::type transition(const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::S_MESSAGE)
            << ": BEFORE => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn << " ; scheduler = " << _event_table.to_string();
        common::Trace < Time >::trace().flush();
#endif

        assert(t >= type::_tl and t <= type::_tn);

        common::Models < Time > receivers =
            _event_table.get_current_models(t);

        add_models_with_inputs(receivers);

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::S_MESSAGE)
            << ": receivers = " << receivers.to_string();
        common::Trace < Time >::trace().flush();
#endif

        for (auto & model : receivers) {
            _event_table.put(model->transition(t), model);
        }

        update_event_table(t);
        type::_tl = t;
        type::_tn = _event_table.get_current_time();
        type::clear_bag();

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::S_MESSAGE)
            << ": AFTER => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn << " ; scheduler = " << _event_table.to_string();
        common::Trace < Time >::trace().flush();
#endif

        return type::_tn;
    }

    void post_event(const typename Time::type& t,
                    const common::ExternalEvent < Time >& event)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::POST_EVENT)
            << ": BEFORE => " << event.to_string();
        common::Trace < Time >::trace().flush();
#endif

        type::add_event(event);
        _graph_manager.post_event(t, event);
        update_event_table(t);
        type::_tn = _event_table.get_current_time();

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::POST_EVENT)
            << ": AFTER => " << event.to_string();
        common::Trace < Time >::trace().flush();
#endif

    }

/*******************************************************************
 * when y-message(y_d, t) with output y_d from d
 *******************************************************************/
    typename Time::type dispatch_events(
        const common::Bag < Time >& bag,
        const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >( type::get_name(), t,
                                              common::Y_MESSAGE)
            << ": BEFORE => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn << " ; bag = " << bag.to_string()
            << " ; " << _event_table.to_string();
        common::Trace < Time >::trace().flush();
#endif

        _graph_manager.dispatch_events(bag, t);
        update_event_table(t);
        type::_tn = _event_table.get_current_time();

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::Y_MESSAGE)
            << ": AFTER => " << "tl = " << type::_tl << " ; tn = " << type::_tn
            << " ; " << _event_table.to_string();
        common::Trace < Time >::trace().flush();
#endif

        return type::_tn;
    }

    common::Value observe(const typename Time::type& /* t */,
                          unsigned int /* index */) const
    {
        assert(false);
        return common::Value();
    }

    void add_models_with_inputs(
        common::Models < Time >& receivers)
    {
        for (auto & model : _graph_manager.children()) {
            if (model->event_number() > 0) {
                if (std::find(receivers.begin(), receivers.end(),
                              model) == receivers.end()) {
                    receivers.push_back(model);
                }
            }
        }
    }

    void update_event_table(typename Time::type t)
    {
        for (auto & model : _graph_manager.children()) {
            if (model->event_number() > 0) {
                _event_table.put(t, model);
            }
        }
    }

protected:
    GraphManager          _graph_manager;
    common::SchedulerType _event_table;
};

} } // namespace artis pdevs

#endif
