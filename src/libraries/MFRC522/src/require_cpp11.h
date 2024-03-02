#line 1 "/media/raja/Data/AmplifyMobility/4G_AC001-EVRE20x4/AC001_Three_Connectors20x4/src/libraries/MFRC522/src/require_cpp11.h"
/**
 * Copyright (c) 2016 by Ludwig Grill (www.rotzbua.de)
 * Throws error if c++11 is not supported
 */
#ifndef REQUIRE_CPP11_H
#define REQUIRE_CPP11_H

#if __cplusplus < 201103L
#error "This library needs at least a C++11 compliant compiler, maybe compiler argument for C++11 support is missing or if you use Arduino IDE upgrade to version >=1.6.6"
#endif

#endif // REQUIRE_CPP11_H
