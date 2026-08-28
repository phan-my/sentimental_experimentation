/*
 * texts.h is the headerfile for texts.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef TEXTS_H
#define TEXTS_H

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// functions
char *_concat_2(const char *s1, const char *s2);
char *concat(int count, ...);

#endif /* TEXTS_H */
