#include "logger.h"

void logger_info(const char *infomation, long long fd) {
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);   
    char *t = asctime(timenow);
    t[strlen(t) - 1] = '\0';
    printf("%16s [\033[32mINFO\033[0m]: %s(%lld)\n", t, infomation, fd);
}

void logger_error(const char *infomation, char *s)
{
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);   
    char *t = asctime(timenow);
    t[strlen(t) - 1] = '\0';
    printf("%16s [\033[37mERRO\033[0m]: %s(%s)\n", t, infomation, s);
}