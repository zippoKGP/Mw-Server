/*
**      XMODULE.CPP
**      Modules install,active,free functions.
**
**      ZJian,2000.7.8.
*/
#include "rays.h"
#include "xmodule.h"


#define MAX_MODULE      64



static  MODULE_FREE_FUNC   free_module[MAX_MODULE];
static  MODULE_ACTIVE_FUNC active_module[MAX_MODULE];


//===========================================================================================================
EXPORT  void    FNBACK  init_modules(void)
{
   int i;
   for (i=0;i<MAX_MODULE;i++)
      free_module[i]=NULL,active_module[i]=NULL;
}



EXPORT  int     FNBACK  install_module(MODULE_INIT_FUNC init,MODULE_FREE_FUNC free,MODULE_ACTIVE_FUNC active)
{
   int i;
   
   if (free_module[MAX_MODULE-1]) return -1;
   if (init()) return -1;
   for (i=0;free_module[i];i++) 
       continue;
   if (free) free_module[i]=free;
   if (active) 
   {
      for (i=0;active_module[i];i++)
          continue;
      active_module[i]=active;
   }
   return 0;
}


EXPORT  void    FNBACK  free_modules(void)
{
   int i;
   for (i=0;free_module[i] && i<MAX_MODULE;i++)
       continue;
   for (--i;i>=0;--i) 
   {
      free_module[i]();
      free_module[i]=NULL,active_module[i]=NULL;
   }
}


EXPORT  void    FNBACK  active_modules(int bActive)
{
   int i;
   for (i=0;i<MAX_MODULE;i++) 
   {
      if (active_module[i]) 
         active_module[i](bActive);
      else
         break;
   }
}

//===========================================================================================================

