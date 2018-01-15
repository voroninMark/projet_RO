/**
 * @file common/utils/Multithreading.hpp
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

#ifndef COMMON_UTILS_MULTITHREADING
#define COMMON_UTILS_MULTITHREADING 1

#include <condition_variable>
#include <queue>
#include <mutex>

namespace artis { namespace common {

struct BaseMessage
{
    virtual ~BaseMessage()
    { }
};

template < typename Msg >
struct Message : BaseMessage
{
    explicit Message(Msg const& content) : _content(content)
    { }

    Msg _content;
};

class Close
{ };

class MessageQueue
{
public:
    template < typename T >
    void push(T const& msg)
    {
        std::lock_guard < std::mutex > lock(_mutex);

        _queue.push(std::make_shared < Message < T > >(msg));
        _condition.notify_all();
    }

    std::shared_ptr < BaseMessage > wait_and_pop()
    {
        std::unique_lock < std::mutex > lock(_mutex);

        _condition.wait(lock, [&]{ return not _queue.empty(); });
        auto res = _queue.front();
        _queue.pop();
        return res;
    }

private:
    std::mutex                                     _mutex;
    std::condition_variable                        _condition;
    std::queue < std::shared_ptr < BaseMessage > > _queue;
};

class Sender
{
public:
    Sender() : _queue(0)
    { }

    explicit Sender(MessageQueue* queue) : _queue(queue)
    { }

    template < typename Message >
    void send(Message const& msg)
    {
        if (_queue) {
            _queue->push(msg);
        }
    }

private:
    MessageQueue* _queue;
};

template < typename PreviousDispatcher, typename Msg, typename Func >
class TemplateDispatcher
{
    template < typename Dispatcher, typename OtherMsg, typename OtherFunc >
    friend class TemplateDispatcher;

public:
    TemplateDispatcher(TemplateDispatcher&& other) : _queue(other._queue),
                                                     _previous(other._previous),
                                                     _function(
                                                         std::move(
                                                             other._function)),
                                                     _chained(other._chained)
    { other._chained = true; }

    TemplateDispatcher(MessageQueue* queue,
                       PreviousDispatcher* previous,
                       Func&& function) :
        _queue(queue), _previous(previous),
        _function(std::forward < Func >(function)), _chained(false)
    { previous->_chained = true; }

    bool dispatch(std::shared_ptr < BaseMessage > const& msg)
    {
        Message < Msg >* message =
            dynamic_cast < Message < Msg >* >(msg.get());

        if (message) {
            _function(message->_content);
            return true;
        } else {
            return _previous->dispatch(msg);
        }
    }

    template < typename OtherMsg, typename OtherFunc >
    TemplateDispatcher < TemplateDispatcher < PreviousDispatcher, Msg, Func >,
                         OtherMsg, OtherFunc >
    handle(OtherFunc&& of)
    {
        return TemplateDispatcher < TemplateDispatcher < PreviousDispatcher,
                                                         Msg, Func >,
                                    OtherMsg, OtherFunc >(
            _queue, this, std::forward < OtherFunc >(of));
    }

    ~TemplateDispatcher() noexcept(false)
    {
        if (not _chained) {
            wait_and_dispatch();
        }
    }

private:
    TemplateDispatcher(TemplateDispatcher const&)=delete;

    TemplateDispatcher& operator=(TemplateDispatcher const&)=delete;

    void wait_and_dispatch()
    {
        for(;;) {
            auto msg = _queue->wait_and_pop();

            if (dispatch(msg)) {
                break;
            }
        }
    }

    MessageQueue*       _queue;
    PreviousDispatcher* _previous;
    Func                _function;
    bool                _chained;
};

class Dispatcher
{
    template < typename Dispatcher, typename Msg, typename Func>
    friend class TemplateDispatcher;

public:
    Dispatcher(Dispatcher&& other) : _queue(other._queue),
                                     _chained(other._chained)
    { other._chained = true; }

    explicit Dispatcher(MessageQueue* queue) : _queue(queue), _chained(false)
    { }

    template < typename Message, typename Func >
    TemplateDispatcher < Dispatcher, Message, Func >
    handle(Func&& function)
    {
        return TemplateDispatcher < Dispatcher, Message, Func >(
            _queue, this, std::forward < Func >(function));
    }

    ~Dispatcher() noexcept(false)
    {
        if (not _chained) {
            wait_and_dispatch();
        }
    }

private:
    Dispatcher(Dispatcher const&)=delete;

    Dispatcher& operator=(Dispatcher const&)=delete;

    void wait_and_dispatch()
    {
        for(;;) {
            auto msg = _queue->wait_and_pop();

            dispatch(msg);
        }
    }

    bool dispatch(std::shared_ptr < BaseMessage > const& msg)
    {
        if (dynamic_cast < Message < Close >* >(msg.get())) {
            throw Close();
        }
        return false;
    }

    MessageQueue* _queue;
    bool          _chained;
};

class Receiver
{
public:
    Receiver()
    { }

    operator Sender()
    { return Sender(&_queue); }

    Dispatcher wait()
    { return Dispatcher(&_queue); }

private:
    MessageQueue _queue;
};

} }  // namespace artis common

#endif
