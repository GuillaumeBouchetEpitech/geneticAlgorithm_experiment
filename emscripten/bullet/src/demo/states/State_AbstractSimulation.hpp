
#pragma once

#include "IState.hpp"

class State_AbstractSimulation : public IState {
public:
  virtual void enter() override;
  virtual void leave() override;

public:
  virtual void handleEvent(const SDL_Event&) override;
  virtual void update(float) override;
  virtual void render(const SDL_Window&) override;
  virtual void resize(int width, int height) override;
  virtual void visibility(bool visible) override;

protected:
  void _updateCommonLogic(float elapsedTime);
  void _updateCameraTracking(float elapsedTime);
};
