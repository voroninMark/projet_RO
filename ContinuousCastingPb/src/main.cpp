/**
 * @file main.cpp
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

#include <graph_manager.hpp>
#include <models.hpp>

#include <artis-star/common/RootCoordinator.hpp>

#include <solution.hpp>
#include <evalCC.hpp>

#include <chrono>
#include <fstream>
#include <iostream>

using namespace cc;
using namespace artis::common;
using namespace std;

/*
class View : public cc::View
{
public:
    View()
    {
        selector("Stack:height", { RootGraphManager::CC,
                    SubGraphManager::STACK, cc::View::ALL, Stack::HEIGHT });
        selector("Crane:moveNumber", { RootGraphManager::CC,
                    SubGraphManager::CRANE, Crane::MOVE_NUMBER });
        selector("Crane:slabNumber", { RootGraphManager::CC,
                    SubGraphManager::CRANE, Crane::SLAB_NUMBER });
    }
};

void stat(const artis::common::RootCoordinator < DoubleTime,
          artis::pdevs::Coordinator < DoubleTime, RootGraphManager,
          GlobalParameters > >& rc)
{
    unsigned int sum = 0;
    unsigned int sum_N = 0;

    for (unsigned int i = 1; i <= 5; ++i) {
        const ::View::Values& values = rc.observer().view("CC").get(
            "Stack:height", (boost::format(":root:CC:stack_%1%:height") %
                             i).str());

        for (::View::Values::const_iterator it = values.begin();
             it != values.end(); ++it) {
            int height;

            it->second(height);
            sum += height;
        }
        sum_N += values.size();
    }
    std::cout << "Average of stack height = " << ((double)sum / sum_N)
              << std::endl;

    {
        const ::View::Values& values = rc.observer().view("CC").get(
            "Crane:moveNumber");
        int move_number;

        values.back().second(move_number);
        std::cout << "Crane move number: " << move_number << std::endl;
    }
    {
        const ::View::Values& values = rc.observer().view("CC").get(
            "Crane:slabNumber");
        int slab_number;

        values.back().second(slab_number);
        std::cout << "Total slab number: " << slab_number << std::endl;
    }
}
*/

int main()
{
	// constants related to the dimension of the optimization problem 
    const unsigned int n_stack = 5;
    const unsigned int n_destination = 8;
    const unsigned int solution_size = n_destination + n_stack * n_destination * (n_destination - 1);

    // evaluation function from the Continuous Casting simulator
    EvalCC eval;

    // Declaration of one solution of the optimization problem
    Solution s ;
	for(int j=0 ; j<1;j++){
		for(unsigned int i = 0;i<solution_size;i++){
			s.push_back(rand() % 100);
		}
		eval(s);
		cout<<s.fitness()<<endl;
		s.print();
		s.clear();
	}
    // solution for random selection: all stacks have the same preference 1
   // s.resize(solution_size, 1);

    // evaluation of the solution
    //eval(s);

    // print the result
    //std::cout << s.to_string() << std::endl;

    return 0;
}
