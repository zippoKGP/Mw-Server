/*
**      FUNCTION.H
**      functions header.
**      ZJian,2000/09/11.
*/
#ifndef FUNCTION_H_INCLUDE
#define FUNCTION_H_INCLUDE      1
#include "xgrafx.h"

typedef enum FUNCTION_RESULT_ENUMS
{ DISPLAY_MESSAGE_TO_SCREEN     =   1,
DISPLAY_MESSAGE_TO_BUFFER       =   0
} FUNCTION_RESULT;

EXPORT  void    FNBACK  show_help_info(char **help_string,BMP *bit_map);
EXPORT  void    FNBACK  display_message16(USTR *message,SLONG flag,BMP *bitmap);
EXPORT  SLONG   FNBACK  ask_yes_no(USTR *title,USTR *yes,USTR *no,BMP *bitmap);
EXPORT  SLONG   FNBACK  ask_yes_no_extend(USTR *title,USTR *yes,USTR *no,BMP *bitmap);
EXPORT  void    FNBACK  redraw_current_path(BMP *bitmap);
EXPORT  SLONG   FNBACK  select_popup_menu(SLONG sx,SLONG sy,USTR *options,BMP *bitmap);
EXPORT  SLONG   FNBACK  load_file_to_buffer(USTR *filename, USTR **buffer);
EXPORT  SLONG   FNBACK  get_buffer_number(USTR *buffer, SLONG *index);
EXPORT  USTR *  FNBACK  get_buffer_string(USTR *buffer, SLONG *index);




#endif//FUNCTION_H_INCLUDE