/*
**      PACKFILE.CPP
**      Packfile functions.
**
**      ZJian,2000.7.11
*/
#include "rays.h"
#include "xsystem.h"
#include "packfile.h"
#include "lzo.h"


#define FILE_BUFFER_NUM     8


typedef struct  tagFILE_BUFFER 
{
	PACK_FILE   *pack_file;
	USTR        *buffer;
	SLONG       freq;
} FILE_BUFFER, *LPFILE_BUFFER;


//VARIABLES ///////////////////////////////////////////////////////////////////////////////////////////////
FILE_BUFFER file_buffer[FILE_BUFFER_NUM] = { {NULL,NULL,0} };
char        virtual_path[_MAX_PATH] = "/\0";
DATA_FILE   *data_file = NULL;


//STATIC FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
static  void    adjust_path(char *s);
static  void    adjust_filename(char *s);
static  void    change_vitual_directory(char *s);
static  USTR*   get_buffer(PACK_FILE *pack_file);
static  void    read_block(USTR *buf, SLONG block, PACK_FILE *pack_file);


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS                                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//FUNCTION  :   open a data file and set it the current data file.
//PARAMETER :   char *filename    -> data file filename.
//RETURN    :   DATA_FILE *       -> pointer of the data file.
EXPORT  DATA_FILE*  FNBACK  open_data_file(char *filename)
{
    DATA_FILE_HEAD  head;
    DATA_FILE * df;
    FILE    *fp;
    SLONG   packed_files, i;

    /* 
    **  Open data file.
    */
    if(NULL == (fp = fopen((const char *)filename, "rb")))
        return  NULL;

    /*
    **  Read data file head, which include a count number(total_files) 
    **  which specify how many packed files in this data file.
    */
    if(sizeof(head) != fread(&head, 1, sizeof(head), fp))
    {
        log_error(1, "read head error of file %s", filename);
        goto some_error;
    }
    if(head.copyright != PACK_FILE_COPYRIGHT)
    {
        log_error(1, "error copyright of file %s", filename);
        goto some_error;
    }
    if(head.id != PACK_FILE_ID)
    {
        log_error(1, "error file id of file %s", filename);
        goto some_error;
    }
    if(head.version != PACK_FILE_VERSION)
    {
        log_error(1, "error file version %s", filename);
        goto some_error;
    }
    packed_files = head.total_files;

    /*
    **  Create a DATA_FILE pointer.
    **  Read packed files information to this DATA_FILE pointer.
    **
    */
    df = (DATA_FILE*)malloc(sizeof(*df) + sizeof(df->file_info[0]) * (packed_files-1));
    df->file = fp;
    df->total_files = packed_files;
    fread(df->file_info, 1, packed_files * sizeof(df->file_info[0]), fp);

    /*
    **  Initialize file buffer for later use.
    **
    */
    if (file_buffer[0].buffer == NULL)
    {
        file_buffer[0].buffer = (USTR*)malloc(0x10000l * FILE_BUFFER_NUM);
        for (i=1; i<FILE_BUFFER_NUM; i++)
        {
            file_buffer[i].pack_file = NULL;
            file_buffer[i].buffer = file_buffer[0].buffer + 0x10000l * i;
        }
    }

    /*
    **  Set current data_file to this opened data_file.
    */
    data_file = df;
    return  (DATA_FILE *)df;


some_error:
    if(fp) fclose(fp);
    return  NULL;
}


//FUNCTION  :   close an opened data file.
//PARAMETER :   DATA_FILE *df       -> data file pointer to be closed.
//RETURN    :   (none)
EXPORT  void    FNBACK  close_data_file(DATA_FILE *df)
{
    if(NULL == df)
        return;

	if (df==data_file) 
    {
		int i;

		data_file = NULL;
		for (i=0;i<FILE_BUFFER_NUM;i++)
        {
			if (file_buffer[i].pack_file) goto _out;
        }
		free(file_buffer[0].buffer);
		file_buffer[0].buffer = NULL;
	}
_out:
	fclose(df->file);
	free(df);
}


//FUNCTION  :   set current data file.
//PARAMETER :   DATA_FILE *df   -> data file pointer.
//RETURN    :   (none)
EXPORT  void    FNBACK  set_data_file(DATA_FILE *df)
{
    data_file = df;
}


