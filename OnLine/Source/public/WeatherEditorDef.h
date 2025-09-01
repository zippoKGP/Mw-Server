#ifndef _WEATHER_EDITOR_DEF_H_
#define _WEATHER_EDITOR_DEF_H_

//#include "Language.h"
#include <dos.h>
//#include <dir.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef unsigned short int  UHINT;
typedef unsigned long       ULONG;
typedef signed long         SLONG;
typedef unsigned char       UCHR;
typedef unsigned char       USTR;

#define VERSION                 200
// �̤j�ϰ�ƥ�
#define MAX_ZONE_PER_MACRO      32
#define MAX_WEATHER_COMBO_TYPE  128
// ��������w�q
#define CLEAR                   0x00000000
#define CLOUDY_LEVEL1           0x00000010
#define CLOUDY_LEVEL2           0x00000020
#define CLOUDY_LEVEL3           0x00000040
#define RAIN_LEVEL1             0x00000100
#define RAIN_LEVEL2             0x00000200
#define RAIN_LEVEL3             0x00000400
#define SNOW_LEVEL1             0x00001000
#define SNOW_LEVEL2             0x00002000
#define SNOW_LEVEL3             0x00004000
#define THUNDER_LEVEL1          0x00010000
#define THUNDER_LEVEL2          0x00020000
#define THUNDER_LEVEL3          0x00040000


// ��Ƶ��c�w�q
// �Ѯ�򥻼v�T�ݩʸ��
typedef struct MW_WEATHER_BASIC_PROPERTY
{
        int     iBossRate;
        int     iProperty;
}TMW_WEATHER_BASIC_PROPERTY;
// �զX�Ѯ𪺸��
typedef struct MW_WEATHER_COMBO_DATA
{
        ULONG   ulWeatherComboType;             // �g�L�|�[�᪺�Ѯ�����
}TMW_WEATHER_COMBO_DATA;
// �v�T�Ǫ��ݩ�
typedef struct MW_PROPERTY_RATE
{
        int     iBossRate;
        int     iAttkDef;
        int     iExp;
        int     iDropMoney;
        int     iFastness;
}TMW_PROPERTY_RATE;
// �ϰ�C�p�ɮ�Ը��
typedef struct MW_WEATHER_ZONE_HOUR_DATA
{
        ULONG   ulWeatherType;                   // �Ѯ�����
        TMW_PROPERTY_RATE       PropertyRateData;       // �v�T�Ǫ��ݩʪ����
}TMW_WEATHER_ZONE_HOUR_DATA;
// �ϰ�C�Ѯ�Ը��
typedef struct MW_WEATHER_ZONE_DAY_DATA
{
        TMW_WEATHER_ZONE_HOUR_DATA      HourData[24];   // �ϰ�C��Ѯ���
}TMW_WEATHER_ZONE_DAY_DATA;
// �ϰ�Ѯ���
typedef struct MW_WEATHER_ZONE_DATA
{
        char    cZoneName[32];                  // �ϰ�W��
        char    cZoneMapName[32];               // �ϰ�a�ϦW��
        TMW_WEATHER_ZONE_DAY_DATA       DayData[7];     // �ϰ�@�g�Ѯ���
}TMW_WEATHER_ZONE_DATA;
// ���@�ɤѮ���
typedef struct MW_WEATHER_MACRO
{
        char    cProjectName[128];              // �M�צW��
        DWORD   dwVersion;                      // ����
        char    cUpdateDate[12];                // ���ʤ��
        char	cUpdateUser[32];                // ���ʤH��
	int     iRepeat;			// �O�_���_����
        TMW_WEATHER_BASIC_PROPERTY  WeatherBasic[13];      // �Ѯ𪺰򥻸��
        TMW_WEATHER_COMBO_DATA  WeatherCombo[MAX_WEATHER_COMBO_TYPE];
        TMW_WEATHER_ZONE_DATA   Zone[MAX_ZONE_PER_MACRO];  // �ϰ���
}TMW_WEATHER_MACRO;

/*
#include "Main.h"
#include "Lzo.h"
#include "About.h"
#include "Login.h"
#include "SetProjectName.h"
#include "setup.h"
*/

#endif

