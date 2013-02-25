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

#include <magot.h>

#include <string.h>
#include <stdbool.h>
#include <assert.h>

magot_t *magot_init(magot_t *opt, char *name,
		    bool flag, bool required,
		    char *help) {
  assert(!flag || !required);
  opt->name = name;
  opt->flag = flag;
  opt->required = required;
  opt->value = NULL;
  opt->arg_name = "arg";
  opt->help = help;
  return opt;
}

magot_t *magot_init_flag(magot_t *opt, char *name, char *help) {
  return magot_init(opt, name, true, false, help);
}

magot_t *magot_init_opt(magot_t *opt, char *name,
			bool required, char *help) {
  return magot_init(opt, name, false, required, help);
}

magot_t *find_opt(int optc, magot_t **opts, char *name) {
  for (int i = 0; i < optc; ++i) {
    if (strcmp(name, opts[i]->name) == 0) {
      return opts[i];
    }
  }
  return NULL;
}

bool magot_parse(magot_err_t *err, int argc,
		 char **argv, int optc, magot_t **optv) {
  for (int i = 1; i < argc; ++i) {
    magot_t *opt = find_opt(optc, optv, argv[i]);
    if (opt == NULL) {
      err->type = MAGOT_ERR_UNKNOWN_OPT;
      err->arg = argv[i];
      return false;
    }
    if (!opt->flag) {
      if (i + 1 == argc) {
	err->type = MAGOT_ERR_MISSING_ARG;
	err->arg = opt->name;
	return false;
      }
      opt->value = argv[++i];
    } else {
      opt->value = "true";
    }
  }
  for (int i = 0; i < optc; ++i) {
    magot_t *opt = optv[i];
    if (opt->required && !magot_isset(opt)) {
      err->type = MAGOT_ERR_MISSING_REQUIRED;
      err->arg = opt->name;
      return false;
    }
  }
  return true;
}

bool magot_isset(magot_t *opt) {
  return opt->value != NULL;
}

void magot_print_help(FILE *f, int optc, magot_t **optv) {
  for (int i = 0; i < optc; ++i) {
    magot_t *opt = optv[i];
    if (opt->flag) {
      fprintf(f, "  %-8s %s\n", opt->name, opt->help);
    } else {
      fprintf(f, "  %s %s\n", opt->name, opt->arg_name);
      fprintf(f, "           %s\n", opt->help);
    }
  }
}

char *magot_errstr(magot_err_t *err) {
  switch (err->type) {
  case MAGOT_ERR_UNKNOWN_OPT: return "unknown option";
  case MAGOT_ERR_MISSING_REQUIRED: return "missing required option";
  case MAGOT_ERR_MISSING_ARG: return "missing argument";
  }
  return NULL;
}
