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

void magot_parser(magot_parser_t *parser, int argc, char **argv) {
  parser->argc = argc;
  parser->argv = argv;
  parser->offset = 1;
  parser->rem_count = 0;
  parser->remaining = NULL;
  parser->style = MAGOT_STYLE_POSIX;
  parser->err_arg = NULL;
  parser->mixed = true;
}

magot_t *magot_init(magot_t *opt,
                    char *name,
                    char *short_name,
                    bool flag,
                    bool required,
                    char *help) {
  assert((!flag || !required)
         && "a flag cannot be required");
  assert((!str_empty(name) || !str_empty(short_name))
         && "option must have non-empty name");
  assert((str_empty(short_name) || strlen(short_name) == 1)
         && "short name must be a single character or empty");
  assert((str_empty(name) || strlen(name) > 1)
	 && "long name must be more than one character long");
  opt->name = name;
  opt->short_name = short_name;
  opt->flag = flag;
  opt->required = required;
  opt->value = NULL;
  opt->arg_name = "arg";
  opt->help = help;
  return opt;
}

magot_t *magot_flag(magot_t *opt, char *name,
		    char *short_name, char *help) {
  return magot_init(opt, name, short_name, true, false, help);
}

magot_t *magot_opt(magot_t *opt,
                   char *name,
                   char *short_name,
                   bool required,
                   char *help) {
  return magot_init(opt, name, short_name, false, required, help);
}

bool args_last(magot_parser_t *a) {
  return a->argc == a->offset + 1;
}
char *args_get(magot_parser_t *a) {
  assert(a->offset < a->argc);
  return a->argv[a->offset];
}
bool args_done(magot_parser_t *a) {
  return a->offset >= a->argc;
}
bool args_next(magot_parser_t *a) {
  a->offset++;
  return args_done(a);
}
char *args_get_next(magot_parser_t *a) {
  bool done = args_next(a);
  assert(!done && "no more args");
  return args_get(a);
}

bool match_short_name(magot_t *opt, char *str) {
  return !str_empty(opt->short_name)
    && strcmp(opt->short_name, str) == 0;
}
bool match_long_name(magot_t *opt, char *str) {
  return !str_empty(opt->name)
    && strcmp(opt->name, str) == 0;
}

magot_t *find_opt(int optc, magot_t **opts, char *name,
                  bool (*match)(magot_t*,char*)) {
  for (int i = 0; i < optc; ++i) {
    magot_t *opt = opts[i];
    if (match(opt, name)) {
      return opt;
    }
  }
  return NULL;
}

bool error(magot_parser_t *parser, magot_errtype_t type, char *arg) {
  parser->err_type = type;
  parser->err_arg = arg;
  return false;
}

bool process_opt(magot_parser_t *args, magot_t *opt) {
  if (opt->flag) {
    opt->value = "";
  } else if (args_last(args)) {
    return error(args, MAGOT_ERR_MISSING_ARG,
                 args_get(args));
  } else {
    opt->value = args_get_next(args);
  }
  return true;
}

bool process_cluster(char *arg, int len,
                     int optc, magot_t **optv,
                     magot_parser_t *parser) {
  for (int i = 1; i < len; ++i) {
    char name[] = { arg[i], '\0' };
    magot_t *opt = find_opt(optc, optv, name, &match_short_name);
    if (opt == NULL) {
      return error(parser, MAGOT_ERR_UNKNOWN_OPT, arg);
    }
    opt->value = "";
  }
  return true;
}

