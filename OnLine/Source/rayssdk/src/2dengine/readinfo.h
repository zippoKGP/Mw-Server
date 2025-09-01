/*
**      READINFO.H
**      read .ini information functions header.
**      ZJian,2000/10/02.
*/
#ifndef READINFO_H_INCLUDE
#define READINFO_H_INCLUDE      1

EXPORT  USTR *  FNBACK  get_ini_information(USTR *group,USTR *keyword,USTR *value);
EXPORT  SLONG   FNBACK  load_ini_information(USTR *filename);
EXPORT  void    FNBACK  free_ini_information(void);
EXPORT  void    FNBACK  list_ini_information(void);

#endif//READINFO_H_INCLUDE