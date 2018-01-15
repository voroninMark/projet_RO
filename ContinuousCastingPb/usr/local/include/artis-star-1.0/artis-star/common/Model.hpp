/**
 * @file Model.hpp
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

#ifndef COMMON_MODEL
#define COMMON_MODEL

#include <artis-star/common/Bag.hpp>
#include <artis-star/common/ExternalEvent.hpp>
#include <artis-star/common/InternalEvent.hpp>
#include <artis-star/common/Scheduler.hpp>
#include <artis-star/common/Value.hpp>

#include <algorithm>
#include <cassert>
#include <map>
#include <iostream>
#include <sstream>

namespace artis { namespace common {

template < class Time >
class ExternalEvent;

template < class Time >
class InternalEvent;

template < class Time >
class Bag;

struct Port
{
    unsigned int index;
    std::string name;
};

typedef std::vector < unsigned int > Ports;
typedef std::map < unsigned int, std::string > PortMap;

template < class Time >
class Model
{
public:
    Model(const std::string& name) :
        _tl(0), _tn(0), _parent(0), _name(name), _inputs(0)
    { }

    virtual ~Model()
    {
        if (_inputs) {
            delete _inputs;
        }
    }

    // structure
    void add_in_port(const Port& port)
    {
        assert(not exist_in_port(port.index));

        _in_ports.push_back(port.index);
        _in_port_map[port.index] = port.name;
    }

    void add_out_port(const Port& port)
    {
        assert(not exist_out_port(port.index));

        _out_ports.push_back(port.index);
        _out_port_map[port.index] = port.name;
    }

    void delete_in_port(const Port& port)
    {
        assert(not exist_in_port(port.index));

        _in_ports.erase(std::find(_in_ports.begin(), _in_ports.end(),
                                  port.index));
        _in_port_map.erase(port.index);
    }

    void delete_out_port(const Port& port)
    {
        assert(not exist_out_port(port.index));

        _out_ports.erase(std::find(_out_ports.begin(), _out_ports.end(),
                                   port.index));
        _out_port_map.erase(port.index);
    }

    bool exist_in_port(unsigned int port_index) const
    {
        return _in_port_map.find(port_index) != _in_port_map.end();
    }

    bool exist_out_port(unsigned int port_index) const
    {
        return _out_port_map.find(port_index) != _out_port_map.end();
    }

    std::string get_in_port_name(unsigned int port_index) const
    {
        assert(exist_in_port(port_index));

        return _in_port_map.find(port_index)->second;
    }

    std::string get_out_port_name(unsigned int port_index) const
    {
        assert(exist_out_port(port_index));

        return _out_port_map.find(port_index)->second;
    }

    const std::string& get_name() const
    { return _name; }

    Model < Time >* get_parent() const
    { return _parent; }

    // TODO: to remove
    virtual int get_receiver_number(typename Time::type t)
    { (void)t; return 0; }

    virtual const Model < Time >* get_submodel(unsigned int index) const
    { (void)index; assert(false); }

    virtual const Model < Time >* get_submodel(unsigned int index,
                                               unsigned int rank) const
    { (void)index; (void)rank; assert(false); }

    virtual unsigned int get_submodel_number(unsigned int index) const
    { (void)index; assert(false); }

    virtual bool is_atomic() const = 0;

    virtual bool is_remote() const
    { return false; }

    virtual std::string observable_name(unsigned int observable_index) const
    { (void)observable_index; assert(false); }

    std::string path() const
    { return (_parent != nullptr ? _parent->path() : "") + ":" + get_name(); }

    void set_parent(Model < Time >* parent)
    { _parent = parent; }

    virtual std::string to_string(int /* level */) const =0;

    // event
    void add_event(const common::ExternalEvent < Time >& message)
    {
        if (_inputs == 0) {
            _inputs = new Bag < Time >;
        }
        _inputs->push_back(message);
    }

    void clear_bag()
    {
        if (_inputs) {
            delete _inputs;
            _inputs = 0;
        }
    }

    unsigned int event_number() const
    {
        if (_inputs) {
            return _inputs->size();
        } else {
            return 0;
        }
    }

    const common::Bag < Time >& get_bag()
    {
        if (_inputs == 0) {
            _inputs = new Bag < Time >;
        }
        return *_inputs;
    }

    // time
    typename Time::type get_tl() const
    { return _tl; }

    typename Time::type get_tn() const
    { return _tn; }

    // devs methods
    virtual common::Value observe(const typename Time::type& t,
                                  unsigned int index) const =0;
    virtual void output(const typename Time::type& t) =0;
    virtual void post_event(const typename Time::type& t,
                            const common::ExternalEvent < Time >& event) =0;
    virtual typename Time::type start(const typename Time::type& t) =0;
    virtual typename Time::type transition(const typename Time::type& t) =0;

    // scheduler
    void handle(SchedulerHandle handle)
    { _handle.handle(handle); }

    const SchedulerHandle& handle() const
    { return _handle.handle(); }

protected:
    typename Time::type _tl;
    typename Time::type _tn;

private :
    Model < Time >* _parent;
    std::string     _name;
    Ports           _in_ports;
    PortMap         _in_port_map;
    Ports           _out_ports;
    PortMap         _out_port_map;

    Bag < Time >*   _inputs;
    SchedulerHandle _handle;
};

template < class Time >
class ModelMap : public std::map < unsigned int, Model < Time >* >
{
public:
    ModelMap()
    { }
    virtual ~ModelMap()
    { }

    std::string to_string() const
    {
        std::ostringstream ss;

        ss << "{ ";
        for (typename ModelMap < Time >::const_iterator it =
                 ModelMap < Time >::begin();
             it != ModelMap < Time >::end(); ++it) {
            ss << it->second->get_name() << " ";
        }
        ss << "}";
        return ss.str();
    }
};

template < class Time >
class Models : public std::vector < Model < Time >* >
{
public:
    Models()
    { }
    virtual ~Models()
    { }

    std::string to_string() const
    {
        std::ostringstream ss;

        ss << "{ ";
        for (typename Models < Time >::const_iterator it =
                 Models < Time >::begin();
             it != Models < Time >::end(); ++it) {
            ss << (*it)->get_name() << " ";
        }
        ss << "}";
        return ss.str();
    }
};

template < class Time >
class ModelsMap : public std::map < unsigned int, Models < Time > >
{
public:
    ModelsMap()
    { }
    virtual ~ModelsMap()
    { }

    std::string to_string() const
    {
        std::ostringstream ss;

        ss << "{ ";
        for (typename ModelsMap < Time >::const_iterator it =
                 ModelsMap < Time >::begin();
             it != ModelsMap < Time >::end(); ++it) {
            ss << it->second.to_string() << " ";
        }
        ss << "}";
        return ss.str();
    }
};

} } // namespace artis common

#endif
