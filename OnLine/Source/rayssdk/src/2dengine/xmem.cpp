/*
**  xmem.cpp
**  memory control functions.
**
**  geaan, 2001.8.6.
*/
#include "rays.h"
#include "xsystem.h"


typedef struct  tagMEM_LINK     MEM_LINK, *LPMEM_LINK;
typedef struct  tagMEM_STAT     MEM_STAT, *LPMEM_STAT;

struct  tagMEM_LINK
{
    void        *mem_ptr;
    signed      mem_size;
    MEM_LINK    *next;
};


struct  tagMEM_STAT
{
    signed  total_size;
    signed  alloc_times;
    signed  realloc_times;
    signed  free_times;
};


MEM_LINK   *mem_link = NULL;
MEM_STAT    mem_stat;


/*
static  void    init_mem_link(MEM_LINK *mem_link)
static  SLONG   insert_mem_link(MEM_LINK *head, MEM_LINK *ins);
static  MEM_LINK *  create_mem_link(void *p, SLONG size);
*/


void *  pack_malloc(size_t size)
{
    void *  p;

    p = GlobalAlloc(GPTR, size);

    return p;
}


void    pack_free(void *p)
{
    GlobalFree(p);
}


void *  pack_realloc(void *p, size_t size)
{
    p = GlobalReAlloc(p, size, 0);
    return p;
}


int     init_xmem(void)
{
    return  0;
}

void    free_xmem(void)
{
}

void    active_xmem(int active)
{
}
