

#include "t_task.hpp"


t_task::t_task(const std::function<void()>& work, const std::function<void()>& oncomplete)
    : m_work(work)
    , m_oncomplete(oncomplete)
{
}

