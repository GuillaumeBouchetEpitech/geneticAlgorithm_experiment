

#pragma once


#include "Consumer.hpp"
#include "t_task.hpp"


#include <list>
#include <vector>
#include <functional>

#include <thread>
#include <mutex>
#include <condition_variable>


class Producer
{
	friend class Consumer; 

private:
    std::thread             m_thread;
    std::mutex              m_mutex;
    std::condition_variable m_cond_var;

    bool m_done{false};
    bool m_notified{true};

    std::list<t_task>	m_list_todo;
    std::list<t_task>	m_list_doing;
    std::list<t_task>	m_list_done;

    std::vector<Consumer*> m_arr_pConsumers;

public:
	Producer();
	~Producer();

public:
	void push(const std::function<void()>& work, const std::function<void()>& complete);
	void update();
	void quit();

private:
	void notifyWorkDone(Consumer* in_consumer);
    void run();
};

