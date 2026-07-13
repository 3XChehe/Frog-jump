#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

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

    uint32_t getCurrentScore() const { return currentScore; }
    uint32_t getHighscore() const { return highscore; }

protected:
    ModelListener* modelListener;
    uint32_t currentScore;
    uint32_t highscore;
};

#endif // MODEL_HPP
