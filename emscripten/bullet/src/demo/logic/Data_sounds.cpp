
#include "Data.hpp"

void Data::initialiseSounds()
{
    sounds.manager.load("./assets/sfx/BazookaSound.wav");
    sounds.manager.load("./assets/sfx/Bomb_2.wav");
    sounds.manager.load("./assets/sfx/Cannon_2.wav");
    sounds.manager.load("./assets/sfx/Cannon_3.wav");
    sounds.manager.load("./assets/sfx/ExplosionSound.wav");
}
