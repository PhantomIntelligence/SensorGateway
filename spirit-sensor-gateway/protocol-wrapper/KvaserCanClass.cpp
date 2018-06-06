#include "KvaserCanClass.h"
#include <canlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define READ_WAIT_INFINITE (unsigned long)(-1)

static unsigned int msgCounter =0;

static void protocole_wrapper::KvaserCanClass::check(char *id, canStatus stat) {
    if (stat != canOK) {
        char buf[50];
        buf[0] = '\0';
        canGetErrorText(stat, buf, sizeof(buf));
        printf("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
    }
}

static void protocole_wrapper::KvaserCanClass::printUsageAndExit(char *prgName)
{
    printf("Usage: '%s <channel>'\n", prgName);
    exit(1);
}

static void protocole_wrapper::KvaserCanClass::sighand(int sig) {
    (void)sig;
}


