/**
 * @file kernel/sss/Dynamics.hpp
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

#ifndef SSS_DYNAMICS
#define SSS_DYNAMICS 1

#include <artis-star/common/Bag.hpp>
#include <artis-star/common/ExternalEvent.hpp>
#include <artis-star/common/Parameters.hpp>

namespace artis { namespace sss {

template < class Time,
           class Parameters = common::NoParameters >
class Dynamics
{
public:
    Dynamics(const std::string& name, const Parameters& /* parameters */) :
        _name(name)
    { }

    virtual ~Dynamics()
    { }

    virtual void transition(const common::Bag < Time >& /* x */,
                            typename Time::type /* t */)
    { }

    virtual typename Time::type start(typename Time::type/* time */)
    { return Time::infinity; }

    common::Bag < Time > lambda(
        typename Time::type /* time */) const
    { return common::Bag < Time >(); }

    virtual void observation(std::ostream& /* file */) const
    { }

    virtual void update_buffer(typename Time::type /* time */)
    { }

    const std::string& get_name() const
    { return _name; }

private:
    std::string _name;
};

} } // namespace artis sss

#endif
