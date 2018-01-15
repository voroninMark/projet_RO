/**
 * @file generator.hpp
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

#ifndef CC_GENERATOR_HPP
#define CC_GENERATOR_HPP

#include <models.hpp>

namespace cc {

struct GeneratorParameters
{
    unsigned int cc_index;
    std::vector < unsigned int > start_indexes;
};

class Generator : public artis::pdevs::Dynamics < artis::common::DoubleTime,
                                                  Generator,
                                                  GeneratorParameters >
{
public:
    enum outputs { OUT };

    Generator(const std::string& name,
              const Context < Generator, GeneratorParameters >& context);
    virtual ~Generator();

    void dint(Time t);
    void dext(Time /* t */, Time /* e */, const Bag& /* msgs */);
    Time start(Time t);
    Time ta(Time t) const;
    Bag lambda(Time /* t */) const;

    Value observe(const Time& /* t */, unsigned int /* index */) const
    { return artis::common::Value(); }

private:
    enum Phase { WAIT, SEND };

    // state
    Phase        _phase;
    unsigned int _index;
    double       _next_time;

    // parameters
    unsigned int                 _cc_index;
    std::vector < unsigned int > _start_indexes;
    Slabs                        _slabs;
    std::vector < double >       _durations;
};

} // namespace cc

#endif
