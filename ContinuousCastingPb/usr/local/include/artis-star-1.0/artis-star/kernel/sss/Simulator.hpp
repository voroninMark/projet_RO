/**
 * @file kernel/sss/Simulator.hpp
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

#ifndef SSS_SIMULATOR
#define SSS_SIMULATOR 1

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/Parameters.hpp>
#include <artis-star/common/Simulator.hpp>
#include <artis-star/common/utils/Trace.hpp>

#include <artis-star/kernel/sss/Model.hpp>

#include <cassert>

namespace artis { namespace sss {

template < class Time, class Dynamics,
           class Parameters = common::NoParameters >
class Simulator : public common::Simulator < Time >,
                  public sss::Model < Time >
{
    typedef Simulator < Time, Dynamics, Parameters > type;

public:
    Simulator(const std::string& name, const typename Time::type& time_step,
        const Parameters& parameters) :
        common::Model < Time >(name),
        common::Simulator < Time >(name),
        sss::Model < Time >(name),
        _dynamics(name, parameters),
        _time_step(time_step)
    { }

    ~Simulator()
    {  }

    virtual bool is_atomic() const
    { return common::Simulator < Time >::is_atomic(); }

    virtual std::string to_string(int level) const
    { return common::Simulator < Time >::to_string(level); }

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

        _dynamics.start(t);
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

    void observation(std::ostream &file) const
    {
        _dynamics.observation(file);
    }

    void output(const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::OUTPUT) << ": BEFORE";
        common::Trace < Time >::trace().flush();
#endif

        if (t == type::_tn) {
            common::Bag < Time > bag = _dynamics.lambda(t);

            if (not bag.empty()) {
                for (auto & event : bag) {
                    event.set_model(this);
                }
                dynamic_cast < common::Coordinator < Time >* >(
                    type::get_parent())->dispatch_events(bag, t);
            }
        }

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::OUTPUT) << ": AFTER";
        common::Trace < Time >::trace().flush();
#endif

    }

    void post_event(const typename Time::type& t,
                    const common::ExternalEvent < Time >& event)
    {

#ifndef WITH_TRACE
        (void)t;
#endif

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::POST_EVENT)
            << ": BEFORE => " << event.to_string();
        common::Trace < Time >::trace().flush();
#endif

        type::add_event(event);

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

        assert(t == type::_tn);

        if (type::is_marked()) {
            if (type::is_send()) {
                type::_tl = t;
                type::_tn = t + _time_step;
            }
        } else {
            _dynamics.transition(type::get_bag(), t);
            type::clear_bag();
        }

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::S_MESSAGE)
            << ": AFTER => " << "tl = " << type::_tl << " ; tn = "
            << type::_tn;
        common::Trace < Time >::trace().flush();
#endif

        return type::_tn;
    }

    virtual void update_buffer(typename Time::type time)
    { _dynamics.update_buffer(time); }

private :
    Dynamics            _dynamics;
    typename Time::type _time_step;
};

} } // namespace artis sss

#endif
