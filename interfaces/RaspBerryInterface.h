
#pragma once
#if defined(RPI)
  #include <wiringPi.h>

  #ifndef CLDL_WRITE
    #define CLDL_WRITE digitalWrite
  #endif

  #ifndef CLDL_READ
    #define CLDL_READ digitalRead
  #endif

  #ifndef CLDL_MODE
    #define CLDL_MODE pinMode
  #endif

  #ifndef CLDL_DELAY_MICROSECONDS
    #define CLDL_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef CLDL_RANDOM
    #define CLDL_RANDOM random(randMax) (int)((1.0 + randMax) * rand() / ( RAND_MAX + 1.0 ) );
    /* Scale rand()'s return value against RAND_MAX using doubles instead of
       a pure modulus to have a more distributed result */
  #endif

  #ifndef CLDL_RANDOM_SEED
    #define CLDL_RANDOM_SEED srand
  #endif
#endif
