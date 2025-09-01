// amp11 - an Audio-MPEG decoder - sample
// Copyright (c) 1997-2000 Niklas Beisert
// See COPYING (GNU General Public License 2) for license

#define VERSION "nb000315"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#if (defined(WIN32)||defined(DOS))&&!defined(_MSC_VER)
#include <dos.h>
#endif

#ifdef DOS
#include "binfplsb.h"
#endif

#ifdef __DOS4G__
#include "timer.h"
#define CLOCKGRAN 1000
#define getclock() tmGetTimer()
#define initclock tmInit
#define closeclock tmClose
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define CLOCKGRAN 1000
#define getclock() timeGetTime()
#define initclock()
#define closeclock()
//zjian changed.
//#include "binfplnt.h"
#include "binfile\\binfplnt.h"
#endif

#ifndef CLOCKGRAN
#ifdef CLOCKS_PER_SEC
#define CLOCKGRAN CLOCKS_PER_SEC
#else
#define CLOCKGRAN 1000000
#endif
#define getclock() clock()
#define initclock()
#define closeclock()
#endif

#ifdef LINUX
#include "binfpllx.h"
#endif

#ifdef UNIX
#include <glob.h>
#endif

//zjian changed.
/*
#include "binfhttp.h"
#include "binfstd.h"
#include "binfarc.h"
#include "binfplwv.h"
#include "binfcon.h"
*/
#include "binfile\\binfhttp.h"
#include "binfile\\binfstd.h"
#include "binfile\\binfarc.h"
#include "binfile\\binfplwv.h"
#include "binfile\\binfcon.h"
#include "ampdec.h"
#include "mpgsplit.h"

struct fileliststruct
{
  char *name;
  fileliststruct *next;
};

static int addfiles(fileliststruct *&fl, char *name)
{
  fileliststruct **l;
  for (l=&fl; *l; l=&(*l)->next);
  if (!strncmp(name, "http://", 7))
  {
    fileliststruct *n=new fileliststruct;
    char *nm=strdup(name);
    if (!n||!nm)
      return -1;
    n->next=0;
    n->name=nm;
    *l=n;
    l=&n->next;
    return 0;
  }
#if (defined(WIN32)||defined(DOS))&&!defined(_MSC_VER)
  if (!strchr(name, '*')&&!strchr(name, '?'))
  {
    fileliststruct *n=new fileliststruct;
    char *nm=strdup(name);
    if (!n||!nm)
      return -1;
    n->next=0;
    n->name=nm;
    *l=n;
    l=&n->next;
    return 0;
  }
  char *srcpath=strdup(name);
  if (strrchr(srcpath, '\\'))
    strrchr(srcpath, '\\')[1]=0;
  else
  if (strrchr(srcpath, '/'))
    strrchr(srcpath, '/')[1]=0;
  else
  if (strrchr(srcpath, ':'))
    strrchr(srcpath, ':')[1]=0;
  else
    srcpath="";
  find_t fnd;
  int fn;
  for (fn=_dos_findfirst(name, _A_NORMAL, &fnd); !fn; fn=_dos_findnext(&fnd))
  {
    fileliststruct *n=new fileliststruct;
    char *nm=new char [strlen(srcpath)+strlen(fnd.name)+1];
    if (!n||!nm)
      return -1;
    n->next=0;
    n->name=nm;
    strcpy(n->name,srcpath);
    strcat(n->name,fnd.name);
    *l=n;
    l=&n->next;
  }
  delete srcpath;
#ifdef WIN32
  _dos_findclose(&fnd);
#endif
  return 0;
#else
#ifdef UNIX
  glob_t g;
  g.gl_pathc=0;
  g.gl_offs=0;
  if (glob(name, GLOB_MARK, 0, &g))
    return -1;
  char **p;
  for (p=g.gl_pathv; *p; p++)
  {
    if ((*p)[strlen(*p)-1]=='/')
      continue;
    fileliststruct *n=new fileliststruct;
    if (!n)
      return -1;
    n->next=0;
    n->name=*p;
    *l=n;
    l=&n->next;
    return 0;
  }
  return 0;
#else
  fileliststruct *n=new fileliststruct;
  char *nm=strdup(name);
  if (!n||!nm)
    return -1;
  n->next=0;
  n->name=nm;
  *l=n;
  l=&n->next;
  return 0;
#endif
#endif
}

