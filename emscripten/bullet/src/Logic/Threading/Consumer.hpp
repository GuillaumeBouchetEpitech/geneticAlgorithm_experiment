

#pragma once


#include <functional>

#include <thread>
#include <mutex>
#include <condition_variable>


class Producer; // <- is fine: used as pointer only


class Consumer
{
private:
    std::thread             m_thread;
    std::mutex              m_mutex;
    std::condition_variable m_cond_var;

    bool m_done{false};
    bool m_notified{true}; // <- is true so isAvailable() return false until the thread is running

    std::function<void()>	m_pWork;

    Producer*	m_pProducer{nullptr};

public:
	Consumer(Producer* pProducer);
	~Consumer();

public:
	void execute(const std::function<void()>& work);
	void quit();

public:
	inline bool	isAvailable() const { return !m_notified; }

private:
    void run();
};

