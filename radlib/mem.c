/* This file is part of GNU RADIUS.
   Copyright (C) 2000,2001 Sergey Poznyakoff
  
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifndef lint
static char rcsid[] = 
"$Id$";
#endif

#define RADIUS_MODULE_MEM_C

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <sysdep.h>
#include <mem.h>
#include <log.h>
#include <pthread.h>

typedef struct entry *Entry;
struct entry {
	Entry next;
};

typedef union bucket *Bucket;

typedef struct bucketclass *Bucketclass;
struct bucketclass {
	Bucketclass next;            /* Link to next class */
	int         cont;            /* Allow contiguous allocation */
	size_t      elsize;          /* Size of an element */
	count_t     elcnt;           /* Number of elements per bucket */
	count_t     allocated_cnt;   /* Number of allocated elements */
	count_t     bucket_cnt;      /* Number of buckets */
	Bucket first;                /* Entry to the bucket chain */
	Entry free;                  /* Entry to the list of free elements */
};

union bucket {
	struct {
		Bucketclass class;
		Bucket next;
	} s;
	Align_t align;
};

Bucketclass bucket_class;
static pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;
static int mem_recursion_level = 0;

static void
mem_lock()
{
	if (mem_recursion_level == 0)
		pthread_mutex_lock(&mem_mutex);
	mem_recursion_level++;
}

static void
mem_unlock()
{
	if (--mem_recursion_level == 0)
		pthread_mutex_unlock(&mem_mutex);
}

Bucketclass alloc_class(size_t size);
Bucket alloc_bucket(Bucketclass);
void *alloc_page();
static void put_back(Bucketclass, void *);
static void put_back_cont(Bucketclass, void *, count_t);

#define ENTRY_SIZE sizeof(struct entry)
#define CLASS_SIZE sizeof(struct bucketclass)
#define BUCKET_SIZE sizeof(union bucket)

#define ENTRY(b, n) \
  	(Entry)((char*)((Bucket)b+1) + (b)->s.class->elsize * (n))
#define bucket_capacity(size) \
        (count_t)((MEM_PAGESIZE-BUCKET_SIZE+(size)-1)/(size)-1)

Bucketclass
alloc_class_mem()
{
	if (!bucket_class) {
		/* bootstrap the whole shmeer */
		struct bucketclass class = {
			(Bucketclass)0,  /* next */
			0,		 /* cont */
			CLASS_SIZE,
			bucket_capacity(CLASS_SIZE),
			0,
			0,
			(Bucket)0,
			(Entry)0
		};
		
		Bucket bp = alloc_bucket(&class);
		bucket_class = &class; /* to fake alloc_entry() */
		bucket_class = alloc_entry(CLASS_SIZE);
		bp->s.class = bucket_class;
		
		bucket_class->first = bp;
		bucket_class->free = class.free;
		bucket_class->bucket_cnt = class.bucket_cnt;
		bucket_class->next = (Bucketclass)0;
		bucket_class->elsize = CLASS_SIZE;
		bucket_class->elcnt = bucket_capacity(CLASS_SIZE);
	}
	return alloc_entry(CLASS_SIZE);
}

#define free_class free_entry

Bucketclass
alloc_class(size)
	size_t size;
{
	Bucketclass bcp;
	count_t count;

	count = bucket_capacity(size);
	if (count < 2) {
		radlog(L_ERR,
		       _("Page too small to alloc %d-byte objects"),
			size);
		return 0;
	}
	if ((bcp = alloc_class_mem()) == (Bucketclass)0) {
		radlog(L_ERR, _("can't allocate bucket class"));
		return 0;
	}

	bcp->elsize = size;
	bcp->elcnt = count;
	bcp->bucket_cnt = 0;
	bcp->allocated_cnt = 0;
	
	if (alloc_bucket(bcp) == (Bucket)0) {
		free_class(bcp);
		return 0;
	}

	bcp->next = bucket_class;
	bucket_class = bcp;
	return bcp;
}


