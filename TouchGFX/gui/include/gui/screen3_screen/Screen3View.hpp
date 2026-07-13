#ifndef SCREEN3VIEW_HPP
#define SCREEN3VIEW_HPP

#include <gui_generated/screen3_screen/Screen3ViewBase.hpp>
#include <gui/screen3_screen/Screen3Presenter.hpp>

class Screen3View : public Screen3ViewBase
{
public:
    Screen3View();
    virtual ~Screen3View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleClickEvent(const touchgfx::ClickEvent& event);
    void updateScores(uint32_t currentScore, uint32_t highscore);
protected:
    touchgfx::Unicode::UnicodeChar scoreBuffer[16];
    touchgfx::Unicode::UnicodeChar highscoreBuffer[16];
};

#endif // SCREEN3VIEW_HPP
