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
#include <stdio.h>
#include <string.h>

#define EMPTY(str) (str == NULL || *str == '\0')

void print_usage(int optc, magot_t **optv, magot_style_t style) {
  puts("Usage: demo [OPTIONS]");
  puts("SYNOPSIS");
  puts("  demo --foo foobar -z");
  puts("  demo -f foobar -qz --lorem-ipsum");
  puts("OPTIONS");
  magot_print_help(stdout, optc, optv, style);
}

char *opt_value_str(magot_t *opt) {
  if (opt->flag) {
    return magot_isset(opt) ? "true" : "false";
  } else {
    return magot_isset(opt) ? opt->value : "N/A";
  }
}

int main(int argc, char **argv) {
  magot_t foo, bar, baz, quux, lorem;
  magot_t *opts[] = {
    magot_opt(&foo, "foo", "f", true,
	      "the foo option. 'tis required."),
    magot_opt(&bar, "bar", "b", false,
	      "the bar option. 'tis optional."),
    magot_flag(&baz, "baz", "z", "a useless flag"),
    magot_flag(&lorem, "lorem-ipsum", NULL, ""),
    magot_flag(&quux, "", "q", "the quux flag"),
  };
  int optc = sizeof(opts) / sizeof(opts[0]);
  foo.arg_name = "file";
  magot_parser_t parser;
  magot_parser(&parser, argc, argv);
  char *remaining[argc - 1];
  parser.remaining = remaining;
  if (argc == 1) {
    print_usage(optc, opts, parser.style);
    return 0;
  }
  magot_err_t err;
  memset(&err, 0, sizeof(magot_err_t));
  bool success = magot_parse(optc, opts, &parser, &err);
  if (!success) {
    printf("%s: %s\n", magot_errstr(&err), err.arg);
    return 1;
  }

  for (int i = 0; i < optc; ++i) {
    magot_t *opt = opts[i];
    printf("%s: <%s>\n",
	   EMPTY(opt->name) ? opt->short_name : opt->name,
	   opt_value_str(opt));
  }
  if (parser.rem_count > 0) {
    fputs("remaining args: ", stdout);
    for (int i = 0; i < parser.rem_count; ++i) {
      fprintf(stdout, "'%s'", parser.remaining[i]);
      if (i + 1 < parser.rem_count) {
	fputs(", ", stdout);
      }
    }
    puts("");
  }
  return 0;
}
