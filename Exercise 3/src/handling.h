#ifndef HANDLING_H
#define HANDLING_H

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h> // Exit_success and exit_failure  exit(EXIT_FAILURE);

#define PRINTMESS(mess) { fprintf(stderr, "{%s:%d}-(%s) %s\n", __FILE__, __LINE__, __FUNCTION__, (mess)); }

#define DEBUG(mess) { PRINTMESS(mess); }

#define FATAL(mess) { PRINTMESS(mess) ; abort(); }

#define FATALERR(errcode) FATAL(strerror(errcode))

// CHECK MACROS
#define CHECKNO(X) {if ((X) == -1) FATALERR(errno);}

#define CHECKNU(X) {if ((X) == NULL) FATALERR(errno);}

#define CHECKNE(X) {if ((X) < 0) FATALERR(errno);}

#define CHECK(X) {if ((X) == NULL || (X) != 0) FATALERR(errno);}

#endif //HANDLING_H