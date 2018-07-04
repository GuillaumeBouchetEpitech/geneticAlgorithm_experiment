

#include "Consumer.hpp"

#include "Producer.hpp"


Consumer::Consumer(Producer* pProducer)
	: m_pProducer(pProducer)
{
    m_thread = std::thread(&Consumer::run, this);
    while (m_notified)
       std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

Consumer::~Consumer()
{
	quit();
}

//
//

void Consumer::execute(const std::function<void()>& work)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_pWork = work;

    m_notified = true;
    m_cond_var.notify_one();
}

void Consumer::quit()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_done = true;
	    m_notified = true;
	    m_cond_var.notify_one();
    }

    if (m_thread.joinable())
        m_thread.join();
}

//
//

void	Consumer::run()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_done)
    {
        m_notified = false;

        while (!m_notified) // loop to avoid spurious wakeups
            m_cond_var.wait(lock);

        if (m_done)
            break;

        //
        //

        if (m_pWork)
        	m_pWork();

        if (m_pProducer)
	        m_pProducer->notifyWorkDone(this);
    }
}

