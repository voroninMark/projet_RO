/**
 * @file kernel/sss/Coordinator.hpp
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

#ifndef SSS_COORDINATOR
#define SSS_COORDINATOR 1

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/Parameters.hpp>
#include <artis-star/common/utils/Trace.hpp>

#include <artis-star/kernel/sss/Model.hpp>

#include <cassert>
#include <iostream>

namespace artis { namespace sss {

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
class Coordinator : public common::Coordinator < Time >,
                    public sss::Model < Time >
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
        sss::Model < Time >(name),
        _graph_manager(this, graph_paramaters),
        _time_step(parameters._time_step)
    { }

    virtual ~Coordinator()
    { }

    virtual bool is_atomic() const
    { return common::Coordinator < Time >::is_atomic(); }

    virtual std::string to_string(int level) const
    { return common::Coordinator < Time >::to_string(level); }

    typename Time::type start(const typename Time::type& t)
    {
        assert(_graph_manager.children().size() > 0);

        type::_tl = t;
        type::_tn = t;
        for (auto & child : _graph_manager.children()) {
            child->start(t);
        }
        return type::_tn;
    }

    typename Time::type dispatch_events(const common::Bag < Time >& bag,
                                        const typename Time::type& t)
    {
        _graph_manager.dispatch_events(bag, t);
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
        if (t == type::_tn) {
            for (auto & model : _graph_manager.children()) {
                model->update_buffer(t);
            }
            for (auto & model : _graph_manager.children()) {
                if (not model->is_send() && model->is_marked()) {
                    model->output(t);
                    model->send();
                }
            }
        }
    }

    void post_event(const typename Time::type& t,
                    const common::ExternalEvent < Time >& event)
    {
        if (t == type::_tn) {
            _graph_manager.post_event(t, event);
        } else {
            _policy(t, event, type::_tl, type::_tn);
        }
    }

    typename Time::type transition(const typename Time::type& t)
    {
        if (t == type::_tn) {
            bool end = true;

            for (auto & event : _policy.bag()) {
                post_event(t, event);
            }
            for (auto & model : _graph_manager.children()) {
                if (not model->is_marked()) {
                    if (model->all_ports_are_assigned()) {
                        model->transition(t);
                        model->mark();
                        end = false;
                    } else {
                        end = false;
                    }
                } else {
                    if (not model->is_send()) {
                        end = false;
                    } else {
                        if (t == model->get_tn()) {
                            model->transition(t);
                        }
                    }
                }
            }

            if (end) {
                for (auto & model : _graph_manager.children()) {
                    model->unmark();
                    model->unsend();
                }
                type::_tl = t;
                type::_tn = t + _time_step;
            }
        }
        type::clear_bag();
        return type::_tn;
    }

    virtual void update_buffer(typename Time::type /* time */)
    { }

private:
    GraphManager        _graph_manager;
    typename Time::type _time_step;
    Policy              _policy;
};

} } // namespace artis sss

#endif