Bucket
alloc_bucket(class)
	Bucketclass class;
{
	Bucket bp;
	union {
		Entry ep;
		char *cp;
	} ptr;
	Entry prev, end_ptr;

	if ((bp = alloc_page()) == (Bucket)0) {
		radlog(L_ERR, _("can't allocate bucket page"));
		return 0;
	}

	bp->s.class = class;
	bp->s.next = class->first;
	class->first = bp;

	if (class->cont) {
		put_back_cont(class, ENTRY(bp, 0), class->elcnt);
		class->allocated_cnt += class->elcnt;
	} else {
		end_ptr = ENTRY(bp, class->elcnt);
		prev = ENTRY(bp, 0);
		for (ptr.ep = ENTRY(bp, 1); ptr.ep < end_ptr;
		     ptr.cp += class->elsize) {
			prev->next = ptr.ep;
			prev = ptr.ep;
		}
		prev->next = class->free;
		class->free = ENTRY(bp, 0);
	}
	
	class->bucket_cnt++;

	debug(1, ("Bucket count for class %d is %d", 
			class->elsize, class->bucket_cnt));
	return bp;
}

/* ************************************************************************** */
/*                          Interface routines.                             
 */

/* Routines for allocation/deallocation of arbitrary memory objects
 */

void *
alloc_entry(size)
	size_t size;
{
	Bucketclass class_ptr;
	Entry ptr;

	mem_lock();

	if (size < ENTRY_SIZE)
		size = ENTRY_SIZE;
	for (class_ptr = bucket_class; class_ptr; class_ptr = class_ptr->next) 
		if (class_ptr->elsize == size && !class_ptr->cont) 
			break;
	if (!class_ptr) {
		class_ptr = alloc_class(size);
		if (!class_ptr) {
			radlog(L_ERR, _("low core: exiting"));
			abort();
		}
	}
	
	if (!class_ptr->free) {
		if (!alloc_bucket(class_ptr)) {
			radlog(L_ERR, _("low core: exiting"));
			abort();
		}
	}

	class_ptr->allocated_cnt++;
	ptr = class_ptr->free;
	class_ptr->free = ptr->next;
	mem_unlock();

	bzero(ptr, class_ptr->elsize);
	return ptr;
}

void
put_back(class, ptr)
	Bucketclass class;
	void *ptr;
{
	Entry eptr = (Entry)ptr;

	class->allocated_cnt--;
	insist(class->allocated_cnt <= class->bucket_cnt * class->elcnt);
	bzero(ptr, class->elsize);
	eptr->next = class->free;
	class->free = eptr;
}

void
free_entry(ptr)
	void *ptr;
{
	Bucketclass class;
	Bucket bucket;

	mem_lock();
	for (class = bucket_class; class; class = class->next)
		for (bucket = class->first; bucket; bucket = bucket->s.next) 
			if (ptr >= (void*)ENTRY(bucket, 0) && 
				ptr < (void*)ENTRY(bucket, class->elcnt)) {
				if (class->cont)
					put_back_cont(class, ptr, 1);
				else
					put_back(class, ptr);
				mem_unlock();
				return;
			}
	mem_unlock();
	debug(1, ("attempt to free an alien pointer (%p)", ptr));
}

/* ************************************************************************* */
/* Functions for dealing with contiguous memory objects
 */

/* Allocate COUNT contiguous objects of size SIZE
 */
