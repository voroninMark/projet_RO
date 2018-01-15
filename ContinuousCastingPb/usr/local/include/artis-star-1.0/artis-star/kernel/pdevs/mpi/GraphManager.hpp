/**
 * @file kernel/pdevs/GraphManager.hpp
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

#ifndef PDEVS_MPI_GRAPH_MANANGER
#define PDEVS_MPI_GRAPH_MANANGER 1

#include <artis-star/kernel/pdevs/mpi/Coordinator.hpp>
#include <artis-star/kernel/pdevs/mpi/LogicalProcessor.hpp>

#include <sstream>

namespace artis { namespace pdevs { namespace mpi {

template < class Time,
           class GraphParameters = common::NoParameters >
class GraphManager : public pdevs::GraphManager < Time, GraphParameters >
{
public:
    GraphManager(common::Coordinator < Time >* coordinator,
                 const GraphParameters& parameters) :
        pdevs::GraphManager < Time, GraphParameters >(coordinator, parameters)
    { }

    virtual ~GraphManager()
    { }

    virtual void dispatch_events_to_parent(common::Node < Time > node,
                                           const common::Value& content,
                                           typename Time::type t)
    { _logical_processor->dispatch_events_to_parent(node, content, t); }

    void set_logical_processor(LogicalProcessor < Time >* logical_processor)
    { _logical_processor = logical_processor; }

private:
    LogicalProcessor < Time >* _logical_processor;
};

} } } // namespace artis pdevs mpi

#endif
