
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#	error "exclude this file to build natively or with multi thread support"
#endif

#include "t_task.hpp"

t_task::t_task(const t_work& work)
    : _work(work)
{
}

t_task::t_task(const t_work& work, const t_work& oncomplete)
    : _work(work)
    , _oncomplete(oncomplete)
{
}