//FUNCTION  :   open a packed file from the current data file.
//PARAMETER :   const char *filename    -> the file's filename to be opened.
//              const char *mode        -> access mode.
//RETURN    :   PACK_FILE *             -> pack file pointer.
EXPORT  PACK_FILE*  FNBACK  pack_fopen(const char *filename,const char *mode)
{
    PACK_FILE *pack_file = NULL;
    FILE    *file = NULL;
    char    name[_MAX_FNAME];
    int     e, b, m;
    ULONG   id, sid;
    
    strcpy(name, filename);

    // Jack, error when process chinese file path. [10:12,9/11/2002]
    //strlwr((char *)name);
    CharLower((LPSTR)name);

    //Jack, 2002.4.25.
    //adjust_filename(name);

    /*
    **  If exist the same name file out of the data_file, we will read it directly.
    **  So, it will be very useful for our update. :-)
    */
    file = fopen(name, mode);
    if(file)
    {
        /* 
        ** Exist the same name file out of the data file 
        */
        pack_file = (PACK_FILE *)malloc(sizeof(PACK_FILE));
        pack_file->file = file;
        fseek(file, 0L, SEEK_END);
        pack_file->len = ftell(file);
        pack_file->offset = 0;
        pack_file->buffer = NULL;
        fseek(file, 0L, SEEK_SET);
    }
    else
    {   
        /* 
        **  There is no any same name file out of the data file 
        */
        if (! data_file)
            return NULL;

        /*
        ** find matched id(filename).
        */
        b = 0;
        e = data_file->total_files - 1;
        id = filename_to_id((USTR*)name);
        while(b <= e) 
        {
            m = (b + e)/2;
            sid = data_file->file_info[m].id;

            if (id == sid) goto _found_id;
            if (id < sid) e = m - 1;
            else b = m + 1;
        }
        return NULL;

_found_id:

        b = (data_file->file_info[m].len + 0xffff) >> 16; // blocks
        pack_file = (PACK_FILE*)malloc(sizeof(PACK_FILE) + (b-1) * sizeof(UHINT));
        pack_file->file = data_file->file;
        pack_file->offset = data_file->file_info[m].offset + b * sizeof(UHINT);
        pack_file->len = data_file->file_info[m].len;
        fseek(data_file->file, data_file->file_info[m].offset, SEEK_SET);
        fread(pack_file->block, 1, sizeof(UHINT) * b, data_file->file);
        pack_file->data_pos = ftell(data_file->file);
        pack_file->buffer = NULL;
    }

    pack_file->pos = 0;
    return(pack_file);
}



//FUNCTION  :   close an opened pack file.
//PARAMETER :   PACK_FILE *pack_file    ->  pack file pointer.
//RETURN    :   always 0.
EXPORT  int     FNBACK  pack_fclose(PACK_FILE *pack_file)
{
    if(!pack_file) return 0;
    if(pack_file->buffer)
    {
		file_buffer[(pack_file->buffer - file_buffer[0].buffer) >> 16].pack_file = NULL;
    }
    if(pack_file->offset == 0)
    {
        fclose(pack_file->file);
    }
    free(pack_file);
    return(0);
}


//FUNCTION  :   seek the access offset of the pack file.
//PARAMETER :   PACK_FILE *pack_file    -> pack file pointer.
//              long seek_offset        -> seek offset.
//              int seek_type           -> seek type(SEEK_CUR, SEEK_END, SEEK_SET).
//RETURN    :   always 0.
EXPORT  int     FNBACK  pack_fseek(PACK_FILE *pack_file,long seek_offset,int seek_type)
{
    long new_pos = 0;

    if(0 == pack_file->offset)
    {
        fseek(pack_file->file,seek_offset,seek_type);
        return 0;
    }
    switch(seek_type)
    {
    case SEEK_CUR:
        new_pos = pack_file->pos + seek_offset;
        break;
    case SEEK_END:
        new_pos = pack_file->len - 1 + seek_offset;
        break;
    case SEEK_SET:
        new_pos = seek_offset;
        break;
    default:
        new_pos = pack_file->pos;
        break;
    }
    if(new_pos < 0)
    {
        new_pos = 0;
    }
    else if((unsigned)new_pos >= pack_file->len)
    {
        new_pos = pack_file->len-1;
    }
    
    if (pack_file->pos >> 16 == (unsigned)new_pos >> 16)
    {
        pack_file->pos = new_pos;
    }
    else 
    {
        int i,block;
        pack_file->pos = new_pos;
        pack_file->buffer = NULL;
        block = new_pos>>16;
        for (i=0, pack_file->data_pos = pack_file->offset; i<block; i++)
        {
            if(pack_file->block[i] == 0)
                pack_file->data_pos += 0x10000l;
            else
                pack_file->data_pos += pack_file->block[i];
        }
    }
    return 0;
}


