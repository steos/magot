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
  puts("SYNOPSIS");
  puts("  demo [OPTIONS]");
  puts("EXAMPLES");
  puts("  demo --foo foobar -z");
  puts("  demo -f foobar -qz --lorem-ipsum");
  puts("OPTIONS");
  magot_print_help(stdout, optc, optv, style);
}

char *opt_value_str(magot_t *opt) {
  if (magot_is_flag(opt)) {
    return magot_isset(opt) ? "true" : "false";
  } else {
    return magot_isset(opt) ? magot_value(opt) : "N/A";
  }
}

int main(int argc, char **argv) {
  // the magot_t struct describes an option
  magot_t foo, bar, baz, quux, lorem;
  magot_t *opts[] = {
    // In case it's not obvious, the first two strings are
    // long name and short name, the boolean specifies whether
    // the opt is required and the last string is the help text.
    magot_opt(&foo, "foo", "f", true,
              "the foo option. 'tis required."),
    magot_opt(&bar, "bar", "b", false,
              "the bar option. 'tis optional."),
    // flags obviously can't be mandatory so there's
    // no required parameter
    magot_flag(&baz, "baz", "z", "a useless flag"),

    // null or empty string is all the same to magot
    magot_flag(&lorem, "lorem-ipsum", NULL, ""),

    // You don't have to give both a long and a short name
    // but at least one of them must not be empty.
    // If the short name is not empty it has to be exactly
    // one character long!
    magot_flag(&quux, "", "q", "the quux flag"),
  };
  // magot needs to know the number of opts
  int optc = sizeof(opts) / sizeof(opts[0]);

  // customize the argument name used in the help output
  magot_set_arg_name(&foo, "file");

  magot_parser_t parser;
  magot_parser(&parser, argc, argv);

  // By default magot starts to parse at the second arg
  // i.e. it ignores the first argument.
  // you can control this by changing the value of
  // the parse offset which defaults to 1.
  // magot_set_offset(&parser, 2);

  // disable mixing of options and remaining arguments
  // When mixing is disabled and magot encounters an
  // unknown option (i.e. remaining arg) it will not
  // consider the subsequent args as options and instead
  // just collect them as remaining args.
  // By default mixing is enabled.
  // Example:
  //    ./demo asdf -f thefoo blub
  // will work fine with mixing enabled and give us "asdf" and
  // "blub" as remaining args and "thefoo" as value for "--foo".
  // But if we disable mixing it will complain that "--foo"
  // was not specified.
  //
  // magot_allow_mixed(&parser, false);

  // If you want to collect remaining arguments allocate
  // an array that's at least as big as the number of args you
  // want to parse. You can get this size by just calling
  // magot_args_size after the parser is initialized.
  char *remaining[magot_args_size(&parser)];
  // If you don't assign this array then no remaining
  // arguments will be collected and magot will fail
  // if it encounters an unknown option.
  magot_set_remaining(&parser, remaining);

  // the opt style defaults to MAGOT_STYLE_POSIX
  // magot_set_style(&parser, MAGOT_STYLE_GNU);

  if (argc == 1) {
    // print usage if we have no args
    print_usage(optc, opts, magot_get_style(&parser));
    return 0;
  }

  bool success = magot_parse(optc, opts, &parser);
  if (!success) {
    fputs("Error: ", stdout);
    magot_print_error(stdout, &parser);
    print_usage(optc, opts, magot_get_style(&parser));
    return 1;
  }

  // dump our results
  for (int i = 0; i < optc; ++i) {
    magot_t *opt = opts[i];
    printf("%s: <%s>\n", magot_name(opt), opt_value_str(opt));
  }
  int rem_size = magot_remaining_size(&parser);
  if (rem_size > 0) {
    fputs("remaining args: ", stdout);
    for (int i = 0; i < rem_size; ++i) {
      fprintf(stdout, "'%s'", remaining[i]);
      if (i + 1 < rem_size) {
        fputs(", ", stdout);
      }
    }
    puts("");
  }
  return 0;
}
