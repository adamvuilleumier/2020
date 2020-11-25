#include "randlib.h"
#include <immintrin.h>

/* Return a random value, using hardware operations.  */
static unsigned long long
hardware_rand64 (void)
{
  unsigned long long int x;
  while (! _rdrand64_step (&x))
    continue;
  return x;
}

/* This is the external function for rand64 sw and hw */
extern unsigned long long rand64 (void)
{
  return hardware_rand64();
}
