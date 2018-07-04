

#include "Producer.hpp"

#include <algorithm>


Producer::Producer()
{
    // todo : move constructor?
    m_arr_pConsumers.push_back(new Consumer(this));
    m_arr_pConsumers.push_back(new Consumer(this));
    m_arr_pConsumers.push_back(new Consumer(this));
    // m_arr_pConsumers.push_back(new Consumer(this));

   //  while (true)
   //  {
   //   auto it = std::find(m_arr_Consumers.begin(), m_arr_Consumers.end(), [](const Consumer& c)
   //   {
   //       return !c.isAvailable();
   //   });

   //   if (it == m_arr_Consumers.end())
            // break;

   //   std::this_thread::sleep_for(std::chrono::milliseconds(1));
   //  }

    m_thread = std::thread(&Producer::run, this);
    while (m_notified)
       std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

Producer::~Producer()
{
    quit();
}

//
//

void Producer::push(const std::function<void()>& work, const std::function<void()>& complete)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_list_todo.push_back(t_task(work, complete));

    m_notified = true;
    m_cond_var.notify_one();
}

void Producer::update()
{
    while (!m_list_done.empty())
    {
        t_task* pCurrent = nullptr;

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            pCurrent = &m_list_done.front();
        }

        // must not be locked as it might push a task
        if (pCurrent->m_oncomplete)
            pCurrent->m_oncomplete();

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_list_done.pop_front();
        }
    }
}

void Producer::quit()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_list_todo.clear();
        m_done = true;

        m_notified = true;
        m_cond_var.notify_one();
    }

    if (m_thread.joinable())
        m_thread.join();
}

//
//

void    Producer::notifyWorkDone(Consumer* in_consumer)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    // find the task in the "doing" list
    auto it = std::find_if(m_list_doing.begin(), m_list_doing.end(), [&](const t_task& in_task) -> bool {
        return (in_task.m_pConsumer == in_consumer);
    });

    // move the task from "doing" to "done"
    if (it != m_list_doing.end())
    {
        m_list_done.push_back(*it);
        m_list_doing.erase(it);
    }

    // notify the producer to check the tasks again
    m_notified = true;
    m_cond_var.notify_one();
}

void    Producer::run()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_done)
    {
        while (!m_notified) // loop to avoid spurious wakeups
            m_cond_var.wait(lock);

        if (m_done)
            break;

        //
        ////

        while (!m_list_todo.empty())
        {
            // look for any available consumer
            Consumer*   pAvailable = nullptr;
            for (auto pConsumer : m_arr_pConsumers)
                if (pConsumer->isAvailable())
                {
                    pAvailable = pConsumer;
                    break;
                }

            if (!pAvailable) // no worker available
                break;

            //

            t_task& curr_task = m_list_todo.front();

            curr_task.m_pConsumer = pAvailable;
            pAvailable->execute(curr_task.m_work);

            m_list_doing.push_back(curr_task);
            m_list_todo.pop_front();
        }

        ////
        //

        m_notified = false;
    }
}


