/**
 * @file graph_manager.cpp
 * See the AUTHORS or Authors.txt file
 */

/*
 * Copyright (C) 2017-2018 ULCO http://www.univ-litoral.fr
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

#ifndef GRAPH_MANAGER_HPP
#define GRAPH_MANAGER_HPP

#include <cluster.hpp>
#include <crane.hpp>
#include <gantry_crane.hpp>
#include <generator.hpp>
#include <models.hpp>
#include <run_out_table.hpp>
#include <stack.hpp>
#include <stock.hpp>

#include <artis-star/kernel/pdevs/Coordinator.hpp>
#include <artis-star/kernel/pdevs/GraphManager.hpp>
#include <artis-star/kernel/pdevs/Simulator.hpp>

#include <boost/format.hpp>

namespace cc {

struct GlobalParameters
{
    std::vector < unsigned int > preferences;
};

typedef artis::common::Coordinator <
    artis::common::DoubleTime > Coordinator;
template < class T >
using PDEVSCoordinator = artis::pdevs::Coordinator <
    artis::common::DoubleTime, T, GlobalParameters >;
typedef artis::pdevs::GraphManager <
    artis::common::DoubleTime, GlobalParameters > GraphManager;
template < class Dyn, class T >
using Simulator = artis::pdevs::Simulator < artis::common::DoubleTime,
                                            Dyn, T >;
using NoParameters = artis::common::NoParameters;

class SubGraphManager : public GraphManager
{
public:
    enum submodels { GENERATOR1 = 0, GENERATOR2, GANTRY_CRANE, RUN_OUT_TABLE1,
                     RUN_OUT_TABLE2, CLUSTER1, CLUSTER2, CRANE,
                     STACK = 10, STOCK = 20 };

    SubGraphManager(Coordinator* coordinator,
                    const GlobalParameters parameters,
                    const NoParameters& graph_parameters) :
        GraphManager(coordinator, parameters, graph_parameters)
    {
        {
            GeneratorParameters p1;
            GeneratorParameters p2;

            p1.cc_index = 21;
            p1.start_indexes = { 2249240, 2249259, 2264121, 2264227, 2264333,
                                 2264539, 2264746, 2264952, 2265157, 2265122,
                                 2265226, 2265638, 2265845, 2265951, 2266158,
                                 2266122, 2266159, 2267261, 2267466, 2267572,
                                 2267777, 2267982, 2268088, 2268293, 2268761,
                                 2269167, 2269374, 2269581, 2269687, 2269894,
                                 2269862, 2270063, 2270059, 2270661, 2270968,
                                 2271176, 2271383, 2271692, 2271898, 2271862,
                                 2272068, 2274461, 2274668, 2274875, 2275082,
                                 2275190, 2275362, 2275359, 2275399, 2276461,
                                 2276668, 2276875, 2277082, 2277189, 2277199,
                                 2281021, 2281227, 2281435, 2281643, 2281751,
                                 2282022, 2282229, };
            p2.cc_index = 21;
            p2.start_indexes = { 2249277, 2249299, 2264161, 2264266, 2264372,
                                 2264577, 2264783, 2264988, 2265194, 2265162,
                                 2265263, 2265674, 2265880, 2265986, 2266192,
                                 2266199, 2267221, 2267426, 2267532, 2267737,
                                 2267942, 2268048, 2268253, 2268721, 2269127,
                                 2269335, 2269543, 2269649, 2269856, 2269822,
                                 2270026, 2270099, 2270621, 2270928, 2271135,
                                 2271344, 2271652, 2271622, 2271823, 2272059,
                                 2272099, 2274421, 2274628, 2274836, 2275043,
                                 2275150, 2275357, 2275322, 2275366, 2276421,
                                 2276628, 2276836, 2277043, 2277148, 2277159,
                                 2281061, 2281267, 2281475, 2281682, 2281789,
                                 2282096, 2282062, 2282267, };
            generator1 = new artis::pdevs::Simulator <
                artis::common::DoubleTime, Generator,
                GeneratorParameters >("g1", p1);
            generator2 = new artis::pdevs::Simulator <
                artis::common::DoubleTime, Generator,
                GeneratorParameters >("g2", p2);

            add_child(GENERATOR1, generator1);
            add_child(GENERATOR2, generator2);
        }
        {
            GantryCraneParameters p;

            p.cluster_number = 2;
            p.stack_number = 5;
            p.preferences = parameters.preferences;
            gantryCrane = new artis::pdevs::Simulator <
                artis::common::DoubleTime, GantryCrane,
                GantryCraneParameters >("gc", p);
            add_child(GANTRY_CRANE, gantryCrane);
        }
        {
            RunOutTableParameters p1;
            RunOutTableParameters p2;

            p1.number = 1;
            p2.number = 2;
            runOutTable1 = new artis::pdevs::Simulator <
                artis::common::DoubleTime, RunOutTable,
                RunOutTableParameters >("r1", p1);
            runOutTable2 = new artis::pdevs::Simulator <
                artis::common::DoubleTime, RunOutTable,
                RunOutTableParameters >("r2", p2);
            add_child(RUN_OUT_TABLE1, runOutTable1);
            add_child(RUN_OUT_TABLE2, runOutTable2);
        }
        {
            ClusterParameters p1;
            ClusterParameters p2;

            p1.number = 1;
            p2.number = 2;
            p1.stack_indexes.push_back(1);
            p1.stack_indexes.push_back(2);
            p1.stack_indexes.push_back(3);
            p2.stack_indexes.push_back(4);
            p2.stack_indexes.push_back(5);
            cluster1 = new artis::pdevs::Simulator <
                artis::common::DoubleTime, Cluster,
                ClusterParameters >("c1", p1);
            cluster2 = new artis::pdevs::Simulator <
                artis::common::DoubleTime, Cluster,
                ClusterParameters >("c2", p2);
            add_child(CLUSTER1, cluster1);
            add_child(CLUSTER2, cluster2);

            out({ cluster1, Cluster::OUT_EMPTY }) >>
                in({ gantryCrane, GantryCrane::EMPTY });
            out({ cluster2, Cluster::OUT_EMPTY }) >>
                in({ gantryCrane, GantryCrane::EMPTY });
            out({ cluster1, Cluster::OUT_FULL }) >>
                in({ gantryCrane, GantryCrane::FULL });
            out({ cluster2, Cluster::OUT_FULL }) >>
                in({ gantryCrane, GantryCrane::FULL });
        }
        {
            CraneParameters p;
            std::vector < unsigned int > c1 = { 1, 2, 3 };
            std::vector < unsigned int > c2 = { 4, 5 };

            p.stack_number = 5;
            p.stack_index_by_cluster.push_back(c1);
            p.stack_index_by_cluster.push_back(c2);
            p.destination_number = 5;
            p.move_duration = 0.05;
            crane = new artis::pdevs::Simulator <
                artis::common::DoubleTime, Crane,
                CraneParameters >("c", p);
            add_child(CRANE, crane);
        }
        {
            for (unsigned int i = 1; i <= 5; ++i) {
                StackParameters p;

                p.number = i;

                std::string model_name =
                    (boost::format("stack_%1%") % i).str();
                artis::pdevs::Simulator <
                    artis::common::DoubleTime,
                    Stack, StackParameters >* simulator =
                    new artis::pdevs::Simulator <
                        artis::common::DoubleTime, Stack,
                    StackParameters >(model_name, p);

                stacks.push_back(simulator);
                add_children(STACK, simulator);
                out({ gantryCrane, GantryCrane::OUT + i }) >>
                    in({ simulator, Stack::IN });
                if (i < 4) {
                    out({ simulator, Stack::OUT_FULL }) >>
                        in({ cluster1, Cluster::FULL });
                    out({ simulator, Stack::EMPTY }) >>
                        in({ cluster1, Cluster::EMPTY });
                    out({ gantryCrane, GantryCrane::OUT_FAIL + 1 }) >>
                        in({ simulator, Stack::FAIL });
                } else {
                    out({ simulator, Stack::OUT_FULL }) >>
                        in({ cluster2, Cluster::FULL });
                    out({ simulator, Stack::EMPTY }) >>
                        in({ cluster2, Cluster::EMPTY });
                    out({ gantryCrane, GantryCrane::OUT_FAIL + 2 }) >>
                        in({ simulator, Stack::FAIL });
                }
                out({ crane, Crane::TAKE + i }) >>
                    in({ simulator, Stack::TAKE });
                out({ simulator, Stack::OUT }) >> in({ crane, Crane::IN });
            }
        }
        {
            for (unsigned int i = 1; i <= 5; ++i) {
                StockParameters p;

                std::string model_name =
                    (boost::format("stock_%1%") % i).str();
                artis::pdevs::Simulator <
                    artis::common::DoubleTime,
                    Stock, StockParameters >* simulator =
                    new artis::pdevs::Simulator <
                        artis::common::DoubleTime, Stock,
                    StockParameters >(model_name, p);

                stocks.push_back(simulator);
                add_children(STOCK, simulator);
                out({ crane, Crane::OUT + i }) >> in({ simulator, Stock::IN });
            }
        }

        out({ generator1, Generator::OUT }) >>
            in({ runOutTable1, RunOutTable::IN });
        out({ runOutTable1, RunOutTable::OUT }) >>
            in({ gantryCrane, GantryCrane::NEW });
        out({ generator2, Generator::OUT }) >>
            in({ runOutTable2, RunOutTable::IN });
        out({ runOutTable2, RunOutTable::OUT }) >>
            in({ gantryCrane, GantryCrane::NEW });

        out({ runOutTable1, RunOutTable::ARRIVED }) >>
                 in({ gantryCrane, GantryCrane::ARRIVED });
        out({ runOutTable2, RunOutTable::ARRIVED }) >>
                 in({ gantryCrane, GantryCrane::ARRIVED });

        out({ gantryCrane, GantryCrane::TAKE }) >>
                 in({ runOutTable1, RunOutTable::TAKE });
        out({ gantryCrane, GantryCrane::TAKE }) >>
                 in({ runOutTable2, RunOutTable::TAKE });

        out({ cluster1, Cluster::OUT_FULL }) >> in({ crane, Crane::FULL });
        out({ cluster2, Cluster::OUT_FULL }) >> in({ crane, Crane::FULL });
        out({ cluster1, Cluster::OUT_EMPTY }) >> in({ crane, Crane::EMPTY });
        out({ cluster2, Cluster::OUT_EMPTY }) >> in({ crane, Crane::EMPTY });
    }

    virtual ~SubGraphManager()
    {
        delete generator1;
        delete generator2;
        delete runOutTable1;
        delete runOutTable2;
        delete gantryCrane;
        for (std::vector < artis::pdevs::Simulator <
                 artis::common::DoubleTime,
                 Stack, StackParameters >*  >::iterator it =
                 stacks.begin(); it != stacks.end(); ++it) {
            delete *it;
        }
        stacks.clear();
        delete cluster1;
        delete cluster2;
        delete crane;
        for (std::vector < artis::pdevs::Simulator <
                 artis::common::DoubleTime,
                 Stock, StockParameters >*  >::iterator it =
                 stocks.begin(); it != stocks.end(); ++it) {
            delete *it;
        }
        stocks.clear();
    }

private:
    artis::pdevs::Simulator < artis::common::DoubleTime, Generator,
                                 GeneratorParameters >* generator1;
    artis::pdevs::Simulator < artis::common::DoubleTime, Generator,
                                 GeneratorParameters >* generator2;

    artis::pdevs::Simulator < artis::common::DoubleTime, RunOutTable,
                                 RunOutTableParameters >* runOutTable1;
    artis::pdevs::Simulator < artis::common::DoubleTime, RunOutTable,
                                 RunOutTableParameters >* runOutTable2;

    artis::pdevs::Simulator <
        artis::common::DoubleTime,
        GantryCrane, GantryCraneParameters >* gantryCrane;

    std::vector < artis::pdevs::Simulator <
                      artis::common::DoubleTime,
                      Stack, StackParameters >* > stacks;

    artis::pdevs::Simulator <
        artis::common::DoubleTime,
        Cluster, ClusterParameters >* cluster1;
    artis::pdevs::Simulator <
        artis::common::DoubleTime,
        Cluster, ClusterParameters >* cluster2;

    artis::pdevs::Simulator <
        artis::common::DoubleTime,
        Crane, CraneParameters >* crane;

    std::vector < artis::pdevs::Simulator <
                      artis::common::DoubleTime,
                      Stock, StockParameters >* > stocks;
};

class RootGraphManager : public GraphManager
{
public:
    enum submodels { CC };

    RootGraphManager(Coordinator* coordinator,
                     const GlobalParameters parameters,
                     const NoParameters& graph_parameters) :
        GraphManager(coordinator, parameters, graph_parameters),
        S("CC", parameters, graph_parameters)
    {
        add_child(CC, &S);
    }

    virtual ~RootGraphManager()
    { }

private:
    PDEVSCoordinator < SubGraphManager > S;
};

} // namespace cc

#endif
