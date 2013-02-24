#ifndef MAGOT_H
#define MAGOT_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum {
  MAGOT_ERR_MISSING_REQUIRED,
  MAGOT_ERR_MISSING_ARG,
  MAGOT_ERR_UNKNOWN_OPT
} magot_errtype_t;

typedef struct {
  char *name;
  bool flag;
  bool required;
  char *help;
  char *arg_name;
  char *value;
} magot_t;

typedef struct {
  magot_errtype_t type;
  char *arg;
} magot_err_t;

magot_t *magot_init(magot_t *opt, char *name,
		    bool flag, bool required, char *help);

magot_t *magot_init_opt(magot_t *opt, char *name,
			bool required, char *help);

magot_t *magot_init_flag(magot_t *opt, char *name, char *help);

bool magot_parse(magot_err_t *err,
		 int argc, char **argv,
		 int optc, magot_t **optv);

bool magot_isset(magot_t *opt);

void magot_print_help(FILE *f, int optc, magot_t **optv);

char *magot_errstr(magot_err_t *err);

#endif /* MAGOT_H */
