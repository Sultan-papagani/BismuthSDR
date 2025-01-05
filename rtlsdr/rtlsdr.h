#pragma once

int rtlsdr_configure(int sample_rate, int gain_index, double frequency);
void rtlsdr_render_ui(int w, int h);
void rtlsdr_stop();
void calculateSmoothing();