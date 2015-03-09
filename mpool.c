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

#include "mpool.h"

#include <malloc.h>
#include <stdio.h>

struct _mpool
{
	void *start; /* the beginning of allocated memory */
	void *pos; /* will be returned by next mpool_alloc() call */
	void *end;
};

const int mpool_prealloc_size = 10240; // 10K
mpool mpool_new()
{
	mpool pool = (mpool) malloc(sizeof(struct _mpool));
	if(!pool) return NULL;

	pool->start = malloc(mpool_prealloc_size);
	pool->pos = pool->start;
	pool->end = pool->start + mpool_prealloc_size;

	return pool;
}

/* Destroy a memory pool and free everything allocated using this pool */
void mpool_free(mpool pool)
{
	if(pool && pool->start)
	{
		free(pool->start);
		pool->start = NULL; /* double-free protection */
	}
}

/* Allocate some memory.
	NOTE: this memory can't be freed (will be released only after mpool is destroyed) */
void *mpool_alloc(mpool pool, int size)
{
	while(pool->pos + size > pool->end)
	{
		int newsize = pool->end - pool->start + mpool_prealloc_size;
		void *newptr = realloc(pool->start, newsize);
		if(!newptr)
		{
			fprintf(stderr, "MPOOL failed to extend: mpool_alloc() failed.\n");
			return NULL;
		}
		pool->start = newptr;
		pool->end = newptr + newsize;
	}

	void *ret = pool->pos;
	pool->pos += size;
	return ret;
}
