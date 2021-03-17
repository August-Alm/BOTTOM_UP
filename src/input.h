/* ***** ***** */

#ifndef INPUT_H
#define INPUT_H

/* ***** ***** */

struct input_handle;

int read_char(struct input_handle *h);

int consume_space(struct input_handle *h);

/* ***** ***** */

struct file_handle;

struct input_handle *input_from_file(struct file_handle *fh);

void init_file_handle(struct file_handle *fh, FILE *fp);

/* ***** ***** */

struct string_handle;

struct input_handle *input_from_string(struct string_handle *sh);

void init_string_handle(struct string_handle *sh, const char *str, int sz);

/* ***** ***** */

#endif // INPUT_H
