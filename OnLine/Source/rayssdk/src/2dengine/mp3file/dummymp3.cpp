//dummy.cpp
//=======================================================
#if	0
//=======================================================


static	int		test_play_mp3(char *mp3_filename, int repeat )
{
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
	
	srand(time(0));
	
	int stopped=0;
	
	stopped=0;

	while( 1 )
	{
		if (stopped==3)
			break;
		if (!repeat)
			break;

		binfile *musicin=0;
		sbinfile smusicin;
		if (smusicin.open(mp3_filename, sbinfile::openro)<0)
		{
			printf("Could not open input file\n");
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
				printf("mpeg stream does not contain audio data\n");
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
		
		if (dec.open(*wavedata, freq, stereo, 1, down, chn))
		{
			printf("Could not open audio mpeg\n");
			return -3;
		}
		
		binfile *outfile;
		binfile nofile;
		outfile=&nofile;
		
		ntplaybinfile musicout;
		
		
		if (musicout.open(freq, stereo, 1, 1152*(stereo?2:1)*2, buflen))
		{
			printf("Could not create output stream\n");
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
			{//------------------------------------------------------------------------
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
			}//------------------------------------------------------------------------
			
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
		while (con.read(0,1));
		
		con.close();
		dec.close();
		wavedata->close();
		musicin->close();
		outfile->close();
	}
	
	return 0;
}




#if 0///////////////////////
#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib") // automatic linking to winmm.dll for visualc
#endif


int ntplaybinfile::open(int rate, int stereo, int bit16, int blen, int nb)
{
  close();

  blklen=blen*(stereo?2:1)*(bit16?2:1);
  nblk=nb;

  hdrs=new WAVEHDR[nblk];
  if (!hdrs)
    return -1;

  playbuf=new char [blklen*nblk];
  if (!playbuf)
    return -1;

  WAVEFORMATEX form;
  form.wFormatTag=WAVE_FORMAT_PCM;
  form.nChannels=stereo?2:1;
  form.nSamplesPerSec=rate;
  form.wBitsPerSample=bit16?16:8;
  form.nBlockAlign=form.nChannels*form.wBitsPerSample/8;
  form.nAvgBytesPerSec=form.nSamplesPerSec*form.nBlockAlign;
  form.cbSize=0;

  if (waveOutOpen(&wavehnd, WAVE_MAPPER, &form, 0, 0, 0))
    return -1;

  int i;
  for (i=0; i<nblk; i++)
    hdrs[i].dwFlags=0;
  curbuflen=0;

  blocking=1;
  linger=-1;

  openmode(modewrite, 0, 0);
  return 0;
}

errstat ntplaybinfile::rawclose()
{
  closemode();

  int i;
  while (1)
  {
    if (linger>=0)
      break;
    for (i=0; i<nblk; i++)
      if (!(hdrs[i].dwFlags&WHDR_DONE)&&(hdrs[i].dwFlags&WHDR_PREPARED))
        break;
    if (i==nblk)
      break;
    Sleep(50);
  }
  waveOutReset(wavehnd);

  for (i=0; i<nblk; i++)
    if (hdrs[i].dwFlags&WHDR_PREPARED)
      waveOutUnprepareHeader(wavehnd, &hdrs[i], sizeof(*hdrs));
  waveOutClose(wavehnd);

  delete hdrs;
  delete playbuf;
  return 0;
}

binfilepos ntplaybinfile::rawwrite(const void *buf, binfilepos len)
{
  int l0=0;
  while (len)
  {
    if (!curbuflen)
    {
      int i;
      for (i=0; i<nblk; i++)
        if (hdrs[i].dwFlags&WHDR_DONE)
          waveOutUnprepareHeader(wavehnd, &hdrs[i], sizeof(*hdrs));
      for (i=0; i<nblk; i++)
        if (!(hdrs[i].dwFlags&WHDR_PREPARED))
          break;
      if (i==nblk)
      {
        if (!blocking)
          return l0;
        Sleep(20);
        continue;
      }
      curbuf=i;
    }
    int l=blklen-curbuflen;
    if (l>len)
      l=len;
    memcpy(playbuf+curbuf*blklen+curbuflen, buf, l);
    *(char**)&buf+=l;
    len-=l;
    l0+=l;
    curbuflen+=l;
    if (curbuflen==blklen)
    {
      hdrs[curbuf].lpData=playbuf+curbuf*blklen;
      hdrs[curbuf].dwBufferLength=blklen;
      hdrs[curbuf].dwFlags=0;
      waveOutPrepareHeader(wavehnd, &hdrs[curbuf], sizeof(*hdrs));
      waveOutWrite(wavehnd, &hdrs[curbuf], sizeof(*hdrs));
      curbuflen=0;
    }
  }
  return l0;
}

ntplaybinfile::ntplaybinfile()
{
}

ntplaybinfile::~ntplaybinfile()
{
  close();
}

binfilepos ntplaybinfile::rawioctl(intm code, void *buf, binfilepos len)
{
  binfilepos ret;
  switch (code)
  {
  case ioctlblocking:
    ret=blocking;
    blocking=len;
    return ret;
  case ioctlblockingget:
    return blocking;
  case ioctllingerget:
    return linger;
  case ioctllinger:
    ret=linger;
    linger=len;
    return ret;
  default:
    return binfile::rawioctl(code, buf, len);
  }
}
#endif///////////////////////



//=======================================================
#endif//0