bool magot_parse(int optc, magot_t **optv, magot_parser_t *parser) {
  bool posix = parser->style == MAGOT_STYLE_POSIX;
  for (; !args_done(parser); args_next(parser)) {
    char *arg = args_get(parser);
    int len = strlen(arg);
    bool valid_opt = len > 1 && arg[0] == '-';
    bool long_opt = valid_opt &&
      ((posix && arg[1] == '-') || (!posix && len > 2));
    if (!valid_opt && parser->remaining == NULL) {
      return error(parser, MAGOT_ERR_UNKNOWN_OPT, arg);
    } else if (!valid_opt && parser->mixed) {
      parser->remaining[parser->rem_count++] = arg;
    } else if (!valid_opt) {
      break;
    } else if (posix && !long_opt && len > 2) {
      if (!process_cluster(arg, len, optc, optv, parser)) {
        return false;
      }
    } else {
      char *name = posix && long_opt ? arg + 2 : arg + 1;
      if (str_empty(name)) {
        return error(parser, MAGOT_ERR_UNKNOWN_OPT, arg);
      }
      bool (*matcher)(magot_t*,char*) = long_opt ?
        &match_long_name : &match_short_name;
      magot_t *opt = find_opt(optc, optv, name, matcher);
      if (opt == NULL) {
        return error(parser, MAGOT_ERR_UNKNOWN_OPT, arg);
      }
      if (!process_opt(parser, opt)) {
        return false;
      }
    }
  }
  for (; !args_done(parser); args_next(parser)) {
    parser->remaining[parser->rem_count++] = args_get(parser);
  }
  for (int i = 0; i < optc; ++i) {
    magot_t *opt = optv[i];
    if (opt->required && !magot_isset(opt)) {
      return error(parser, MAGOT_ERR_MISSING_REQUIRED,
                   !str_empty(opt->name) ? opt->name : opt->short_name);
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
    if (!str_empty(opt->help)) {
      fprintf(f, "    %s\n", opt->help);
    }
  }
}

void magot_print_error(FILE *f, magot_parser_t *parser) {
  assert(parser != NULL);
  assert(parser->err_arg != NULL);
  const bool posix = parser->style == MAGOT_STYLE_POSIX;
  switch (parser->err_type) {
    case MAGOT_ERR_UNKNOWN_OPT:
      fprintf(f, "option '%s' is unknown\n", parser->err_arg);
      break;
    case MAGOT_ERR_MISSING_REQUIRED:
      fprintf(f, "missing required option '%s%s'\n",
              posix && strlen(parser->err_arg) > 1 ? "--" : "-",
              parser->err_arg);
      break;
    case MAGOT_ERR_MISSING_ARG:
      fprintf(f, "missing argument for option '%s'\n", parser->err_arg);
      break;
    default:
      assert(false && "err type not handled");
  }
}

void magot_unset(magot_t *opt) {
  opt->value = NULL;
}

bool magot_is_flag(magot_t *opt) {
  return opt->flag;
}

void magot_set_arg_name(magot_t *opt, char *name) {
  opt->arg_name = name;
}

char *magot_long_name(magot_t *opt) {
  return opt->name;
}

char *magot_short_name(magot_t *opt) {
  return opt->short_name;
}

char *magot_name(magot_t *opt) {
  return str_empty(opt->name) ? opt->short_name : opt->name;
}

char *magot_value(magot_t *opt) {
  return opt->value;
}

void magot_set_style(magot_parser_t *p, magot_style_t style) {
  p->style = style;
}

magot_style_t magot_get_style(magot_parser_t *p) {
  return p->style;
}

void magot_set_remaining(magot_parser_t *p, char **rem) {
  p->remaining = rem;
}

int magot_remaining_size(magot_parser_t *p) {
  return p->rem_count;
}

void magot_set_offset(magot_parser_t *p, int offset) {
  assert(offset >= 0 && "parser offset must be >= 0");
  p->offset = offset;
}

char *magot_err_arg(magot_parser_t *p) {
  return p->err_arg;
}

magot_errtype_t magot_err_type(magot_parser_t *p) {
  return p->err_type;
}

int magot_args_size(magot_parser_t *p) {
  return p->argc - p->offset;
}

void magot_allow_mixed(magot_parser_t *p, bool mixed) {
  p->mixed = mixed;
}
