
#if 1
#ifndef CUST_ETH
#define CUST_ETH

#include <Ethernet.h>
#include <Preferences.h>

extern Preferences preferences;

void ethernetSetup();

void ethernetLoop();

void parse_string(String s,uint8_t op_type);
uint8_t x2i(char *);

#endif

#endif