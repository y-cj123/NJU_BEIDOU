#ifndef BD_SPLIT_H
#define BD_SPLIT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <string.h>

int BD_read(int port_f, unsigned char from_addr[], unsigned char recieve_data[]);

#endif
