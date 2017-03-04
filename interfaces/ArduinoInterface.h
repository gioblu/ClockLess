
#pragma once
#if defined(ARDUINO)
  #include <Arduino.h>

  #ifndef CLDL_WRITE
    #define CLDL_WRITE digitalWrite
  #endif

  #ifndef CLDL_READ
    #define CLDL_READ digitalRead
  #endif

  #ifndef CLDL_READ_PULL_DOWN
    #define CLDL_READ_PULL_DOWN(P) { \
      CLDL_WRITE(P, LOW); \
      CLDL_MODE(P, INPUT); \
    }
  #endif

  #ifndef CLDL_MODE
    #define CLDL_MODE pinMode
  #endif

  #ifndef CLDL_DELAY_MICROSECONDS
    #define CLDL_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef CLDL_RANDOM
    #define CLDL_RANDOM random
  #endif

  #ifndef CLDL_RANDOM_SEED
    #define CLDL_RANDOM_SEED randomSeed
  #endif
#endif
