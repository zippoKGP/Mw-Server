

// =========== local routine protype define ==========
void fnProcessFliLineCompress(unsigned char *buffer);
void fnProcessFliColor(unsigned char *buffer);
void fnProcessFliBrun(unsigned char *buffer);
void fnProcessFliCopy(unsigned char *buffer);
void fnProcessFliBlack(unsigned char *buffer);
void fnProcessFlcColor(unsigned char *buffer);
void fnProcessFlcLineCompress(unsigned char *buffer);
void fnMoveData2Screen(short int xpos,short int ypos,unsigned char *buffer,unsigned char count);
void fnSetData2Screen(short int xpos,short int ypos,unsigned char data,unsigned char count);
void fnSet2Data2Screen(short int xpos,short int ypos,unsigned short int data,unsigned char count);
void fnSetPart256Palette(short int coloridx,unsigned char *pal,unsigned char count);
void fnSetFull256Palette(unsigned char *pal);
void fnSetFullScreen(int len,unsigned char data);
void fnMoveFullScreen(unsigned char *buffer,int len);

// ================== for new version
void fnSetPartPalette(short int index,unsigned char count,unsigned char *pal);

#define FLIC_FILE_HEADER_LENGTH         128
#define FLIC_FRAME_HEADER_LENGTH        16
#define FLIC_CHUNK_HEADER_LENGTH        6

#define TAN_MEMORY_SIZE                70000

int SCREEN_X_BYTE=640;
int SCREEN_Y_BYTE=480;

LOCAL_FUNC UHINT *bit_map;

typedef struct
{
  int           FileSize;
  unsigned char Magic[2];
  short int     TotalFrame;
  short int     Width;
  short int     Height;
  short int     Depth;
  short int     Flag;
  short int     Speed;
  char          dummy[110];
}FLIC_FILE_HEADER;

typedef struct
{
  int           FrameSize;
  unsigned char Magic[2];
  short int     TotalChunk;
  char          dummy[8];
}FLIC_FRAME_HEADER;

typedef struct
{
  int           ChunkSize;
  short int     ChunkType;
}FLIC_CHUNK_HEADER;

FILE              *FlicFp;
FLIC_FILE_HEADER  FlicFileHeader;
FLIC_FRAME_HEADER FlicFrameHeader;
unsigned char     *FlicFrameBuffer=(unsigned char *)NULL;
int               FlicLastFramePos;
int               FlicStartFramePos;
int               FlicNowFrameCount;
int               FrameBufferSize;

// ------------- for TAN routinue -----------------
FILE *tan_fp;
SLONG tan_flag=0;

SLONG FLIC_open_flic_file(USTR *name,SLONG mode_flag,UHINT *memory_plane)
{
int frame_size;

        if(mode_flag==0)
           {
           SCREEN_X_BYTE=640;
           SCREEN_Y_BYTE=480;
           }
        else
           {
           SCREEN_X_BYTE=320;
           SCREEN_Y_BYTE=200;
           }
        FlicFp=fopen((const char *)name,"rb");
        if(FlicFp==NULL)
          {
          sprintf((char *)print_rec,"File %s open error",name);
          log_error((UCHR *)print_rec,1);
          return(TTN_ERROR);
          }
        fseek(FlicFp,0L,SEEK_SET);
        fread(&FlicFileHeader,1,FLIC_FILE_HEADER_LENGTH,FlicFp);
        if(FlicFrameBuffer != NULL)
           free(FlicFrameBuffer);
        frame_size=(int)FlicFileHeader.Width * (int)FlicFileHeader.Height;
        frame_size=frame_size * (int)FlicFileHeader.Depth / 8;
        FlicFrameBuffer=(UCHR *)malloc(frame_size+512);
        FrameBufferSize=frame_size + 512;
        FlicNowFrameCount=0;
        FlicLastFramePos=FLIC_FILE_HEADER_LENGTH;
        bit_map=memory_plane;
        wmemset(ture_color_index,0xFFFF,256);
        memset(char_screen_buffer,0x00,SCREEN_SIZE);
        return(FlicFileHeader.TotalFrame);
}

void  FLIC_close_flic_file(void)
{
        if(FlicFp != NULL)
           fclose(FlicFp);
        if(FlicFrameBuffer!=NULL)
           free(FlicFrameBuffer);
        FlicFrameBuffer=NULL;
        make_system_palette();
}

