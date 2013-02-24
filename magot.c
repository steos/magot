#include <magot.h>

#include <string.h>
#include <stdbool.h>
#include <assert.h>

void magot_init(magot_t *opt, char *name, bool flag, bool required){
  assert(!flag || !required);
  opt->name = name;
  opt->flag = flag;
  opt->required = required;
  opt->value = NULL;
}

void magot_init_flag(magot_t *opt, char *name) {
  magot_init(opt, name, true, false);
}

void magot_init_opt(magot_t *opt, char *name, bool required) {
  magot_init(opt, name, false, required);
}

magot_t *find_opt(int optc, magot_t **opts, char *name) {
  for (int i = 0; i < optc; ++i) {
    if (strcmp(name, opts[i]->name) == 0) {
      return opts[i];
    }
  }
  return NULL;
}

char *errmsg(char *msg, char *name) {
  char *buf = malloc(strlen(msg) + strlen(name));
  strcat(buf, msg);
  strcat(buf, name);
  return buf;
}

bool magot_parse(magot_err_t *err, int argc, char **argv, int optc, ...) {
  va_list vargs;
  va_start(vargs, optc);
  magot_t *optv[optc];
  for (int i = 0; i < optc; ++i) {
    optv[i] = va_arg(vargs, magot_t*);
  }
  va_end(vargs);

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
