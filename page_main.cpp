#include "page_main.h"
#include "imgui/imgui.h"
#include "rtlsdr.h"
#include <bits/stdc++.h>

void render(int w, int h)
{
    ImGui::Begin("Main", NULL, WINDOW_FLAGS);
    draw_UI(w, h);
    ImGui::End();
}

void quit()
{
    stop();
}