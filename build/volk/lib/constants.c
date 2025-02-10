/* -*- c++ -*- */
/*
 * Copyright 2013, 2014 Free Software Foundation, Inc.
 *
 * This file is part of VOLK
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <volk/constants.h>

const char*
volk_prefix()
{
  const char *prefix = getenv("VOLK_PREFIX");
  if (prefix != NULL) return prefix;
  return "";
}

const char*
volk_version()
{
  return "3.1.2";
}

const char*
volk_c_compiler()
{
  return "gcc.exe (x86_64-win32-seh-rev0, Built by MinGW-Builds project) 14.2.0 \nCopyright (C) 2024 Free Software Foundation, Inc. \nThis is free software see the source for copying conditions.  There is NO \nwarranty not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";
}

const char*
volk_compiler_flags()
{
  return "C:/mingw64/bin/gcc.exe:::  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign \nC:/mingw64/bin/g++.exe::: -g -fcx-limited-range -Wall \ngeneric:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign  \nsse2_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 \nsse3_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 \nssse3_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 -mssse3 \nsse4_a_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 -msse4a -mpopcnt \nsse4_1_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 \nsse4_2_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt \navx_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt -mavx \navx2_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt -mavx -mfma -mavx2 \navx512f_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt -mavx -mfma -mavx2 -mavx512f \navx512cd_64_mmx:::GNU:::-O3 -DNDEBUG  -fcx-limited-range -Wall -Werror=incompatible-pointer-types -Werror=pointer-sign -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt -mavx -mfma -mavx2 -mavx512f -mavx512cd";
}

const char*
volk_available_machines()
{
  return "generic;sse2_64_mmx;sse3_64_mmx;ssse3_64_mmx;sse4_a_64_mmx;sse4_1_64_mmx;sse4_2_64_mmx;avx_64_mmx;avx2_64_mmx;avx512f_64_mmx;avx512cd_64_mmx";
}
