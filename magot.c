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

magot_t *magot_init(magot_t *opt,
		    char *name,
		    char *short_name,
		    bool flag,
		    bool required,
		    char *help) {
  assert(!flag || !required);
  opt->name = name;
  opt->short_name = short_name;
  opt->flag = flag;
  opt->required = required;
  opt->value = NULL;
  opt->arg_name = "arg";
  opt->help = help;
  return opt;
}

magot_t *magot_init_flag(magot_t *opt,
			 char *name,
			 char *short_name,
			 char *help) {
  return magot_init(opt, name, short_name,
		    true, false, help);
}

magot_t *magot_init_opt(magot_t *opt,
			char *name,
			char *short_name,
			bool required,
			char *help) {
  return magot_init(opt, name, short_name,
		    false, required, help);
}

typedef struct {
  int argc;
  char **argv;
  int offset;
} args_t;

bool args_last(args_t *a) {
  return a->argc == a->offset + 1;
}
char *args_get(args_t *a) {
  assert(a->offset < a->argc);
  return a->argv[a->offset];
}
void args_next(args_t *a) {
  a->offset++;
}
bool args_done(args_t *a) {
  return a->offset >= a->argc;
}

char *args_get_next(args_t *a) {
  args_next(a);
  return args_get(a);
}

magot_t *find_opt(int optc, magot_t **opts, char *name) {
  for (int i = 0; i < optc; ++i) {
    magot_t *opt = opts[i];
    if ((opt->name != NULL
	 && strcmp(opt->name, name) == 0) ||
	(opt->short_name != NULL
	 && strcmp(opt->short_name, name) == 0)) {
      return opt;
    }
  }
  return NULL;
}

bool process_opt(args_t *args, magot_t *opt, magot_err_t *err) {
  if (opt->flag) {
    opt->value = "";
    return true;
  }
  if (args_last(args)) {
    err->type = MAGOT_ERR_MISSING_ARG;
    err->arg = args_get(args);
    return false;
  }
  opt->value = args_get_next(args);
  return true;
}

bool magot_parse(int argc, char **argv,
		 int optc, magot_t **optv,
		 magot_parseconf_t *conf,
		 magot_err_t *err) {
  args_t args;
  args.argc = argc;
  args.argv = argv;
  args.offset = 1;
  bool posix = conf->style == MAGOT_STYLE_POSIX;
  for (; !args_done(&args); args_next(&args)) {
    char *arg = args_get(&args);
    int len = strlen(arg);
    if (len > 1 && arg[0] == '-') {
      if (len > 2 && arg[1] != '-' && posix) {
	for (int i = 1; i < len; ++i) {
	  char name[] = { arg[i], '\0' };
	  magot_t *opt = find_opt(optc, optv, name);
	  if (opt == NULL) {
	    err->type = MAGOT_ERR_UNKNOWN_OPT;
	    err->arg = arg;
	    return false;
	  }
	  opt->value = "";
	}
      } else {
	char *name = arg[1] == '-' ?
	  arg + 2 : arg + 1;
	magot_t *opt = find_opt(optc, optv, name);
	if (opt == NULL) {
	  err->type = MAGOT_ERR_UNKNOWN_OPT;
	  err->arg = arg;
	  return false;
	}
	if (!process_opt(&args, opt, err)) {
	  return false;
	}
      }
    } else {
      err->type = MAGOT_ERR_UNKNOWN_OPT;
      err->arg = arg;
      return false;
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

bool str_empty(char *str) {
  return str == NULL || strlen(str) == 0;
}

void magot_print_help(FILE *f, int optc, magot_t **optv,
		      magot_style_t style) {
  char *long_prefix = style == MAGOT_STYLE_GNU ? "-" : "--";
  for (int i = 0; i < optc; ++i) {
    magot_t *opt = optv[i];
    fputs("  ", f);
    bool have_long = false;
    if (!str_empty(opt->name)) {
      fputs(long_prefix, f);
      fputs(opt->name, f);
      have_long = true;
    }
    if (!str_empty(opt->short_name)) {
      if (have_long) {
	fputs(", ", f);
      }
      fputs("-", f);
      fputs(opt->short_name, f);
    }
    if (!opt->flag) {
      fprintf(f, " <%s>\n", opt->arg_name);
    } else {
      fputs("\n", f);
    }
    fprintf(f, "    %s\n", opt->help);
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
