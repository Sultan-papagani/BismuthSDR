@echo off
SET srcdir="C:\Users\sultan\Desktop\BismuthSDR\volk\lib"
SET PATH=%1;%PATH%
volk_test_all volk_16ic_s32f_deinterleave_real_32f

