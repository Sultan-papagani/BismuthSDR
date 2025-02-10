@echo off
SET srcdir="C:\Users\sultan\Desktop\BismuthSDR\volk\lib"
SET PATH=%1;%PATH%
volk_test_all volk_8ic_x2_s32f_multiply_conjugate_32fc

