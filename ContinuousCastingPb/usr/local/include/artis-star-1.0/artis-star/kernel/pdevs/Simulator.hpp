/**
 * @file kernel/pdevs/Simulator.hpp
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

#ifndef PDEVS_SIMULATOR
#define PDEVS_SIMULATOR

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/Parameters.hpp>
#include <artis-star/common/Simulator.hpp>
#include <artis-star/common/utils/String.hpp>
#include <artis-star/common/utils/Trace.hpp>

#include <cassert>

namespace artis { namespace pdevs {

template < class Time, class Dynamics, class Parameters >
class Simulator;

template < class Time, class Dynamics,
           class Parameters = common::NoParameters >
class Context
{
    typedef pdevs::Simulator < Time, Dynamics, Parameters > Simulator;

public:
    Context(const Parameters& parameters, Simulator* simulator) :
        _parameters(parameters), _simulator(simulator)
    { }

    virtual ~Context()
    { }

    const Parameters& parameters() const
    { return _parameters; }

    Simulator* simulator() const
    { return _simulator; }

private:
    const Parameters& _parameters;
    Simulator*        _simulator;
};

template < class Time, class Dynamics,
           class Parameters = common::NoParameters >
class Simulator : public common::Simulator < Time >
{
    typedef Simulator < Time, Dynamics, Parameters > type;

public :
    Simulator(const std::string& name, const Parameters& parameters) :
        common::Model < Time >(name),
        common::Simulator < Time >(name),
        _dynamics(name, Context < Time, Dynamics, Parameters >(parameters,
                                                               this))
    { }

    ~Simulator()
    { }

    virtual std::string to_string(int level) const
    {
        std::ostringstream ss;

        ss << common::String::make_spaces(level * 2) << "p-devs simulator \""
           << type::get_name() << "\""<< std::endl;
        return ss.str();
    }

/*************************************************
 * when i-message(t)
 *   tl = t - e
 *   tn = tl + ta(s)
 *************************************************/
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

        type::_tl = t;
        type::_tn =
            type::_tl + _dynamics.start(t);

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

    common::Value observe(const typename Time::type& t,
                          unsigned int index) const
    { return _dynamics.observe(t, index); }

    virtual std::string observable_name(unsigned int observable_index) const
    { return _dynamics.observable_name(observable_index); }

/*************************************************
 * when *-message(t)
 *   if (t = tn) then
 *     y = lambda(s)
 *     send y-message(y,t) to parent
 *************************************************/
    void output(const typename Time::type& t)
    {

#ifdef WITH_TRACE
        common::Trace < Time >::trace()
            << common::TraceElement < Time >(type::get_name(), t,
                                             common::OUTPUT) << ": BEFORE";
        common::Trace < Time >::trace().flush();
#endif

        if(t == type::_tn) {
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

/*************************************************
 * when x-message(t)
 *   if (x is empty and t = tn) then
 *       s = delta_int(s)
 *  else if (x isn't empty and t = tn)
 *       s = delta_conf(s,x)
 *  else if (x isn't empty and t < tn)
 *    e = t - tl
 *    s = delta_ext(s,e,x)
 *  tn = t + ta(s)
 *  tl = t
 *************************************************/
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

        assert(type::_tl <= t and t <= type::_tn);

        if(t == type::_tn) {
            if (type::event_number() == 0) {
                _dynamics.dint(t);
            } else {
                _dynamics.dconf(t, t - type::_tl, type::get_bag());
            }
        } else {
            _dynamics.dext(t, t - type::_tl, type::get_bag());
        }
        type::_tn = t + _dynamics.ta(t);
        type::_tl = t;
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

private :
    Dynamics _dynamics;
};

} } // namespace artis pdevs

#endif
