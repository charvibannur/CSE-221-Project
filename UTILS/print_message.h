#ifndef _PRINT_MESSAGE_H
#define _PRINT_MESSAGE_H

#include <stdlib.h>
#include <stdio.h>

#define _TEST_RESULT 0
#define _GENERIC_INFO 1
#define _DEBUG_MSG 2

#define _SHOW_LEVEL _TEST_RESULT

void print_uint64(uint64_t result) {
  printf("%lu\n", result);
}

void print_message(int message_type, char* message) {
  if (message_type <= _SHOW_LEVEL) {
    printf("%s", message);
  }
}

void print_error_message(char* message) {
  printf("%s", message);
  exit(-1);
}

#endif