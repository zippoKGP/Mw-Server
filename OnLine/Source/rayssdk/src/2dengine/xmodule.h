/*
**    XMODULE.H
**    module functions header.
**    ZJian,2000/7/10.
*/
#ifndef _XMODULE_H_INCLUDE_
#define _XMODULE_H_INCLUDE_      1


#define install(a)   install_module(init_##a,free_##a,active_##a)



typedef int  (*MODULE_INIT_FUNC)(void);
typedef void (*MODULE_FREE_FUNC)(void);
typedef void (*MODULE_ACTIVE_FUNC)(int bActive);



EXPORT   void  FNBACK   init_modules(void);
EXPORT   int   FNBACK   install_module(MODULE_INIT_FUNC init,MODULE_FREE_FUNC free,MODULE_ACTIVE_FUNC active);
EXPORT   void  FNBACK   free_modules(void);
EXPORT   void  FNBACK   active_modules(int bActive);

#endif//_XMODULE_H_INCLUDE_
