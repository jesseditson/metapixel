/*
 * lispreader.h
 *
 * Copyright (C) 1998-2004 Mark Probst
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __LISPREADER_H__
#define __LISPREADER_H__

#include <stdio.h>

#include <allocator.h>

#define LISP_STREAM_MMAP_FILE  1
#define LISP_STREAM_STRING     2
#define LISP_STREAM_FILE       3
#define LISP_STREAM_ANY        4

#define LISP_LAST_MMAPPED_STREAM   LISP_STREAM_STRING

#define LISP_TYPE_INTERNAL      -3
#define LISP_TYPE_PARSE_ERROR   -2
#define LISP_TYPE_EOF           -1
#define LISP_TYPE_NIL           0
#define LISP_TYPE_SYMBOL        1
#define LISP_TYPE_INTEGER       2
#define LISP_TYPE_STRING        3
#define LISP_TYPE_REAL          4
#define LISP_TYPE_CONS          5
#define LISP_TYPE_PATTERN_CONS  6
#define LISP_TYPE_BOOLEAN       7
#define LISP_TYPE_PATTERN_VAR   8

#define LISP_PATTERN_ANY        1
#define LISP_PATTERN_SYMBOL     2
#define LISP_PATTERN_STRING     3
#define LISP_PATTERN_INTEGER    4
#define LISP_PATTERN_REAL       5
#define LISP_PATTERN_BOOLEAN    6
#define LISP_PATTERN_LIST       7
#define LISP_PATTERN_OR         8
#define LISP_PATTERN_NUMBER     9

typedef struct
{
    int type;

    union
    {
	FILE *file;
	struct
	{
	    char *buf;
	    char *end;
	    char *pos;
	} mmap;
        struct
	{
	    void *data;
	    int (*next_char) (void *data);
	    void (*unget_char) (char c, void *data);
	} any;
    } v;
} lisp_stream_t;

typedef struct _lisp_object_t lisp_object_t;
struct _lisp_object_t
{
    int type;

    union
    {
	struct
	{
	    struct _lisp_object_t *car;
	    struct _lisp_object_t *cdr;
	} cons;

	char *string;
	int integer;
	float real;

	struct
	{
	    int type;
	    int index;
	    struct _lisp_object_t *sub;
	} pattern;
    } v;
};

lisp_stream_t* lisp_stream_init_path (lisp_stream_t *stream, const char *path);
lisp_stream_t* lisp_stream_init_file (lisp_stream_t *stream, FILE *file);
lisp_stream_t* lisp_stream_init_string (lisp_stream_t *stream, char *buf);
lisp_stream_t* lisp_stream_init_any (lisp_stream_t *stream, void *data, 
				     int (*next_char) (void *data),
				     void (*unget_char) (char c, void *data));

void lisp_stream_free_path  (lisp_stream_t *stream);

lisp_object_t* lisp_read_with_allocator (allocator_t *allocator, lisp_stream_t *in);
lisp_object_t* lisp_read (lisp_stream_t *in);

void lisp_free_with_allocator (allocator_t *allocator, lisp_object_t *obj);
void lisp_free (lisp_object_t *obj);

lisp_object_t* lisp_read_from_string_with_allocator (allocator_t *allocator, const char *buf);
lisp_object_t* lisp_read_from_string (const char *buf);

int lisp_compile_pattern (lisp_object_t **obj, int *num_subs);
int lisp_match_pattern (lisp_object_t *pattern, lisp_object_t *obj, lisp_object_t **vars, int num_subs);
int lisp_match_string (const char *pattern_string, lisp_object_t *obj, lisp_object_t **vars);

int lisp_type (lisp_object_t *obj);
int lisp_integer (lisp_object_t *obj);
float lisp_real (lisp_object_t *obj);
char* lisp_symbol (lisp_object_t *obj);
char* lisp_string (lisp_object_t *obj);
int lisp_boolean (lisp_object_t *obj);
lisp_object_t* lisp_car (lisp_object_t *obj);
lisp_object_t* lisp_cdr (lisp_object_t *obj);

lisp_object_t* lisp_cxr (lisp_object_t *obj, const char *x);

lisp_object_t* lisp_make_integer_with_allocator (allocator_t *allocator, int value);
lisp_object_t* lisp_make_real_with_allocator (allocator_t *allocator, float value);
lisp_object_t* lisp_make_symbol_with_allocator (allocator_t *allocator, const char *value);
lisp_object_t* lisp_make_string_with_allocator (allocator_t *allocator, const char *value);
lisp_object_t* lisp_make_cons_with_allocator (allocator_t *allocator, lisp_object_t *car, lisp_object_t *cdr);
lisp_object_t* lisp_make_boolean_with_allocator (allocator_t *allocator, int value);

lisp_object_t* lisp_make_integer (int value);
lisp_object_t* lisp_make_real (float value);
lisp_object_t* lisp_make_symbol (const char *value);
lisp_object_t* lisp_make_string (const char *value);
lisp_object_t* lisp_make_cons (lisp_object_t *car, lisp_object_t *cdr);
lisp_object_t* lisp_make_boolean (int value);

int lisp_list_length (lisp_object_t *obj);
lisp_object_t* lisp_list_nth_cdr (lisp_object_t *obj, int index);
lisp_object_t* lisp_list_nth (lisp_object_t *obj, int index);

void lisp_dump (lisp_object_t *obj, FILE *out);

#define lisp_nil()           ((lisp_object_t*)0)

#define lisp_nil_p(obj)      (obj == 0)
#define lisp_integer_p(obj)  (lisp_type((obj)) == LISP_TYPE_INTEGER)
#define lisp_real_p(obj)     (lisp_type((obj)) == LISP_TYPE_REAL)
#define lisp_symbol_p(obj)   (lisp_type((obj)) == LISP_TYPE_SYMBOL)
#define lisp_string_p(obj)   (lisp_type((obj)) == LISP_TYPE_STRING)
#define lisp_cons_p(obj)     (lisp_type((obj)) == LISP_TYPE_CONS)
#define lisp_boolean_p(obj)  (lisp_type((obj)) == LISP_TYPE_BOOLEAN)

#endif
