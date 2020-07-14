
#include "Data.hpp"

void Data::initialiseSounds()
{
    sounds.soundManager.load("./assets/sfx/BazookaSound.wav");
    sounds.soundManager.load("./assets/sfx/Bomb_2.wav");
    sounds.soundManager.load("./assets/sfx/Cannon_2.wav");
    sounds.soundManager.load("./assets/sfx/Cannon_3.wav");
    sounds.soundManager.load("./assets/sfx/ExplosionSound.wav");
}
