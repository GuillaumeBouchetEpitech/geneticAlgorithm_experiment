
#pragma once

#include <functional>

namespace multithreading
{
    using WorkCallback = std::function<void()>;

    class Consumer;

    class IProducer
    {
        friend Consumer; // <= so the consumers can call _notifyWorkDone()

    public:
        virtual ~IProducer() = default;

    protected:
        virtual void _notifyWorkDone(Consumer* in_consumer) = 0;
    };

};
