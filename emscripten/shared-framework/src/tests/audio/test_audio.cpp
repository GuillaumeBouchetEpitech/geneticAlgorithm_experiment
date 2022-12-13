

#include "framework/audio/OpenALSoundManager.hpp"

#include "framework/TraceLogger.hpp"

#include <thread>

void test_audio() {
  D_MYLOG(" => test_audio()");

  OpenALSoundManager soundManager;

  soundManager.loadOggFromFile(666, "./src/tests/audio/assets/Cannon_3.ogg");

  const glm::vec3 centerPos = {100, 100, 0};

  soundManager.setListener(centerPos, glm::vec3(1, 0, 0), glm::vec3(0, 0, 1));

  soundManager.setVolume(0.125f);

  {
    D_MYLOG("normal");
    soundManager.playSound(666, centerPos, 1.0f, 1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  {
    D_MYLOG("higher pitch");
    soundManager.playSound(666, centerPos, 1.0f, 2.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
  }

  {
    D_MYLOG("lower pitch");
    soundManager.playSound(666, centerPos, 1.0f, 0.5f);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  }

  soundManager.setVolume(0.25f);

  {
    struct SoundDir {
      std::string_view name;
      glm::vec3 dir;
    };

    std::array<SoundDir, 10> allSoundDirs = {{
      {"left-back", glm::vec3(-7, +7, 0)},
      {"left", glm::vec3(0, +10, 0)},
      {"left-front", glm::vec3(+7, +7, 0)},
      {"front", glm::vec3(+10, 0, 0)},
      {"right-front", glm::vec3(+7, -7, 0)},
      {"right", glm::vec3(0, -10, 0)},
      {"right-back", glm::vec3(-7, -7, 0)},
      {"back", glm::vec3(-10, 0, 0)},
      {"top", glm::vec3(0, 0, +10)},
      {"down", glm::vec3(0, 0, -10)},
    }};

    for (const SoundDir& data : allSoundDirs) {
      D_MYLOG("normal " << data.name);
      soundManager.playSound(666, centerPos + data.dir, 1.0f, 4.0f);
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  D_MYLOG(" => DONE");
}
