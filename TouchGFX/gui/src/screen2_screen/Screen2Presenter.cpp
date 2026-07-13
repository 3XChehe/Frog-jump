#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

Screen2Presenter::Screen2Presenter(Screen2View& v)
    : view(v)
{

}

void Screen2Presenter::activate()
{
    if (model)
    {
        model->resetCurrentScore();
    }
}

void Screen2Presenter::deactivate()
{

}

void Screen2Presenter::onCommandReceived(uint16_t cmd)
{
    view.moveCat(cmd);
}

void Screen2Presenter::addScore(uint32_t points)
{
    if (model)
    {
        model->addScore(points);
    }
}

void Screen2Presenter::notifyScore()
{
    if (model)
    {
        model->notifyScore();
    }
}

void Screen2Presenter::playSound(SoundEvent sound)
{
    if (model)
    {
        model->playSound(sound);
    }
}

void Screen2Presenter::scoreUpdated(uint32_t currentScore, uint32_t highscore)
{
    view.updateScore(currentScore);
}
