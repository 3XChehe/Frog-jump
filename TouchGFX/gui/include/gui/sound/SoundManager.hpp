#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <stdint.h>

class SoundManager
{
public:
    static SoundManager& getInstance()
    {
        static SoundManager instance;
        return instance;
    }

    void playJumpSound();
    void playCashSound();
    void playCrashSound();
    void playSinkWaterSound();
    void playGameOverSound();
    void stopSound();

private:
    SoundManager() {}
    ~SoundManager() {}
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
};

#endif // SOUNDMANAGER_HPP
