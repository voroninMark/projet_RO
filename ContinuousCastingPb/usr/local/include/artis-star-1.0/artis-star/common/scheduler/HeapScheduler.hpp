/**
 * @file HeapScheduler.hpp
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

#ifndef COMMON_SCHEDULER_HEAP_SCHEDULER_HPP
#define COMMON_SCHEDULER_HEAP_SCHEDULER_HPP 1

#include <artis-star/common/InternalEvent.hpp>

#include <boost/heap/fibonacci_heap.hpp>

#include <sstream>

namespace artis { namespace common {

template < class Time >
class Model;

template < class Time >
class Models;

namespace scheduler {

template < class Time, class T >
class HeapScheduler :
        public boost::heap::fibonacci_heap <
    InternalEvent < Time >,
    boost::heap::compare <
        EventCompare < InternalEvent < Time > > > >
{
public:
    typedef HeapScheduler < Time, T > type;
    typedef Model < Time >            model_type;
    typedef Models < Time >           models_type;
    typedef InternalEvent < Time >    internal_event_type;

    HeapScheduler()
    { }
    virtual ~HeapScheduler()
    { }

    model_type* get_current_model()
    {
        return type::top().get_model();
    }

    models_type get_current_models(typename Time::type time) const
    {
        models_type models;
        typename models_type::iterator it;

        for (typename type::ordered_iterator it = type::ordered_begin();
             it != type::ordered_end() and it->get_time() == time; ++it) {
            std::string str = it->get_model()->get_name();
            auto it2 = find_if(models.begin(), models.end(),
                               [&str](const model_type* obj) {
                                   return obj->get_name() == str;
                               });

            if (it2 == models.end()) {
                models.push_back(it->get_model());
            }
        }
        return models;
    }

    typename Time::type get_current_time() const
    {
        return type::top().get_time();
    }

    void init(typename Time::type time, model_type* model)
    {
        model->handle(T(type::push(internal_event_type(time, model))));
    }

    void put(typename Time::type time, const model_type* model)
    {
        typename Time::type previous_time =
            (*model->handle()._handle).get_time();

        if (previous_time != time) {
            (*model->handle()._handle).set_time(time);
            if (previous_time < time) {
                type::decrease(model->handle()._handle);
            } else if (previous_time > time) {
                type::increase(model->handle()._handle);
            }
        }
    }

    std::string to_string() const
    {
        std::stringstream ss;

        ss << "Scheduler = { ";
        for (typename type::ordered_iterator it = type::ordered_begin();
             it != type::ordered_end(); ++it) {
            ss << "(" << it->get_time() << " -> " << it->get_model()->get_name()
               << ") ";
        }
        ss << "} [" << type::size() << "]";
        return ss.str();
    }
};

} } } // namespace artis common scheduler

#endif
