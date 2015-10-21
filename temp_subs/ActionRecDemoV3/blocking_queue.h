//
//  blocking_queue.h
//  ServerCode
//
//  Created by Subs on 10/17/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#ifndef blocking_queue_h
#define blocking_queue_h


#include <mutex>
#include <condition_variable>
#include <deque>

template <typename T>
class myqueue
{
private:
    std::mutex              d_mutex;
    std::condition_variable d_condition;
    std::deque<T>           d_queue;
public:
    void push(T const& value) {
        {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            d_queue.push_front(value);
        }
        this->d_condition.notify_one();
    }
    T pop() {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        this->d_condition.wait(lock, [=]{ return !this->d_queue.empty(); });
        T rc(std::move(this->d_queue.back()));
        this->d_queue.pop_back();
        return rc;
    }
    
};

#endif /* blocking_queue_h */
