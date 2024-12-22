#include "page_main.h"
#include "imgui/imgui.h"
#include "rtlsdr.h"
#include <bits/stdc++.h>
#include "imgui_internal.h"


void init_something()
{
}

void render(int w, int h)
{
    draw_UI(w,h);
}


void quit()
{
    stop();
}