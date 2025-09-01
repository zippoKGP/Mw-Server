/*
**      XKISS.CPP
**      kiss? no! Keep It Simple,Stupid.
**
**      ZJian,2000.10.02.
**          all functions in these file can be used anywhere and they
**          are all independency of your engine.
**          i write these functions with smile and happy, and i wish
**          so to you.
**
*/
#include "rays.h"
#include "xkiss.h"

//////////////////////////////////////////////////////////////////////////////////////////
//          STRING FUNCTIONS                                                            //
//////////////////////////////////////////////////////////////////////////////////////////
//功能 : 寻找中英文混杂的字串的分割点
//参数 : char *str          -> 目标字串
//       SLONG start        -> 起点位置
//       SLONG len          -> 从起点位置开始的长度
//返回 : 如果到达字串结尾，返回-1，否则，返回分割点的位置。
EXPORT  SLONG   FNBACK  find_word_clip_position(char *str, SLONG start, SLONG len)
{
    int len2;
    char *current, *end, *next;

    //如果字串长度小于或者等于起点位置，则表示已经到达字串结束，返回-1.
    len2 = strlen((const char *)str);
    if(len2 <= start)
        return  -1;

    current = str + start;
    end = str + min( start + len, len2 );
    while(current < end)
    {
        next = CharNext(current);
        if(next > end) //表示刚跳过的是汉字(2 bytes)
            break;
        else if(next == end) //表示刚跳过的是字母(1 byte)
        {
            current = next;
            break;
        }
        else
        {
            current = next;
        }
    }
    return  current - str;
}


//功能 : 寻找在中英文混杂的字串中保留末尾一定长度字节时, 文字分割点的位置.
//参数 : char * str             -> 字串
//       SLONG  reserved_len    -> 保留长度
//返回 : SLONG                  -> 文字分割点的位置
EXPORT  SLONG   FNBACK  find_word_reserved_last_position(char *str, SLONG reserved_len)
{
    int len;
    char *tail, *last;

    //如果字串的长度比保留长度小，则直接返回0。
    len = strlen((const char *)str);
    if(len <= reserved_len)
        return  0;

    //依次从末尾开始往前回溯，直到达到保留长度。
    tail = str + len;
    last = tail;
    while(tail - last < reserved_len)
    {
        last = CharPrev(str, last);
    }
    return  last - str;
}


//功能 : 将中英文混杂的字串转换成为大写.
//参数 : char *str      -> 欲转换的字串, 将存放转换后的结果.
//返回 : char *         -> 转换后的结果字串指标
EXPORT  char *  FNBACK  word_upper(char *str)
{
    return CharUpper(str);
}


//功能 : 将中英文混杂的字串转换成为小写.
//参数 : char *str      -> 欲转换的字串, 将存放转换后的结果.
//返回 : char *         -> 转换后的结果字串指标
EXPORT  char *  FNBACK  word_lower(char *str)
{
    return  CharLower(str);
}


//功能 : 删除字串末尾的字(汉字或者字母)
//参数 : char *str      -> 待操作的字串.
//返回 : char *         -> 结果字串.
EXPORT  char*   FNBACK  delete_last_word(char *str)
{
    SLONG   len;
    char *next, *last;

    len = strlen((const char *)str);
    next = str + len;
    last = CharPrev(str, next);
    last[0] = 0;
    return  str;
}



EXPORT  SLONG   FNBACK  get_buffer_line(char *buffer, SLONG *buffer_index, SLONG buffer_size, 
                                        char *line, SLONG line_size)
{
    SLONG   i, index, len, end;

    index = *buffer_index;
    if(index >= buffer_size)
    {
        len = 0;
        line[len] = 0x00;
        return  TTN_NOT_OK;
    }

    end = index;
    for(i=index; i<buffer_size;i++)
    {
        if(buffer[i] == 0x0d)   //line end: 0x0d,0x0a
        {
            end = i;
            i = i+1;
            if(i < buffer_size)
            {
                if(buffer[i] == 0x0a)
                    i = i+1;
            }
            break;
        }
    }
    len = min(end-index, line_size);
    if(len > 0)
        memcpy(line, &buffer[index], len);
    line[len]=0x00;
    index = i;
    *buffer_index = index;
    return  TTN_OK;
}


EXPORT  SLONG   FNBACK  get_string(USTR *out,SLONG out_max,USTR *in,SLONG *in_from,
                                   USTR *end_list,SLONG end_len)
{
    SLONG offset=-1;    // set offset of get string to -1 for the first do process
    SLONG i;    // temp variable

    --(*in_from);   // dec (*in_from) for the first do process
    do
    {
        out[++offset]=in[++(*in_from)];
        for(i=end_len-1;i>=0;--i)
        {
            if(out[offset]==end_list[i])
            {
                out[offset]=0x00;
                break;
            }
        }
    }while(out[offset] && offset<out_max);
    return offset;
}

