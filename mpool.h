/*
	Formula manager - the mathematical library.
	Copyright (C) 2010 Edward Chernenko.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

#ifndef _MPOOL_H
#define _MPOOL_H

typedef struct _mpool *mpool;

/* Create a memory pool */
mpool mpool_new() __attribute__((malloc warn_unused_result));

/* Destroy a memory pool and free everything allocated using this pool */
void mpool_free(mpool pool);

/* Allocate some memory.
	NOTE: this memory can't be freed (will be released only after mpool is destroyed) */
void *mpool_alloc(mpool pool, int size) __attribute__((malloc warn_unused_result nonnull(1)));
#define mpool_malloc mpool_alloc

#endif
