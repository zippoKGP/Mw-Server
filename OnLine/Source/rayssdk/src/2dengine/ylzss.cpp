/*
**      YLZSS.CPP
**      LZSS compress functions.
**
**      ZJian,1999.12.4.
**          TAB=4
**
*/
#include "rays.h"
#include "xsystem.h"
#include "ylzss.h"
#include "ybitio.h"



#define INDEX_BIT_COUNT         12
#define LENGTH_BIT_COUNT        4
#define WINDOW_SIZE             (1<<INDEX_BIT_COUNT)
#define RAW_LOOK_AHEAD_SIZE     (1<<LENGTH_BIT_COUNT)
#define BREAK_EVEN              ((1+INDEX_BIT_COUNT+LENGTH_BIT_COUNT)/9)
#define LOOK_AHEAD_SIZE         (RAW_LOOK_AHEAD_SIZE+BREAK_EVEN)
#define TREE_ROOT               WINDOW_SIZE
#define END_OF_STREAM           0
#define UNUSED                  0
#define MOD_WINDOW(a)           ((a)&(WINDOW_SIZE-1))

/******************************************************************************/
static  unsigned char   window[WINDOW_SIZE];
static  struct  {
    int parent;
    int smaller_child;
    int larger_child;
} tree[WINDOW_SIZE+1];

static  char            data_buffer[17];
static  int             flag_bit_mask;
static  unsigned int    buffer_offset;
static  unsigned long   original_data_size;
static  unsigned long   original_data_offset;
static  unsigned long   compressed_data_size;
static  unsigned long   compressed_data_offset;
/******************************************************************************/

static  void    init_tree(int r);
static  void    contract_node(int old_node,int new_node);
static  void    replace_node(int old_node,int new_node);
static  int     find_next_node(int node);
static  void    delete_string(int p);
static  int     add_string(int new_node,int *match_position);
static  void    LZSS_file_compress(FILE  *input, BITFILE *output);
static  void    LZSS_file_expand(BITFILE *input, FILE *output);
static  void    init_output_buffer(void);
static  int     flush_output_buffer(unsigned char *output);
static  int     output_char(int data,unsigned char *output);
static  int     output_pair(int position,int length,unsigned char *output);
static  void    init_input_buffer(unsigned char *input);
static  int     input_bit(unsigned char *input);
static  int     get_original_char(unsigned char *input);
static  void    put_original_char(int data,unsigned char *output);
static  int     get_compressed_char(unsigned char *input);
static  void    LZSS_data_compress(unsigned char *input,unsigned char *output,unsigned long *input_size,unsigned long *output_size);
static  void    LZSS_data_expand(unsigned char *input,unsigned char *output,unsigned long *input_size,unsigned long *output_size);

EXPORT  SLONG   FNBACK  LZSS_compress_file(USTR *input_filename,USTR *output_filename);
EXPORT  SLONG   FNBACK  LZSS_expand_file(USTR *input_filename,USTR *output_filename);
EXPORT  SLONG   FNBACK  LZSS_compress_data(USTR *input,USTR *output,ULONG *input_size,ULONG *output_size);
EXPORT  SLONG   FNBACK  LZSS_expand_data(USTR *input,USTR *output,ULONG *input_size,ULONG *output_size);

/******************************************************************************/

static  void    init_tree(int r)
{
    int     i;
    for(i=0;i<(WINDOW_SIZE+1);i++)
    {
        tree[i].parent=UNUSED;
        tree[i].larger_child=UNUSED;
        tree[i].smaller_child=UNUSED;
    }
    tree[TREE_ROOT].larger_child=r;
    tree[r].parent=TREE_ROOT;
    tree[r].larger_child=UNUSED;
    tree[r].smaller_child=UNUSED;
}

static  void    contract_node(int old_node,int new_node)
{
    tree[new_node].parent=tree[old_node].parent;
    if(tree[tree[old_node].parent].larger_child==old_node)
        tree[tree[old_node].parent].larger_child=new_node;
    else
        tree[tree[old_node].parent].smaller_child=new_node;
    tree[old_node].parent=UNUSED;
}

