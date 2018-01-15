/**
 * @file kernel/dtss/Coordinator.hpp
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

#ifndef DTSS_COORDINATOR
#define DTSS_COORDINATOR 1

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/Parameters.hpp>
#include <artis-star/common/utils/Trace.hpp>

#include <cassert>
#include <iostream>

namespace artis { namespace dtss {

template < class Time >
class Parameters
{
public:
    Parameters(typename Time::type time_step) : _time_step(time_step)
    { }

    typename Time::type _time_step;
};

template < class Time, class Policy, class GraphManager,
           class Parameters = Parameters < Time >,
           class GraphParameters = common::NoParameters >
class Coordinator : public common::Coordinator < Time >
{
    typedef Coordinator < Time, Policy, GraphManager,
                          Parameters, GraphParameters > type;

public:
    typedef Parameters parameters_type;
    typedef GraphParameters graph_parameters_type;

    Coordinator(const std::string& name,
                const Parameters& parameters,
                const GraphParameters& graph_paramaters) :
        common::Model < Time >(name),
        common::Coordinator < Time >(name),
        _graph_manager(this, graph_paramaters),
        _time_step(parameters._time_step)
    { }

    virtual ~Coordinator()
    { }

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
            child->start(t);
        }
        type::_tl = t;
        type::_tn = t;

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::I_MESSAGE)
            << ": AFTER => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

        return type::_tn;
    }

    typename Time::type dispatch_events(const common::Bag < Time >& bag,
                                        const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::Y_MESSAGE)
            << ": BEFORE => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn << " ; bag = " << bag.to_string();
        common::Trace < Time >::trace().flush();
#endif

        _graph_manager.dispatch_events(bag, t);

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::Y_MESSAGE)
            << ": BEFORE => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

        return type::_tn;
    }

    void observation(std::ostream& file) const
    {
        for (auto & child : _graph_manager.children()) {
            child->observation(file);
        }
    }

    void output(const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::OUTPUT)
            << ": BEFORE => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

        if (t == type::_tn) {
            for (auto & model : _graph_manager.children()) {
                model->output(t);
            }
        }

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::OUTPUT)
            << ": AFTER => " << "tl = " << type::_tl << " ; tn = " << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

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

        if (t == type::_tn) {
            _graph_manager.post_event(t, event);
        } else {
            _policy(t, event, type::_tl, type::_tn);
        }

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::POST_EVENT)
            << ": AFTER => " << event.to_string();
        common::Trace < Time >::trace().flush();
#endif

    }

    typename Time::type transition(const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::S_MESSAGE)
            << ": BEFORE => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

        if (t == type::_tn) {
            for (auto & event : _policy.bag()) {
                post_event(t, event);
            }
            for (auto & model : _graph_manager.children()) {
                model->transition(t);
            }
            type::_tl = t;
            type::_tn = t + _time_step;
        }
        type::clear_bag();

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::S_MESSAGE)
            << ": AFTER => " << "tl = " << type::_tl << " ; tn = " << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

        return type::_tn;
    }

private:
    GraphManager        _graph_manager;
    typename Time::type _time_step;
    Policy              _policy;
};

} } // namespace artis dtss

#endif
