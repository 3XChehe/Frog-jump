#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    virtual void handleTickEvent()
    {
        model.tick();
        FrontendApplicationBase::handleTickEvent();
    }

    // Change screen
    void gotoScreen1ScreenNoTransition();
    void gotoScreen2ScreenNoTransition();
    void gotoScreen3ScreenNoTransition();

private:
    // Logic change to Screen 1
    void gotoScreen1ScreenNoTransitionImpl();
    touchgfx::Callback<FrontendApplication> transitionCallback_Screen1;

    // Logic change to Screen 2
    void gotoScreen2ScreenNoTransitionImpl();
    touchgfx::Callback<FrontendApplication> transitionCallback_Screen2;

    // Logic change to Screen 3
    void gotoScreen3ScreenNoTransitionImpl();
    touchgfx::Callback<FrontendApplication> transitionCallback_Screen3;
};

#endif // FRONTENDAPPLICATION_HPP
