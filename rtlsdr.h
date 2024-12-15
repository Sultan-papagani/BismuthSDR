#pragma once

int configure_rtlsdr(int sample_rate, int gain_index, double frequency);
void draw_UI(int w, int h);
void stop();
void calculateSmoothing();