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


//=======================================================================================
// our own source
//=======================================================================================
#include "rays.h"
#include "xsystem.h"
#include "mp3.h"


#define	LOCK_MP3CFG()		EnterCriticalSection(&g_csMp3CFG)
#define	UNLOCK_MP3CFG()		LeaveCriticalSection(&g_csMp3CFG)


#define	MP3_INIT			0x01
#define	MP3_PLAY			0x02





typedef	struct	tagMP3_CFG
{
	int		repeat;			//=0; repeat play flag
	float	vol;			//=1;
	float	bal;			//=0;
	int		down;			//=0;
	int		chn;			//=0;
	float	ctr;			//=0;
	float	sep;			//=1;
	float	srnd;			//=1;
	float	pitch;			//=0;
	float	pitch0;			//=1764;
	int		stopped;		//=0;
	int		buflen;			//=4;
	int		pause;			//=0
	int		playing;		//=0
	int		chvol;			//=0
	int		chpitch;		//=0
    char    filename[_MAX_FNAME];   //store mp3 filename.
} MP3_CFG, *LPMP3_CFG;


static	CRITICAL_SECTION	g_csMp3CFG;

static	MP3_CFG		mp3_cfg ;
static	DWORD		dwMp3ThreadID	=	0;
static	HANDLE		hPlayMp3Thread	=	0;
static	DWORD		dwMp3Flag		=	0;

static	DWORD   WINAPI  PlayMp3Thread ( LPVOID lpParameter );


static	void	init_mp3_cfg(void)
{
	mp3_cfg.repeat	= 0;
	mp3_cfg.vol		= 1;
	mp3_cfg.bal		= 0;
	mp3_cfg.down	= 0;
	mp3_cfg.chn		= 0;
	mp3_cfg.ctr		= 0;
	mp3_cfg.sep		= 1;
	mp3_cfg.srnd	= 1;
	mp3_cfg.pitch	= 0;
	mp3_cfg.pitch0	= 1764;
	mp3_cfg.stopped	= 0;
	mp3_cfg.buflen	= 4;
	mp3_cfg.pause	= 0;
	mp3_cfg.playing = 0;
	mp3_cfg.chvol	= 0;
	mp3_cfg.chpitch = 0;
	mp3_cfg.chvol	= 1;
	mp3_cfg.chpitch = 1;
    memset(mp3_cfg.filename, '\0', _MAX_FNAME);
}



EXPORT	int		FNBACK  init_mp3(void)
{
	if(dwMp3Flag & MP3_INIT)	// have already initialized
		return 0;

	dwMp3Flag = 0;
	InitializeCriticalSection( &g_csMp3CFG );
	LOCK_MP3CFG();
	init_mp3_cfg();
	UNLOCK_MP3CFG();

	dwMp3Flag |= MP3_INIT;
	return 0;
}


EXPORT	void	FNBACK  active_mp3(int active)
{
	if(active)
	{
		LOCK_MP3CFG();
		mp3_cfg.pause = 0;
		UNLOCK_MP3CFG();
	}
	else
	{
		LOCK_MP3CFG();
		mp3_cfg.pause = 1;
		UNLOCK_MP3CFG();
	}
}


EXPORT	void	FNBACK  free_mp3(void)
{
	if( !(dwMp3Flag & MP3_INIT) )
		return;
	stop_mp3();

	DeleteCriticalSection( &g_csMp3CFG );
	dwMp3Flag = 0;
}



EXPORT  void    FNBACK  play_mp3(SLONG loop, USTR *filename)
{
	if( !(dwMp3Flag & MP3_INIT ) )	// have not init
		return;
	if( dwMp3Flag & MP3_PLAY )	// have been play
		return;

	LOCK_MP3CFG();
    // TODO: Jack, need to use restored mp3 volume. [6/14/2002]
	init_mp3_cfg();
	mp3_cfg.repeat = loop;
    strcpy((char *)mp3_cfg.filename, (const char *)filename);
	UNLOCK_MP3CFG();

	hPlayMp3Thread = CreateThread( NULL, 0, PlayMp3Thread, NULL, 0, &dwMp3ThreadID );
	if(NULL == hPlayMp3Thread)
	{
		return;
	}

	dwMp3Flag |= MP3_PLAY;
}


