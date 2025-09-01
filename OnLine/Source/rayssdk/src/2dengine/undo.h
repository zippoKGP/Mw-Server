/*
**      UNDO.H
**      undo functions header.
**      ZJian,2000.09.28.
*/
#ifndef UNDO_H_INCLUDE
#define UNDO_H_INCLUDE      1

#define MAX_UNDO_RECORDS    1024
#define MAX_UNDO_TIMES      (MAX_UNDO_RECORDS/2)

typedef struct  tagUNDO_HEAD
{
    ULONG   copyright;  
    ULONG   id;
    ULONG   version;
    ULONG   head_size;
    ULONG   data_size;      // data size of all undo records 
    SLONG   read_index;     // index of undo pointer
    SLONG   write_index;    // index of do pointer
    SLONG   offset[MAX_UNDO_RECORDS];   // undo record offset
} UNDO_HEAD,*LPUNDO_HEAD;

typedef struct  tagUNDO_RECORD
{
    ULONG   type;
    ULONG   size;
    USTR    data[4];
} UNDO_RECORD,*LPUNDO_RECORD;



EXPORT  UNDO_RECORD *   FNBACK  create_undo_record(SLONG data_size);
EXPORT  void            FNBACK  destroy_undo_record(UNDO_RECORD **ur);
EXPORT  SLONG           FNBACK  create_undo_file(USTR *filename);
EXPORT  SLONG           FNBACK  flush_undo_file(USTR *filename);
EXPORT  SLONG           FNBACK  process_undo_file_with_do(USTR *filename,UNDO_RECORD *undo_record);
EXPORT  SLONG           FNBACK  process_undo_file_with_undo(USTR *filename,UNDO_RECORD **undo_record);
EXPORT  SLONG           FNBACK  process_undo_file_with_redo(USTR *filename,UNDO_RECORD **undo_record);


#endif//UNDO_H_INCLUDE
