
// #include "SoundManager.hpp"

// #include "demo/utilities/ErrorHandler.hpp"

// #include "demo/utilities/math/RandomNumberGenerator.hpp"

// #include <SDL2/SDL_mixer.h> // <= Mix_LoadWAV/PlayChannel/FreeChunk

// #include <algorithm>

// // #include <AL/al.h>
// // #include <AL/alc.h>



// // // #include <numeric>
// // #include <fstream>

// // std::int32_t convert_to_int(char* buffer, std::size_t len)
// // {
// //     std::int32_t a = 0;
// //     // if (std::endian::native == std::endian::little)
// //         std::memcpy(&a, buffer, len);
// //     // else
// //     //     for (std::size_t i = 0; i < len; ++i)
// //     //         reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
// //     return a;
// // }

// // bool load_wav_file_header(std::ifstream& file,
// //                           std::uint8_t& channels,
// //                           std::int32_t& sampleRate,
// //                           std::uint8_t& bitsPerSample,
// //                           ALsizei& size)
// // {
// //     char buffer[4];
// //     if (!file.is_open())
// //         return false;

// //     // the RIFF
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read RIFF");
// //     if (std::strncmp(buffer, "RIFF", 4) != 0)
// //         D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)");

// //     // the size of the file
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read size of file");

// //     // the WAVE
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read WAVE");
// //     if (std::strncmp(buffer, "WAVE", 4) != 0)
// //         D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)");

// //     // "fmt/0"
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read fmt/0");

// //     // this is always 16, the size of the fmt data chunk
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read the 16");

// //     // PCM should be 1?
// //     if (!file.read(buffer, 2))
// //         D_THROW(std::runtime_error, "ERROR: could not read PCM");

// //     // the number of channels
// //     if (!file.read(buffer, 2))
// //         D_THROW(std::runtime_error, "ERROR: could not read number of channels");
// //     channels = convert_to_int(buffer, 2);

// //     // sample rate
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read sample rate");
// //     sampleRate = convert_to_int(buffer, 4);

// //     // (sampleRate * bitsPerSample * channels) / 8
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8");

// //     // ?? dafaq
// //     if (!file.read(buffer, 2))
// //         D_THROW(std::runtime_error, "ERROR: could not read dafaq");

// //     // bitsPerSample
// //     if (!file.read(buffer, 2))
// //         D_THROW(std::runtime_error, "ERROR: could not read bits per sample");
// //     bitsPerSample = convert_to_int(buffer, 2);

// //     // data chunk header "data"
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read data chunk header");
// //     if (std::strncmp(buffer, "data", 4) != 0)
// //         D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)");

// //     // size of data
// //     if (!file.read(buffer, 4))
// //         D_THROW(std::runtime_error, "ERROR: could not read data size");
// //     size = convert_to_int(buffer, 4);

// //     /* cannot be at the end of file */
// //     if (file.eof())
// //         D_THROW(std::runtime_error, "ERROR: reached EOF on the file");
// //     if (file.fail())
// //         D_THROW(std::runtime_error, "ERROR: fail state set on the file");

// //     return true;
// // }

// // char* load_wav(const std::string& filename,
// //                std::uint8_t& channels,
// //                std::int32_t& sampleRate,
// //                std::uint8_t& bitsPerSample,
// //                ALsizei& size)
// // {
// //     std::ifstream in(filename, std::ios::binary);
// //     if (!in.is_open())
// //         D_THROW(std::runtime_error, "ERROR: Could not open \"" << filename << "\"");
// //     if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
// //         D_THROW(std::runtime_error, "ERROR: Could not load wav header of \"" << filename << "\"");

// //     char* data = new char[size];

// //     in.read(data, size);

// //     return data;
// // }



// // ALCdevice* device;
// // ALCcontext* context;
// // ALuint buffer;
// // ALuint source;

// // SoundManager::SoundManager()
// // {
// //     device = alcOpenDevice(NULL);
// //     if (!device)
// //         D_THROW(std::runtime_error, "Fail to open the OpenAL device");

// //     context = alcCreateContext(device, NULL);
// //     if (!alcMakeContextCurrent(context))
// //         D_THROW(std::runtime_error, "Fail to create the OpenAL context");

// //     ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
// //     alListener3f(AL_POSITION, 0, 0, 1.0f);
// //     // check for errors
// //     alListener3f(AL_VELOCITY, 0, 0, 0);
// //     // check for errors
// //     alListenerfv(AL_ORIENTATION, listenerOri);