EXPORT  void    FNBACK  stop_mp3(void)
{
	if( !( dwMp3Flag & MP3_INIT) )	// not init
		return;
	if( !( dwMp3Flag & MP3_PLAY) )	// not play
		return;

	LOCK_MP3CFG();
	mp3_cfg.stopped = 3;	//user break
	UNLOCK_MP3CFG();

    if( hPlayMp3Thread )
    {
        Sleep( 100 );
		hPlayMp3Thread = 0;
    }

	dwMp3Flag &= ~MP3_PLAY;
}


EXPORT  void    FNBACK  set_mp3_volume(SLONG volume)
{
	float	vol;

	if( !(dwMp3Flag & MP3_INIT ) )
		return;

	//( -10000 ~ 0) compatible with DirectSound
	if(volume > 0)
		volume = 0;
	if(volume < -10000)
		volume = -10000;
	vol =  1.0 - (float)( fabs((float)volume)/10000.0 );

	LOCK_MP3CFG();
	mp3_cfg.vol = vol;
	mp3_cfg.chvol = 1;
	UNLOCK_MP3CFG();
}


EXPORT  void    FNBACK  set_mp3_pan(SLONG pan)
{
	if( !(dwMp3Flag & MP3_INIT ) )
		return;
}


EXPORT	SLONG	FNBACK	is_mp3_playing(void)
{
	SLONG	playing;

	if( !(dwMp3Flag & MP3_INIT ) )
		return FALSE;
	if( !(dwMp3Flag & MP3_PLAY) )
		return FALSE;

	LOCK_MP3CFG();
	playing = mp3_cfg.playing;
	UNLOCK_MP3CFG();
	return playing;
}


