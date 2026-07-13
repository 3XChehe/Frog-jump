#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

enum SoundEvent
{
    SOUND_JUMP,
    SOUND_CASH,
    SOUND_CRASH,
    SOUND_SINK_WATER,
    SOUND_GAME_OVER
};

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    void addScore(uint32_t points);
    void resetCurrentScore();
    void notifyScore();
    void playSound(SoundEvent sound);

    uint32_t getCurrentScore() const { return currentScore; }
    uint32_t getHighscore() const { return highscore; }

protected:
    ModelListener* modelListener;
    uint32_t currentScore;
    uint32_t highscore;
};

#endif // MODEL_HPP
