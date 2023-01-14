
#include "headers.hpp"

TEST(audio, load_ogg_file) {
  OpenALSoundManager soundManager;

  // soundManager.loadOggFromFile(666, "./tests/src/audio/assets/Cannon_3.ogg");
  soundManager.loadOggFromFile(666, "./src/audio/assets/Cannon_3.ogg");

  const glm::vec3 centerPos = {100, 100, 0};

  soundManager.setListener(centerPos, glm::vec3(1, 0, 0), glm::vec3(0, 0, 1));

  soundManager.setVolume(0.125f);

  {
    D_MYLOG("normal pitch");
    soundManager.playSound(666, centerPos, 1.0f, 1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  {
    D_MYLOG("twice higher pitch");
    soundManager.playSound(666, centerPos, 1.0f, 2.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
  }

  {
    D_MYLOG("twice lower pitch");
    soundManager.playSound(666, centerPos, 1.0f, 0.5f);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  }

  soundManager.setVolume(0.25f);

  {
    struct SoundDirection {
      std::string_view name;
      glm::vec3 dir;
    };

    std::array<SoundDirection, 10> allSoundDirections = {{
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

    for (const SoundDirection& data : allSoundDirections) {
      D_MYLOG("four times higher pitch, 3d sound " << data.name);
      soundManager.playSound(666, centerPos + data.dir, 1.0f, 4.0f);
      std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
  }

  // EXPECT_EQ(result, true);
}
