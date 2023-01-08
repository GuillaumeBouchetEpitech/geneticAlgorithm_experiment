
#include "State_Paused.hpp"

#include "StateManager.hpp"

#include "framework/helpers/GLMath.hpp"

void
State_Paused::handleEvent(const SDL_Event& event) {
  switch (event.type) {
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
  case SDL_KEYDOWN:
  case SDL_KEYUP: {
    StateManager::get()->returnToPreviousState();
    break;
  }
  }
}

void
State_Paused::update(float elapsedTime) {
  static_cast<void>(elapsedTime); // <= unused
}

void
State_Paused::visibility(bool visible) {
  static_cast<void>(visible); // <= unused
}
