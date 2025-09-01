/*
**  xmem.h
**  memory control functions header.
**
**  geaan, 2001.8.6.
*/
#ifndef _XMEM_H_
#define _XMEM_H_


void *  pack_malloc(size_t size);
void    pack_free(void *p);
void *  pack_realloc(void *p, size_t size);

int     init_xmem(void);
void    active_xmem(int active);
void    free_xmem(void);


#endif//_XMEM_H_
