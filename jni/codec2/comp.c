/*---------------------------------------------------------------------------*\

  FILE........: comp.h
  AUTHOR......: Markovtsev Vadim
  DATE CREATED: 19/11/12

  Complex number routines implementation.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2009 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include "comp.h"
#if defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <assert.h>

void init_comp_array(COMP *ptr, int size) {
    assert(size > 0);
    int i;
#if !defined(__ARM_NEON__)
    for(i=0; i<size; i++) {
        ptr[i].real = 0.0f;
        ptr[i].imag = 0.0f;
    }
#else
    assert(size % 2 == 0 && "size should be even");
    const float32x4_t zerovec = { 0.0f, 0.0f, 0.0f, 0.0f };
    for(i=0; i<size; i+=2) {
        vst1q_f32((float *)&ptr[i], zerovec);
    }
#endif
}
