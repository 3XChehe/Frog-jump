#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/events/ClickEvent.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleClickEvent(const touchgfx::ClickEvent& event)
{
	// Handle touch gfx button click
    if (event.getType() == touchgfx::ClickEvent::RELEASED)
    {
        int16_t x = event.getX();
        int16_t y = event.getY();

        // Start button zone
        if (x >= 12 && x <= 132 && y >= 240 && y <= 320)
        {
            application().gotoScreen2ScreenNoTransition();
        }
    }

    Screen1ViewBase::handleClickEvent(event);
}
