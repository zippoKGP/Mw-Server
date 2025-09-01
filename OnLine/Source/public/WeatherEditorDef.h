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
// 最大區域數目
#define MAX_ZONE_PER_MACRO      32
#define MAX_WEATHER_COMBO_TYPE  128
// 氣候類型定義
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


// 資料結構定義
// 天氣基本影響屬性資料
typedef struct MW_WEATHER_BASIC_PROPERTY
{
        int     iBossRate;
        int     iProperty;
}TMW_WEATHER_BASIC_PROPERTY;
// 組合天氣的資料
typedef struct MW_WEATHER_COMBO_DATA
{
        ULONG   ulWeatherComboType;             // 經過疊加後的天氣類型
}TMW_WEATHER_COMBO_DATA;
// 影響怪物屬性
typedef struct MW_PROPERTY_RATE
{
        int     iBossRate;
        int     iAttkDef;
        int     iExp;
        int     iDropMoney;
        int     iFastness;
}TMW_PROPERTY_RATE;
// 區域每小時氣候資料
typedef struct MW_WEATHER_ZONE_HOUR_DATA
{
        ULONG   ulWeatherType;                   // 天氣類型
        TMW_PROPERTY_RATE       PropertyRateData;       // 影響怪物屬性的資料
}TMW_WEATHER_ZONE_HOUR_DATA;
// 區域每天氣候資料
typedef struct MW_WEATHER_ZONE_DAY_DATA
{
        TMW_WEATHER_ZONE_HOUR_DATA      HourData[24];   // 區域每日天氣資料
}TMW_WEATHER_ZONE_DAY_DATA;
// 區域天氣資料
typedef struct MW_WEATHER_ZONE_DATA
{
        char    cZoneName[32];                  // 區域名稱
        char    cZoneMapName[32];               // 區域地圖名稱
        TMW_WEATHER_ZONE_DAY_DATA       DayData[7];     // 區域一週天氣資料
}TMW_WEATHER_ZONE_DATA;
// 全世界天氣資料
typedef struct MW_WEATHER_MACRO
{
        char    cProjectName[128];              // 專案名稱
        DWORD   dwVersion;                      // 版本
        char    cUpdateDate[12];                // 異動日期
        char	cUpdateUser[32];                // 異動人員
	int     iRepeat;			// 是否重復執行
        TMW_WEATHER_BASIC_PROPERTY  WeatherBasic[13];      // 天氣的基本資料
        TMW_WEATHER_COMBO_DATA  WeatherCombo[MAX_WEATHER_COMBO_TYPE];
        TMW_WEATHER_ZONE_DATA   Zone[MAX_ZONE_PER_MACRO];  // 區域資料
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

