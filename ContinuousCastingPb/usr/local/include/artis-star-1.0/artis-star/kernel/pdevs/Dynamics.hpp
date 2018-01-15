/**
 * @file kernel/pdevs/Dynamics.hpp
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

#ifndef PDEVS_DYNAMICS
#define PDEVS_DYNAMICS

#include <artis-star/common/Bag.hpp>
#include <artis-star/common/ExternalEvent.hpp>
#include <artis-star/common/Parameters.hpp>
#include <artis-star/kernel/pdevs/Simulator.hpp>

#include <string>
#include <vector>

namespace artis { namespace pdevs {

template < class Time, class Dyn, class Parameters = common::NoParameters >
class Dynamics
{
    typedef pdevs::Simulator < Time, Dyn, Parameters > Simulator;

public:
    struct Observable
    {
        unsigned int index;
        std::string name;
    };

    typedef std::map < unsigned int, std::string > Observables;

    Dynamics(const std::string& name,
             const Context < Time, Dyn, Parameters >& context) :
        _name(name), _simulator(context.simulator())
    { }

    virtual ~Dynamics()
    { }

    virtual void dconf(typename Time::type /* t */,
                       typename Time::type /* e */,
                       const common::Bag < Time >& /* bag */)
    { }

    virtual void dint(typename Time::type /* t */)
    { }

    virtual void dext(typename Time::type /* t */,
                      typename Time::type /* e */,
                      const common::Bag < Time >& /* bag */)
    { }

    virtual typename Time::type start(typename Time::type /* time */)
    { return Time::infinity; }

    virtual typename Time::type ta(typename Time::type /* time */) const
    { return Time::infinity; }

    virtual common::Bag < Time > lambda(
        typename Time::type /* time */) const
    { return common::Bag < Time >(); }

    virtual common::Value observe(const typename Time::type& /* t */,
                                  unsigned int /* index */) const
    { return common::Value(); }

    const std::string& get_name() const
    { return _name; }

    void input_port(common::Port p)
    {
        _simulator->add_in_port(p);
    }

    void input_ports(std::initializer_list < common::Port > list)
    {
        for (typename std::initializer_list < common::Port >::iterator it =
                 list.begin(); it != list.end(); ++it) {
            _simulator->add_in_port(*it);
        }
    }

    void observable(Observable observable)
    {
        _observables[observable.index] = observable.name;
    }

    void observables(std::initializer_list < Observable > list)
    {
        for (typename std::initializer_list < Observable >::iterator it =
                 list.begin(); it != list.end(); ++it) {
            _observables[it->index] = it->name;
        }
    }

    virtual std::string observable_name(unsigned int observable_index) const
    {
        assert(_observables.find(observable_index) != _observables.end());

        return _observables.find(observable_index)->second;
    }

    void output_port(common::Port p)
    {
        _simulator->add_out_port(p);
    }

    void output_ports(std::initializer_list < common::Port > list)
    {
        for (typename std::initializer_list < common::Port >::iterator it =
                 list.begin(); it != list.end(); ++it) {
            _simulator->add_out_port(*it);
        }
    }

private:
    std::string _name;
    Simulator*  _simulator;
    Observables _observables;
};

} } // namespace artis pdevs

#endif
