/* This file is part of magot.
 *
 * Magot is free software. You can redistribute it and/or modify it
 * under the terms of the Do What The Fuck You Want To Public License,
 * Version 2, as published by Sam Hocevar. See http://www.wtfpl.net/
 * for more details.
 *
 * Magot is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 */

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

typedef enum {
  MAGOT_STYLE_POSIX,
  MAGOT_STYLE_GNU
} magot_style_t;

typedef struct {
  magot_style_t style;
} magot_parseconf_t;

typedef struct {
  char *name;
  char *short_name;
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

magot_t *magot_init(magot_t *opt,
		    char *name,
		    char *short_name,
		    bool flag,
		    bool required,
		    char *help);

magot_t *magot_init_opt(magot_t *opt,
			char *name,
			char *short_name,
			bool required,
			char *help);

magot_t *magot_init_flag(magot_t *opt,
			 char *name,
			 char *short_name,
			 char *help);

bool magot_parse(int argc, char **argv,
		 int optc, magot_t **optv,
		 magot_parseconf_t *conf,
		 magot_err_t *err);

bool magot_isset(magot_t *opt);

void magot_print_help(FILE *f,
		      int optc,
		      magot_t **optv,
		      magot_style_t style);

char *magot_errstr(magot_err_t *err);

#endif /* MAGOT_H */