//FUNCTION  :   read data from the opened pack file.
//PARAMETER :   void *buffer            -> buffer for store the read data.
//              int block_size          -> each block size.
//              int block_number        -> total block number.
//              PACK_FILE *pack_file    -> pack file pointer read from.
//RETURN    :   unsigned int            -> read size of bytes.
//PS        :   strongly recommend use block_size = 1.
EXPORT  unsigned int    FNBACK  pack_fread(void *buffer,unsigned int block_size,unsigned int block_number,PACK_FILE *pack_file)
{
    SLONG   size;
	SLONG   block, read_size;
	SLONG   block_pos;
	USTR    *buf;

	buf = (USTR*)buffer;
    
    /*
    **  Zero(0) offset for an exist out-of-pack file.
    */
    if(0 == pack_file->offset)
    {
        read_size = fread(buf, block_size, block_number, pack_file->file);
        return  read_size;
    }

    size = block_size * block_number;
	if (pack_file->pos + size > pack_file->len) 
        read_size = size = pack_file->len - pack_file->pos;
	else 
        read_size = size;
    if(read_size <= 0)
        return  0;

    /*
    **  Find the exactly block(each block size = 0x10000) and the position from the block.
    */
	block = pack_file->pos >> 16;
	block_pos = pack_file->pos & 0xffff;

	if (block_pos)
    {/* It's not the begin of the block. */

        /*  Read this block if we didn't ready the pack_file's buffer.
        */
		if (NULL == pack_file->buffer)
        {
			get_buffer(pack_file);
			read_block(pack_file->buffer, block, pack_file);
		}

        /*  If the range of we wanted read is included by this block, all we need to do
        **  is get what we want and return.
        */
		if (block_pos + size <= 0x10000)
        {
			memcpy(buf, pack_file->buffer + block_pos, size);
			pack_file->pos += size;
			if ((pack_file->pos & 0xffff) == 0)
            {
                if(pack_file->block[block] == 0)
                    pack_file->data_pos += 0x10000l;
                else
                    pack_file->data_pos += pack_file->block[block];
            }
			return (ULONG)size;
		}

        /*  Read range is out of this block, so, we need to read to the end of this block, and
        **  later the other blocks.
        */
		memcpy(buf, pack_file->buffer + block_pos, 0x10000 - block_pos);
		buf += 0x10000 - block_pos;
        if(pack_file->block[block] == 0)
            pack_file->data_pos += 0x10000l;
        else
            pack_file->data_pos += pack_file->block[block];

		pack_file->pos = (++block) << 16;
		size -= (0x10000-block_pos);
	}

    /*  Now, read the whole blocks which cover the read range.
    */
	while (size > 0xffff)
    {
		read_block(buf, block, pack_file);
		buf += 0x10000;

        if(pack_file->block[block] == 0)
            pack_file->data_pos += 0x10000l;
        else
            pack_file->data_pos += pack_file->block[block];
            
		pack_file->pos = (++block) << 16;
		size -= 0x10000;
	}

	if (size == 0)
        return (ULONG)read_size;

    /*  Read the tail block datas.
    */
	get_buffer(pack_file);
	read_block(pack_file->buffer, block, pack_file);
	memcpy(buf, pack_file->buffer, size);

	pack_file->pos += size;

	return (ULONG)read_size;
}


//FUNCTION  :   write data to a pack file.
//PARAMETER :   void *buffer                -> data buffer for write.
//              unsigned int block_size     -> each block size.
//              unsigned int block_number   -> total block number.
//              PACK_FILE *pack_file        -> pack file pointer write to.
//RETURN    :   unsigned int                -> write bytes.
//PS        :   this function can only write data to a direct(out of .pak) pack file now.
EXPORT  unsigned int    FNBACK  pack_fwrite(const void *buffer,unsigned int block_size,unsigned int block_number,PACK_FILE *pack_file)
{
    unsigned int write_bytes = 0;

    if(0 == pack_file->offset)
        return fwrite(buffer,block_size,block_number,pack_file->file);
    
    return(write_bytes);
}


//FUNCTION  :   get the size of bytes of the pack file.
//PARAMETER :   PACK_FILE *pack_file    -> pack file pointer.
//RETURN    :   long                    -> file size of bytes.
EXPORT  long    FNBACK  pack_fsize(PACK_FILE *pack_file)
{
    return((long)pack_file->len);
}


//FUNCTION  :   tell the current access offset of the pack file.
//PARAMETER :   PACK_FILE *pack_file    -> pack file pointer.
//RETURN    :   long                    -> current access offset.
EXPORT  long    FNBACK  pack_ftell(PACK_FILE *pack_file)
{
	if (pack_file->offset == 0) 
        return ftell(pack_file->file);
	return pack_file->pos;
}


