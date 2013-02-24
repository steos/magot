#include <magot.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  magot_t foo, bar, baz;
  magot_init_opt(&foo, "-f", true);
  magot_init_opt(&bar, "-b", false);
  magot_init_flag(&baz, "-z");

  magot_err_t err;
  memset(&err, 0, sizeof(magot_err_t));
  bool success = magot_parse(&err, argc, argv, 3, &foo, &bar, &baz);
  if (!success) {
    switch (err.type) {
    case MAGOT_ERR_UNKNOWN_OPT:
      printf("option '%s' is unknown\n", err.arg);
      break;
    case MAGOT_ERR_MISSING_REQUIRED:
      printf("missing required option '%s'\n", err.arg);
      break;
    case MAGOT_ERR_MISSING_ARG:
      printf("missing argument for option '%s'\n", err.arg);
      break;
    }
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
