#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <stdint.h>
#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void onCommandReceived(uint16_t cmd) {}
    virtual void scoreUpdated(uint32_t currentScore, uint32_t highscore) {}
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
