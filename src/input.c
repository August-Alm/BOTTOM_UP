/* ***** ****** */

#include "input.h"
#include <stdlib.h>
#include <ctype.h>

/* ***** ****** */

typedef int (*read_t)(struct input_handle *);

struct input_handle
{
    read_t read;
	int line;
	int column;
    int peek;
};

int current_line(struct input_handle *h)
{
    return h->line;
}

int current_column(struct input_handle *h)
{
    return h->column;
}

/* ***** ****** */

int peek_char(struct input_handle *h)
{
    return h->peek;
}

int read_char(struct input_handle *h)
{
	read_t read = h->read;
	int c = h->peek;
	while (c == '\r') {
		c = read(h);
		if (c != '\n') {
			fprintf(
                stderr,
			    "Error at (%d, %d). "
			    "Line and column numbers expect '\r' to always be followed by '\n'.",
			    h->line, h->column
            );
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
        c = h->peek;
	}
	h->column++;
    h->peek = read(h);
	return c;
}

int read_nonspace_char(struct input_handle *h) {
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

struct file_handle *new_file_handle(FILE *fp)
{
    struct file_handle *fh = malloc(sizeof(struct file_handle));
    fh->base.read = read_from_file;
	fh->base.line = 1;
	fh->base.column = 1;
    fh->base.peek = fgetc(fp);
    fh->fp = fp;
}

void free_file_handle(struct file_handle *fh)
{
    fclose(fh->fp);
    free(fh);
}

/* ***** ***** */

struct string_handle
{
    struct input_handle base;
	char *string;
    int position;
};

struct input_handle *input_from_string(struct string_handle *sh)
{
    return &sh->base;
}

int read_from_string(struct input_handle *h)
{
    struct string_handle *sh = (struct string_handle*)h;
    char c = (sh->string)[(sh->position)];
	if (c == '\0') {
		return -1;
	}
    sh->position++;
	return c;
}

struct string_handle *new_string_handle(char *str)
{
    struct string_handle *sh = malloc(sizeof(struct string_handle)); 
	sh->base.read = read_from_string;
	sh->base.line = 1;
	sh->base.column = 1;
    sh->base.peek = *str;
	sh->string = str;
	sh->position = 1;
    return sh;
}

void free_string_handle(struct string_handle *sh)
{
    free(sh->string);
    free(sh);
}

/* ***** ****** */

// end of input.c