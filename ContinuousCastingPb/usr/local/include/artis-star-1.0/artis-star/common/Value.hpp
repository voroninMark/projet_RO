/**
 * @file Value.hpp
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

#ifndef COMMON_VALUE
#define COMMON_VALUE 1

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>

#include <cstring>
#include <typeinfo>
#include <vector>

#include <iostream>

namespace artis { namespace common {

class Value
{
public:
    Value() : _content(nullptr), _size(0)
    { }

    template < typename T >
    Value(T value)
    { assign(&value, sizeof(T), typeid(T).hash_code()); }

    Value(void* content, size_t size)
    { assign(content, size, typeid(void*).hash_code()); }

    Value(const char* value, unsigned int size)
    { assign(value, size * sizeof(char), typeid(char*).hash_code()); }

    Value(const Value& value)
    {
        if (value._content) {
            assign(value._content, value._size, value._type_id);
        } else {
            _content = nullptr;
            _size = 0;
            _type_id = 0;
        }
    }

    virtual ~Value()
    { if (_content != nullptr) delete[] _content; }

    bool empty() const
    { return _content == nullptr; }

    template < typename T >
    void operator()(T& value) const
    {
        assert(_type_id == typeid(T).hash_code());

        value = *(T*)(_content);
    }

    template < typename Z >
    bool is_type() const
    { return _type_id == typeid(Z).hash_code(); }

    void operator=(const Value& value)
    {
        if (_content) {
            delete _content;
        }
        if (value._content) {
            assign(value._content, value._size, value._type_id);
        } else {
            _content = nullptr;
            _size = 0;
            _type_id = 0;
        }
    }


    std::string to_string() const
    {
        if (is_type < double >()) {
            double v;

            operator()(v);
            return std::to_string(v);
        } else if (is_type < int >()) {
            int v;

            operator()(v);
            return std::to_string(v);
        } else if (is_type < bool >()) {
            bool v;

            operator()(v);
            return v ? "true" : "false";
        } if (is_type < std::vector < double > >()) {
            std::vector < double > v;

            operator()(v);
            return "";
        } else if (is_type < std::vector < int > >()) {
            std::vector < int > v;

            operator()(v);
            return "";
        } else if (is_type < std::vector < bool > >()) {
            std::vector < bool > v;

            operator()(v);
            return "";
        } else {
            return "<unstringify>";
        }
    }

private:
    void assign(const void* content, size_t size, size_t type_id)
    {
        _content = new char[size];
        std::memcpy(_content, content, size);
        _size = size;
        _type_id = type_id;
    }

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        (void) version;

        ar & _size;
        if (Archive::is_loading::value) {
            assert(_content == nullptr);
            _content = new char[_size];
        }
        ar & boost::serialization::make_array < char >(_content, _size);
        ar & _type_id;
    }

    char*  _content;
    size_t _size;
    size_t _type_id;
};

} } // namespace artis common

#endif
