/* ***** ****** */

#include <ctype.h>
#include <stdio.h>
#include "input.h"

/* ***** ****** */

typedef int (*read_t)(struct input_handle *);

struct input_handle
{
    read_t read;
	int line;
	int column;
};

/* ***** ****** */

int read_char(struct input_handle *h)
{
	read_t read = h->read;
	int c = read(h);
	while (c == '\r') {
		c = read(h);
		if (c != '\n') {
			fprintf(stderr,
			        "Error at (%d, %d). "
			        "Line and column numbers expect '\r' to always be followed by  '\n'.",
					h->line, h->column);
			h->column += 2;
			return c;
		}
		h->line++;
		h->column = 0;
		c = read(h);
	}
	while (c == '\n') {
		h->line++;
		h->column = 0;
		c = read(h);
	}
	h->column++;
	return c;
}

int consume_space(struct input_handle *h) {
	int c = read_char(h);
	while (isspace(c)) {
		c = read_char(h);
	}
	return c;
}

/* ***** ***** */

struct file_handle
{
    struct input_handle base;
    FILE *fp;
};

struct input_handle *input_from_file(struct file_handle *fh)
{
    return &fh->base;
}

int read_from_file(struct input_handle *h)
{
    struct file_handle *fh = (struct file_handle*)h;
    return fgetc(fh->fp);
}

void init_file_handle(struct file_handle *fh, FILE *fp)
{
    fh->base.read = read_from_file;
	fh->base.line = 1;
	fh->base.column = 1;
    fh->fp = fp;
}

/* ***** ***** */

struct string_handle
{
    struct input_handle base;
	const char *string;
    int size;
    int position;
};

struct input_handle *input_from_string(struct string_handle *sh)
{
    return &sh->base;
}

int read_from_string(struct input_handle *h)
{
    struct string_handle *sh = (struct string_handle*)h;
	if (sh->position == sh->size) {
		return -1;
	}
	return (sh->string)[sh->position++];
}

void init_string_handle(struct string_handle *sh, const char *str, int sz)
{
	sh->base.read = read_from_string;
	sh->base.line = 1;
	sh->base.column = 1;
	sh->string = str;
	sh->size = sz;
	sh->position = 0;
}

/* ***** ****** */

// end of input.c
