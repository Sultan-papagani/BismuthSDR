/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of VOLK
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

/*!
 * \page volk_8u_x3_encodepolar_8u_x2
 *
 * \b Overview
 *
 * encode given data for POLAR code
 *
 * <b>Dispatcher Prototype</b>
 * \code
 * void volk_8u_x3_encodepolar_8u_x2(unsigned char* frame, const unsigned char*
 * frozen_bit_mask, const unsigned char* frozen_bits, const unsigned char* info_bits,
 * unsigned int frame_size, unsigned int info_bit_size) \endcode
 *
 * \b Inputs
 * \li frame: buffer for encoded frame
 * \li frozen_bit_mask: bytes with 0xFF for frozen bit positions or 0x00 otherwise.
 * \li frozen_bits: values of frozen bits, 1 bit per byte
 * \li info_bits: info bit values, 1 bit per byte
 * \li frame_size: power of 2 value for frame size.
 * \li info_bit_size: number of info bits in a frame
 *
 * \b Outputs
 * \li frame: polar encoded frame.
 *
 * \b Example
 * \code
 * int frame_exp = 10;
 * int frame_size = 0x01 << frame_exp;
 * int num_info_bits = frame_size;
 * int num_frozen_bits = frame_size - num_info_bits;
 *
 * // function sets frozenbit positions to 0xff and all others to 0x00.
 * unsigned char* frozen_bit_mask = get_frozen_bit_mask(frame_size, num_frozen_bits);
 *
 * // set elements to desired values. Typically all zero.
 * unsigned char* frozen_bits = (unsigned char) volk_malloc(sizeof(unsigned char) *
 * num_frozen_bits, volk_get_alignment());
 *
 * unsigned char* frame = (unsigned char) volk_malloc(sizeof(unsigned char) * frame_size,
 * volk_get_alignment()); unsigned char* temp = (unsigned char)
 * volk_malloc(sizeof(unsigned char) * frame_size, volk_get_alignment());
 *
 * unsigned char* info_bits = get_info_bits_to_encode(num_info_bits);
 *
 * volk_8u_x3_encodepolar_8u_x2(
 *     frame, temp, frozen_bit_mask, frozen_bits, info_bits, frame_size);
 *
 * volk_free(frozen_bit_mask);
 * volk_free(frozen_bits);
 * volk_free(frame);
 * volk_free(temp);
 * volk_free(info_bits);
 * \endcode
 */

#ifndef VOLK_KERNELS_VOLK_VOLK_8U_X3_ENCODEPOLAR_8U_X2_U_H_
#define VOLK_KERNELS_VOLK_VOLK_8U_X3_ENCODEPOLAR_8U_X2_U_H_
#include <stdio.h>
#include <volk/volk_8u_x2_encodeframepolar_8u.h>

static inline void interleave_frozen_and_info_bits(unsigned char* target,
                                                   const unsigned char* frozen_bit_mask,
                                                   const unsigned char* frozen_bits,
                                                   const unsigned char* info_bits,
                                                   const unsigned int frame_size)
{
    unsigned int bit;
    for (bit = 0; bit < frame_size; ++bit) {
        *target++ = *frozen_bit_mask++ ? *frozen_bits++ : *info_bits++;
    }
}

#ifdef LV_HAVE_GENERIC

