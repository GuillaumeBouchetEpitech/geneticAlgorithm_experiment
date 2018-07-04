

#pragma once


#include <functional>


class Consumer; // <- is fine: used as pointer only


struct t_task
{
public:
    std::function<void()>  	m_work{nullptr};
    std::function<void()>  	m_oncomplete{nullptr};
    Consumer*				m_pConsumer{nullptr};

public:
	t_task() = default;
    t_task(const std::function<void()>& work, const std::function<void()>& oncomplete);
};

// typedef std::vector<t_task>         t_tasks;

