/*
**    PACKFILE.H
**    pack file functions header.
**
**    Jack, 2002.4.23.
*/
#ifndef _PACKFILE_H_
#define _PACKFILE_H_


//DEFINES /////////////////////////////////////////////////////////////////////////////
#define PACK_FILE_VERSION       0x00010000
#define PACK_FILE_COPYRIGHT     MAKEFOURCC('R','A','Y','S')
#define PACK_FILE_ID            MAKEFOURCC('P','A','K',' ')


#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif//_SIZE_T_DEFINED


//STRUCTURES //////////////////////////////////////////////////////////////////////////
typedef struct  tagDATA_FILE_HEAD
{
    ULONG   copyright;
    ULONG   id;
    ULONG   version;
    ULONG   head_size;
    //
    SLONG   total_files;
} DATA_FILE_HEAD, *LPDATA_FILE_HEAD;


typedef struct  tagFILE_INFO
{
    ULONG   id;         //32 bit file id.
    ULONG   offset;     //file offset from the start of data file.
    ULONG   len;        //length of original decompressed file.
} FILE_INFO, *LPFILE_INFO;


typedef struct  tagDATA_FILE
{
   FILE         *file;          //really file pointer
   SLONG        total_files;    //total packed files in this data file.
   FILE_INFO    file_info[1];   //packed file information.
} DATA_FILE,*LPDATA_FILE;


#pragma pack(push)
#pragma pack(2)
typedef struct  tagPACK_FILE
{
   FILE     *file;      // data file pointer
   ULONG    offset;     // offset of the first block, 0 for single file
   ULONG    len;        // file length
   ULONG    pos;        // file data pointer(for decompressed data)
   USTR     *buffer;    // read or write buffer(64K size)
   ULONG    data_pos;   // data position(for compressed data file)
   UHINT    block[1];   // length for each block
} PACK_FILE,*LPPACK_FILE;
#pragma pack(pop)



//FUNCTIONS ///////////////////////////////////////////////////////////////////////////
EXPORT  DATA_FILE*  FNBACK  open_data_file(char *filename);
EXPORT  void        FNBACK  close_data_file(DATA_FILE *df);
EXPORT  void        FNBACK  set_data_file(DATA_FILE *df);
//
EXPORT  PACK_FILE *  FNBACK  pack_fopen(const char *filename,const char *mode);
EXPORT  int          FNBACK  pack_fclose(PACK_FILE *pack_file);
EXPORT  int          FNBACK  pack_fseek(PACK_FILE *pack_file,long seek_offset,int seek_type);
EXPORT  unsigned int FNBACK  pack_fread(void *buffer,unsigned int block_size,unsigned int block_number,PACK_FILE *pack_file);
EXPORT  unsigned int FNBACK  pack_fwrite(const void *buffer,unsigned int block_size,unsigned int block_number,PACK_FILE *pack_file);
EXPORT  long         FNBACK  pack_fsize(PACK_FILE *pack_file);
EXPORT  long         FNBACK  pack_ftell(PACK_FILE *pack_file);
//
EXPORT  ULONG   FNBACK  filename_to_id(USTR *name);
                                       


#endif//_PACKFILE_H_
