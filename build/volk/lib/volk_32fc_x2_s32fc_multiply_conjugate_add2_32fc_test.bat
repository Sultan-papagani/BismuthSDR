@echo off
SET srcdir="C:\Users\sultan\Desktop\BismuthSDR\volk\lib"
SET PATH=%1;%PATH%
volk_test_all volk_32fc_x2_s32fc_multiply_conjugate_add2_32fc

