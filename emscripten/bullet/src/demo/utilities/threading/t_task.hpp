
#pragma once

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include <functional>

struct t_task
{
public:
    typedef std::function<void()> t_work;

public:
    t_work _work = nullptr;
    t_work _oncomplete = nullptr;
    class Consumer* _consumer = nullptr;

public:
    t_task() = default;
    t_task(const t_work& work);
    t_task(const t_work& work, const t_work& oncomplete);
};
