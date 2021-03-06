
/*
 * Copyright (C) 2013 Frank James
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

#include "error.h"

type_error *errors () {
	return error_p;
}

void error (char *message, char *location) {
	type_error *e = gc_malloc (sizeof(type_error));
	if (message != NULL) {
		e->message = gc_new_string(message);
	} else {
		e->message = gc_new_string("");
	}

	if (location != NULL) {
		e->location = gc_new_string(location);
	} else {
		e->location = gc_new_string("");
	}
	
	e->next = error_p;
	error_p = e;
}

void error_clear () {
	error_p = NULL;
}

void print_errors () {
  type_error *err;
  type_cell *errs;

  err = errors();
  if (err != NULL) {
    while (err != NULL) {
      errs = cons (err->message, cons (err->location, NULL));
      format_ ("Error: ~A at ~A~%", errs);
      err = err->next;
    }
  } 
}

