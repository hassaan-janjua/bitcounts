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

#define LUTC4_8bit(v) (((0xe9949440 >> (((v)>>3) & 0x1E)) & 3) + ((((v)) & 0xF0) != 0) + ((0xe9949440 >> (((v)<<1) & 0x1E)) & 3) + ((((v)) & 0x0F) != 0))


#define LUTC4_32bit(v) \
  ((((0xe9949440 >> (((v)>>27) & 0x1E)) & 3) + ((((v)) & 0xF0000000) != 0) + ((0xe9949440 >> (((v)>>23) & 0x1E)) & 3) + ((((v)) & 0x0F000000) != 0)) + \
   (((0xe9949440 >> (((v)>>19) & 0x1E)) & 3) + ((((v)) & 0x00F00000) != 0) + ((0xe9949440 >> (((v)>>15) & 0x1E)) & 3) + ((((v)) & 0x000F0000) != 0)) + \
   (((0xe9949440 >> (((v)>>11) & 0x1E)) & 3) + ((((v)) & 0x0000F000) != 0) + ((0xe9949440 >> (((v)>>7)  & 0x1E)) & 3) + ((((v)) & 0x00000F00) != 0)) + \
   (((0xe9949440 >> (((v)>>3)  & 0x1E)) & 3) + ((((v)) & 0x000000F0) != 0) + ((0xe9949440 >> (((v)<<1)  & 0x1E)) & 3) + ((((v)) & 0x0000000F) != 0)))

void init_lut_4bit();
void init_lut_8bit();

uint8_t logn_count_8bit(uint8_t n);
uint8_t logn_count_32bit(uint32_t n);

uint8_t basic_count_loop_8bit(uint8_t n);
uint8_t basic_count_loop_32bit(uint32_t n);

uint8_t basic_count_no_loop_8bit(uint8_t n);
uint8_t basic_count_no_loop_32bit(uint32_t n);

uint8_t bitfield_count_8bit(uint8_t n);
uint8_t bitfield_count_32bit(uint32_t n);

uint8_t lut8_count_8bit(uint8_t n);
uint8_t lut8_count_32bit(uint32_t n);

uint8_t lut4_count_8bit(uint8_t n);
uint8_t lut4_count_32bit(uint32_t n);

uint8_t lutc4_count_8bit(uint8_t n);
uint8_t lutc4_count_32bit(uint32_t n);

uint8_t bittwidling_count_8bit(uint8_t n);
uint8_t bittwidling_count_32bit(uint32_t n);


int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

uint8_t lut8[256];
uint8_t lut4[16];

void init_lut_4bit() {
  uint16_t i;
  for (i=0; i < 16; i++) {
    lut4[i] = basic_count_loop_8bit(i);
  }
}


void init_lut_8bit() {
  uint16_t i;
  for (i=0; i < 256; i++) {
    lut8[i] = basic_count_loop_8bit(i);
  }
}


uint8_t logn_count_8bit(uint8_t n) {
  n = (n&0x55) + ((n>>1) & 0x55);
  n = (n&0x33) + ((n>>2) & 0x33);
  return (n&0xF) + ((n>>4) & 0xF);
}


uint8_t logn_count_32bit(uint32_t n) {
  n = (n&0x55555555) + ((n>>1) & 0x55555555);
  n = (n&0x33333333) + ((n>>2) & 0x33333333);
  n = (n&0x0F0F0F0F) + ((n>>4) & 0x0F0F0F0F);
  n = (n&0x00FF00FF) + ((n>>8) & 0x00FF00FF);
  return (n&0x0000FFFF) + ((n>>16) & 0x0000FFFF);
}


uint8_t bittwidling_count_8bit(uint8_t n) {
  n = n - ((n >> 1) & 0x55);
  n = (n & 0x33) + ((n >> 2) & 0x33);
  return (((n + (n >> 4)) & 0xF));
}