short int FLIC_read_flic_data(void)
{
        fseek(FlicFp,FlicLastFramePos,SEEK_SET);
        fread(&FlicFrameHeader,1,FLIC_FRAME_HEADER_LENGTH,FlicFp);
        FlicLastFramePos+=FlicFrameHeader.FrameSize;
        while((FlicFrameHeader.Magic[0]==0x00)&&(FlicFrameHeader.Magic[1]==0xf1))
          {
          fseek(FlicFp,FlicLastFramePos,SEEK_SET);
          fread(&FlicFrameHeader,1,FLIC_FRAME_HEADER_LENGTH,FlicFp);
          FlicLastFramePos+=FlicFrameHeader.FrameSize;
          }
        if(((FlicFrameHeader.Magic[0]!=0xfa)&&(FlicFrameHeader.Magic[1]!=0xf1))||
            (FlicFrameHeader.FrameSize+16 > FrameBufferSize))
          {
          fclose(FlicFp);
          return(-1);
          }
        fread(FlicFrameBuffer,1,FlicFrameHeader.FrameSize,FlicFp);
        return(0);
}

short int FLIC_play_flic_frame(void)
{
short int i;
int   old_frame_buffer_index;
int   frame_buffer_index=0;
int   chunk_size;
short int chunk_type;

        if(FlicFrameHeader.TotalChunk == 0)
           return(0);
        for(i=0;i<FlicFrameHeader.TotalChunk;i++)
           {
           chunk_size=((FLIC_CHUNK_HEADER *)&FlicFrameBuffer[frame_buffer_index])->ChunkSize;
           chunk_type=((FLIC_CHUNK_HEADER *)&FlicFrameBuffer[frame_buffer_index])->ChunkType;
           old_frame_buffer_index=frame_buffer_index;
           frame_buffer_index+=6;
           switch(chunk_type)
              {
              case 0x0c:
                 fnProcessFliLineCompress(&FlicFrameBuffer[frame_buffer_index]);
                 break;
              case 0x0b:
                 fnProcessFliColor(&FlicFrameBuffer[frame_buffer_index]);
                 break;
              case 0x0f:
                 fnProcessFliBrun(&FlicFrameBuffer[frame_buffer_index]);
                 break;
              case 0x10:
                 fnProcessFliCopy(&FlicFrameBuffer[frame_buffer_index]);
                 break;
              case 0x0d:
                 fnProcessFliBlack(&FlicFrameBuffer[frame_buffer_index]);
                 break;
              case 0x04:
                 fnProcessFlcColor(&FlicFrameBuffer[frame_buffer_index]);
                 break;
              case 0x07:
                 fnProcessFlcLineCompress(&FlicFrameBuffer[frame_buffer_index]);
                 break;
              case 0x12:
                 break;
              default:
                 printf("%c%c%c%c%c\n",0x07,0x07,0x07,0x07,0x07);
                 break;
              }
           frame_buffer_index=old_frame_buffer_index+chunk_size;
           }
        return(0);
}

void fnProcessFliLineCompress(unsigned char *buffer)
{
unsigned char *usebuffer;
short int     start_line;
short int     change_line;
short int     i,j;
unsigned char data;
unsigned char count;
short int     xpos,ypos;
short unsigned int packet_num;

        usebuffer=buffer;
        start_line=*(short int *)usebuffer;
        change_line=*(short int *)(usebuffer+2);
        usebuffer+=4;
        ypos=start_line;
        for(i=0;i<change_line;i++)
          {
          ypos=start_line+i;
          xpos=0;
          data=*usebuffer++;
          packet_num=data;
          if(packet_num == 0)
             continue;
          for(j=0;j<packet_num;j++)
             {
             data=*usebuffer++;
             xpos+=data;
             data=*usebuffer++;
             if(data & 0x80)
                {
                count= (255-data)+1;
                data=*usebuffer++;
                fnSetData2Screen(xpos,ypos,data,count);
                xpos+=count;
                }
             else
                {
                count=data;
                fnMoveData2Screen(xpos,ypos,usebuffer,count);
                usebuffer+=count;
                xpos+=count;
                }
             }
          }
}

void fnProcessFliColor(unsigned char *buffer)
{
unsigned char *usebuffer;
short int     packet_num;
unsigned char data;
short int     i;
short int     change_color;

        usebuffer=buffer;
        packet_num=*(short int *)usebuffer;
        usebuffer+=2;
        change_color=0;
        for(i=0;i<packet_num;i++)
           {
           data=*usebuffer++;
           change_color+=data;
           data=*usebuffer++;
           if(data == 0)
              {
              fnSetFull256Palette(usebuffer);
              usebuffer+=768;
              }
           else
              {
              fnSetPart256Palette(change_color,usebuffer,data);
              change_color+=data;
              usebuffer+=(data*3);
              }
           }
}

