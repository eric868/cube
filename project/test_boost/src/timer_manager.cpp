#include <iostream>
#include "timer_manager.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 

void TimerManager::Register(int timeinterval, timer_callback cb)
{
    auto it = clocks_.find(timeinterval);
    if (it != clocks_.end())
    {
        it->second->Register(cb);
    }
    else
    {
        std::shared_ptr<Timer> timer = std::make_shared<Timer>(timeinterval);
        timer->Register(cb);
        clocks_[timeinterval] = timer;
    }
}

void TimerManager::Register(std::string time_point, timer_callback cb)
{
    auto it = alarms_.find(time_point);
    if (it != alarms_.end())
    {
        it->second->Register(cb);
    }
    else
    {
        std::shared_ptr<TimePoint> value = std::make_shared<TimePoint>(time_point);
        value->Register(cb);
        alarms_[time_point] = value;
    }
}

void TimerManager::start()
{
    work_ = std::make_shared<boost::asio::io_service::work>(processor_service_);
    iothread_ = thread_ptr(new boost::thread(std::bind(&TimerManager::run, this, &processor_service_)));
    for (auto it = clocks_.begin(); it != clocks_.end(); ++it) {
        if (it->second) {
            it->second->start();
        }
    }

    for (auto it = alarms_.begin(); it != alarms_.end(); ++it) {
        if (it->second) {
            it->second->start();
        }
    }
    //std::cout << "TimerManager processor thread started... threadid: " << iothread_->get_id();
}


void TimerManager::run(boost::asio::io_service* io_service)
{
    io_service->run();
    std::cout << " io service loop end";
}

TimerManager::Timer::Timer(int interval)
    : interval_(interval)
{
}

void TimerManager::Timer::Register(timer_callback cb)
{
    processors_.push_back(cb);
}

void TimerManager::Timer::start()
{
    timer_ = std::make_shared<boost::asio::deadline_timer>(
        TimerManager::get_timer_manager().get_io_service(), 
        boost::posix_time::seconds(interval_));
    timer_->expires_from_now(boost::posix_time::milliseconds(interval_));
    timer_->async_wait(std::bind(&TimerManager::Timer::handler_timer, shared_from_this(), std::placeholders::_1));
}

void TimerManager::Timer::handler_timer(const boost::system::error_code& error)
{
    if (error != boost::asio::error::operation_aborted) {
        // Timer was not cancelled, take necessary action.
        for (auto it = processors_.begin(); it != processors_.end(); ++it) {
            if (*it) {
                (*it)();
            }
        }
        timer_->expires_from_now(boost::posix_time::milliseconds(interval_));
        timer_->async_wait(std::bind(&TimerManager::Timer::handler_timer, shared_from_this(), std::placeholders::_1));
    }
}

TimerManager::TimePoint::TimePoint(std::string time_point)
    : time_point_(time_point)
{
}

void TimerManager::TimePoint::Register(timer_callback cb)
{
    processors_.push_back(cb);
}

void TimerManager::TimePoint::start()
{
    timer_ = std::make_shared<boost::asio::deadline_timer>(
        TimerManager::get_timer_manager().get_io_service());
    boost::gregorian::date d1 = boost::gregorian::day_clock::local_day();
    std::string ptime_str = boost::gregorian::to_iso_extended_string(d1) + ' ' + time_point_;
    boost::posix_time::ptime p1 = boost::posix_time::time_from_string(ptime_str);
    boost::posix_time::time_duration duration = p1 - boost::posix_time::second_clock::local_time();
    if (duration.total_seconds() <= 0) {
        duration += boost::posix_time::seconds(3600 * 24);
    }
    timer_->expires_from_now(duration);
    timer_->async_wait(std::bind(&TimerManager::TimePoint::handler_timer, shared_from_this(), std::placeholders::_1));
}

void TimerManager::TimePoint::handler_timer(const boost::system::error_code& error)
{
    if (error != boost::asio::error::operation_aborted) {
        // Timer was not cancelled, take necessary action.
        for (auto it = processors_.begin(); it != processors_.end(); ++it) {
            if (*it) {
                (*it)();
            }
        }
        timer_->expires_from_now(boost::posix_time::hours(24));
        timer_->async_wait(std::bind(&TimerManager::TimePoint::handler_timer, shared_from_this(), std::placeholders::_1));
    }
}

