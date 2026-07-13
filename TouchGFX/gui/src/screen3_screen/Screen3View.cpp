#include <gui/screen3_screen/Screen3View.hpp>
#include <touchgfx/events/ClickEvent.hpp>
Screen3View::Screen3View()
{

}

void Screen3View::setupScreen()
{
    Screen3ViewBase::setupScreen();
}

void Screen3View::tearDownScreen()
{
    Screen3ViewBase::tearDownScreen();
}
void Screen3View::handleClickEvent(const touchgfx::ClickEvent& event)
{
    /* Chỉ xử lý khi nhả tay (RELEASED) để tránh trigger 2 lần */
    if (event.getType() == touchgfx::ClickEvent::RELEASED)
    {
        int16_t x = event.getX();
        int16_t y = event.getY();

        /* Vùng start_button: X=12, Y=240, W=120, H=80 → (12..132, 240..320) */
        if (x >= 32 && x <= 82 && y >= 243 && y <= 298)
        {
            application().gotoScreen2ScreenNoTransition();
        }else if(x >= 156 && x <= 206 && y >= 243 && y <= 298){
        	application().gotoScreen1ScreenNoTransition();
        }
    }

    /* Gọi base để không ảnh hưởng các widget khác */
    Screen3ViewBase::handleClickEvent(event);
}