static int scramblefilelist(fileliststruct *&fl)
{
  fileliststruct **l;
  int n=0;
  int i;
  for (l=&fl; *l; l=&(*l)->next)
    n++;
  fileliststruct *nl;
  for (nl=0; n; n--)
  {
    l=&fl;
    for (i=rand()%n; i; i--)
      l=&(*l)->next;
    fileliststruct *p=*l;
    *l=(*l)->next;
    p->next=nl;
    nl=p;
  }
  fl=nl;
  return 0;
}




int main(int argc, char **argv)
{
  char *proxy=0;
  char *outname=0;
  char *logname=0;
  fileliststruct *flp;
  int i;
  int writewav=0;
  int scramble=1;
  int repeat=0;
  float vol=1;
  float bal=0;
  int tonull=0;
  int down=0;
  int chn=0;
  int quiet=0;
  float ctr=0;
  float sep=1;
  float srnd=1;
  float pitch=0;
  float pitch0=1764;
  int buflen=4;
  fileliststruct *filelist=0;
  for (i=1; i<argc; i++)
    if (argv[i][0]=='-')
    {
      switch (argv[i][1])
      {
      case 'n': tonull=1; break;
      case 'm': chn=1; break;
      case 'q': quiet=1; break;
      case 's': chn=2; break;
      case 'S': chn=-2; break;
      case '2': down=1; break;
      case '4': down=2; break;
      case 'i': srnd=-1; break;
      case 'P': proxy=argv[i]+2; break;
      case 'l': logname=argv[i]+2; break;
      case 'v': vol=atof(argv[i]+2); break;
      case 'b': bal=atof(argv[i]+2); break;
      case 'c': ctr=atof(argv[i]+2); break;
      case 'p': sep=atof(argv[i]+2); break;
      case 'f': pitch=atof(argv[i]+2); break;
      case 'F': pitch0=atof(argv[i]+2); break;
      case 'o': outname=argv[i]+2; break;
      case 'w': writewav=1; break;
      case 'R': repeat=1; break;
      case 'O': scramble=0; break;
      case 'B': buflen=atoi(argv[i]+2); break;
      default: printf("unknown switch\n"); return 1;
      }
    }
    else
      addfiles(filelist, argv[i]);

  if (!quiet)
    printf("amp11   Audio-MPEG decoder (%s)  (c) 1997-2000 Niklas Beisert\n\n", VERSION);

  if (!filelist)
  {
    printf("amp11 comes with ABSOLUTELY NO WARRANTY, for details see COPYING (GPL 2)\n");
    printf("\n");
    printf("amp11 [-q] [-n] [-PPROXY] [-lLOG] INFILE|URL [-oOUTFILE|-w]\n");
    printf("      [-m] [-s] [-S] [-2] [-4]\n");
    printf("      [-vVOL]\n");
    printf("      [-pSEP] [-cCTR] [-bBAL] [-i]\n");
    printf("      [-fPITCH] [-FFREQ0]\n");
    printf("      [-R] [-O]\n");
    printf("      [-BBUFLEN]\n");
    return 1;
  }

  int nfile=0;
  for (flp=filelist; flp; flp=flp->next)
    nfile++;

  if ((nfile!=1)&&!quiet)
    printf("playing %i files\n\n", nfile);

  srand(time(0));

  int nextstep=0;
  int stopped=0;
  int pos=0;

  while (1)
  {
    if (stopped==3)
      break;
    pos+=nextstep;
    if ((pos==nfile)&&(stopped<2)&&!repeat)
      break;
    if (pos<0)
      pos+=nfile;
    if (pos>=nfile)
      pos-=nfile;
    if (!pos&&(stopped<2)&&scramble)
      scramblefilelist(filelist);
    nextstep=1;
    stopped=0;

    flp=filelist;
    for (i=0; i<pos; i++)
      flp=flp->next;
    char *filename=flp->name;
    if (!quiet)
    {
      if (nfile!=1)
        printf("%i: ", pos+1);
      printf("%s\n", filename);
    }

    binfile *musicin=0;
    sbinfile smusicin;
    httpbinfile httpmusicin;
    if (!strncmp(filename, "http://", 7))
    {
      if (httpmusicin.open(filename, proxy, 0, 0))
      {
        printf("Could not open input URL\n");
        continue;
      }
      httpmusicin.ioctl(httpmusicin.ioctlrrbufset, 16384);
      musicin=&httpmusicin;
    }
    else
    {
      if (smusicin.open(filename, sbinfile::openro)<0)
      {
        printf("Could not open input file\n");
        continue;
      }

      musicin=&smusicin;
    }

    sbinfile logfile;
    if (logname)
    {
      logfile.open(logname, sbinfile::openrw|sbinfile::opentr);
      musicin->ioctl(musicin->ioctlrsetlog, &logfile, 0);
    }

    abinfile awavedata;
    mpegsplitstream mpgsplit;
    binfile *wavedata=musicin;
    if (peekub4(*musicin)==0x1BA) // full mpeg stream
    {
      if (mpgsplit.open(*musicin, 0xC0, 0))
      {
        printf("mpeg stream does not contain audio data\n");
        continue;
      }
      wavedata=&mpgsplit;
    }
    else
    if (peekub4(*musicin)==0x52494646) // RIFF aka .WAV
    {
      char riff[4];
      int blklen=12;
      while (blklen&&!musicin->ioctl(musicin->ioctlreof))
        blklen-=musicin->read(0, blklen);
      while (1)
      {
        if (musicin->read(riff, 4)!=4)
        {
          printf("Could not find data chunk in RIFF file\n");
          return 1;
        }
        blklen=getil4(*musicin);
        if (!memcmp(riff, "data", 4))
          break;
        while (blklen&&!musicin->ioctl(musicin->ioctlreof))
          blklen-=musicin->read(0, blklen);
      }
      awavedata.open(*musicin, musicin->tell(), blklen);
      wavedata=&awavedata;
    }
    else
    if (peekub4(*musicin)==0x494E464F) // what's this? ah: I N F O ... ? did i do this?
    {
      getib4(*musicin);
      int blklen=getib4(*musicin);
      while (blklen&&!musicin->ioctl(musicin->ioctlreof))
        blklen-=musicin->read(0, blklen);
      if (musicin->getmode()&binfile::modeseek)
      {
        char tag[3];
        int pos=musicin->tell();
        musicin->seekend(-128);
        musicin->read(tag, 3);
        awavedata.open(*musicin, pos, musicin->length()-pos-(memcmp(tag, "TAG", 3)?0:128));
        wavedata=&awavedata;
      }
    }
    else
    if (peekub4(*musicin)&0xFFFFFF00==0x49443300) // ID3v2
    {
      getib4(*musicin);
      getib2(*musicin);
      int blklen=getu1(*musicin)<<21;
      blklen+=getu1(*musicin)<<14;
      blklen+=getu1(*musicin)<<7;
      blklen+=getu1(*musicin);
      while (blklen&&!musicin->ioctl(musicin->ioctlreof))
        blklen-=musicin->read(0, blklen);
      if (musicin->getmode()&binfile::modeseek)
      {
        awavedata.open(*musicin, musicin->tell(), musicin->length()-musicin->tell());
        wavedata=&awavedata;
      }
    }
    else
    if (peekub2(*musicin)<0xFFF0)
    {
      int blklen=1024;
      while (blklen&&!musicin->ioctl(musicin->ioctlreof))
      {
        int s=peekub2(*musicin);
        if ((s>=0xFFF0)&&(s!=0xFFFF))
          break;
        blklen-=musicin->read(0, 1);
      }
      if (musicin->getmode()&binfile::modeseek)
      {
        char tag[3];
        int pos=musicin->tell();
        musicin->seekend(-128);
        musicin->read(tag, 3);
        awavedata.open(*musicin, pos, musicin->length()-pos-(memcmp(tag, "TAG", 3)?0:128));
        wavedata=&awavedata;
      }
    }
    else
    if (musicin->getmode()&binfile::modeseek)
    {
      char tag[3];
      musicin->seekend(-128);
      musicin->read(tag, 3);
      awavedata.open(*musicin, 0, musicin->length()-(memcmp(tag, "TAG", 3)?0:128));
      wavedata=&awavedata;
    }

    int freq,stereo;

    static ampegdecoder dec;
    if (!quiet)
    {
      int lay,lfe,freq,stereo,rate;
      if (dec.getheader(*wavedata,lay,lfe,freq,stereo,rate))
        printf("layer %i, %i Hz %s, %i kbps\n", lay+1, freq, stereo?"stereo":"mono", rate/1000);
    }

    if (dec.open(*wavedata, freq, stereo, 1, down, chn))
    {
      printf("Could not open audio mpeg\n");
      continue;
    }

    if (!quiet)
      printf("playing %i Hz %s\n", freq, stereo?"stereo":"mono");

    fflush(stdout);

    binfile *outfile;
    sbinfile diskfile;
    wavplaybinfile wavfile;
    binfile nofile;
    outfile=&nofile;

#ifdef WIN32
    ntplaybinfile musicout;
#endif
#ifdef DOS
    sbplaybinfile musicout;
#endif
#ifdef LINUX
    linuxplaybinfile musicout;
#endif

    char *newname=0;
    if (writewav)
    {
      newname=new char[strlen(filename)+5];
      if (strrchr(filename, '\\'))
        strcpy(newname, strrchr(filename, '\\')+1);
      else
      if (strrchr(filename, '/'))
        strcpy(newname, strrchr(filename, '/')+1);
      else
      if (strrchr(filename, ':'))
        strcpy(newname, strrchr(filename, ':')+1);
      else
        strcpy(newname, filename);
      if (!strcmp(newname+strlen(newname)-4, ".mp3"))
        newname[strlen(newname)-4]=0;
      strcat(newname, ".wav");
      outname=newname;
    }

    if (outname)
    {
      if (diskfile.open(outname, sbinfile::openrw|sbinfile::opentr))
      {
        printf("Could not create output file\n");
        continue;
      }
      outfile=&diskfile;
      if (!strcmp(outname+strlen(outname)-4, ".wav"))
      {
        wavfile.open(diskfile, freq, stereo, 1, 0);
        outfile=&wavfile;
      }
      printf("writing to %s\n", newname);
    }
#ifdef WIN32
    else
    if (!tonull)
    {
      if (musicout.open(freq, stereo, 1, 1152*(stereo?2:1)*2, buflen))
      {
        printf("Could not create output stream\n");
        continue;
      }
      SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
      outfile=&musicout;
    }
#endif
#ifdef LINUX
    else
    if (!tonull)
    {
      if (musicout.open(freq, stereo, 1))
      {
        printf("Could not create output stream\n");
        continue;
      }
//      setpriority(PRIO_PROCESS,getpid(),-20);
      outfile=&musicout;
    }
#endif
#ifdef DOS
    else
    if (!tonull)
    {
      if (musicout.open(freq, stereo, 1, 0x20000))
      {
        printf("Could not create output stream\n");
        continue;
      }
      outfile=&musicout;
    }
#endif

    delete newname;

    float vols[3][3];
    vols[0][0]=0.5*(1.0-bal)*(1.0-ctr+sep);
    vols[0][1]=0.5*(1.0-bal)*(1.0+ctr-sep);
    vols[0][2]=1-bal;
    vols[1][0]=0.5*(1.0+bal)*(1.0-ctr-sep)*srnd;
    vols[1][1]=0.5*(1.0+bal)*(1.0+ctr+sep)*srnd;
    vols[1][2]=(1.0+bal)*srnd;
    vols[2][0]=0.5*(1.0-ctr);
    vols[2][1]=0.5*(1.0+ctr);
    vols[2][2]=1.0;
    dec.ioctl(dec.ioctlsetstereo, vols, 0);

    char sampbuf[4608];

    int bps=(stereo?4:2)*freq;

#ifdef TIME
    initclock();
    long t0=0;
#endif

    idconsolebinfile con;
    con.open();
    int bread=0;

    int chvol=1;
    int chpitch=1;
    int pause=0;

    while (!dec.eof())
    {
      unsigned char key=0;
      while (con.read(&key,1))
      {
        switch (key)
        {
        case 'q': case 'x': case 4: stopped=3; break;
        case '\r': stopped=1; break;
        case 'n': stopped=2; break;
        case 'f': dec.seekcur(bps*2); break;
        case 'r': dec.seekcur(-bps*2); break;
        case 'F': dec.seekcur(bps*15); break;
        case 'R': dec.seekcur(-bps*15); break;
        case 'l': stopped=2; nextstep=-1; break;
        case '+': vol*=1.05; chvol=1; break;
        case '-': vol/=1.05; chvol=1; break;
        case 'b': pitch-=0.01; chpitch=1; break;
        case 't': pitch+=0.01; chpitch=1; break;
        case ' ': pause=!pause; break;
        }
      }
      if (stopped)
        break;
      if (chvol)
      {
        chvol=0;
        float vol0=vol;
        dec.ioctl(dec.ioctlsetvol, &vol0, 0);
      }
      if (chpitch)
      {
        chpitch=0;
        static float l3equal[576];
        for (i=0; i<576; i++)
          l3equal[i]=exp(pitch*log((i+0.5)*freq/(576.0*2*pitch0)));
        dec.ioctl(dec.ioctlsetequal576, l3equal, 0);
      }

      int l=4608;
      if (pause)
        memset(sampbuf, 0, l);
      else
      {
#ifdef TIME
        if (!quiet)
          t0-=getclock();
#endif
        l=dec.read(sampbuf, l);
        if (!l)
          break;
#ifdef TIME
        if (!quiet)
          t0+=getclock();
#endif
        bread+=l;
      }
      outfile->write(sampbuf, l);
      if (!quiet)
      {
        fprintf(stdout, "\r%.3f", (float)((float)dec.tell()/bps));
        if (dec.length())
          fprintf(stdout, "/%.3f=%.3f", (float)((float)dec.length()/bps), (float)((float)dec.tell()/dec.length()));
#ifdef TIME
        float t=(float)t0/CLOCKGRAN;
        fprintf(stdout, ":%.3f=%.3f", (float)t, (float)((float)t*bps/bread));
#endif
        fflush(stdout);
      }
    }

#ifdef TIME
    closeclock();
#endif

    if (stopped)
      outfile->ioctl(outfile->ioctllinger, 0);
    while (con.read(0,1));

    if (!quiet)
      printf("\n\n");

    con.close();
    dec.close();
    wavedata->close();
    musicin->close();
    logfile.close();
    outfile->close();
    diskfile.close();
  }

  return 0;
}



