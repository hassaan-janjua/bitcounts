/*
 ============================================================================
 Name        : bits.c
 Author      : HJ
 Version     :
 Copyright   : no strings attached
 Description : compare bit count techniques
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#define LUTC4(v) (((lutc >> (((v)>>3) & 0x1E)) & 3) + !!(((v)>>3) & 0x1E) + ((lutc >> (((v)<<1) & 0x1E)) & 3) + !!(((v)<<1) & 0x1E))


uint8_t basic_count_loop(uint8_t n);
uint8_t basic_count_no_loop(uint8_t n);

int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

uint8_t lut8[256];
uint8_t lut4[16];
uint32_t lutc;


uint8_t lutd[256];

volatile uint32_t val;


void init_lutc_4bit() {
	uint16_t i;
	lutc = 0;
	for (i=1; i < 16; i++) {
		lutc |= (basic_count_loop(i)-1) << (i<<1);
	}
}

void init_lut_4bit() {
	uint16_t i;
	for (i=0; i < 16; i++) {
		lut4[i] = basic_count_loop(i);
	}
}


void init_lut_8bit() {
	uint16_t i;
	for (i=0; i < 256; i++) {
		lut8[i] = basic_count_loop(i);
	}
}


uint8_t logn_count(uint8_t n) {
	n = (n&0x55) + ((n>>1) & 0x55);
	n = (n&0x33) + ((n>>2) & 0x33);
	return (n&0xF) + ((n>>4) & 0xF);
}


uint8_t logn_count_4byte(uint32_t n) {
	n = (n&0x55555555) + ((n>>1) & 0x55555555);
	n = (n&0x33333333) + ((n>>2) & 0x33333333);
	n = (n&0x0F0F0F0F) + ((n>>4) & 0x0F0F0F0F);
	n = (n&0x00FF00FF) + ((n>>8) & 0x00FF00FF);
	return (n&0x0000FFFF) + ((n>>16) & 0x0000FFFF);
}



uint8_t basic_count_loop(uint8_t n) {
	uint8_t c = 0;
	while (n) {
		c += n&1;
		n>>=1;
	}
	return c;
}

uint8_t basic_count_loop_4byte(uint32_t n) {
	uint8_t c = 0;
	while (n) {
		c += n&1;
		n>>=1;
	}
	return c;
}

uint8_t basic_count_no_loop(uint8_t n) {
	uint8_t c = 0;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n;
	return c;
}

struct bits {
	uint8_t  a:1;
	uint8_t  b:1;
	uint8_t  c:1;
	uint8_t  d:1;
	uint8_t  e:1;
	uint8_t  f:1;
	uint8_t  g:1;
	uint8_t  h:1;
};

uint8_t bitfield_count(uint8_t n) {
	struct bits *b =  (struct bits *)&n;
	return b->a + b->b + b->c + b->d + b->e + b->f + b->g + b->h;
}

uint8_t basic_count_no_loop_4byte(uint32_t n) {
	uint8_t c = 0;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;

	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;

	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;

	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n&1; n>>=1;
	c += n;

	return c;
}

void count_bitfield_4byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		uint32_t t = i;
		val += bitfield_count(t&0xFF); t>>= 8;
		val += bitfield_count(t&0xFF); t>>= 8;
		val += bitfield_count(t&0xFF); t>>= 8;
		val += bitfield_count(t);
	}
}

void count_logn_4byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += logn_count(i&0xFF);
	}
}

void count_basic_loop_4byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += logn_count_4byte(i);
	}
}

void count_basic_no_loop_4byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += basic_count_no_loop_4byte(i);
	}
}

void count_lut_8bit_4byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		uint32_t t = i;
		val += lut8[t&0xFF]; t>>= 8;
		val += lut8[t&0xFF]; t>>= 8;
		val += lut8[t&0xFF]; t>>= 8;
		val += lut8[t];
	}
}

void count_lut_4bit_4byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		uint32_t t = i;
		val += lut4[(t>>4) & 0xF] + lut4[t & 0xF]; t>>= 8;
		val += lut4[(t>>4) & 0xF] + lut4[t & 0xF]; t>>= 8;
		val += lut4[(t>>4) & 0xF] + lut4[t & 0xF]; t>>= 8;
		val += lut4[(t>>4) & 0xF] + lut4[t & 0xF];
	}
}

void count_lutc_4bit_4byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		uint32_t t = i;
		val += LUTC4(t&0xFF); t>>= 8;
		val += LUTC4(t&0xFF); t>>= 8;
		val += LUTC4(t&0xFF); t>>= 8;
		val += LUTC4(t);
	}
}

void count_bitfield_1byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += bitfield_count(i&0xFF);
	}
}

void count_logn_1byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += logn_count(i&0xFF);
	}
}

void count_basic_loop_1byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += basic_count_loop(i&0xFF);
	}
}

void count_basic_no_loop_1byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += basic_count_no_loop(i&0xFF);
	}
}

void count_lut_8bit_1byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += lut8[i&0xFF];
	}
}

void count_lut_4bit_1byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += lut4[(i>>4) & 0xF] + lut4[i & 0xF];
	}
}


void count_lutc_4bit_1byte(uint32_t n) {
	uint32_t i;
	for (i=0; i < n; i++) {
		val += LUTC4(i);
	}
}

void benchmark(void (*f)(uint32_t), uint32_t n, char *name) {
	uint64_t t;
	struct timespec start, end;

	clock_gettime(CLOCK_MONOTONIC, &start);
	f(n);
	clock_gettime(CLOCK_MONOTONIC, &end);
	t = timespecDiff(&end, &start);
	printf("%s: %"PRIu64 "\n", name, t);

}


int main(void) {
	init_lut_8bit();
	init_lut_4bit();
	init_lutc_4bit();

	benchmark(count_basic_loop_1byte, 	    1000000, "count_basic_loop_1byte    ");
	benchmark(count_basic_no_loop_1byte, 	1000000, "count_basic_no_loop_1byte ");
	benchmark(count_logn_1byte, 			1000000, "count_logn_1byte          ");
	benchmark(count_bitfield_1byte, 		1000000, "count_bitfield_1byte      ");
	benchmark(count_lut_8bit_1byte, 		1000000, "count_lut_8bit_1byte      ");
	benchmark(count_lut_4bit_1byte, 		1000000, "count_lut_4bit_1byte      ");
	benchmark(count_lutc_4bit_1byte, 		1000000, "count_lutc_4bit_1byte     ");

	printf("\n");

	benchmark(count_basic_loop_4byte, 		1000000, "count_basic_loop_4byte    ");
	benchmark(count_basic_no_loop_4byte, 	1000000, "count_basic_no_loop_4byte ");
	benchmark(count_logn_4byte, 			1000000, "count_logn_4byte          ");
	benchmark(count_bitfield_4byte, 		1000000, "count_bitfield_4byte      ");
	benchmark(count_lut_8bit_4byte, 		1000000, "count_lut_8bit_4byte      ");
	benchmark(count_lut_4bit_4byte, 		1000000, "count_lut_4bit_4byte      ");
	benchmark(count_lutc_4bit_4byte, 		1000000, "count_lutc_4bit_4byte     ");

	return EXIT_SUCCESS;
}
