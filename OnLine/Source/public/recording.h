/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2005 ALL RIGHTS RESERVED.

  *file name    : recording.h
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/11
******************************************************************************/ 

#ifndef WORLDSERVER_RECORDING_H
#define WORLDSERVER_RECORDING_H


#if 1

#define DEBUG_RECORD printf

#define FILE_DEBUG_RECORD writeDebugFile

#define DEBUG_RECORD_FILE_LINE DEBUG_RECORD("%s, %d\n", __FILE__, __LINE__);

#define DEBUG_RECORD_MESSAGE(p) DEBUG_RECORD("%s\n", #p);

#define DEBUG_MESSAGE_FILE_LINE Msg("%s %s, %d\n", #p, __FILE__, __LINE__);

#define DEBUG_RECORD_POS(p) Msg("%s : %.4f, %.4f, %.4f \n", #p, (p)[0], (p)[1], (p)[2]);

#define DEBUG_RECORD_VEC(p) Msg("%s : %.4f, %.4f, %.4f \n", #p, (p).x, (p).y, (p).z);

#define TRACE_RECORD_CALL(p) \
{	double old_time = Plat_FloatTime(); \
	DEBUG_RECORD("\n=====================================================\n"); \
	DEBUG_RECORD("call %s start. \n", #p); \
	hr = (p); \
	double new_time = Plat_FloatTime(); \
	DEBUG_RECORD("call %s end, time = %.6f, return : %d\n", #p, new_time-old_time, hr); \
	if (FAILED(hr)) { Assert(0); return hr; } \
}

#define TIME_FUNCTION_CALL(p) \
{	double _old_time = Plat_FloatTime(); \
	(p); \
	double _new_time = Plat_FloatTime(); \
	DEBUG_RECORD("call function: %s, time = %.3f msec\n", #p, (_new_time-_old_time)*1000.0f); \
}

#else

#define DEBUG_RECORD_FILE_LINE 0

#define DEBUG_RECORD_MESSAGE(p) 0

#define TRACE_RECORD_CALL(p) (hr = (p));

#define DEBUG_RECORD_POS(p) ((void)0)

#define DEBUG_RECORD_VEC(p) ((void)0)

#define TIME_FUNCTION_CALL

#endif

//////////////////////////////////////////////////////////////////////////
//
#define TRY_EXCEPT_RECORD(p) __try{p;} __except(true) { writeDebugFile("Except function : %s, %s, %d\n", #p, __FILE__, __LINE__); }

#define TRY_EXCEPT_RECORD_MSG(p, msg) __try{p;} __except(true) { writeDebugFile("Except function : %s, %s, %d\nmsg: %s"#p, __FILE__, __LINE__, msg); }

#define TRY_EXCEPT_RECORD_NET_MSG(p, msg) __try{p;} __except(true) { writeDebugFile("Except function : %s, %s, %d\n msg: %x"#p, __FILE__, __LINE__, msg); }

#define TRY_EXCEPT_RECORD_FILE(p) __try{p;} __except(true) { DEBUG_RECORD("Except function : %s, %s, %d\n"#p, __FILE__, __LINE__); }

//////////////////////////////////////////////////////////////////////////
//
#define STRING_2_IP(str, ip)								\
{															\
	unsigned char key[16] = {0};							\
	sscanf(str, "%d.%d.%d.%d", key+0, key+1, key+2, key+3); \
	ip = UGIP_ABCD(key[0], key[1], key[2], key[3]);			\
} 

#endif
