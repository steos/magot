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

bool str_empty(char *str) {
  return str == NULL || *str == '\0';
}

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

bool error(magot_err_t *err,
	       magot_errtype_t type,
	       char *arg) {
  err->type = type;
  err->arg = arg;
  return false;
}

bool process_opt(args_t *args, magot_t *opt, magot_err_t *err) {
  if (opt->flag) {
    opt->value = "";
  } else if (args_last(args)) {
    return error(err, MAGOT_ERR_MISSING_ARG,
		     args_get(args));
  } else {
    opt->value = args_get_next(args);
  }
  return true;
}

bool process_cluster(char *arg, int len,
		     int optc, magot_t **optv,
		     magot_err_t *err) {
  for (int i = 1; i < len; ++i) {
    char name[] = { arg[i], '\0' };
    magot_t *opt = find_opt(optc, optv, name);
    if (opt == NULL) {
      return error(err, MAGOT_ERR_UNKNOWN_OPT, arg);
    }
    opt->value = "";
  }
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
    bool valid_opt = len > 1 && arg[0] == '-';
    if (!valid_opt) {
      return error(err, MAGOT_ERR_UNKNOWN_OPT, arg);
    }
    if (posix && len > 2 && arg[1] != '-') {
      if (!process_cluster(arg, len, optc, optv, err)) {
	return false;
      }
    } else {
      char *name = arg[1] == '-' ?
	arg + 2 : arg + 1;
      if (str_empty(name)) {
	return error(err, MAGOT_ERR_UNKNOWN_OPT, arg);
      }
      magot_t *opt = find_opt(optc, optv, name);
      if (opt == NULL) {
	return error(err, MAGOT_ERR_UNKNOWN_OPT, arg);
      }
      if (!process_opt(&args, opt, err)) {
	return false;
      }
    }
  }
  for (int i = 0; i < optc; ++i) {
    magot_t *opt = optv[i];
    if (opt->required && !magot_isset(opt)) {
      return error(err, MAGOT_ERR_MISSING_REQUIRED,
		       opt->name);
    }
  }
  return true;
}

bool magot_isset(magot_t *opt) {
  return opt->value != NULL;
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
