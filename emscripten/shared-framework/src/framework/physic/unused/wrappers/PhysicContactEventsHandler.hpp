
#include "../PhysicDefinitions.hpp"

#include <functional>

using ContactCallback = std::function<void(ContactEvent, PhysicContactData*)>;

namespace PhysicContactEventsHandler {
void initialise(ContactCallback callback);
};