void fnProcessFliBrun(unsigned char *buffer)
{
unsigned char *usebuffer;
short int     packet_num;
unsigned char data;
unsigned char count;
short int     i;
short int     j;
short int     xpos,ypos;

        usebuffer=buffer;
        for(i=0;i<FlicFileHeader.Height;i++)
           {
           ypos=i;
           xpos=0;
           data=*usebuffer++;
           packet_num=data;
           for(j=0;j<packet_num;j++)
              {
              data=*usebuffer++;
              if(data & 0x80)
                 {
                 count= (255-data)+1;
                 fnMoveData2Screen(xpos,ypos,usebuffer,count);
                 xpos+=count;
                 usebuffer+=count;
                 }
              else
                 {
                 count=data;
                 data=*usebuffer++;
                 fnSetData2Screen(xpos,ypos,data,count);
                 xpos+=count;
                 }
              }
           }
}

void fnProcessFliCopy(unsigned char *buffer)
{
unsigned char *usebuffer;
int           len;

        usebuffer=buffer;
        len=FlicFileHeader.Width;
        len*=FlicFileHeader.Height;
        fnMoveFullScreen(usebuffer,len);
}

void fnProcessFliBlack(unsigned char *buffer)
{
unsigned char *usebuffer;
int           len;

        usebuffer=buffer;
        len=FlicFileHeader.Width;
        len*=FlicFileHeader.Height;
//        fnSetFullScreen(len,0x00);
        wmemset(bit_map,0x00,SCREEN_SIZE);
}

void fnProcessFlcColor(unsigned char *buffer)
{
unsigned char *usebuffer;
short int     packet_num;
unsigned char data;
short int     i,j=0;
short int     change_color;

        usebuffer=buffer;
        packet_num=*(short int *)usebuffer;
        usebuffer+=2;
        change_color=0;
        for(i=0;i<packet_num;i++)
           {
           data=*usebuffer++;
           change_color+=data;
           data=*usebuffer++;
           if(data == 0)
              {
              for(j=0;j<768;j++)
                 usebuffer[j]>>=2;
              fnSetFull256Palette(usebuffer);
              usebuffer+=768;
              }
           else
              {
              for(j=0;j<data*3;j++)
                 usebuffer[j]>>=2;
              fnSetPart256Palette(change_color,usebuffer,data);
              change_color+=data;
              usebuffer+=(data*3);
              }
           }
}

void fnProcessFlcLineCompress(unsigned char *buffer)
{
unsigned char *usebuffer;
short int     line_count;
unsigned short int temp;
short int     i,j;
unsigned char data;
unsigned char count;
short int     xpos,ypos;
short unsigned int packet_num;

        usebuffer=buffer;
        line_count=*(short int *)usebuffer;
        temp=*(short int *)(usebuffer+2);
        if(temp & 0x8000)
           {
           ypos=(65535-temp)+1;
           usebuffer+=4;
           }
        else
           {
           ypos=0;
           usebuffer+=2;
           }
        for(i=0;i<line_count;i++)
           {
           xpos=0;
           temp=*(short int *)usebuffer;
           usebuffer+=2;
           if(temp & 0x8000)
              {
              if((temp & 0xFF00) == 0x8000)
                 bit_map[(SLONG)(ypos+i)*(SLONG)FlicFileHeader.Width+(SLONG)FlicFileHeader.Width-1]=(UCHR)(temp & 0x00FF);
              else
                 ypos+=((65535-temp)+1);
              temp=*(short int *)usebuffer;
              usebuffer+=2;
              }
           packet_num=temp;
           for(j=0;j<packet_num;j++)
              {
              data=*usebuffer++;
              xpos+=data;
              data=*usebuffer++;
              if(data & 0x80)
                 {
                 count=(255-data)+1;
                 temp=*(short int *)usebuffer;
                 usebuffer+=2;
                 fnSet2Data2Screen(xpos,(short int)(ypos+i),temp,count);
                 xpos+=count;
                 xpos+=count;
                 }
              else
                 {
                 count=data*2;
                 fnMoveData2Screen(xpos,(short int)(ypos+i),usebuffer,count);
                 usebuffer+=count;
                 xpos+=count;
                 }
              }
           }
}

