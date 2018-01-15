/**
 * @file artis-star/common/observer/Observer.hpp
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

#ifndef ARTIS_OBSERVER_OBSERVER_HPP
#define ARTIS_OBSERVER_OBSERVER_HPP

#include <artis-star/common/Model.hpp>
#include <artis-star/common/observer/View.hpp>

#include <vector>

namespace artis { namespace observer {

template < typename Time >
class Observer
{
public:
    typedef std::map < std::string, View < Time >* > Views;

    Observer(const common::Model < Time >* model) :
        _model(model)
    { }

    virtual ~Observer()
    {
        for (typename Views::iterator it = _views.begin(); it != _views.end();
             ++it) {
            delete it->second;
        }
    }

    void attachView(const std::string& name, View < Time >* view)
    {
        _views[name] = view;
        view->attachModel(_model);
    }

    Views* cloneViews() const
    {
        Views* v = new Views();

        for (typename Views::const_iterator it = _views.begin();
             it != _views.end(); ++it) {
            (*v)[it->first] = it->second->clone();
        }
        return v;
    }

    const View < Time >& view(const std::string& name) const
    { return *_views.find(name)->second; }

    const Views& views() const
    { return _views; }

    void init()
    { }

    void observe(double t)
    {
        for (typename Views::iterator it = _views.begin(); it != _views.end();
             ++it) {
            it->second->observe(t);
        }
    }

private:
    const common::Model < Time >* _model;
    Views                         _views;
};

} }

#endif
