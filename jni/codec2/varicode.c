//==========================================================================
// Name:            varicode.h
// Purpose:         Varicode encoded and decode functions
// Created:         Nov 24, 2012
// Authors:         David Rowe
//
// To test:
//          $ gcc varicode.c -o varicode -DVARICODE_UNITTEST -Wall
//          $ ./varicode
// 
// License:
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.1,
//  as published by the Free Software Foundation.  This program is
//  distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//==========================================================================

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "varicode.h"
#include "varicode_table.h"


/*
  output is an unpacked array of bits of maximum size max_out.  Note
  unpacked arrays are a more suitable form for modulator input.
*/

int varicode_encode(short varicode_out[], char ascii_in[], int max_out, int n_in) {
    int            n_out, index, n_zeros, v_len;
    unsigned short byte1, byte2, packed;

    n_out = 0;

    while(n_in && (n_out < max_out)) {

        assert((unsigned int)(*ascii_in) < 128);

        index = 2*(unsigned int)(*ascii_in);
        byte1 = varicode_table[index];
        byte2 = varicode_table[index+1];
        packed = (byte1 << 8) + byte2;
        //printf("n_in: %d ascii_in: %c index: %d packed 0x%x\n", n_in, *ascii_in, index, packed);
        ascii_in++;

        n_zeros = 0;
        v_len = 0;
        while ((n_zeros < 2) && (n_out < max_out) && (v_len <= VARICODE_MAX_BITS)) {
            if (packed & 0x8000) {
                *varicode_out = 1;
                n_zeros = 0;
            }
            else {
                *varicode_out = 0;
                n_zeros++;
            }
            //printf("packed: 0x%x *varicode_out: %d n_zeros: %d v_len: %d\n", packed, *varicode_out, n_zeros,v_len );
            packed <<= 1;
            varicode_out++;

            n_out++;
            v_len++;
        }
        assert(v_len <= VARICODE_MAX_BITS);

        n_in--;
            
    }

    return n_out;
}

void varicode_decode_init(struct VARICODE_DEC *dec_states)
{
    dec_states->state = 0;
    dec_states->n_zeros = 0;
    dec_states->v_len = 0;
    dec_states->packed = 0;
}

static int decode_one_bit(struct VARICODE_DEC *s, char *single_ascii, short varicode_in)
{
    int            found, i;
    unsigned short byte1, byte2;

    //printf("decode_one_bit : state: %d varicode_in: %d packed: 0x%x n_zeros: %d\n",
    //       s->state, varicode_in, s->packed, s->n_zeros);

    if (s->state == 0) {
        if (!varicode_in)
            return 0;
        else 
            s->state = 1;
    }

    if (s->state == 1) {
        if (varicode_in) {
            s->packed |= (0x8000 >> s->v_len);
            s->n_zeros = 0;
        }
        else {
            s->n_zeros++;
        }
        s->v_len++;

        found = 0;

        /* end of character code */

        if (s->n_zeros == 2) {
            if (s->v_len) {
                byte1 = s->packed >> 8;
                byte2 = s->packed & 0xff;

                /* run thru table but note with bit errors means we might
                   not actually find a match */

                for(i=0; i<128; i++) {
                    if ((byte1 == varicode_table[2*i]) && (byte2 == varicode_table[2*i+1])) {
                        found = 1;
                        *single_ascii = i;
                    }
                }
            }
            varicode_decode_init(s);
        }

        /* code can run too long if we have a bit error */

        if (s->v_len > VARICODE_MAX_BITS)
            varicode_decode_init(s);
    }

    return found;
}

int varicode_decode(struct VARICODE_DEC *dec_states, char ascii_out[], short varicode_in[], int max_out, int n_in) {
    int            output, n_out;
    char           single_ascii;

    n_out = 0;

    //printf("varicode_decode: n_in: %d\n", n_in);

    while(n_in && (n_out < max_out)) {
        output = decode_one_bit(dec_states, &single_ascii, *varicode_in);
        varicode_in++;
        n_in--;
        if (output) {
            *ascii_out++ = single_ascii;
            n_out++;
        }            
    }

    return n_out;
}


#ifdef VARICODE_UNITTEST
int main(void) {
    char *ascii_in;
    short *varicode;
    int  i, n_varicode_bits_out, n_ascii_chars_out, length, half;
    char *ascii_out;
    struct VARICODE_DEC dec_states;

    length = sizeof(varicode_table)/2;

    ascii_in = (char*)malloc(length);
    varicode = (short*)malloc(VARICODE_MAX_BITS*sizeof(short)*length);
    ascii_out = (char*)malloc(length);
    
    // 1. test all Varicode codes -------------------------------------------------------------

    for(i=0; i<length; i++)
        ascii_in[i] = (char)i;
    n_varicode_bits_out = varicode_encode(varicode, ascii_in, VARICODE_MAX_BITS*length, length);

    //printf("n_varicode_bits_out: %d\n", n_varicode_bits_out);

    // split decode in half to test how it preseves state between calls 

    varicode_decode_init(&dec_states);
    half = n_varicode_bits_out/2;
    n_ascii_chars_out  = varicode_decode(&dec_states, ascii_out, varicode, length, half);
    //printf("n_ascii_chars_out: %d\n", n_ascii_chars_out);

    n_ascii_chars_out += varicode_decode(&dec_states, &ascii_out[n_ascii_chars_out], 
                                         &varicode[half], length-n_ascii_chars_out, n_varicode_bits_out - half);
    //printf("n_ascii_chars_out: %d\n", n_ascii_chars_out);

    assert(n_ascii_chars_out == length);

    //for(i=0; i<n_varicode_bits_out; i++) {
    //    printf("%d \n", varicode[i]);
    //}

    //printf("ascii_out: %s\n", ascii_out);

    if (memcmp(ascii_in, ascii_out, length) == 0)
        printf("Test 1 Pass\n");
    else
        printf("Test 1 Fail\n");

    // 2. Test some ascii with a run of zeros -----------------------------------------------------

    sprintf(ascii_in, "CQ CQ CQ, this is VK5DGR");
    assert(strlen(ascii_in) < length);

    for(i=0; i<3; i++) {
        n_varicode_bits_out = varicode_encode(varicode, ascii_in, VARICODE_MAX_BITS*length, strlen(ascii_in));
        n_ascii_chars_out   = varicode_decode(&dec_states, ascii_out, varicode, length, n_varicode_bits_out);
        ascii_out[n_ascii_chars_out] = 0;

        printf("ascii_out: %s\n", ascii_out);
        if (strcmp(ascii_in, ascii_out) == 0)
            printf("Test 2 Pass\n");
        else
            printf("Test 2 Fail\n");

        memset(varicode, 0, sizeof(short)*20);
        n_ascii_chars_out = varicode_decode(&dec_states, ascii_out, varicode, length, 20);
        assert(n_ascii_chars_out == 0);
    }

    free(ascii_in);
    free(ascii_out);
    free(varicode);

    return 0;
}
#endif
