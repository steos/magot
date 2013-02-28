# magot

<b>M</b>agot <b>a</b>in't <b>g</b>et<b>o</b>p<b>t</b>.

## What?

Magot is a minimal library for command line argument parsing. The main
objective is to provide basic argument handling capabilities with a
minimal amount of setup code.

The code is strict C99 and should build fine with any compliant C
compiler without any warnings. The Makefile targets GNU Make >= 3.80.

Suggestions, contributions and flames welcome.

### Features

* POSIX or GNU style options
* flag clusters when using POSIX style
* mandatory arguments
* print usage summary
* collect remaining arguments
* no heap allocations

### Catch?

The API is not finalized therefore the soname will _not_ change on BC
breaks until it is (if ever - just link to it statically if you are
crazy enough to actually use it).

This is first and foremost my personal toy. The code is in all
likelyhood horribly inefficient. No warranty. No support. Use at own
risk. Yada yada.

## Why?

Getopt makes me want to gouge my eyes out.

## How?

Look at demo.c for an example with explanations.

```c
#include <magot.h>
int main(int argc, char **argv) {
  magot_t foo, bar, baz, quux;
  magot_t *opts[] = {
    magot_opt(&foo, "foo", "f", true, "foo option"),
    magot_opt(&bar, "bar", "b", false, "bar option"),
    magot_flag(&baz, "baz", "z", "baz flag"),
    magot_flag(&quux, "quux", "q", "quux flag")
  };

  int optc = sizeof(opts) / sizeof(opts[0]);
  magot_parser_t parser;
  magot_parser(&parser, argc, argv);
  magot_set_style(&parser, MAGOT_STYLE_GNU);

  if (!magot_parse(optc, opts, &parser)) {
    magot_print_error(stdout, &parser);
    puts("OPTIONS");
    magot_print_help(stdout, optc, opts, magot_get_style(&parser));
    return 1;
  }

  printf("foo option: %s\n", magot_value(&foo));
  if (magot_isset(&bar)) {
    printf("bar option: %s\n", magot_value(&bar));
  }
  if (magot_isset(&baz)) {
    puts("baz flag present");
  }
  if (magot_isset(&quux)) {
    puts("quux flag present");
  }
  return 0;
}
```

## Build?

Just execute make to build everything (performs no optimizations, see
CFLAGS below)

    make

To install execute

    make install

To customize the location set the PREFIX variable. The default
location is /usr/local.

    make install PREFIX=~/my/stuff

The install target will copy magot.h to $PREFIX/include and libmagot.a
and libmagot.so to $PREFIX/lib.

If you have pandoc installed you can convert the readme to html with

    make doc

To remove all generated files execute

    make clean

### CFLAGS

Note that by default the CFLAGS don't specify any optimizations. If
you want an optimized build just set the appropriate CFLAGS

    make CFLAGS=-O2

Magot makes liberal use of assertions. If you're confident that it
doesn't break just compile with NDEBUG if you are sure that you want
to disable them (not recommended)

    make CFLAGS='-O2 -DNDEBUG'

Conversely, if you're debugging with gdb set the -g flag to improve
your debugging experience

    make CFLAGS=-g

## Use?

[Do whatever the fuck you want to][1]. But don't come whining to me if
it breaks (and it will). Good luck.

[1]: http://www.wtfpl.net/
