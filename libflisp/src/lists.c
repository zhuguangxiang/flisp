
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

#include "lists.h"


type_cell *cons (void *car, void *cdr) {
	type_cell *c = gc_new_cell();

	c->car = car;
	c->cdr = cdr;

    return c;
}

void *cell_car(type_cell *cell) {
	gc_type t = get_type (cell);
	if (t == TYPE_NULL) {
		return NULL;
	} else if (t == TYPE_CELL) {
		return cell->car;
	} else {
		error("Not a cons cell", "CAR");
		return NULL;
	}
}

void *cell_cdr(type_cell *cell) {
	gc_type t = get_type (cell);
	if (t == TYPE_NULL) {
		return NULL;
	} else if (t == TYPE_CELL) {
		return cell->cdr;
	} else {
		error("Not a cons cell", "CDR");
		return NULL;
	}
}

void *cell_caar(type_cell *cell) {
	return cell_car(cell_car(cell));
}

void *cell_cddr(type_cell *cell) {
	return cell_cdr(cell_cdr(cell));
}

void *cell_cdar(type_cell *cell) {
	return cell_cdr(cell_car(cell));
}

void *cell_cadr(type_cell *cell) {
	return cell_car(cell_cdr(cell));
}

void set_car(type_cell *cell, void *val) {
	cell->car = val;
}

void set_cdr(type_cell *cell, void *val) {
	cell->cdr = val;
}

void cell_push(type_cell **place, void *val) {
	gc_type t = get_type (*place);
	if (t == TYPE_NULL || t == TYPE_CELL) {
		(*place) = cons(val, *place);
	} else {
		error ("Can't push onto something not a list", "CELL-PUSH");
	}
}

void *cell_pop(type_cell **place) {
    void *ret;
	gc_type t = get_type(*place);
	if (t == TYPE_NULL) {
		ret = NULL;
	} else if (t == TYPE_CELL) {
		ret = (*place)->car;
		*place = cell_cdr(*place);
	} else {
		error ("Can't pop from something not a list", "CELL-POP");
	}
    return ret;
}


/* this does essentially cons(cons(key, val), alist) */
type_cell *acons (void *key, void *val, type_cell *alist) {
	type_cell *c = gc_new_cell();
	type_cell *d = gc_new_cell();
	
	c->car = key;
	c->cdr = val;
    
	d->car = c;
	d->cdr = alist;

	return d;
}

void *assoc (void *key, type_cell *alist) {
	type_cell *c;
	
	while (alist != NULL) {
        c = cell_car(alist);
        if (c == NULL) {
            break;
        }
		if (eql(key, c->car)) {
            return c;
		}
		
		alist = alist->cdr;
	}
    
	return NULL;
}

type_cell *mapcar (void *(*proc)(void *), type_cell *args) {
	type_cell *ret = NULL;

	while (args != NULL) {
		if (get_type(args) == TYPE_CELL) {
			cell_push (&ret, (proc)(args->car));
			args = cell_cdr(args);
		} else {
			error ("Must be a proper list", "MAPCAR");
			break;
		}
	}

	return ret;
}

type_cell *copy_list (type_cell *l) {
	type_cell *ret, **builder;
	gc_type t;
	
	ret = NULL;
	builder = &ret;
	
	t = get_type (l);
	if (t == TYPE_NULL) {
		/* empty */
		ret = NULL;
	} else if (t == TYPE_CELL) {
		while (l != NULL) {
			if (get_type(l) != TYPE_CELL) {
				*builder = l;
				break;
			} else {
				*builder = cons (cell_car (l), NULL);
				builder = (type_cell **)&((*builder)->cdr);
			}
			l = l->cdr;
		}
	} else {
		error ("Can't copy a non-list", "COPY-LIST");
	}
	return ret;
}

type_cell *append (type_cell *lists) {
    type_cell *c, **builder;
    type_cell *tmp;
    gc_type t;

    c = NULL;
    builder = &c;
    while (lists != NULL) {
        /* safety check for dotted list */
        if (get_type(lists) != TYPE_CELL) {
        /* dotted list */
          error ("Can only append lists", "APPEND");
          return NULL;
        } else {
            tmp = cell_car(lists);
            t = get_type(tmp);
            if (t != TYPE_CELL && t != TYPE_NULL) {
                /* package it into a list. maybe shouldn't do this */
                *builder = cons (tmp, NULL);
                builder = (type_cell **)&((*builder)->cdr);
            } else {
                *builder = copy_list (tmp);
                while (*builder != NULL) {
                    if (get_type (*builder) != TYPE_CELL) {
                        /* dotted list */
                        error ("Cannot append dotted lists", "APPEND");
                        return NULL;
                    }
                    builder = (type_cell **)&((*builder)->cdr);
                }
            }
        }
        lists = lists->cdr;
    }
    return c;
}