//FUNCTION  :   convert a string filename to a 32bit ID.
//PARAMETER :   USTR *name          -> filename
//RETURN    :   ULONG               -> 32 bit ID of the filename.
EXPORT  ULONG   FNBACK  filename_to_id(USTR *name)
{
    ULONG   s;
    SLONG   i;

    for (s=i=0; name[i]; i++)
       s = (s+(i+1)*name[i]) % 0x8000000bu * 0xffffffefu;
    return s^0x12345678;
}
//////////////////////////////////////////////////////////////////////////////////////////////
// STATIC FUNCTIONS                                                                         //
//////////////////////////////////////////////////////////////////////////////////////////////
//FUNCTION  :   adjust path, convert to lower case, process "..", change '\' to '/'.
static  void    adjust_path(char *s)
{
    int     i;
    char    tmp[_MAX_PATH], *p;

    // Jack. [10:12,9/11/2002]
    //strlwr(s);  //change to lower case chars.
    CharLower((LPSTR)s);

    for (i=0,p=s; *p; ++p,++i)
    {
        if (*p=='\\')   //change '\' to '/'
        {
            *p='/';
        }
        else if (*p=='.' && *(p-1)=='/') 
        {
            if (*(p+1)=='.' && (*(p+2)=='/' || *(p+2)=='\\' || *(p+2)==0)) 
            {
                // process '..'
                for (i -= 2; i>=0; i--)
                {
                    if (tmp[i]=='/') 
                        break;
                }
                if (i<0) i+=2;
                p += 2;
            }
        }
        else
        {
            tmp[i] = *p;
        }
    }
    tmp[i] = 0;
    strcpy(s, tmp);
}


//FUNCTION  :   adjust filename, insert virtual_path control.
static  void    adjust_filename(char *s)
{
	char tmp[_MAX_PATH];

	if (s[0]=='/' || s[0]=='\\') 
    {
        strcpy(tmp,s);
    }
	else 
    {
		strcpy(tmp, virtual_path);
		strcat(tmp, s);
	}
	strcpy(s, tmp);
	adjust_path(tmp);
	strcpy(s, tmp+1);
}


//FUNCTION  :   change virtual directory.
static  void    change_vitual_directory(char *s)
{
	if (s[0]=='/' || s[0]=='\\') 
    {
        strcpy(virtual_path, s);
    }

	strcat(virtual_path, s);
	adjust_path(virtual_path);
	if (virtual_path[strlen(virtual_path)-1] != '/')
    {
		strcat(virtual_path,"/");
    }
}


static  USTR *  get_buffer(PACK_FILE *pack_file)
{
	SLONG   i, s, *freq, buf, p = 0;

    /*
    **  If the pack_file has data buffer, just update file_buffers' frequence.
    **  We will set the last used file_buffer's frequence as the largest number.
    */
	if (pack_file->buffer)
    {
		freq = &file_buffer[(pack_file->buffer - file_buffer[0].buffer) >> 16].freq;
		for (s=i= 0;i<FILE_BUFFER_NUM;i++)
        {
			if (file_buffer[i].pack_file && file_buffer[i].freq > *freq)
            {
                --file_buffer[i].freq;
                ++s;
            }
        }
		*freq += s;
		return  pack_file->buffer;
	}

    /*
    **  Seek for an unused file buffer for our pack_file.
    */
	for (s=i=0;i<FILE_BUFFER_NUM;i++)
    {
		if (file_buffer[i].pack_file) 
            ++s;
		else 
            buf = i;
    }

    /*
    **  If there is any unused file_buffer, we use it.
    */
	if (s < FILE_BUFFER_NUM)
    {
		file_buffer[buf].pack_file = pack_file;
		file_buffer[buf].freq = s;
        pack_file->buffer = file_buffer[buf].buffer;

		return  pack_file->buffer;
	}

    /*
    **  When all file_buffers are used by other pack_files, we will use the pioneer file_buffer.
    */
	for (i=0; i<FILE_BUFFER_NUM; i++)
    {
		if (file_buffer[i].freq == 0 && p == 0)
        {
            file_buffer[i].freq = FILE_BUFFER_NUM - 1;
            file_buffer[i].pack_file = pack_file;
            pack_file->buffer = file_buffer[i].buffer;
            p = 1;
        }
		else if (file_buffer[i].freq != 0) 
        {
            --file_buffer[i].freq;
        }
    }

	return pack_file->buffer;
}



static  void read_block(USTR *buf, SLONG block, PACK_FILE *pack_file)
{
	static unsigned char compress_buf[0x10000];

	fseek(pack_file->file, pack_file->data_pos, SEEK_SET);
	if (pack_file->block[block] == 0)
    {
		fread(buf, 1, 0x10000, pack_file->file);
	}
    else
    {
        fread(compress_buf, 1, pack_file->block[block], pack_file->file);
        lzo_decompress (compress_buf, pack_file->block[block], buf);
    }
}





//=======================================================================