static  void    replace_node(int old_node,int new_node)
{
    int     parent;
    parent=tree[old_node].parent;
    if(tree[parent].smaller_child==old_node)
        tree[parent].smaller_child=new_node;
    else
        tree[parent].larger_child=new_node;
    tree[new_node]=tree[old_node];
    tree[tree[new_node].smaller_child].parent=new_node;
    tree[tree[new_node].larger_child].parent=new_node;
    tree[old_node].parent=UNUSED;
}

static  int     find_next_node(int node)
{
    int     next;
    next=tree[node].smaller_child;
    while(tree[next].larger_child!=UNUSED)
        next=tree[next].larger_child;
    return(next);
}

static  void    delete_string(int p)
{
    int     replacement;
    if(tree[p].parent==UNUSED)
        return;
    if(tree[p].larger_child==UNUSED)
        contract_node(p,tree[p].smaller_child);
    else if(tree[p].smaller_child==UNUSED)
        contract_node(p,tree[p].larger_child);
    else
    {
        replacement=find_next_node(p);
        delete_string(replacement);
        replace_node(p,replacement);
    }
}

static  int     add_string(int new_node,int *match_position)
{
    int     i;
    int     test_node;
    int     delta=0;
    int     match_length;
    int     *child;
    
    if(new_node==END_OF_STREAM)
        return(0);
    test_node=tree[TREE_ROOT].larger_child;
    match_length=0;
    for(;;)
    {
        for(i=0;i<LOOK_AHEAD_SIZE;i++)
        {
            delta=window[MOD_WINDOW(new_node+i)]-window[MOD_WINDOW(test_node+i)];
            if(delta)
                break;
        }
        if(i>=match_length)
        {
            match_length=i;
            *match_position=test_node;
            if(match_length>=LOOK_AHEAD_SIZE)
            {
                replace_node(test_node,new_node);
                return(match_length);
            }
        }
        if(delta>0)
            child=&tree[test_node].larger_child;
        else
            child=&tree[test_node].smaller_child;
        if(*child==UNUSED)
        {
            *child=new_node;
            tree[new_node].parent=test_node;
            tree[new_node].larger_child=UNUSED;
            tree[new_node].smaller_child=UNUSED;
            return(match_length);
        }
        test_node=*child;
    }
}

static  void    LZSS_file_compress(FILE  *input,BITFILE *output)
{
    int     i;
    int     c;
    int     look_ahead_bytes;
    int     current_position;
    int     replace_count;
    int     match_length;
    int     match_position;
    
    current_position=1;
    for(i=0;i<LOOK_AHEAD_SIZE;i++)
    {
        if((c=getc(input))==EOF)
            break;
        window[current_position+i]=(unsigned char)c;
    }
    look_ahead_bytes=i;
    init_tree(current_position);
    
    match_length=0;
    match_position=0;
    while(look_ahead_bytes>0)
    {
        if(match_length>look_ahead_bytes)
            match_length=look_ahead_bytes;
        if(match_length<=BREAK_EVEN)
        {
            output_bit(output,1);
            output_bits(output,(ULONG)window[current_position],8);
            replace_count=1;
        }
        else
        {
            output_bit(output,0);
            output_bits(output,(ULONG)match_position,INDEX_BIT_COUNT);
            output_bits(output,(ULONG)(match_length-(BREAK_EVEN+1)),LENGTH_BIT_COUNT);
            replace_count=match_length;
        }
        for(i=0;i<replace_count;i++)
        {
            delete_string(MOD_WINDOW(current_position+LOOK_AHEAD_SIZE));
            if((c=getc(input))==EOF)
                look_ahead_bytes--;
            else
                window[MOD_WINDOW(current_position+LOOK_AHEAD_SIZE)]=(unsigned char)c;
            current_position=MOD_WINDOW(current_position+1);
            if(look_ahead_bytes)
                match_length=add_string(current_position,&match_position);
        }
    }
    output_bit(output,0);
    output_bits(output,(ULONG)END_OF_STREAM,INDEX_BIT_COUNT);
}

