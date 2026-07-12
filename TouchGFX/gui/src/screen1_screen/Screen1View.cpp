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
    /* Chỉ xử lý khi nhả tay (RELEASED) để tránh trigger 2 lần */
    if (event.getType() == touchgfx::ClickEvent::RELEASED)
    {
        int16_t x = event.getX();
        int16_t y = event.getY();

        /* Vùng start_button: X=12, Y=240, W=120, H=80 → (12..132, 240..320) */
        if (x >= 12 && x <= 132 && y >= 240 && y <= 320)
        {
            application().gotoScreen2ScreenNoTransition();
        }
    }

    /* Gọi base để không ảnh hưởng các widget khác */
    Screen1ViewBase::handleClickEvent(event);
}