void *
calloc_entry(count, size)
	count_t count;
	size_t size;
{
	Bucketclass class_ptr;
	Entry       first, last;
	Entry       pre_first, pre_last;
	size_t      found;
	int         dead_loop;

	/* Fix-up entry size if less than minimum */
	if (size < ENTRY_SIZE)
		size = ENTRY_SIZE;
	mem_lock();
	/* Find the appropriate class */
	for (class_ptr = bucket_class; class_ptr; class_ptr = class_ptr->next) 
		if (class_ptr->cont && class_ptr->elsize == size) 
			break;
	if (!class_ptr) { 
		/* allocate one if not found */
		class_ptr = alloc_class(size);
		if (!class_ptr) {
			radlog(L_ERR, _("low core: exiting"));
			abort();
		}
		class_ptr->cont = 1;
	}

	/* It is a fatal error to request more than elcnt objects.	
	 * One should be careful when selecting the pagesize
	 */
	if (count > class_ptr->elcnt) {
		radlog(L_CRIT, 
		       _("calloc_entry(): too many contiguous objects requested (%d/%d)"),
		       count, class_ptr->elcnt);
		abort();
	}

	/* Allocation loop. We try twice:
	 * At the first pass we try to find `count' contiguous objects
 	 * from the buckets already allocated. If we fail, we allocate
	 * one more bucket and retry. It is obviously no use trying to
	 * fix something if we fail on second pass.
	 * Variables are used as follows:
	 * 	first		-	points to the first element in the
	 *				sequence being tried.
	 *	last		-	points to (currently) last element
	 *				in the sequence being tried.
	 *	pre_first	-	is the element preceeding first.
	 *	pre_last	-	is the element preceeding last.
	 *	found		-	number of contiguous elements counted
	 *				so far from `first'.
	 *	dead_loop	-	counter preventing dead loops. 
	 */

	dead_loop = 0;

again:	
	first = class_ptr->free;
	found = 0;
	last  = first;
	pre_first  = pre_last = NULL;
	while (last) {
		if (++found == count)
			break;
		pre_last = last;
		last = last->next;
		if (((char*)last - (char*)pre_last) != class_ptr->elsize) {
			first = last;
			pre_first  = pre_last;
			found = 0;
			/* start again */
		}
	}

	if (found != count) {
		if (dead_loop++) {
			radlog(L_CRIT, _("calloc_entry(): dead loop detected"));
			abort();
		}
		if (!alloc_bucket(class_ptr)) {
			radlog(L_ERR, _("low core: exiting"));
			abort();
		}
		goto again;
	}

	class_ptr->allocated_cnt += count;
	if (pre_first)	
		pre_first->next = last->next;
	else
		class_ptr->free = last->next;
	mem_unlock();
	bzero(first, class_ptr->elsize * count);
	return first;
}
	
/* Free COUNT contiguous objects of size SIZE. The objects should have been
 * allocated using calloc_entry().
 */
void
cfree_entry(ptr, count)
	void *ptr;
	count_t count;
{
	Bucketclass class;
	Bucket bucket;

	mem_lock();
	for (class = bucket_class; class; class = class->next)
		for (bucket = class->first; bucket; bucket = bucket->s.next) 
			if (ptr >= (void*)ENTRY(bucket, 0) && 
				ptr < (void*)ENTRY(bucket, class->elcnt)) {
				put_back_cont(class, ptr, count);
				mem_unlock();
				return;
			}
	mem_unlock();
	debug(1, ("attempt to free an alien pointer (%p)", ptr));
}

void
put_back_cont(class, ptr, count)
	Bucketclass class;
	void *ptr;
	count_t count;
{
	int   i;
	Entry sptr = (Entry)ptr;
	Entry eptr;
	Entry this_p, prev_p;

	class->allocated_cnt -= count;
	bzero(ptr, class->elsize * count);

	/* Link the objects being freed into a list */
	eptr = sptr;
	for (i = 1; i < count; i++) {
		eptr->next = (Entry)((char*)eptr + class->elsize);
		eptr = eptr->next;
	}

	/* Now, sptr and eptr point to the first and last elements of
	 * the list to be put back into the available list.
	 */

	/* Find first available element with address numerically greater than
	 * the address of the last element
	 */
	prev_p = NULL;	
	for (this_p = class->free; this_p; this_p = this_p->next) {
		if (eptr < this_p) 
			break;
		prev_p = this_p;
	}

	/* Attach the list to the available list
	 */
	if (!prev_p) {
		eptr->next = class->free;
		class->free = sptr;
	} else {
		eptr->next = prev_p->next;
		prev_p->next = sptr;
	}
}

/* ************************************************************************* */
/* String allocation
 */

typedef union {
	struct {
	unsigned char nblk;      /* number of allocated blocks */
	unsigned char nref;      /* number of references */
	} s;
	Align_t align;
} STRHDR;
#define HDRSIZE sizeof(STRHDR)

