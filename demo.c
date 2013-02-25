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

void print_usage(int optc, magot_t **optv) {
  puts("Usage: demo [OPTIONS]");
  puts("SYNOPSIS");
  puts("  demo -foo foobar -z");
  puts("OPTIONS");
  magot_print_help(stdout, optc, optv);
}

int main(int argc, char **argv) {
  magot_t foo, bar, baz;
  magot_t *opts[] = {
    magot_init_opt(&foo, "-foo", true,
		   "the foo option. 'tis required."),
    magot_init_opt(&bar, "-b", false,
		   "the bar option. 'tis optional."),
    magot_init_flag(&baz, "-z", "a useless flag")
  };
  int optc = sizeof(opts) / sizeof(opts[0]);
  foo.arg_name = "file";

  if (argc == 1) {
    print_usage(optc, opts);
    return 0;
  }

  magot_err_t err;
  memset(&err, 0, sizeof(magot_err_t));
  bool success = magot_parse(&err, argc, argv, optc, opts);
  if (!success) {
    printf("%s: %s\n", magot_errstr(&err), err.arg);
    return 1;
  }

  printf("foo option: %s\n", foo.value);
  if (magot_isset(&bar)) {
    printf("bar option: %s\n", bar.value);
  }
  if (magot_isset(&baz)) {
    puts("baz flag present");
  }
  return 0;
}