static  void    LZSS_file_expand(BITFILE *input,FILE *output)
{
    int     i;
    int     current_position;
    int     c;
    int     match_length;
    int     match_position;
    
    current_position=1;
    for(;;)
    {
        if(input_bit(input))
        {
            c=(int)input_bits(input,8);
            putc(c,output);
            window[current_position]=(unsigned char)c;
            current_position=MOD_WINDOW(current_position+1);
        }
        else
        {
            match_position=(int)input_bits(input,INDEX_BIT_COUNT);
            if(match_position==END_OF_STREAM)
                break;
            match_length=(int)input_bits(input,LENGTH_BIT_COUNT);
            match_length+=BREAK_EVEN;
            for(i=0;i<=match_length;i++)
            {
                c=window[MOD_WINDOW(match_position+i)];
                putc(c,output);
                window[current_position]=(unsigned char)c;
                current_position=MOD_WINDOW(current_position+1);
            }
        }
    }
}

static  void    init_output_buffer(void)
{
    data_buffer[0]=0;
    flag_bit_mask=1;
    buffer_offset=1;
}

static  int     flush_output_buffer(unsigned char *output)
{
    if(buffer_offset==1)
        return(1);
    memcpy(&output[compressed_data_offset],data_buffer,buffer_offset);
    compressed_data_offset+=buffer_offset;
    compressed_data_size+=buffer_offset;
    init_output_buffer();
    return(1);
}

static  int     output_char(int data,unsigned char *output)
{
    data_buffer[buffer_offset++]=(char)data;
    data_buffer[0]|=flag_bit_mask;
    flag_bit_mask<<=1;
    if(flag_bit_mask==0x100)
        return(flush_output_buffer(output));
    else
        return(1);
}

static  int     output_pair(int position,int length,unsigned char *output)
{
    data_buffer[buffer_offset] = (char)(((char)( length << 4 )) & 0xf0);
    data_buffer[buffer_offset++] |= ((char)(position>>8)) & 0x0f;
    data_buffer[buffer_offset++] = (char)(position & 0xff);
    flag_bit_mask<<=1;
    if(flag_bit_mask==0x100)
        return(flush_output_buffer(output));
    else
        return(1);
}

static  void    init_input_buffer(unsigned char *input)
{
    flag_bit_mask=1;
    data_buffer[0]=(char)input[compressed_data_offset++];
}

static  int     input_bit(unsigned char *input)
{
    if(flag_bit_mask==0x100)
        init_input_buffer(input);
    flag_bit_mask<<=1;
    return(data_buffer[0]&(flag_bit_mask>>1));
}

static  int     get_original_char(unsigned char *input)
{
    return((original_data_offset>=original_data_size)?EOF:input[original_data_offset++]);
}

static  void    put_original_char(int data,unsigned char *output)
{
    output[original_data_offset++]=(char)data;
}

static  int     get_compressed_char(unsigned char *input)
{
    return((compressed_data_offset>=compressed_data_size)?EOF:input[compressed_data_offset++]);
}

static  void    LZSS_data_compress(unsigned char *input,unsigned char *output,unsigned long *input_size,unsigned long *output_size)
{
    int     i;
    int     c;
    int     look_ahead_bytes;
    int     current_position;
    int     replace_count;
    int     match_length;
    int     match_position;
    
    original_data_size=*input_size;
    original_data_offset=0;
    compressed_data_size=0;
    compressed_data_offset=0;
    init_output_buffer();
    
    current_position=1;
    for(i=0;i<LOOK_AHEAD_SIZE;i++)
    {
        if((c=get_original_char(input))==EOF)
            break;
        window[current_position+i]=(unsigned char)c;
    }
    look_ahead_bytes=i;
    init_tree(current_position);
    
    match_length=0;
    match_position=0;
    while(look_ahead_bytes>0)
    {
        if(match_length>look_ahead_bytes)
            match_length=look_ahead_bytes;
        if(match_length<=BREAK_EVEN)
        {
            output_char(window[current_position],output);
            replace_count=1;
        }
        else
        {
            output_pair(match_position,match_length-(BREAK_EVEN+1),output);
            replace_count=match_length;
        }
        for(i=0;i<replace_count;i++)
        {
            delete_string(MOD_WINDOW(current_position+LOOK_AHEAD_SIZE));
            if((c=get_original_char(input))==EOF)
                look_ahead_bytes--;
            else
                window[MOD_WINDOW(current_position+LOOK_AHEAD_SIZE)]=(unsigned char)c;
            current_position=MOD_WINDOW(current_position+1);
            if(look_ahead_bytes)
                match_length=add_string(current_position,&match_position);
        }
    }
    flush_output_buffer(output);
    *output_size=compressed_data_size;
}

