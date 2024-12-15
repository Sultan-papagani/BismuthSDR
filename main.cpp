#include "UI.h"
#include "rtlsdr.h"

int main(int, char**)
{
    configure_rtlsdr(2400000, 12, 92000000);
    enter_ui_loop();
    stop();
}