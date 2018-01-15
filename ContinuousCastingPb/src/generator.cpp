/**
 * @file generator.cpp
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

#include <generator.hpp>

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace cc {

Generator::Generator(const std::string& name,
                     const Context < Generator, GeneratorParameters >&
                     context) :
    artis::pdevs::Dynamics < artis::common::DoubleTime,
                             Generator, GeneratorParameters >(name, context),
    _cc_index(context.parameters().cc_index),
    _start_indexes(context.parameters().start_indexes)
{
    output_port({ OUT, "out" });

    std::ifstream f("../data/input.csv");
    std::vector < std::string > columns;
    char line[256];
    unsigned int index = 0;
    unsigned int stack_index = _start_indexes[index];
    double timestamp = -1;

    do {
        f.getline(line, 256);

        std::string l = line;

        boost::split(columns, l, boost::is_any_of(";"));

        if (columns.size() == 10) {
            unsigned int cc_index = boost::lexical_cast < int >(columns[0]);

            if (cc_index == _cc_index) {
                bool ok = true;
                unsigned int current_index =
                    boost::lexical_cast < int >(columns[2]);

                if (stack_index == current_index) {
                    ++stack_index;
                } else if (index < _start_indexes.size() - 1 and
                           current_index == _start_indexes[index + 1]) {
                    ++index;
                    stack_index = _start_indexes[index] + 1;
                } else {
                    ok = false;
                }
                if (ok) {
                    double current_length =
                        boost::lexical_cast < double >(columns[7]);
                    double current_width =
                        boost::lexical_cast < double >(columns[8]);
                    double current_timestamp =
                        boost::lexical_cast < double >(columns[1]);
                    char current_destination =
                        boost::lexical_cast < char >(columns[5]);
                    double duration;

                    if (timestamp == -1) {
                        duration = 0;
                    } else {
                        duration = (current_timestamp - timestamp) * 24 * 60;
                    }
                    timestamp = current_timestamp;

                    Slab slab;

                    slab.length = current_length;
                    slab.width = current_width;
                    // Minus A char value (65) and adding 1 to get valid destination
                    slab.destination = current_destination - 64;
                    slab.index = current_index;
                    slab.cc_number = _cc_index;
                    _slabs.push_back(slab);

                    _durations.push_back(duration);
                }
            }
        }
    } while (not f.eof());
    f.close();
}

Generator::~Generator()
{ }

void Generator::dint(Time /* t */)
{
    if (_phase == SEND) {
 //        _next_time = t + _rand.normal(5, 0.5);
        _next_time = _durations[_index];
        _phase = WAIT;
    } else if (_phase == WAIT) {
        _phase = SEND;
        ++_index;
    }
}

void Generator::dext(Time /* t */, Time /* e */, const Bag& /* msgs */)
{ }

Time Generator::start(Time /* t */)
{
    _phase = WAIT;
    _index = 0;
//    _next_time = t + _rand.normal(3, 0.5);
    _next_time = _durations[_index];
    return _next_time;
}

Time Generator::ta(Time /* t */) const
{
    if (_phase == WAIT) {
        return _next_time;
    } else {
        return 0;
    }
}

Bag Generator::lambda(Time t) const
{
    Bag msgs;

    if (_phase == SEND) {
        Slab slab = _slabs[_index];

        slab.table_number = -1;
        slab.max_date = -1;

#ifdef WITH_TRACE_MODEL
        Trace::trace() << TraceElement(get_name(), t, artis::common::LAMBDA)
                       << "out: " << slab.to_string();
        Trace::trace().flush();
#endif

        msgs.push_back(ExternalEvent(OUT, slab));
    }
    return msgs;
}

} // namespace cc
