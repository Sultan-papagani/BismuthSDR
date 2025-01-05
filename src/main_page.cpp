#include <imgui.h>
#include <imgui_internal.h>
#include <rtlsdr.h>
#include <bits/stdc++.h>
#include <main_page.h>
#include <rtlsdr.h>

void init_main_page()
{
    rtlsdr_configure(2400000.0, 10, 92000000.0);
    return;
}

void render_main_page(int w, int h)
{
    

    rtlsdr_render_ui(w,h);


}


void exit_main_page()
{
    rtlsdr_stop();
    return;
}