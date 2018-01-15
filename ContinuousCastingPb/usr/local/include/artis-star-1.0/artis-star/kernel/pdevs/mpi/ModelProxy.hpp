/**
 * @file kernel/pdevs/mpi/ModelProxy.hpp
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

#ifndef PDEVS_MPI_MODEL_PROXY
#define PDEVS_MPI_MODEL_PROXY 1

#include <artis-star/common/Coordinator.hpp>
#include <artis-star/common/Model.hpp>

#include <boost/mpi/communicator.hpp>

namespace artis { namespace pdevs { namespace mpi {

enum Tags
{
    finish_send_tag,
    output_send_tag,
    post_event_send_tag,
    start_send_tag,
    transition_send_tag,
    output_receive_tag,
    tn_receive_tag
};

template < class Time >
class ModelProxy : public common::Model < Time >
{
    typedef common::Model < Time > parent_type;
    typedef ModelProxy < Time > type;

public:
    ModelProxy(const std::string& name, int rank, bool atomic) :
        common::Model < Time >(name), _atomic(atomic), _rank(rank)
    { }

    virtual ~ModelProxy()
    { _communicator.send(_rank, finish_send_tag); }

    virtual bool is_atomic() const
    { return _atomic; }

    virtual std::string to_string(int level) const
    {
        (void) level;

        return std::string();
    }

    virtual void observation(std::ostream& file) const
    {
        (void) file;
    }

    virtual void output(const typename Time::type& t)
    {
        try {
            typename common::Bag < Time > bag;

            _communicator.send(_rank, output_send_tag, t);
            _communicator.recv(_rank, output_receive_tag, bag);
            dispatch_events(bag, t);
        } catch (const boost::mpi::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    virtual void post_event(const typename Time::type& t,
                            const common::ExternalEvent < Time >& event)
    {
        try {
            _communicator.send(_rank, post_event_send_tag, t);
            _communicator.send(_rank, post_event_send_tag, event);
        } catch (const boost::mpi::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    virtual typename Time::type dispatch_events(common::Bag < Time >& bag,
                                                const typename Time::type& t)
    {
        for (auto & event : bag) {
            event.set_model(this);
        }
        return dynamic_cast < common::Coordinator < Time >* >(
            parent_type::get_parent())->dispatch_events(bag, t);
    }

    virtual typename Time::type start(const typename Time::type& t)
    {
        try {
            _communicator.send(_rank, start_send_tag, t);

            typename Time::type tn;

            _communicator.recv(_rank, tn_receive_tag, tn);
            type::_tl = t;
            type::_tn = tn;
        } catch (const boost::mpi::exception& e) {
            std::cout << e.what() << std::endl;
        }
        return type::_tn;
    }

    virtual typename Time::type transition(const typename Time::type& t)
    {
        try {
            _communicator.send(_rank, transition_send_tag, t);

            typename Time::type tn;

            _communicator.recv(_rank, tn_receive_tag, tn);
            type::_tl = t;
            type::_tn = tn;
        } catch (const boost::mpi::exception& e) {
            std::cout << e.what() << std::endl;
        }
        return type::_tn;
    }

private:
    bool _atomic;
    boost::mpi::communicator _communicator;
    int _rank;
};

} } } // namespace artis pdevs mpi

#endif