void  fnMoveData2Screen(short int xpos,short int ypos,unsigned char *buffer,unsigned char count)
{
int adr;
SLONG i;


        adr=(int)ypos*SCREEN_X_BYTE+xpos;
// stephen  ture color version
//        memcpy(&bit_map[adr],buffer,count);

        for(i=0;i<count;i++)
           {
           bit_map[adr+i]=ture_color_index[buffer[i]];
           }
         memcpy(&char_screen_buffer[adr],buffer,count);
}

void fnSetData2Screen(short int xpos,short int ypos,unsigned char data,unsigned char count)
{
int adr;

        adr=(int)ypos*SCREEN_X_BYTE+xpos;
// stephen  ture color version
//        memset(&bit_map[adr],data,count);
        wmemset(&bit_map[adr],ture_color_index[data],count);

        memset(&char_screen_buffer[adr],data,count);
}

void fnSet2Data2Screen(short int xpos,short int ypos,unsigned short int data,unsigned char count)
{
int adr;
int i;
unsigned char d0,d1;

        adr=(int)ypos*SCREEN_X_BYTE+xpos;
        d0=(unsigned char)(data & 0x00FF);
        d1=(unsigned char)((data & 0xFF00) >> 8);
        for(i=0;i<count;i++)
           {
// stephen   ture color version
//           bit_map[adr+i*2]=d0;
//           bit_map[adr+i*2+1]=d1;
           bit_map[adr+i*2]=ture_color_index[d0];
           bit_map[adr+i*2+1]=ture_color_index[d1];

           char_screen_buffer[adr+i*2]=d0;
           char_screen_buffer[adr+i*2+1]=d1;
           }
}

void fnSetPartPalette(short int index,unsigned char count,unsigned char *pal)
{
SHINT i;
SHINT j;

SLONG change_flag;
SLONG offset;
UHINT old_color_index[256];

        wmemcpy(old_color_index,ture_color_index,256);

        for(j=0,i=index;i<index+(SHINT)count;i++,j++)
           {
           FLC_palette[i*3]=pal[j*3];
           FLC_palette[i*3+1]=pal[j*3+1];
           FLC_palette[i*3+2]=pal[j*3+2];
           }
        convert_ture_color_index(FLC_palette,ture_color_index);

        change_flag=0;
        for(i=0;i<256;i++)
           {
           if(old_color_index[i]!=ture_color_index[i])
              {
              change_flag=1;
              i=256;
              }
           }

        if(change_flag==1)
           {
           for(offset=0;offset<SCREEN_SIZE;offset++)
              bit_map[offset]=ture_color_index[char_screen_buffer[offset]];
           }
}

void fnSetPart256Palette(short int coloridx,unsigned char *pal,unsigned char count)
{
UHINT old_color_index[256];
SHINT i;

SLONG offset;
SLONG change_flag;

        wmemcpy(old_color_index,ture_color_index,256);

        fnSetPartPalette(coloridx,count,pal);
        convert_ture_color_index(FLC_palette,ture_color_index);

        change_flag=0;
        for(i=0;i<256;i++)
           {
           if(old_color_index[i]!=ture_color_index[i])
              {
              change_flag=1;
              i=256;
              }
           }

        if(change_flag==1)
           {
           for(offset=0;offset<SCREEN_SIZE;offset++)
              bit_map[offset]=ture_color_index[char_screen_buffer[offset]];
           }

}

void fnSetFull256Palette(unsigned char *pal)
{

        fnSetPartPalette((SHINT)0,(UCHR )255,pal);
        convert_ture_color_index(FLC_palette,ture_color_index);
}

void fnSetFullScreen(int len,unsigned char data)
{
// stephen  ture color version
//      memset(bit_map,data,SCREEN_X_BYTE*SCREEN_Y_BYTE);
        wmemset(bit_map,ture_color_index[data],SCREEN_SIZE);

        memset(char_screen_buffer,data,SCREEN_SIZE);
}

void fnMoveFullScreen(unsigned char *buffer,int len)
{
// stephen  ture color version
//        memcpy(bit_map,buffer,len);
SLONG i;

        for(i=0;i<len;i++)
           bit_map[i]=ture_color_index[buffer[i]];

        memcpy(char_screen_buffer,buffer,len);
}

SLONG FLIC_get_last_position(void)
{
        return(FlicLastFramePos);
}

void FLIC_set_last_position(SLONG no)
{
        FlicLastFramePos=no;
}

