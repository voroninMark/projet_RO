/**
 * @file artis-star/observer/Output.hpp
 * @author See the AUTHORS file
 */

/*
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

#ifndef ARTIS_COMMON_OBSERVER_OUTPUT_HPP
#define ARTIS_COMMON_OBSERVER_OUTPUT_HPP

#include <artis-star/common/observer/Observer.hpp>
#include <artis-star/common/observer/View.hpp>
#include <artis-star/utils/DateTime.hpp>

#include <boost/format.hpp>

namespace artis { namespace observer {

template < typename Time >
class Output
{
public:
    Output(const Observer < Time >& observer) : _observer(observer)
    { }

    virtual ~Output()
    { }

    void operator()() const
    {
        const typename Observer < Time >::Views& views = _observer.views();

        for (typename Observer < Time >::Views::const_iterator it =
                 views.begin(); it != views.end(); ++it) {
            std::ofstream o((boost::format("%1%.csv") % it->first).str());
            typename View < Time >::Values values = it->second->values();
            double begin = it->second->begin();
            double end = it->second->end();

            o.precision(10);
            // write header
            o << "time";
            for (typename View < Time >::Values::const_iterator
                     itv = values.begin(); itv != values.end(); ++itv) {
                o << ";" << itv->first;
            }
            o << std::endl;

            // write values
            for (double t = begin; t <= end; ++t) {
                o << utils::DateTime::toJulianDay(t);
                // o << t;
                for (typename View < Time >::Values::const_iterator itv =
                         values.begin(); itv != values.end(); ++itv) {
                    typename View < Time >::Value::const_iterator itp =
                        itv->second.begin();

                    while (itp != itv->second.end() and itp->first < t) {
                        ++itp;
                    }
                    o << ";";
                    if (itp != itv->second.end()) {
                        o << itp->second;
                    } else {
                        o << "NA";
                    }
                }
                o << std::endl;
            }
        }
    }

private:
    const Observer < Time >& _observer;
};

} }

#endif
