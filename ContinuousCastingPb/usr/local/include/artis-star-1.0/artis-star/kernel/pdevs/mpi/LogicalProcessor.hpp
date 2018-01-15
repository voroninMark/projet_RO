/**
 * @file kernel/pdevs/mpi/LogicalProcessor.hpp
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

#ifndef PDEVS_MPI_LOGICAL_PROCESSOR
#define PDEVS_MPI_LOGICAL_PROCESSOR 1

#include <artis-star/kernel/pdevs/mpi/ModelProxy.hpp>

#include <boost/mpi/communicator.hpp>

namespace artis { namespace pdevs { namespace mpi {

template < class Time >
class LogicalProcessor
{
    typedef LogicalProcessor < Time > type;

public:
    LogicalProcessor(common::Model < Time >* model, int rank, int parent) :
        _rank(rank),
        _parent(parent),
        _model(model)
    { }

    virtual ~LogicalProcessor()
    { }

    void dispatch_events_to_parent(common::Node < Time > node,
                                   const common::Value& content,
                                   typename Time::type t)
    {
        (void) t;

        _output_bag.push_back(
            common::ExternalEvent <Time >(node, content));
    }

    void loop()
    {
        typename Time::type t;

        for(;;) {
            boost::mpi::status msg = _communicator.probe();

            switch (msg.tag()) {
            case finish_send_tag:
                return;
            case output_send_tag:
                {
                    _communicator.recv(_parent, output_send_tag, t);
                    _model->output(t);
                    _communicator.send(_parent, output_receive_tag,
                                       _output_bag);
                    _output_bag.clear();
                    break;
                }
            case post_event_send_tag:
                {
                    common::ExternalEvent < Time > event;

                    _communicator.recv(_parent, post_event_send_tag, t);
                    _communicator.recv(_parent, post_event_send_tag, event);
                    _model->post_event(t, event);
                    break;
                }
            case start_send_tag:
                _communicator.recv(_parent, start_send_tag, t);
                _communicator.send(_parent, tn_receive_tag,
                                   _model->start(t));
                break;
            case transition_send_tag:
                _communicator.recv(_parent, transition_send_tag, t);
                _communicator.send(_parent, tn_receive_tag,
                                   _model->transition(t));
                break;
            default:
                throw std::runtime_error("Invalid tag");
            }
        }
    };

private:
    int _rank;
    int _parent;
    boost::mpi::communicator _communicator;
    common::Model < Time >*  _model;
    common::Bag < Time >     _output_bag;
};

} } } // namespace artis pdevs mpi

#endif