int test_play_mp3_old(char *mp3_filename)
{
//	char *proxy=0;
//	char *outname=0;
//	char *logname=0;
//	fileliststruct *flp;
	int i;
//	int writewav=0;
//	int scramble=1;
//	int repeat=0;
	float vol=1;
	float bal=0;
//	int tonull=0;
	int down=0;
	int chn=0;
//	int quiet=0;
	float ctr=0;
	float sep=1;
	float srnd=1;
	float pitch=0;
	float pitch0=1764;
	int buflen=4;
//	fileliststruct *filelist=0;

	//(1) analyse argv -------------------------------------------------------------
	//for (i=1; i<argc; i++)
//	{
		//if (argv[i][0]=='-')
//		{
			/*
			switch (argv[i][1])
			{
			case 'n': tonull=1; break;
			case 'm': chn=1; break;
			case 'q': quiet=1; break;
			case 's': chn=2; break;
			case 'S': chn=-2; break;
			case '2': down=1; break;
			case '4': down=2; break;
			case 'i': srnd=-1; break;
			case 'P': proxy=argv[i]+2; break;
			case 'l': logname=argv[i]+2; break;
			case 'v': vol=atof(argv[i]+2); break;
			case 'b': bal=atof(argv[i]+2); break;
			case 'c': ctr=atof(argv[i]+2); break;
			case 'p': sep=atof(argv[i]+2); break;
			case 'f': pitch=atof(argv[i]+2); break;
			case 'F': pitch0=atof(argv[i]+2); break;
			case 'o': outname=argv[i]+2; break;
			case 'w': writewav=1; break;
			case 'R': repeat=1; break;
			case 'O': scramble=0; break;
			case 'B': buflen=atoi(argv[i]+2); break;
			default: printf("unknown switch\n"); return 1;
			}
			*/
//		}
//		else
//		{
//			addfiles(filelist, argv[i]);
//			addfiles(filelist, mp3_filename);
//		}
//	}
	
	/*
	if (!quiet)
		printf("amp11   Audio-MPEG decoder (%s)  (c) 1997-2000 Niklas Beisert\n\n", VERSION);
	*/
	/*
	if (!filelist)
	{
		printf("amp11 comes with ABSOLUTELY NO WARRANTY, for details see COPYING (GPL 2)\n");
		printf("\n");
		printf("amp11 [-q] [-n] [-PPROXY] [-lLOG] INFILE|URL [-oOUTFILE|-w]\n");
		printf("      [-m] [-s] [-S] [-2] [-4]\n");
		printf("      [-vVOL]\n");
		printf("      [-pSEP] [-cCTR] [-bBAL] [-i]\n");
		printf("      [-fPITCH] [-FFREQ0]\n");
		printf("      [-R] [-O]\n");
		printf("      [-BBUFLEN]\n");
		return 1;
	}
	*/

	/*
	int nfile=0;
	for (flp=filelist; flp; flp=flp->next)
		nfile++;
	*/
	
	/*
	if ((nfile!=1)&&!quiet)
		printf("playing %i files\n\n", nfile);
	*/
	
	srand(time(0));
	
//	int nextstep=0;
	int stopped=0;
	int pos=0;
		
	//(2) loop play -------------------------------------------------------------------
//	while (1)
	{
//		if (stopped==3)	//quit
//			break;
//		pos+=nextstep;
//		if ((pos==nfile)&&(stopped<2)&&!repeat)
//			break;

		/*
		if (pos<0)
			pos+=nfile;
		if (pos>=nfile)
			pos-=nfile;
		*/
		/*
		if (!pos&&(stopped<2)&&scramble)
			scramblefilelist(filelist);
		*/
//		nextstep=1;
		stopped=0;
		
		/*
		flp=filelist;
		for (i=0; i<pos; i++)
			flp=flp->next;
		*/

		//char *filename=flp->name;
		char *filename = mp3_filename;
		/*
		if (!quiet)
		{
			if (nfile!=1)
				printf("%i: ", pos+1);
			printf("%s\n", filename);
		}
		*/
		
		binfile *musicin=0;
		sbinfile smusicin;
		/*
		httpbinfile httpmusicin;
		if (!strncmp(filename, "http://", 7))
		{
			if (httpmusicin.open(filename, proxy, 0, 0))
			{
				printf("Could not open input URL\n");
				continue;
			}
			httpmusicin.ioctl(httpmusicin.ioctlrrbufset, 16384);
			musicin=&httpmusicin;
		}
		else
		*/
//		{
			if (smusicin.open(filename, sbinfile::openro)<0)
			{
				printf("Could not open input file\n");
				return -1;
//				continue;
			}
			
			musicin=&smusicin;
//		}
		
		
		
		/*
		sbinfile logfile;
		if (logname)
		{
			logfile.open(logname, sbinfile::openrw|sbinfile::opentr);
			musicin->ioctl(musicin->ioctlrsetlog, &logfile, 0);
		}
		*/
		
		abinfile awavedata;
		mpegsplitstream mpgsplit;
		binfile *wavedata=musicin;
		if (peekub4(*musicin)==0x1BA) // full mpeg stream
		{
			if (mpgsplit.open(*musicin, 0xC0, 0))
			{
				printf("mpeg stream does not contain audio data\n");
				return -2;
//				continue;
			}
			wavedata=&mpgsplit;
		}
		else if (peekub4(*musicin)==0x52494646) // RIFF aka .WAV
		{
			char riff[4];
			int blklen=12;
			while (blklen&&!musicin->ioctl(musicin->ioctlreof))
				blklen-=musicin->read(0, blklen);
			while (1)
			{
				if (musicin->read(riff, 4)!=4)
				{
					printf("Could not find data chunk in RIFF file\n");
					return 1;
				}
				blklen=getil4(*musicin);
				if (!memcmp(riff, "data", 4))
					break;
				while (blklen&&!musicin->ioctl(musicin->ioctlreof))
					blklen-=musicin->read(0, blklen);
			}
			awavedata.open(*musicin, musicin->tell(), blklen);
			wavedata=&awavedata;
		}
		else if (peekub4(*musicin)==0x494E464F) // what's this? ah: I N F O ... ? did i do this?
		{
			getib4(*musicin);
			int blklen=getib4(*musicin);
			while (blklen&&!musicin->ioctl(musicin->ioctlreof))
				blklen-=musicin->read(0, blklen);
			if (musicin->getmode()&binfile::modeseek)
			{
				char tag[3];
				int pos=musicin->tell();
				musicin->seekend(-128);
				musicin->read(tag, 3);
				awavedata.open(*musicin, pos, musicin->length()-pos-(memcmp(tag, "TAG", 3)?0:128));
				wavedata=&awavedata;
			}
		}
		else if (peekub4(*musicin)&0xFFFFFF00==0x49443300) // ID3v2
		{
			getib4(*musicin);
			getib2(*musicin);
			int blklen=getu1(*musicin)<<21;
			blklen+=getu1(*musicin)<<14;
			blklen+=getu1(*musicin)<<7;
			blklen+=getu1(*musicin);
			while (blklen&&!musicin->ioctl(musicin->ioctlreof))
				blklen-=musicin->read(0, blklen);
			if (musicin->getmode()&binfile::modeseek)
			{
				awavedata.open(*musicin, musicin->tell(), musicin->length()-musicin->tell());
				wavedata=&awavedata;
			}
		}
		else if (peekub2(*musicin)<0xFFF0)
		{
			int blklen=1024;
			while (blklen&&!musicin->ioctl(musicin->ioctlreof))
			{
				int s=peekub2(*musicin);
				if ((s>=0xFFF0)&&(s!=0xFFFF))
					break;
				blklen-=musicin->read(0, 1);
			}
			if (musicin->getmode()&binfile::modeseek)
			{
				char tag[3];
				int pos=musicin->tell();
				musicin->seekend(-128);
				musicin->read(tag, 3);
				awavedata.open(*musicin, pos, musicin->length()-pos-(memcmp(tag, "TAG", 3)?0:128));
				wavedata=&awavedata;
			}
		}
		else if (musicin->getmode()&binfile::modeseek)
		{
			char tag[3];
			musicin->seekend(-128);
			musicin->read(tag, 3);
			awavedata.open(*musicin, 0, musicin->length()-(memcmp(tag, "TAG", 3)?0:128));
			wavedata=&awavedata;
		}
		
		int freq,stereo;
		
		static ampegdecoder dec;
		/*
		if (!quiet)
		{
			int lay,lfe,freq,stereo,rate;
			if (dec.getheader(*wavedata,lay,lfe,freq,stereo,rate))
				printf("layer %i, %i Hz %s, %i kbps\n", lay+1, freq, stereo?"stereo":"mono", rate/1000);
		}
		*/
		
		if (dec.open(*wavedata, freq, stereo, 1, down, chn))
		{
			printf("Could not open audio mpeg\n");
			return -3;
//			continue;
		}
		
		/*
		if (!quiet)
			printf("playing %i Hz %s\n", freq, stereo?"stereo":"mono");
		*/

		/*
		fflush(stdout);
		*/
		
		binfile *outfile;
//		sbinfile diskfile;
//		wavplaybinfile wavfile;
		binfile nofile;
		outfile=&nofile;
		
		ntplaybinfile musicout;

		/*
		char *newname=0;
		if (writewav)
		{
			newname=new char[strlen(filename)+5];
			if (strrchr(filename, '\\'))
				strcpy(newname, strrchr(filename, '\\')+1);
			else if (strrchr(filename, '/'))
				strcpy(newname, strrchr(filename, '/')+1);
			else if (strrchr(filename, ':'))
				strcpy(newname, strrchr(filename, ':')+1);
			else
				strcpy(newname, filename);
			if (!strcmp(newname+strlen(newname)-4, ".mp3"))
				newname[strlen(newname)-4]=0;
			strcat(newname, ".wav");
			outname=newname;
		}
		*/

		/*
		if (outname)
		{
			if (diskfile.open(outname, sbinfile::openrw|sbinfile::opentr))
			{
				printf("Could not create output file\n");
				continue;
			}
			outfile=&diskfile;
			if (!strcmp(outname+strlen(outname)-4, ".wav"))
			{
				wavfile.open(diskfile, freq, stereo, 1, 0);
				outfile=&wavfile;
			}
			printf("writing to %s\n", newname);
		}
		else if (!tonull)
		*/
//		{
			if (musicout.open(freq, stereo, 1, 1152*(stereo?2:1)*2, buflen))
			{
				printf("Could not create output stream\n");
				return -4;
//				continue;
			}
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
			outfile=&musicout;
//		}
		
		/*
		delete newname;
		*/
		
		float vols[3][3];
		vols[0][0]=0.5*(1.0-bal)*(1.0-ctr+sep);
		vols[0][1]=0.5*(1.0-bal)*(1.0+ctr-sep);
		vols[0][2]=1-bal;
		vols[1][0]=0.5*(1.0+bal)*(1.0-ctr-sep)*srnd;
		vols[1][1]=0.5*(1.0+bal)*(1.0+ctr+sep)*srnd;
		vols[1][2]=(1.0+bal)*srnd;
		vols[2][0]=0.5*(1.0-ctr);
		vols[2][1]=0.5*(1.0+ctr);
		vols[2][2]=1.0;
		dec.ioctl(dec.ioctlsetstereo, vols, 0);
		
		char sampbuf[4608];
		
		int bps=(stereo?4:2)*freq;
		
		idconsolebinfile con;
		con.open();
		int bread=0;
		
		int chvol=1;
		int chpitch=1;
		int pause=0;
		
		while (!dec.eof())
		{
			unsigned char key=0;
			while (con.read(&key,1))
			{
				switch (key)
				{
				case 'q': case 'x': case 4: stopped=3; break;
				case '\r': stopped=1; break;
				case 'n': stopped=2; break;
				case 'f': dec.seekcur(bps*2); break;
				case 'r': dec.seekcur(-bps*2); break;
				case 'F': dec.seekcur(bps*15); break;
				case 'R': dec.seekcur(-bps*15); break;
//				case 'l': stopped=2; nextstep=-1; break;
				case '+': vol*=1.05; chvol=1; break;
				case '-': vol/=1.05; chvol=1; break;
				case 'b': pitch-=0.01; chpitch=1; break;
				case 't': pitch+=0.01; chpitch=1; break;
				case ' ': pause=!pause; break;
				}
			}
			if (stopped)
				break;
			if (chvol)
			{
				chvol=0;
				float vol0=vol;
				dec.ioctl(dec.ioctlsetvol, &vol0, 0);
			}
			if (chpitch)
			{
				chpitch=0;
				static float l3equal[576];
				for (i=0; i<576; i++)
					l3equal[i]=exp(pitch*log((i+0.5)*freq/(576.0*2*pitch0)));
				dec.ioctl(dec.ioctlsetequal576, l3equal, 0);
			}
			
			int l=4608;
			if (pause)
				memset(sampbuf, 0, l);
			else
			{
				l=dec.read(sampbuf, l);
				if (!l)
					break;
				bread+=l;
			}
			outfile->write(sampbuf, l);
			/*
			if (!quiet)
			{
				fprintf(stdout, "\r%.3f", (float)((float)dec.tell()/bps));
				if (dec.length())
					fprintf(stdout, "/%.3f=%.3f", (float)((float)dec.length()/bps), (float)((float)dec.tell()/dec.length()));
				fflush(stdout);
			}
			*/
		}
		if (stopped)
			outfile->ioctl(outfile->ioctllinger, 0);
		while (con.read(0,1));
		
		/*
		if (!quiet)
			printf("\n\n");
		*/
		
		con.close();
		dec.close();
		wavedata->close();
		musicin->close();
		/*
		logfile.close();
		*/
		outfile->close();
//		diskfile.close();
  }
  
  return 0;
}
