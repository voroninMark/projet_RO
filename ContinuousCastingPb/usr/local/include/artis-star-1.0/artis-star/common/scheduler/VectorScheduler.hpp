/**
 * @file VectorScheduler.hpp
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

#ifndef COMMON_SCHEDULER_VECTOR_SCHEDULER_HPP
#define COMMON_SCHEDULER_VECTOR_SCHEDULER_HPP 1

#include <artis-star/common/InternalEvent.hpp>
#include <artis-star/common/scheduler/SchedulerHandle.hpp>

#include <algorithm>
#include <sstream>

namespace artis { namespace common { namespace scheduler {

template < class Time >
class VectorScheduler :
        protected std::vector < InternalEvent < Time, NoSchedulerHandle > >
{
public:
    VectorScheduler()
    { }
    virtual ~VectorScheduler()
    { }

    Model < Time, NoSchedulerHandle >* get_current_model()
    {
        return VectorScheduler < Time >::front().get_model();
    }

    Models < Time, NoSchedulerHandle > get_current_models(
        typename Time::type time) const
    {
        Models < Time, NoSchedulerHandle > models;

        for (typename VectorScheduler < Time >::const_iterator it =
                 VectorScheduler < Time >::begin();
             it != VectorScheduler < Time >::end() and it->get_time() == time;
             ++it) {
            models.push_back(it->get_model());
        }
        return models;
    }

    typename Time::type get_current_time() const
    { return VectorScheduler < Time >::front().get_time(); }

    void init(typename Time::type time,
              Model < Time, NoSchedulerHandle >* model)
    {
        VectorScheduler < Time >::push_back(
            InternalEvent < Time, NoSchedulerHandle >(time, model));
        std::sort(VectorScheduler < Time >::begin(),
                  VectorScheduler < Time >::end());
    }

    void put(typename Time::type time, Model < Time, NoSchedulerHandle >* model)
    {
        remove(model);
        VectorScheduler < Time >::push_back(
            InternalEvent < Time, NoSchedulerHandle >(time, model));
        std::sort(VectorScheduler < Time >::begin(),
                  VectorScheduler < Time >::end());
    }

    std::string to_string() const
    {
        std::stringstream ss;

        ss << "Scheduler = { ";
        for (typename VectorScheduler < Time >::const_iterator it =
                 VectorScheduler < Time >::begin();
             it != VectorScheduler < Time >::end(); ++it) {
            ss << "(" << it->get_time() << " -> " << it->get_model()->get_name()
               << ") ";
        }
        ss << "}";
        return ss.str();
    }

private:
    void remove(Model < Time, NoSchedulerHandle >* model)
    {
        typename VectorScheduler < Time >::iterator jt =
            VectorScheduler < Time >::begin();

        while (jt != VectorScheduler < Time >::end()) {
            if (jt->get_model() == model) {
                jt = VectorScheduler < Time >::erase(jt);
            } else {
                ++jt;
            }
        }
    }
};

} } } // namespace artis common scheduler

#endif