uint8_t bittwidling_count_32bit(uint32_t n) {
  n = n - ((n >> 1) & 0x55555555);
  n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
  return (((n + (n >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}


uint8_t basic_count_loop_8bit(uint8_t n) {
  uint8_t c = 0;
  while (n) {
    c += n&1;
    n>>=1;
  }
  return c;
}


uint8_t basic_count_loop_32bit(uint32_t n) {
  uint8_t c = 0;
  while (n) {
    c += n&1;
    n>>=1;
  }
  return c;
}


uint8_t basic_count_no_loop_8bit(uint8_t n) {
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


uint8_t basic_count_no_loop_32bit(uint32_t n) {
  uint8_t c = 0;
  c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1;
  c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1;
  c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1;
  c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1;
  c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1;
  c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1;
  c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1;
  c += n&1; n>>=1; c += n&1; n>>=1; c += n&1; n>>=1; c += n;
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

uint8_t bitfield_count_8bit(uint8_t n) {
  struct bits *b =  (struct bits *)&n;
  return b->a + b->b + b->c + b->d + b->e + b->f + b->g + b->h;
}


struct bits_32bit {
  uint32_t  a1:1; uint32_t  a2:1; uint32_t  a3:1; uint32_t  a4:1;
  uint32_t  b1:1; uint32_t  b2:1; uint32_t  b3:1; uint32_t  b4:1;
  uint32_t  c1:1; uint32_t  c2:1; uint32_t  c3:1; uint32_t  c4:1;
  uint32_t  d1:1; uint32_t  d2:1; uint32_t  d3:1; uint32_t  d4:1;
  uint32_t  e1:1; uint32_t  e2:1; uint32_t  e3:1; uint32_t  e4:1;
  uint32_t  f1:1; uint32_t  f2:1; uint32_t  f3:1; uint32_t  f4:1;
  uint32_t  g1:1; uint32_t  g2:1; uint32_t  g3:1; uint32_t  g4:1;
  uint32_t  h1:1; uint32_t  h2:1; uint32_t  h3:1; uint32_t  h4:1;

};

uint8_t bitfield_count_32bit(uint32_t n) {
  struct bits_32bit *b =  (struct bits_32bit *)&n;
  return  b->a1 + b->b1 + b->c1 + b->d1 + b->e1 + b->f1 + b->g1 + b->h1 +
          b->a2 + b->b2 + b->c2 + b->d2 + b->e2 + b->f2 + b->g2 + b->h2 +
          b->a3 + b->b3 + b->c3 + b->d3 + b->e3 + b->f3 + b->g3 + b->h3 +
          b->a4 + b->b4 + b->c4 + b->d4 + b->e4 + b->f4 + b->g4 + b->h4;
}


uint8_t lut8_count_8bit(uint8_t n) {
  return lut8[n];
}


uint8_t lut8_count_32bit(uint32_t n) {
  return lut8[n&0xFF] + lut8[(n>>8) & 0xFF] + lut8[(n>>16) & 0xFF] + lut8[n>>24];
}


uint8_t lut4_count_8bit(uint8_t n) {
  return lut4[n&0xF] + lut4[n>>4];
}


uint8_t lut4_count_32bit(uint32_t n) {
  return  lut4[n&0xF]          + lut4[(n>>4)  & 0xF] + lut4[(n>>8)  & 0xF] + lut4[(n>>12) & 0xF] +
          lut4[(n>>16) & 0xF]  + lut4[(n>>20) & 0xF] + lut4[(n>>24) & 0xF] + lut4[n>>28];
}


uint8_t lutc4_count_8bit(uint8_t n) {
  return LUTC4_8bit(n);
}


uint8_t lutc4_count_32bit(uint32_t n) {
  return LUTC4_32bit(n);
}


void benchmark32(uint8_t (*f)(uint32_t), uint32_t n, char *name) {
  uint64_t t;
  uint64_t v = 0;
  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (uint32_t i=0; i < n; i++) {
    v += f(i);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  t = timespecDiff(&end, &start);
  printf("%s: %"PRIu64 "\t\t%"PRIu64 "\n", name, t, v);

}


void benchmark8(uint8_t (*f)(uint8_t), uint32_t n, char *name) {
  uint64_t t;
  uint64_t v = 0;

  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (uint32_t i=0; i < n; i++) {
    v += f(i&0xFF);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  t = timespecDiff(&end, &start);
  printf("%s: %"PRIu64 "\t\t%"PRIu64 "\n", name, t, v);

}


int test_bit_counter_8bit(uint8_t (*f)(uint8_t), char *message) {
  int passed = 1;
  for (uint32_t i = 0; i < 256 && passed; i++) {
    if (f(i) != basic_count_loop_8bit(i))
      passed = 0;
  }

  if (passed) {
    printf("Passed: %s\n", message);
  } else {
    printf("Failed: %s\n", message);
  }

  return passed;
}


int test_bit_counter_32bit(uint8_t (*f)(uint32_t), char *message) {
  int passed = 1;

  // Test first 1024 numbers
  for (uint32_t i = 0; i < 1024 && passed; i++) {
    if (f(i) != basic_count_loop_32bit(i))
      passed = 0;
  }

  // Test last 1024 numbers
  for (uint32_t i = 0xFFFFFFFF; i > (0xFFFFFFFF-1024) && passed; i--) {
    if (f(i) != basic_count_loop_32bit(i))
      passed = 0;
  }

  // Test middle 1024 numbers
  for (uint32_t i = 0x0000FFFF; i > (0x0000FFFF-1024) && passed; i--) {
    if (f(i) != basic_count_loop_32bit(i))
      passed = 0;
  }

  if (passed) {
    printf("Passed: %s\n", message);
  } else {
    printf("Failed: %s\n", message);
  }

  return passed;
}


void test_bit_counters() {
  init_lut_8bit();
  init_lut_4bit();

  test_bit_counter_8bit(basic_count_no_loop_8bit, "basic_count_no_loop_8bit");
  test_bit_counter_8bit(bitfield_count_8bit, "bitfield_count_8bit");
  test_bit_counter_8bit(basic_count_loop_8bit, "basic_count_loop_8bit");
  test_bit_counter_8bit(lutc4_count_8bit, "lutc4_count_8bit");
  test_bit_counter_8bit(lut8_count_8bit, "lut8_count_8bit");
  test_bit_counter_8bit(lut4_count_8bit, "lut4_count_8bit");
  test_bit_counter_8bit(logn_count_8bit, "logn_count_8bit");
  test_bit_counter_8bit(bittwidling_count_8bit, "bittwidling_count_8bit");

  test_bit_counter_32bit(basic_count_no_loop_32bit, "basic_count_no_loop_32bit");
  test_bit_counter_32bit(bitfield_count_32bit, "bitfield_count_32bit");
  test_bit_counter_32bit(basic_count_loop_32bit, "basic_count_loop_32bit");
  test_bit_counter_32bit(lutc4_count_32bit, "lutc4_count_32bit");
  test_bit_counter_32bit(lut8_count_32bit, "lut8_count_32bit");
  test_bit_counter_32bit(lut4_count_32bit, "lut4_count_32bit");
  test_bit_counter_32bit(logn_count_32bit, "logn_count_32bit");
  test_bit_counter_32bit(bittwidling_count_32bit, "bittwidling_count_32bit");

}


void becnhmark_bit_counters() {
  init_lut_8bit();
  init_lut_4bit();

  benchmark8(basic_count_no_loop_8bit,   10000000, "basic_count_no_loop_8bit  ");
  benchmark8(bitfield_count_8bit,        10000000, "bitfield_count_8bit       ");
  benchmark8(basic_count_loop_8bit,      10000000, "basic_count_loop_8bit     ");
  benchmark8(lutc4_count_8bit,           10000000, "lutc4_count_8bit          ");
  benchmark8(logn_count_8bit,            10000000, "logn_count_8bit           ");
  benchmark8(lut4_count_8bit,            10000000, "lut4_count_8bit           ");
  benchmark8(bittwidling_count_8bit,     10000000, "bittwidling_count_8bit    ");
  benchmark8(lut8_count_8bit,            10000000, "lut8_count_8bit           ");

  printf("\n");

  benchmark32(basic_count_no_loop_32bit, 10000000, "basic_count_no_loop_32bit ");
  benchmark32(bitfield_count_32bit,      10000000, "bitfield_count_32bit      ");
  benchmark32(basic_count_loop_32bit,    10000000, "basic_count_loop_32bit    ");
  benchmark32(lutc4_count_32bit,         10000000, "lutc4_count_32bit         ");
  benchmark32(logn_count_32bit,          10000000, "logn_count_32bit          ");
  benchmark32(lut4_count_32bit,          10000000, "lut4_count_32bit          ");
  benchmark32(bittwidling_count_32bit,   10000000, "bittwidling_count_32bit   ");
  benchmark32(lut8_count_32bit,          10000000, "lut8_count_32bit          ");
}

int main(void) {

  test_bit_counters();
  becnhmark_bit_counters();

  return EXIT_SUCCESS;
}
