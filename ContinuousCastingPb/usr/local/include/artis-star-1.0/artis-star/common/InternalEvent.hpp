/**
 * @file InternalEvent.hpp
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

#ifndef COMMON_INTERNAL_EVENT
#define COMMON_INTERNAL_EVENT 1

#include <artis-star/common/Model.hpp>

#include <functional>

namespace artis { namespace common {

template < class Time >
class Model;

template < class Time >
class InternalEvent
{
public:
    InternalEvent(const typename Time::type& time, Model < Time >* model)
        : _time(time), _model(model)
    { }

    virtual ~InternalEvent()
    { }

    Model < Time >* get_model() const
    { return _model; }

    typename Time::type get_time() const
    { return _time; }

    bool operator<(InternalEvent const &e) const
    {
        return _time < e._time;
    }

    bool operator>(InternalEvent const &e) const
    {
        return _time > e._time;
    }

    bool operator>=(InternalEvent const &e) const
    {
        return _time >= e._time;
    }

    bool operator==(InternalEvent const &e) const
    {
        return _time == e._time;
    }

    void set_time(typename Time::type time)
    { _time = time; }

private:
    typename Time::type _time;
    Model < Time >*     _model;
};

template < typename Event >
struct EventCompare
    : std::binary_function < Event, Event, bool >
{
    bool operator()(const Event &left, const Event &right) const
    { return left >= right; }
};

} } // namespace artis common

#endif
