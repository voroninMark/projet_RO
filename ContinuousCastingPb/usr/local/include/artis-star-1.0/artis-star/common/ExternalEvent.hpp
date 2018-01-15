/**
 * @file ExternalEvent.hpp
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

#ifndef COMMON_EXTERNAL_EVENT
#define COMMON_EXTERNAL_EVENT 1

#include <artis-star/common/Model.hpp>
#include <artis-star/common/Node.hpp>
#include <artis-star/common/Value.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace artis { namespace common {

template < class Time >
class Node;
template < class Time >
class Model;

template < class Time >
class ExternalEvent
{
public:
    ExternalEvent(unsigned int port_index, const Value& data) :
        _port_index(port_index), _model(nullptr), _data(data)
    { }

    ExternalEvent(const Node < Time >& node, const Value& data) :
        _port_index(node.get_port_index()),
        _model(node.get_model()),
        _data(data)
    { }

    ExternalEvent(const ExternalEvent& event) :
        _port_index(event._port_index), _model(event._model),
        _data(event._data)
    { }

    ExternalEvent() : _model(nullptr)
    { }

    virtual ~ExternalEvent()
    { }

    const Value& data() const
    { return _data; }

    unsigned int get_port_index() const
    { return _port_index; }

    void data(const Value& data)
    { _data = data; }

    Model < Time >* get_model() const
    { return _model; }

    bool on_port(unsigned int port_index) const
    { return _port_index == port_index; }

    void set_model(Model < Time >* model)
    { _model = model; }

    std::string to_string() const
    {
        std::ostringstream ss;

        ss << "( " << _port_index << " , "
           << (_model ? _model->get_name() : "<>")
           << " , ";
        if (not _data.empty()) {
            ss << _data.to_string();
        } else {
            ss << "null";
        }
        ss << ")";
        return ss.str();
    }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        (void) version;

        ar & _port_index;
        _model = 0;
        // ar & _model;
        ar & _data;
        // ar & _model->get_name();
    }

    unsigned int    _port_index;
    Model < Time >* _model;
    Value           _data;
};

} } // namespace artis common

#endif
