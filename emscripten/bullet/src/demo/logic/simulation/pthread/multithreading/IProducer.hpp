
#pragma once

#include "demo/utilities/NonCopyable.hpp"

#include <functional>

namespace multithreading
{
    using WorkCallback = std::function<void()>;

    class Consumer;

    class IProducer
        : public NonCopyable
    {
        // friendship so the consumers can call _notifyWorkDone()
        friend Consumer;

    public:
        virtual ~IProducer() = default;

    protected:
        virtual void _notifyWorkDone(Consumer* in_consumer) = 0;
    };

};
