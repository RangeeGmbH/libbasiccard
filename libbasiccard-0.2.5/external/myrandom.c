// random provider for libbasiccard
// FIXME: This is really not a secure PRNG, but good enough for our purposes

#include <stdlib.h>

#ifdef USE_TOMSRANDOM

#ifndef BITS_OF_ENTROPY
#define BITS_OF_ENTROPY 64
#endif

#include <tomcrypt.h>
prng_state bci_int_prng;
#endif

void seedmyrandom(long seed) {
#if defined(USE_TOMSRANDOM)
  register_prng(&fortuna_desc);
  rng_make_prng(BITS_OF_ENTROPY, find_prng("fortuna"), &bci_int_prng, NULL);
  fortuna_add_entropy((unsigned char *)(&seed), sizeof(seed), &bci_int_prng);
  fortuna_ready(&bci_int_prng);
#else
  srand(seed);
#endif  
}

long myrandom(void) {
  long result;
#if defined(USE_TOMSRANDOM)
  fortuna_read((unsigned char *)(&result), sizeof(result), &bci_int_prng);
#else
  // FIXME: Since RAND_MAX may be smaller than long, 
  //        we only make the assumption that the output of rand() is at least 8 bits
  int i;
  
  for (i=0;i<sizeof(result);i++) {
    result ^= (rand() & 0xff) << (i*8);
  }
#endif
  return result;
}
