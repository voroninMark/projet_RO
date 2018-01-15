/**
 * @file artis-star/common/observer/View.hpp
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

#ifndef ARTIS_COMMON_OBSERVER_VIEW_HPP
#define ARTIS_COMMON_OBSERVER_VIEW_HPP

#include <artis-star/common/Model.hpp>
#include <artis-star/common/time/DoubleTime.hpp>
#include <artis-star/common/Value.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace artis { namespace observer {

template < typename Time >
class View
{
    typedef std::vector < int > Selector;

public:
    typedef std::vector < std::pair < double, common::Value > > Values;
    typedef std::map < std::string, Values > VariableValues;
    typedef std::map < std::string, VariableValues > SelectorValues;

    enum vars { ALL = -1 };

    View() : _model(0)
    { }

    virtual ~View()
    { }

    void attachModel(const artis::common::Model < Time >* m)
    { _model = m; }

    double begin() const
    {
        double t = common::DoubleTime::infinity;

        // for (SelectorValues::const_iterator it = _values.begin(); it!= _values.end();
        //      ++it) {
        //     if (t > it->second.begin()->first) {
        //         t = it->second.begin()->first;
        //     }
        // }
        return t;
    }

    View* clone() const
    {
        View* v = new View();

        // v->_selectors = _selectors;
        // for (Values::const_iterator it = _values.begin(); it!= _values.end();
        //      ++it) {

        //     v->_values[it->first] = Value();
        //     Value::const_iterator itp = it->second.begin();

        //     while (itp != it->second.end()) {
        //         v->_values[it->first].push_back(*itp);
        //         ++itp;
        //     }
        // }
        // v->_model = 0;
        return v;
    }

    double end() const
    {
        double t = 0;

        // for (SelectorValues::const_iterator it = _values.begin(); it!= _values.end();
        //      ++it) {
        //     if (t < it->second.back().first) {
        //         t = it->second.back().first;
        //     }
        // }
        return t;
    }

    double get(double t, const std::string& selector_name,
               const std::string& variable_name) const
    {
        SelectorValues::const_iterator it = _values.find(selector_name);

        if (it != _values.end()) {
            VariableValues::const_iterator itp = it->second.find(variable_name);

            if (itp != it->second.end()) {
                Values::const_iterator itv = itp->second.begin();

                while (itv != itp->second.end() and itv->first < t) {
                    ++itv;
                }
                if (itv != itp->second.end()) {
                    // TODO: to improve
                    return boost::lexical_cast < double >(itv->second);
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        }
        return 0;
    }

    const Values& get(const std::string& selector_name,
                      const std::string& variable_name) const
    {
        SelectorValues::const_iterator it = _values.find(selector_name);

        if (it != _values.end()) {
            VariableValues::const_iterator itv = it->second.find(variable_name);

            if (itv != it->second.end()) {
                return itv->second;
            } else {
                assert(false);
            }
        } else {
            assert(false);
        }
    }

    const Values& get(const std::string& selector_name) const
    {
        SelectorValues::const_iterator it = _values.find(selector_name);

        if (it != _values.end()) {
            assert(it->second.size() == 1);

            return it->second.begin()->second;
        } else {
            assert(false);
        }
    }

    void observe(double time, const common::Model < common::DoubleTime >* model,
                 const std::string& selector_name, unsigned int variable_index)
    {
        std::string path = (boost::format("%1%:%2%") % model->path() %
                            model->observable_name(variable_index)).str() ;
        VariableValues& values = _values[selector_name];

        if (values.find(path) == values.end()) {
            values[path] = Values();
        }
        values[path].push_back(
            std::make_pair(time, model->observe(time, variable_index)));
    }

    void observe(const Selector& chain, unsigned int i,
                 double time, const common::Model < common::DoubleTime >* model,
                 const std::string& selector_name, unsigned int variable_index)
    {
        while (i < chain.size() - 1 and chain[i + 1] != ALL and model) {
            assert(chain[i] >= 0);
            model = model->get_submodel((unsigned int)chain[i]);
            ++i;
        }
        if (i < chain.size() - 1 and chain[i + 1] == ALL) {
            for (size_t model_index = 0;
                 model_index < model->get_submodel_number(chain[i]);
                 ++model_index) {
                assert(chain[i] >= 0);
                observe(chain, i + 2, time,
                        model->get_submodel((unsigned int)chain[i],
                                            model_index),
                        selector_name, variable_index);
            }
        } else {
            if (model) {
                observe(time, model, selector_name, variable_index);
            }
        }
    }

    virtual void observe(double time)
    {
        for (typename Selectors::const_iterator it = _selectors.begin();
             it != _selectors.end(); ++it) {
            const common::Model < common::DoubleTime >* model = _model;

            if (it->second.size() > 1) {
                size_t i = 0;

                observe(it->second, i, time, model, it->first,
                        it->second.back());
            } else {
                if (model) {
                    observe(time, model, it->first, it->second.back());
                }
            }
        }
    }

    void selector(const std::string& name, const Selector& chain)
    {
        _selectors[name] = chain;
    }

    const SelectorValues& values() const
    { return _values;}

private:
    typedef std::map < std::string, Selector > Selectors;

    Selectors                            _selectors;
    SelectorValues                       _values;
    const artis::common::Model < Time >* _model;
};

} }

#endif