// //     alGenBuffers((ALuint)1, &buffer);
// //     // check for errors

// //     std::uint8_t channels;
// //     std::int32_t sampleRate;
// //     std::uint8_t bitsPerSample;
// //     ALsizei size;
// //     char* soundData = load_wav("./assets/sfx/BazookaSound.wav", channels, sampleRate, bitsPerSample, size);

// //     ALenum format;
// //     if (channels == 1 && bitsPerSample == 8)
// //         format = AL_FORMAT_MONO8;
// //     else if (channels == 1 && bitsPerSample == 16)
// //         format = AL_FORMAT_MONO16;
// //     else if (channels == 2 && bitsPerSample == 8)
// //         format = AL_FORMAT_STEREO8;
// //     else if (channels == 2 && bitsPerSample == 16)
// //         format = AL_FORMAT_STEREO16;
// //     else
// //         D_THROW(std::runtime_error, "ERROR: unrecognised wave format: " << channels << " channels, " << bitsPerSample << " bps");


// //     alBufferData(buffer, format, soundData, size, sampleRate);
// //     // check for errors




// //     alGenSources((ALuint)1, &source);
// //     // check for errors
// //     alSourcef(source, AL_PITCH, 1);
// //     // check for errors
// //     alSourcef(source, AL_GAIN, 1);
// //     // check for errors
// //     alSource3f(source, AL_POSITION, 0, 0, 0);
// //     // check for errors
// //     alSource3f(source, AL_VELOCITY, 0, 0, 0);
// //     // check for errors
// //     alSourcei(source, AL_LOOPING, AL_FALSE);
// //     // check for errros

// //     alSourcei(source, AL_BUFFER, buffer);



// //     alSourcePlay(source);
// // }

// // SoundManager::~SoundManager()
// // {
// //     alcCloseDevice(device);
// // }

// SoundManager::SoundManager()
// {
//     const int totalChannels = 16;

//     for (int ii = 0; ii < totalChannels; ++ii)
//         _freeChannels.push_back(ii);

//     Mix_AllocateChannels(totalChannels);
// }

// //

// void SoundManager::load(const std::string& filename, int volume)
// {
//     if (_soundMap.count(filename) > 0)
//         D_THROW(std::runtime_error, "Already loaded sound"
//                 << ", filename: \"" << filename << "\"");

//     Mix_Chunk* newChunk;

//     if (!(newChunk = Mix_LoadWAV(filename.c_str())))
//         D_THROW(std::runtime_error, "Could not load a sound"
//                 << ", filename: \"" << filename << "\""
//                 << ", error: " << Mix_GetError());

//     _soundMap[filename] = { newChunk, volume };
//     _soundNames.push_back(filename);
// }

// void SoundManager::play(const std::string& filename)
// {
//     auto it = _soundMap.find(filename);
//     if (it == _soundMap.end())
//         D_THROW(std::runtime_error, "Sound not found"
//                 << ", filename: \"" << filename << "\"");

//     auto soundData = it->second;
//     auto* chunk = soundData.chunk;

//     // find free channel
//     if (_freeChannels.empty())
//     {
//         // find oldest sound

//         auto cmpFunc = [](const t_playingSoundData& dataA, const t_playingSoundData& dataB) { return dataA.age > dataB.age; };
//         std::sort(_playingSounds.begin(), _playingSounds.end(), cmpFunc);

//         _freeChannels.push_back(_playingSounds.back().channel);
//         _playingSounds.pop_back();
//     }

//     int channel = _freeChannels.back();
//     _freeChannels.pop_back();

//     _playingSounds.push_back({ ++_currentAge, channel, chunk });

//     // D_MYLOG("channel=" << channel);

//     if (Mix_Playing(channel))
//         Mix_HaltChannel(channel);

//     if (Mix_PlayChannel(channel, chunk, 0) == -1)
//         D_THROW(std::runtime_error, "Could not play a sound"
//             << ", filename: \"" << filename << "\""
//             << ", error: " << Mix_GetError());

//     Mix_Volume(channel, soundData.volume);
//     // // Mix_SetDistance
//     // Mix_SetPanning(channel, 255, 255 - 255);

// }

// void SoundManager::playRandom()
// {
//     int soundIndex = t_RNG::getRangedValue(0, _soundNames.size());

//     play(_soundNames.at(soundIndex));
// }