static  void    LZSS_data_expand(unsigned char *input,unsigned char *output,unsigned long *input_size,unsigned long *output_size)
{
    int     i;
    int     current_position;
    int     c;
    int     match_length;
    int     match_position;
    
    compressed_data_size=*input_size;
    compressed_data_offset=0;
    original_data_size=*output_size;
    original_data_offset=0;
    init_input_buffer(input);
    
    current_position=1;
    while(original_data_offset<original_data_size)
    {
        if(input_bit(input))
        {
            c=get_compressed_char(input);
            put_original_char(c,output);
            window[current_position]=(unsigned char)c;
            current_position=MOD_WINDOW(current_position+1);
        }
        else
        {
            match_length=get_compressed_char(input);
            match_position=get_compressed_char(input);
            match_position|=(match_length&0xf)<<8;
            match_length>>=4;
            match_length+=BREAK_EVEN;
            for(i=0;i<=match_length;i++)
            {
                c=window[MOD_WINDOW(match_position+i)];
                put_original_char(c,output);
                window[current_position]=(unsigned char)c;
                current_position=MOD_WINDOW(current_position+1);
            }
        }
    }
    *output_size=original_data_size;
}
/******************************************************************************/
EXPORT  SLONG   FNBACK  LZSS_compress_file(USTR *input_filename,USTR *output_filename)
{
    FILE            *fp=NULL;
    BITFILE        *bf=NULL;
    
    fp=fopen((const char *)input_filename,"rb");
    if(fp==NULL)
    {
        sprintf((char *)print_rec,"ERROR : file %s open error(LZSS_compress_file)",input_filename);
        log_error(1,print_rec);
        return(TTN_ERROR);
    }
    bf=open_output_bitfile((char *)output_filename);
    if(bf==NULL)
    {
        sprintf((char *)print_rec,"ERROR : file %s open error(LZSS_compress_file)",output_filename);
        log_error(1,print_rec);
        if(fp)fclose(fp);
        return(TTN_ERROR);
    }
    LZSS_file_compress(fp,bf);
    close_output_bitfile(bf);
    if(fp)fclose(fp);
    return(TTN_OK);
}

EXPORT  SLONG   FNBACK  LZSS_expand_file(USTR *input_filename,USTR *output_filename)
{
    FILE            *fp=NULL;
    BITFILE        *bf=NULL;
    
    fp=fopen((const char *)output_filename,"wb");
    if(fp==NULL)
    {
        sprintf((char *)print_rec,"ERROR : file %s open error(LZSS_expand_file)",input_filename);
        log_error(1,print_rec);
        return(TTN_ERROR);
    }
    bf=open_input_bitfile((char *)input_filename);
    if(bf==NULL)
    {
        sprintf((char *)print_rec,"ERROR : file %s open error(LZSS_expand_file)",output_filename);
        log_error(1,print_rec);
        if(fp)fclose(fp);
        return(TTN_ERROR);
    }
    LZSS_file_expand(bf,fp);
    close_input_bitfile(bf);
    if(fp)fclose(fp);
    return(TTN_OK);
}

EXPORT  SLONG   FNBACK  LZSS_compress_data(USTR *input,USTR *output,ULONG *input_size,ULONG *output_size)
{
    unsigned long   in_size;
    unsigned long   out_size;
    
    in_size=(unsigned long)*input_size;
    LZSS_data_compress((unsigned char *)input,(unsigned char *)output,&in_size,&out_size);
    *output_size=(SLONG)out_size;
    return(TTN_OK);
}

EXPORT  SLONG   FNBACK  LZSS_expand_data(USTR *input,USTR *output,ULONG *input_size,ULONG *output_size)
{
    unsigned long   in_size;
    unsigned long   out_size;
    
    in_size=(unsigned long)*input_size;
    out_size=(unsigned long)*output_size;
    LZSS_data_expand((unsigned char *)input,(unsigned char *)output,&in_size,&out_size);
    *output_size=(SLONG)out_size;
    return(TTN_OK);
}