static inline void
volk_8u_x3_encodepolar_8u_x2_generic(unsigned char* frame,
                                     unsigned char* temp,
                                     const unsigned char* frozen_bit_mask,
                                     const unsigned char* frozen_bits,
                                     const unsigned char* info_bits,
                                     unsigned int frame_size)
{
    // interleave
    interleave_frozen_and_info_bits(
        temp, frozen_bit_mask, frozen_bits, info_bits, frame_size);
    volk_8u_x2_encodeframepolar_8u_generic(frame, temp, frame_size);
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSSE3
#include <tmmintrin.h>

static inline void
volk_8u_x3_encodepolar_8u_x2_u_ssse3(unsigned char* frame,
                                     unsigned char* temp,
                                     const unsigned char* frozen_bit_mask,
                                     const unsigned char* frozen_bits,
                                     const unsigned char* info_bits,
                                     unsigned int frame_size)
{
    // interleave
    interleave_frozen_and_info_bits(
        temp, frozen_bit_mask, frozen_bits, info_bits, frame_size);
    volk_8u_x2_encodeframepolar_8u_u_ssse3(frame, temp, frame_size);
}

#endif /* LV_HAVE_SSSE3 */

#ifdef LV_HAVE_AVX2
#include <immintrin.h>
static inline void
volk_8u_x3_encodepolar_8u_x2_u_avx2(unsigned char* frame,
                                    unsigned char* temp,
                                    const unsigned char* frozen_bit_mask,
                                    const unsigned char* frozen_bits,
                                    const unsigned char* info_bits,
                                    unsigned int frame_size)
{
    interleave_frozen_and_info_bits(
        temp, frozen_bit_mask, frozen_bits, info_bits, frame_size);
    volk_8u_x2_encodeframepolar_8u_u_avx2(frame, temp, frame_size);
}
#endif /* LV_HAVE_AVX2 */

#endif /* VOLK_KERNELS_VOLK_VOLK_8U_X3_ENCODEPOLAR_8U_X2_U_H_ */

#ifndef VOLK_KERNELS_VOLK_VOLK_8U_X3_ENCODEPOLAR_8U_X2_A_H_
#define VOLK_KERNELS_VOLK_VOLK_8U_X3_ENCODEPOLAR_8U_X2_A_H_

#ifdef LV_HAVE_SSSE3
#include <tmmintrin.h>
static inline void
volk_8u_x3_encodepolar_8u_x2_a_ssse3(unsigned char* frame,
                                     unsigned char* temp,
                                     const unsigned char* frozen_bit_mask,
                                     const unsigned char* frozen_bits,
                                     const unsigned char* info_bits,
                                     unsigned int frame_size)
{
    interleave_frozen_and_info_bits(
        temp, frozen_bit_mask, frozen_bits, info_bits, frame_size);
    volk_8u_x2_encodeframepolar_8u_a_ssse3(frame, temp, frame_size);
}
#endif /* LV_HAVE_SSSE3 */

#ifdef LV_HAVE_AVX2
#include <immintrin.h>
static inline void
volk_8u_x3_encodepolar_8u_x2_a_avx2(unsigned char* frame,
                                    unsigned char* temp,
                                    const unsigned char* frozen_bit_mask,
                                    const unsigned char* frozen_bits,
                                    const unsigned char* info_bits,
                                    unsigned int frame_size)
{
    interleave_frozen_and_info_bits(
        temp, frozen_bit_mask, frozen_bits, info_bits, frame_size);
    volk_8u_x2_encodeframepolar_8u_a_avx2(frame, temp, frame_size);
}
#endif /* LV_HAVE_AVX2 */

#ifdef LV_HAVE_RVV
static inline void volk_8u_x3_encodepolar_8u_x2_rvv(unsigned char* frame,
                                                    unsigned char* temp,
                                                    const unsigned char* frozen_bit_mask,
                                                    const unsigned char* frozen_bits,
                                                    const unsigned char* info_bits,
                                                    unsigned int frame_size)
{
    interleave_frozen_and_info_bits(
        temp, frozen_bit_mask, frozen_bits, info_bits, frame_size);
    volk_8u_x2_encodeframepolar_8u_rvv(frame, temp, frame_size);
}
#endif /* LV_HAVE_RVV */

#ifdef LV_HAVE_RVVSEG
static inline void
volk_8u_x3_encodepolar_8u_x2_rvvseg(unsigned char* frame,
                                    unsigned char* temp,
                                    const unsigned char* frozen_bit_mask,
                                    const unsigned char* frozen_bits,
                                    const unsigned char* info_bits,
                                    unsigned int frame_size)
{
    interleave_frozen_and_info_bits(
        temp, frozen_bit_mask, frozen_bits, info_bits, frame_size);
    volk_8u_x2_encodeframepolar_8u_rvvseg(frame, temp, frame_size);
}
#endif /* LV_HAVE_RVVSEG */

#endif /* VOLK_KERNELS_VOLK_VOLK_8U_X3_ENCODEPOLAR_8U_X2_A_H_ */
