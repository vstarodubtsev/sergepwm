#ifndef CLI_H
#define CLI_H

#include "microrl/microrl.h"
#include "microrl/config.h"

int execute (int argc, const char * const * argv);

#ifdef _USE_COMPLETE
char ** complet (int argc, const char * const * argv);
#endif

#ifdef _USE_CTLR_C
void sigint (void);
#endif

#endif
