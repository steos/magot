# What?

*M*agot *a*in't *g*et*o*p*t*.

Magot is a minimal library for command line argument parsing. The main
objective is to provide basic argument handling capabilities with a
minimal amount of setup code.

The API is not finalized therefore the soname will _not_ change on BC
breaks until it is (if ever).  No warranty. No support. Use at own
risk.

The code is strict C99 and should build fine with any compliant C
compiler without any warnings.  The Makefile targets GNU Make >= 3.80.

Suggestions, contributions and flames welcome.

## Why?

Getopt makes me want to gouge my eyes out.

## How?

    #include <magot.h>
    int main(int argc, char **argv) {
      magot_t foo, bar, baz;
        magot_t *opts[] = {
        magot_init_opt(&foo, "-f", true, "foo option"),
        magot_init_opt(&bar, "-b", false, "bar option"),
        magot_init_flag(&baz, "-z", "baz flag")
      };
      int optc = sizeof(opts) / sizeof(opts[0]);
      magot_err_t err;
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

## Use?

Do whatever the fuck you want to under the terms of the
[WTFPL][1]. But don't come whining to me if it breaks (and it
will). Good luck.


[1]: http://www.wtfpl.net/
