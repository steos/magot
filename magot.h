#ifndef MAGOT_H
#define MAGOT_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
  MAGOT_ERR_MISSING_REQUIRED,
  MAGOT_ERR_MISSING_ARG,
  MAGOT_ERR_UNKNOWN_OPT
} magot_errtype_t;

typedef struct {
  char *name;
  bool flag;
  bool required;
  char *value;
} magot_t;

typedef struct {
  magot_errtype_t type;
  char *arg;
} magot_err_t;

void magot_init(magot_t *opt, char *name, bool flag, bool required); 

void magot_init_opt(magot_t *opt, char *name, bool required);

void magot_init_flag(magot_t *opt, char *name);

bool magot_parse(magot_err_t *err, int argc, char **argv, int optc, ...);

bool magot_isset(magot_t *opt);

#endif /* MAGOT_H */
