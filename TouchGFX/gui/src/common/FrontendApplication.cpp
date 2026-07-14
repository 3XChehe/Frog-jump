#include <gui/common/FrontendApplication.hpp>
#include <gui/common/FrontendHeap.hpp>
#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <gui/screen3_screen/Screen3View.hpp>
#include <gui/screen3_screen/Screen3Presenter.hpp>
#include <touchgfx/transitions/NoTransition.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
}

// Go to Screen 1
void FrontendApplication::gotoScreen1ScreenNoTransition()
{
    transitionCallback_Screen1 = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoScreen1ScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback_Screen1;
}

void FrontendApplication::gotoScreen1ScreenNoTransitionImpl()
{
    touchgfx::makeTransition<Screen1View, Screen1Presenter, touchgfx::NoTransition, Model>(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

// Go to Screen 2
void FrontendApplication::gotoScreen2ScreenNoTransition()
{
    transitionCallback_Screen2 = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoScreen2ScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback_Screen2;
}

void FrontendApplication::gotoScreen2ScreenNoTransitionImpl()
{
    touchgfx::makeTransition<Screen2View, Screen2Presenter, touchgfx::NoTransition, Model>(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

// Go to Screen 3
void FrontendApplication::gotoScreen3ScreenNoTransition()
{
    transitionCallback_Screen3 = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoScreen3ScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback_Screen3;
}

void FrontendApplication::gotoScreen3ScreenNoTransitionImpl()
{
    touchgfx::makeTransition<Screen3View, Screen3Presenter, touchgfx::NoTransition, Model>(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}
