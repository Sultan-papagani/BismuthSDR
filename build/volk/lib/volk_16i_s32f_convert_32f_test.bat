@echo off
SET srcdir="C:\Users\sultan\Desktop\BismuthSDR\volk\lib"
SET PATH=%1;%PATH%
volk_test_all volk_16i_s32f_convert_32f

