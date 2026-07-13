#include <gui/screen3_screen/Screen3View.hpp>
#include <gui/screen3_screen/Screen3Presenter.hpp>

Screen3Presenter::Screen3Presenter(Screen3View& v)
    : view(v)
{

}

void Screen3Presenter::activate()
{
    if (model)
    {
        model->notifyScore();
    }
}

void Screen3Presenter::deactivate()
{

}

void Screen3Presenter::scoreUpdated(uint32_t currentScore, uint32_t highscore)
{
    view.updateScores(currentScore, highscore);
}
