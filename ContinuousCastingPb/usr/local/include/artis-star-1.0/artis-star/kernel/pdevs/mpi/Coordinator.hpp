/**
 * @file kernel/pdevs/mpi/Coordinator.hpp
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

#ifndef PDEVS_MPI_COORDINATOR
#define PDEVS_MPI_COORDINATOR 1

#include <artis-star/kernel/pdevs/Coordinator.hpp>
#include <artis-star/kernel/pdevs/mpi/LogicalProcessor.hpp>

namespace artis { namespace pdevs { namespace mpi {

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

public:
    Coordinator(const std::string& name,
                const Parameters& parameters,
                const GraphParameters& graph_parameters) :
        common::Model < Time >(name),
        pdevs::Coordinator < Time, GraphManager,
                             Parameters, GraphParameters >(name, parameters,
                                                           graph_parameters)
    { }

    virtual ~Coordinator()
    { }

    virtual bool is_remote() const
    { return true; }

    void set_logical_processor(LogicalProcessor < Time >* logical_processor)
    { parent_type::_graph_manager.set_logical_processor(logical_processor); }
};

} } } // namespace artis pdevs mpi

#endif
