/**
 * @file kernel/pdevs/multithreading/Coordinator.hpp
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

#ifndef PDEVS_MULTITHREADING_COORDINATOR
#define PDEVS_MULTITHREADING_COORDINATOR 1

#include <artis-star/common/utils/Multithreading.hpp>
#include <artis-star/kernel/pdevs/Coordinator.hpp>

#include <thread>

namespace artis { namespace pdevs { namespace multithreading {

template < class Time>
struct start_message
{
    explicit start_message(typename Time::type t) : _t(t)
    { }

    typename Time::type _t;
};

template < class Time>
struct transition_message
{
    explicit transition_message(typename Time::type t) : _t(t)
    { }

    typename Time::type _t;
};

template < class Time >
struct done_start_message
{
    explicit done_start_message(typename Time::type tn,
                                common::Model < Time >* child) :
        _tn(tn), _child(child)
    { }

    typename Time::type     _tn;
    common::Model < Time >* _child;
};

template < class Time >
struct done_transition_message
{
    explicit done_transition_message(typename Time::type tn,
                                     common::Model < Time >* child) :
        _tn(tn), _child(child)
    { }

    typename Time::type     _tn;
    common::Model < Time >* _child;
};

template < class Time,
           class GraphManager,
           class Parameters = common::NoParameters,
           class GraphParameters = common::NoParameters >
class Coordinator : public pdevs::Coordinator < Time, GraphManager,
                                                Parameters, GraphParameters >
{
    typedef pdevs::Coordinator < Time, GraphManager,
                                 Parameters, GraphParameters > parent_type;
    typedef Coordinator < Time, GraphManager,
                          Parameters, GraphParameters > type;
    typedef done_start_message < Time > done_start_message_type;
    typedef start_message < Time > start_message_type;
    typedef done_transition_message < Time > done_transition_message_type;
    typedef transition_message < Time > transition_message_type;

public:
    Coordinator(const std::string& name,
                const Parameters& parameters,
                const GraphParameters& graph_parameters) :
        common::Model < Time >(name),
        pdevs::Coordinator < Time, GraphManager,
                             Parameters, GraphParameters >(name, parameters,
                                                           graph_parameters)
    {
        type::_graph_manager.init();
        _thread = new std::thread([&]{ loop(); });
    }

    virtual ~Coordinator()
    {
        done();
        _thread->join();
        delete _thread;
    }

    void done()
    { get_sender().send(artis::common::Close()); }

    artis::common::Sender get_sender()
    { return _incoming; }

    void set_sender(common::Sender sender)
    { _sender = sender; }

    void loop()
    {
        try
        {
            for(;;) {
                _incoming.wait()
                    .template handle < start_message_type >(
                        [&](start_message_type const& msg)
                        {
                            typename Time::type tn = start(msg._t);
                            _sender.send(done_start_message_type(tn, this));
                        })
                    .template handle < done_start_message_type >(
                        [&](done_start_message_type const& msg)
                        {
                            type::_event_table.init(msg._tn, msg._child);
                            --_received;
                            if (_received == 0) {
                                _received_mutex.unlock();
                            }
                        })
                    .template handle < transition_message_type >(
                        [&](transition_message_type const& msg)
                        {
                            typename Time::type tn = transition(msg._t);
                            _sender.send(done_transition_message_type(tn,
                                                                      this));
                        })
                    .template handle < done_transition_message_type >(
                        [&](done_transition_message_type const& msg)
                        {
                            type::_event_table.put(msg._tn, msg._child);
                            --_received;
                            if (_received == 0) {
                                _received_mutex.unlock();
                            }
                        });
            }
        }
        catch(artis::common::Close const&)
        { }
    }

    typename Time::type start(const typename Time::type& t)
    {
        _received = 0;
        for (auto & child : parent_type::_graph_manager.children()) {
            if (child->is_atomic()) {
                type::_event_table.init(child->start(type::_tn), child);
            } else {
                ++_received;
            }
        }

        if (_received > 0) {
            _received_mutex.lock();
            type::_graph_manager.start(t);

            std::lock_guard < std::mutex > lock(_received_mutex);
        }

        type::_tl = t;
        type::_tn = type::_event_table.get_current_time();
        return type::_tn;
    }

    // TODO: to remove
    virtual int get_receiver_number(typename Time::type t)
    { return type::_event_table.get_current_models(t).size(); }

    typename Time::type transition(const typename Time::type& t)
    {
        assert(t >= type::_tl and t <= type::_tn);

        common::Models < Time > receivers =
            type::_event_table.get_current_models(t);

        type::add_models_with_inputs(receivers);

        _received = 0;
        for (auto & model : receivers) {
            if (model->is_atomic()) {
                type::_event_table.put(model->transition(t), model);
            } else {
                ++_received;
            }
        }

        if (_received > 0) {
            _received_mutex.lock();
            type::_graph_manager.transition(receivers, t);

            std::lock_guard < std::mutex > lock(_received_mutex);
        }

        parent_type::update_event_table(t);
        type::_tl = t;
        type::_tn = type::_event_table.get_current_time();
        type::clear_bag();

        return type::_tn;
    }

private:
    std::thread*               _thread;
    artis::common::Receiver _incoming;
    artis::common::Sender   _sender;
    unsigned int               _received;
    std::mutex                 _received_mutex;
};

} } } // namespace artis pdevs multithreading

#endif