EXPORT  void    FNBACK  skip_compartment(USTR *in,SLONG *in_from,USTR *skip_list,SLONG skip_len)
{
    SLONG i;    // temp variable

    while(in[(*in_from)])
    {
        for(i=skip_len-1;i>=0;--i)
        {
            if(in[(*in_from)]==skip_list[i])    
                break;
        }
        if(i<0) break;  // dismatch skip conditions, finished
        else ++(*in_from);  // match skip conditions, skip it
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
//      FILE & DIRECTORY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  void    FNBACK  change_file_extension(USTR *dest_filename,USTR *src_filename,USTR *extension)
{
    SLONG last_dot; // src_filename length && last '.' index of src_filename 
    SLONG i;    // temp variable

    last_dot=(SLONG)strlen((const char *)src_filename);
    for(i=last_dot-1;i>=0;--i)  // get index of last '.' if exist
    {
        if('.'==src_filename[i])    
        {
            last_dot=i;
            break;
        }
    }
    memcpy(dest_filename,src_filename,last_dot);
    dest_filename[last_dot]='.';
    dest_filename[last_dot+1]=0x00;
    if('.'==extension[0])
        strcat((char *)dest_filename,(const char *)&extension[1]);
    else
        strcat((char *)dest_filename,(const char *)&extension[0]);
}


EXPORT  SLONG   FNBACK  is_file_exist(USTR *filename)
{
    FILE *fp=NULL;
    if(NULL==(fp=fopen((const char *)filename,"rb")))
        return(FALSE);
    if(fp) fclose(fp);
    return(TRUE);
}

EXPORT SLONG FNBACK make_directory(USTR *path)
{
    USTR sub_path[_MAX_PATH];
    SLONG len,i=0,ret_val=1;
    
    memset((char *)sub_path,0x00,_MAX_PATH);
    len=(SLONG)strlen((const char *)path)+1;
    while(i<len)
    {
        if((path[i]=='\\')||(path[i]=='/')||(path[i]=='\0'))
            ret_val = _mkdir((const char *)sub_path);
        sub_path[i]=path[i];
        i++;
    }
    if(0==ret_val) return TTN_OK;
    else return TTN_NOT_OK;
}

EXPORT SLONG FNBACK remove_directory(USTR *path)
{
    SLONG i=(SLONG)strlen((const char *)path)+1;
    SLONG ret_val=1;
    
    while(i>0)
    {
        if((path[i]=='\\')||(path[i]=='/')||(path[i]=='\0'))
        {
            path[i]='\0';
            ret_val= _rmdir((const char *)path);
        }
        i--;
    }
    if(0==ret_val) return TTN_OK;
    else return TTN_NOT_OK;
}

EXPORT  USTR *  FNBACK  get_pure_filename(USTR *filename)
{
    static USTR pure_filename[_MAX_FNAME];
    SLONG i,last_dot;
    
    if(0==strcmpi((const char *)filename,".."))
    {
        strcpy((char *)pure_filename,"..");
    }
    else
    {
        last_dot=(SLONG)strlen((const char *)filename);
        for(i=last_dot-1;i>=0;--i)
        {
            if('.'==filename[i])
            {
                last_dot=i;
                break;
            }
        }
        memcpy(pure_filename,filename,last_dot);
        pure_filename[last_dot]=0x00;
    }
    return((USTR *)pure_filename);
}

EXPORT  USTR *  FNBACK  get_file_extension(USTR *filename)
{
    static USTR fileext[_MAX_EXT];
    SLONG i,last_dot,len;
    
    if(strcmpi((const char *)filename,"..")==0)
    {
        fileext[0]=0x00;
    }
    else
    {
        last_dot=len=(SLONG)strlen((const char *)filename);
        for(i=last_dot-1;i>=0;--i)
        {
            if('.'==filename[i])
            {
                last_dot=i;
                break;
            }
        }
        memcpy(fileext,&filename[last_dot],len+1-last_dot);
    }
    return((USTR *)fileext);
}


EXPORT  USTR *  FNBACK  get_file_path(USTR *path_fname)
{
    static USTR path[_MAX_PATH];
    SLONG last_slash;

    last_slash=(SLONG)strlen((const char *)path_fname) - 1;
    while(last_slash >= 0)
    {
        if('\\'==path_fname[last_slash] || '/'==path_fname[last_slash])
        {
            break;
        }
        last_slash--;
    }
    if(last_slash <= 0)
        return  NULL;

    memcpy(path, path_fname, last_slash);
    path[last_slash] = 0x00;
    return (USTR *)path;
}


EXPORT  USTR *  FNBACK  get_nopath_filename(USTR *path_fname)
{
    static  USTR fname[_MAX_FNAME];
    SLONG   len,i;
    len=(SLONG)strlen((const char *)path_fname);
    for(i=len-1;i>=0;--i)
    {
        if('\\'==path_fname[i] || '/'==path_fname[i])
            break;
    }
    strcpy((char *)fname,(const char *)&path_fname[i+1]);
    return((USTR *)fname);
}

EXPORT  USTR *  FNBACK  make_full_filename(USTR *path,USTR *filename,USTR *fileext)
{
    static USTR full_filename[_MAX_PATH+_MAX_FNAME];
    SLONG len;

    len=(SLONG)strlen((const char *)path);
    if(len>0)
    {
        if( '\\'!=path[len-1] )
            sprintf((char *)full_filename,"%s\\%s%s",path,filename,fileext);
        else
            sprintf((char *)full_filename,"%s%s%s",path,filename,fileext);
    }
    else
    {
        sprintf((char *)full_filename,"%s%s",filename,fileext);
    }
    return (USTR *)full_filename;
}

EXPORT  USTR *  FNBACK  adjust_file_path(USTR *path)
{
    static USTR fpath[_MAX_PATH];
    SLONG len,i;
    len=(SLONG)strlen((const char *)path);
    for(i=len;i>=0;--i) // process with end 0x00
    {
        if('\\'==path[i])
            fpath[i]='/';
        else
            fpath[i]=path[i];
    }
    return((USTR *)fpath);
}

EXPORT  void    FNBACK  oper_whole_path(USTR *spath,USTR *tpath)
{
    SLONG   len;
    UHINT   driver;
    UHINT   driver1;
    USTR    temp[256];

    driver=2;
    memset(temp,0x00,256);
    memset(tpath,0x00,sizeof(tpath));
    if(spath[0]=='\\')                      // from current driver root
    {
        driver=(UHINT)(_getdrive());
        tpath[0]=(UCHR)('A'+driver-1);
        tpath[1]=':';
        tpath[2]=0x00;
        if(spath[1]!=0x00)
        {
            // Jack, fixed bug of convert chinese directory name. [8/23/2002]
            //strcat((char *)tpath,strupr((char *)spath));
            strcat((char *)tpath, CharUpper((LPSTR)spath));
        }
        return;
    }
    else
    {
        if(spath[1]==':')                   // from other driver
        {
            if(spath[2]=='\\')               // from other driver root
            {
                // Jack, fixed bug of convert chinese directory name. [8/23/2002]
                //strcpy((char *)tpath,strupr((char *)spath));
                strcpy((char *)tpath, CharUpper((LPSTR)spath));

                if( tpath[3]==0x00 )
                    tpath[2]=0x00;
                return;
            }
            else                              // from other driver current dir
            {
                driver=(UHINT)(_getdrive());
                driver1=(UHINT)(char_upper(spath[0])-'A'+1);
                _chdrive(driver1);
                getcwd((char *)&temp[0],255);
                strcat((char *)tpath,(const char *)&temp[0]);
                len=strlen((const char *)tpath);
                if(tpath[len-1]=='\\')
                    tpath[len-1]=0x00;
                if(spath[2] != '\0')
                {
                    if(spath[2]=='.')
                    {
                        if(spath[3]!='\0')
                        {
                            //printf("Input path error\n");
                            return;
                            /*
                            exit(-3);
                            */
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        strcat((char *)tpath,"\\");

                        // Jack, fixed bug of convert chinese directory name. [8/23/2002]
                        //strcat((char *)tpath,strupr((char *)&spath[2]));
                        strcat((char *)tpath, CharUpper((LPSTR)&spath[2]));
                    }
                }
                _chdrive(driver);
                return;
            }
        }
        else                                 // from current driver current
        {
            getcwd((char *)&temp[0],255);
            strcat((char *)tpath,(const char *)&temp[0]);
            strcat((char *)tpath,"\\");

            // Jack, fixed bug of convert chinese directory name. [8/23/2002]
            //strcat((char *)tpath,(const char *)strupr((char *)spath));
            strcat((char *)tpath, CharUpper((LPSTR)spath));

            return;
        }
    }
}



EXPORT  SLONG   FNBACK  is_directory_exist(USTR *path)
{
    SLONG bak_drive;
    USTR bak_path[_MAX_PATH];
    USTR full_path[_MAX_PATH];
    USTR copy_path[_MAX_PATH];
    SLONG des_drive;
    SLONG res;

    strcpy((char *)copy_path, (const char *)path);
    oper_whole_path(copy_path,full_path);
    bak_drive = _getdrive();
    des_drive = full_path[0] - 'A' + 1;
    _chdrive(des_drive);
    _getdcwd(des_drive,(char *)bak_path,_MAX_PATH);
    res = _chdir((const char *)full_path);
    _chdir((const char *)bak_path);
    _chdrive(bak_drive);

    return (res == 0) ? TRUE : FALSE ;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//      DATA COMPUTE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  void    FNBACK  run_random_init(void)
{
    //geaan, 2001.10.9.
    /*
    time_t  tnow;
    USTR    temp[4];
    USTR    disp_rec[256];
    SLONG   count;
    SLONG   i;
    
    tnow=time(NULL);
    strcpy((char *)disp_rec,ctime(&tnow));
    temp[0]=disp_rec[17];
    temp[1]=disp_rec[18];
    temp[2]=(UCHR)NULL;
    count=atoi((const char *)temp)*100;
    for(i=0;i<count;i++)
    {
        rand();
    }
    */

    /* Seed the random-number generator with current time so that
    * the numbers will be different every time we run.
    */
    srand( (unsigned)time( NULL ) );
}

EXPORT  ULONG   FNBACK  checksum_ulong_table(ULONG *table, ULONG length)
{
    ULONG sum = 0L;
    ULONG *end_ptr = table+((length+3) & ~3) / sizeof(ULONG);
    
    while (table < end_ptr)
        sum += *table++;
    return sum;
}

EXPORT  void    FNBACK  switch_data_flag(ULONG *data,ULONG flag)
{
    update_data_bit(data,flag,UDB_SWT_BIT);
    //    if( (*data) & flag )
    //        (*data) &= ~flag;
    //    else
    //        (*data) |= flag;
}



EXPORT  SLONG   FNBACK  update_data_bit(ULONG *data, ULONG mask, SLONG flag)
{
    switch(flag)
    {
    case UDB_CHK_BIT:
        if( (*data) & mask )    return TRUE;
        else  return FALSE;
        //break;
    case UDB_SET_BIT:
        (*data) &= ~mask;
        (*data) |= mask;
        return 0;
        //break;
    case UDB_ADD_BIT:
        (*data) |= mask;
        return 0;
        //break;
    case UDB_DEC_BIT:
        (*data) &= ~mask;
        return 0;
        //break;
    case UDB_SWT_BIT:
        if( (*data) & mask )
            (*data) &= ~mask;
        else
            (*data) |= mask;
        return 0;
        //break;
    }
    return 0;
}



EXPORT  UCHR    FNBACK  compute_uchar_chksum(USTR *buffer, SLONG size)
{
    UCHR chksum = (UCHR)0;
    UCHR *start_ptr = buffer;
    UCHR *end_ptr = buffer+size;

    while(start_ptr<end_ptr)
        chksum = (UCHR)( chksum + (*start_ptr++) );
    return chksum;
}



EXPORT  void    FNBACK  pack_uchar(USTR *buffer, UCHR data, SLONG *index)
{
    UCHR *p = buffer + (*index);

    *p = (UCHR)data;
    *index += 1;
}



EXPORT  void    FNBACK  pack_uhint(USTR *buffer, UHINT data, SLONG *index)
{
    UCHR *p = buffer + (*index);

    *p = (UCHR) (data & 0xff);
    *(p+1) = (UCHR) (( data >> 8) & 0xff);
    (*index) += 2;
}



EXPORT  void    FNBACK  pack_ulong(USTR *buffer, ULONG data, SLONG *index)
{
    UCHR *p = buffer + (*index);

    *p = (UCHR) (data & 0xff);
    *(p+1) = (UCHR) ((data>>8) & 0xff);
    *(p+2) = (UCHR) ((data>>16) & 0xff);
    *(p+3) = (UCHR) ((data>>24) & 0xff);
    (*index) += 4;
}



EXPORT  UCHR    FNBACK  unpack_uchar(USTR *buffer, SLONG *index)
{
    UCHR data = buffer[*index];
    *index += 1;
    return data;
}



EXPORT  UHINT   FNBACK  unpack_uhint(USTR *buffer, SLONG *index)
{
    UHINT data;
    UCHR *p = buffer + (*index);

    data = (UHINT)( (*p)
        + ( ( (*(p+1)) << 8 ) & 0xff00 )  );
    *index += 2;

    return data;
}



EXPORT  ULONG   FNBACK  unpack_ulong(USTR *buffer, SLONG *index)
{
    ULONG data;
    UCHR *p = buffer + (*index);

    data = (ULONG)( (*p)
        + ( ( (*(p+1)) << 8 ) & 0xff00 )
        + ( ( (*(p+2)) << 16 ) & 0xff0000 )
        + ( ( (*(p+3)) << 24 ) & 0xff000000 ) );
    *index += 4;

    return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  TIME ABOUT FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  void    FNBACK  get_time(USTR *buffer)
{
    time_t  tnow;
    
    tnow=time(NULL);
    strcpy((char *)buffer,ctime(&tnow));
}

////////////////////////////////////////////////////////////////////////////////////////////
//============================================================================================
