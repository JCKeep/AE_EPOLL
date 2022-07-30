#ifndef LOGGER
#define LOGGER

#include "config.h"


void logger_info(const char *infomation, long long fd);
void logger_error(const char *infomation, char *s);

#endif