#define BLKCNT(length) ((length + HDRSIZE + MINSTRSIZE - 1) / (MINSTRSIZE - 1))

/* alloc_string(): Allocate a string of given length
 */
char *
alloc_string(length)
	size_t length;
{
	count_t count;
	STRHDR  *p;
	
	count = BLKCNT(length);
	if (count > 255) {
		radlog(L_CRIT, _("alloc_string(): string too long"));
		abort();
	}
	p = calloc_entry(count, MINSTRSIZE);
	p->s.nref = 1;
	p->s.nblk = count;
	return (char*)(p + 1);
}

/* make_string(): create a string object capable of holding the
 * STRVAL and store the latter into it.
 */
char *
make_string(strval)
	char *strval;
{
	char *p = alloc_string(strlen(strval)+1);
	return strcpy(p, strval);
}

/* dup_string(): create a copy of existing string object
 */
char *
dup_string(str)
	char *str;
{
	STRHDR *hp;

	if (!str)
		return NULL;
	hp = (STRHDR*)str - 1;
	if (hp->s.nref == 255) /* FIXME: use limits.h */
		return make_string(str);
	hp->s.nref++;
	return str;
}

/* replace_string(): replace a string value of the given string object
 * with the new string value.
 */
char *
replace_string(str, strvalue)
	char **str;
	char *strvalue;
{
	int length = strlen(strvalue);
	STRHDR *hp;

	if (!*str)
		return *str = make_string(strvalue);
	
	hp = (STRHDR*)*str - 1;
	if ( hp->s.nref > 1 || hp->s.nblk < BLKCNT(length) ) {
		free_string(*str);
		*str = alloc_string(length + 1);
	}
	strcpy(*str, strvalue);
	return *str;
}

/* free_string(): Decrement reference counter of the given string object
 * and deallocate the object when the counter becomes zero
 */
void
free_string(str)
	char *str;
{
	STRHDR *hp;

	if (!str)
		return;
	
	hp = (STRHDR*)str - 1;
	if (--hp->s.nref)
		return;
	cfree_entry(hp, hp->s.nblk);
}

/* ************************************************************************* */
/* Statistics/debugging routines
 */

void
mem_get_stat(stat)
	MEM_STAT *stat;
{
	Bucketclass class;
	
	stat->class_cnt = 0;
	stat->bucket_cnt = 0;
	stat->bytes_allocated = 0;
	stat->bytes_used = 0;
	
	for (class = bucket_class; class; class = class->next) {
		stat->class_cnt++;
		stat->bucket_cnt += class->bucket_cnt;
		stat->bytes_allocated +=
			class->bucket_cnt * class->elcnt * class->elsize;
		stat->bytes_used  +=  class->allocated_cnt * class->elsize;
		/* sanity check */
		insist(class->allocated_cnt <= class->bucket_cnt * class->elcnt);	
#ifdef MAINTAINER_MODE
		check_cont(class);
#endif
	}
}

int
mem_stat_enumerate(fun, closure)
	int (*fun)();
	void *closure;
{
	CLASS_STAT stat;
	Bucketclass class;
	
	stat.index = 0;
	for (class = bucket_class; class; class = class->next, stat.index++) {
		stat.elsize = class->elsize;
		stat.cont = class->cont;
		stat.elcnt = class->elcnt;
		stat.bucket_cnt = class->bucket_cnt;
		stat.allocated_cnt = class->allocated_cnt;
		if ((*fun)(&stat, closure))
			return 1;
	}
	return 0;
}

#ifdef MAINTAINER_MODE
int
check_cont(class)
	Bucketclass class;
{
	Entry ep, prev;
	
	if (class->cont) {
		ep = class->free;
		prev = NULL;
		while (ep) {
			if (prev && ep < prev) {
				radlog(L_CRIT,
				       "CLASS %d CONTAINS ERRORS",
				       class->elsize);
				break;
			}
			prev = ep;
			ep = ep->next;
		}
	}
}
#endif
