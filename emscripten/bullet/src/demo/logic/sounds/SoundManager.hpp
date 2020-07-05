
// #pragma once

// #include <map>
// #include <vector>

// class Mix_Chunk;

// class SoundManager
// {
// private:

//     struct t_soundData
//     {
//         Mix_Chunk* chunk;
//         int volume;
//     };

//     std::map<std::string, t_soundData>  _soundMap;
//     std::vector<std::string>            _soundNames;

//     int _currentAge = 0;

//     struct t_playingSoundData
//     {
//         int age;
//         int channel;
//         Mix_Chunk* chunk;
//     };

//     std::vector<t_playingSoundData> _playingSounds;
//     std::vector<int> _freeChannels;

// public:
//     SoundManager();
//     // ~SoundManager();

// public:
//     void load(const std::string& filename, int volume);
//     // void unload();
//     void play(const std::string& filename);
//     void playRandom();

// };