static	DWORD   WINAPI  PlayMp3Thread ( LPVOID lpParameter )
{
	char  mp3_filename[_MAX_FNAME];
	float vol=1;
	float bal=0;
	int down=0;
	int chn=0;
	float ctr=0;
	float sep=1;
	float srnd=1;
	float pitch=0;
	float pitch0=1764;
	int buflen=4;
	int repeat;
	int stopped;
	int pause;
	char sampbuf[4608];
	int chvol;
	int chpitch;
	char disp_rec[512];

	//we use the lpParameter for our mp3 filename
	memset(sampbuf, 0, 4608);

	LOCK_MP3CFG();
	mp3_cfg.playing = TRUE;
    strcpy((char *)mp3_filename, (const char *)mp3_cfg.filename);
	UNLOCK_MP3CFG();
	
	while( 1 )
	{
		LOCK_MP3CFG();
		repeat	= mp3_cfg.repeat;
		vol		= mp3_cfg.vol;
		bal		= mp3_cfg.bal;
		down	= mp3_cfg.down;
		chn		= mp3_cfg.chn;
		ctr		= mp3_cfg.ctr;
		sep		= mp3_cfg.sep;
		srnd	= mp3_cfg.srnd;
		pitch	= mp3_cfg.pitch;
		pitch0	= mp3_cfg.pitch0;
		stopped	= mp3_cfg.stopped;
		buflen	= mp3_cfg.buflen;
		pause   = mp3_cfg.pause;
		chvol	= mp3_cfg.chvol;
		chpitch = mp3_cfg.chpitch;
		UNLOCK_MP3CFG();

		if (stopped==3)
			break;

		binfile *musicin=0;
		sbinfile smusicin;
		if (smusicin.open(mp3_filename, sbinfile::openro)<0)
		{
			sprintf((char *)disp_rec, "open file %s error", mp3_filename);
			log_error(1, (USTR*)disp_rec);
			return -1;
		}
		musicin=&smusicin;
		
		abinfile awavedata;
		mpegsplitstream mpgsplit;
		binfile *wavedata=musicin;
		if (peekub4(*musicin)==0x1BA) // full mpeg stream
		{
			if (mpgsplit.open(*musicin, 0xC0, 0))
			{
				sprintf((char *)disp_rec, "mpeg stream does not contain audio data");
				log_error(1, (USTR*)disp_rec);
				return -2;
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
					sprintf((char *)disp_rec, "Could not find data chunk in RIFF file");
					log_error(1, (USTR*)disp_rec);
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
		
		if (dec.open(*wavedata, freq, stereo, 1, down, chn))
		{
			sprintf((char *)disp_rec, "Could not open audio mpeg");
			log_error(1, (USTR*)disp_rec);
			return -3;
		}
		
		binfile *outfile;
		binfile nofile;
		outfile=&nofile;
		
		ntplaybinfile musicout;
		
		
		if (musicout.open(freq, stereo, 1, 1152*(stereo?2:1)*2, buflen))
		{
			sprintf((char *)disp_rec, "Could not create output stream");
			log_error(1, (USTR*)disp_rec);
			return -4;
		}
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		outfile=&musicout;
		
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
		
		int bps=(stereo?4:2)*freq;
		
		/*
		idconsolebinfile con;
		con.open();
		*/
		
		int bread=0;
		
		while (!dec.eof())
		{
			LOCK_MP3CFG();
			repeat	= mp3_cfg.repeat;
			vol		= mp3_cfg.vol;
			bal		= mp3_cfg.bal;
			down	= mp3_cfg.down;
			chn		= mp3_cfg.chn;
			ctr		= mp3_cfg.ctr;
			sep		= mp3_cfg.sep;
			srnd	= mp3_cfg.srnd;
			pitch	= mp3_cfg.pitch;
			pitch0	= mp3_cfg.pitch0;
			stopped	= mp3_cfg.stopped;
			buflen	= mp3_cfg.buflen;
			pause   = mp3_cfg.pause;
			chvol	= mp3_cfg.chvol;
			chpitch = mp3_cfg.chpitch;
			UNLOCK_MP3CFG();

			{//------------------------------------------------------------------------
				/*
				unsigned char key=0;
				while (con.read(&key,1))
				{
					switch (key)
					{
					case 'q': case 'x': case 4: stopped=3; break;
					case '\r': stopped=1; break;
					case 'f': dec.seekcur(bps*2); break;
					case 'r': dec.seekcur(-bps*2); break;
					case 'F': dec.seekcur(bps*15); break;
					case 'R': dec.seekcur(-bps*15); break;
					case '+': vol*=1.05; chvol=1; break;
					case '-': vol/=1.05; chvol=1; break;
					case 'b': pitch-=0.01; chpitch=1; break;
					case 't': pitch+=0.01; chpitch=1; break;
					case ' ': pause=!pause; break;
					}
				}
				*/
			}//------------------------------------------------------------------------
			
			if (stopped)
				break;
			
			if (chvol)
			{
				LOCK_MP3CFG();
				mp3_cfg.chvol=0;
				UNLOCK_MP3CFG();

				float vol0=vol;
				dec.ioctl(dec.ioctlsetvol, &vol0, 0);
			}
			
			if (chpitch)
			{
				LOCK_MP3CFG();
				mp3_cfg.chpitch=0;
				UNLOCK_MP3CFG();

				static float l3equal[576];
				for (int i=0; i<576; i++)
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
		}
		if (stopped)
			outfile->ioctl(outfile->ioctllinger, 0);
		/*
		while (con.read(0,1));
		con.close();
		*/
		dec.close();
		wavedata->close();
		musicin->close();
		outfile->close();

		if ((stopped<2) && !repeat)
			break;
	}

	LOCK_MP3CFG();
	mp3_cfg.playing = FALSE;
	UNLOCK_MP3CFG();

	return 0;
}


