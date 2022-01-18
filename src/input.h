/* ***** ***** */

#ifndef INPUT_H
#define INPUT_H

/* ***** ***** */

#include <stdio.h>

/* ***** ***** */

struct input_handle;

int current_line(struct input_handle *h);

int current_column(struct input_handle *h);

int peek_char(struct input_handle *h);

int read_char(struct input_handle *h);

int read_nonspace_char(struct input_handle *h);

/* ***** ***** */

struct file_handle;

struct input_handle *input_from_file(struct file_handle *fh);

struct file_handle *new_file_handle(FILE *fp);

void free_file_handle(struct file_handle *fh);

/* ***** ***** */

struct string_handle;

struct input_handle *input_from_string(struct string_handle *sh);

struct string_handle *new_string_handle(char *str);

void free_string_handle(struct string_handle *sh);

/* ***** ***** */

#endif // INPUT_H