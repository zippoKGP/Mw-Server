/*
**      KISS.CPP
**      kiss? no! keep it simple,stupid.
**      ZJian,2000.10.02.
**
**      all functions in these file can be used anywhere and they
**      are all independency of your engine.
**      i write these functions with smile and happy, and i wish
**      so to you.
**
*/
#ifndef _XKISS_H_INCLUDE_
#define _XKISS_H_INCLUDE_       1


#define char_upper(ch)  ( ((ch)>='a' && (ch)<='z') ? ('A'+(ch)-'a') : (ch) )
#define char_lower(ch)  ( ((ch)>='A' && (ch)<='Z') ? ('a'+(ch)-'A') : (ch) )


//STRING FUNCTIONS
EXPORT  SLONG   FNBACK  find_word_clip_position(char *str, SLONG start, SLONG len);
EXPORT  SLONG   FNBACK  find_word_reserved_last_position(char *str, SLONG reserved_len);
EXPORT  char *  FNBACK  word_upper(char *str);
EXPORT  char *  FNBACK  word_lower(char *str);  
EXPORT  char *  FNBACK  delete_last_word(char *str);
EXPORT  SLONG   FNBACK  get_buffer_line(char *buffer, SLONG *buffer_index, SLONG buffer_size, char *line, SLONG line_size);
EXPORT  SLONG   FNBACK  get_string(USTR *out,SLONG out_max,USTR *in,SLONG *in_from,
                                   USTR *end_list,SLONG end_len);
EXPORT  void    FNBACK  skip_compartment(USTR *in,SLONG *in_from,USTR *skip_list,SLONG skip_len);


//FILE & DIRECTORY FUNCTIONS
EXPORT  void    FNBACK  change_file_extension(USTR *dest_filename,USTR *src_filename,USTR *extension);
EXPORT  SLONG   FNBACK  is_file_exist(USTR *filename);
EXPORT  SLONG   FNBACK  is_directory_exist(USTR *path);
EXPORT  SLONG   FNBACK  make_directory(USTR *path);
EXPORT  SLONG   FNBACK  remove_directory(USTR *path);
EXPORT  USTR *  FNBACK  get_pure_filename(USTR *filename);
EXPORT  USTR *  FNBACK  get_file_extension(USTR *filename);
EXPORT  USTR *  FNBACK  get_file_path(USTR *path_fname);
EXPORT  USTR *  FNBACK  get_nopath_filename(USTR *path_fname);
EXPORT  USTR *  FNBACK  make_full_filename(USTR *path,USTR *filename,USTR *fileext);
EXPORT  USTR *  FNBACK  adjust_file_path(USTR *path);
EXPORT  void    FNBACK  oper_whole_path(USTR *spath,USTR *tpath);


//DATA COMPUTE FUNCTIONS
EXPORT  void    FNBACK  run_random_init(void);
EXPORT  ULONG   FNBACK  checksum_ulong_table(ULONG *table, ULONG length);
EXPORT  void    FNBACK  switch_data_flag(ULONG *data,ULONG flag);
EXPORT  SLONG   FNBACK  update_data_bit(ULONG *data, ULONG mask, SLONG flag);
EXPORT  UCHR    FNBACK  compute_uchar_chksum(USTR *buffer, SLONG size);
EXPORT  void    FNBACK  pack_uchar(USTR *buffer, UCHR data, SLONG *index);
EXPORT  void    FNBACK  pack_uhint(USTR *buffer, UHINT data, SLONG *index);
EXPORT  void    FNBACK  pack_ulong(USTR *buffer, ULONG data, SLONG *index);
EXPORT  UCHR    FNBACK  unpack_uchar(USTR *buffer, SLONG *index);
EXPORT  UHINT   FNBACK  unpack_uhint(USTR *buffer, SLONG *index);
EXPORT  ULONG   FNBACK  unpack_ulong(USTR *buffer, SLONG *index);


//TIME ABOUT FUNCTIONS
EXPORT  void    FNBACK  get_time(USTR *buffer);



// update_data_bits flag ------------------------------------------------------
typedef enum    UPDATE_DATA_BIT_FLAG_ENUMS
{   UDB_CHK_BIT     =   1,  
UDB_SET_BIT         =   2,  
UDB_GET_BIT         =   3,  
UDB_ADD_BIT         =   4,  
UDB_DEC_BIT         =   5,  
UDB_SWT_BIT         =   6   
} UPDATE_DATA_BIT_FLAG;

//-------------------------------------------------------------------------

#endif//_XKISS_H_INCLUDE_
