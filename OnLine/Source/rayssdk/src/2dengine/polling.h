/*
**      POLLING.H
**      polling functions header.
**      ZJian,2000/09/06.
*/
#ifndef POLLING_H_INCLUDE
#define POLLING_H_INCLUDE       1

EXPORT  SLONG   FNBACK  init_polling_task(SLONG index);
EXPORT  SLONG   FNBACK  append_polling_task(SLONG index,void (*func)());
EXPORT  SLONG   FNBACK  delete_polling_task(SLONG index,void (*func)());
EXPORT  SLONG   FNBACK  exec_polling_task(SLONG index);
EXPORT  SLONG   FNBACK  clear_polling_task(SLONG index);
EXPORT  SLONG   FNBACK  insert_polling_task(SLONG index,void (*func)(),void (*pos_func)());

#endif//POLLING_H_INCLUDE
