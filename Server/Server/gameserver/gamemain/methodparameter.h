#ifndef _MethodParameter_h_
#define _MethodParameter_h_

#include "CharacterDB.h"

/*
효과	EFF
장착	EQP
소지	HAV
*/

/*
약어정리..
GE : 일반공격
SK : 스킬공격
FC : 포스공격
CtDm : 지속피해
CtHp : 지속이득
Tm : 시간
Lv : 레벨
Avd : 회피
Rev : 회복
Mx : Max
Spd : 속도
Att : Attack
Fc : 공격력
Def : 방어(력)
Lck : 잠금
EXP : 경험치
Exp : 숙련도
Ext : 범위
DC : 할인률
Abs : 절대
Crt : 크리티컬
*/

//SF, ITEM, RES ..

struct _EFF_RATE
{
	enum {

		//[SF, ITEM] 일반 최종공격율
		GE_AttFc_ = 0, GA1 = 1, GA2 = 2,	

		//[SF, ITEM] 스킬_최종공격율	
		SK_AttFc = 3,				
		
		//[SF, ITEM] 포스_최종공격율		
		FC_AttFc = 4,				

		//[ITEM] 일반_스턴확률	
		GE_Stun = 5,				

		//[SF, ITEM] 방어력증가율	
		Part_Def = 6,		
		
		//[SF, ITEM] FP소모율	
		FP_Consum = 7,				

		//[ITEM] 포스_지속피해보조_시간_증가율
		FC_CtDm_Tm = 8,				

		//[ITEM] HP_최대치율	
		HP_Mx = 9, 

		//[ITEM] FP_최대치율	
		FP_Mx = 10, 

		//[ITEM] SP_최대치율	
		SP_Mx = 11,
		
		//[SF, ITEM] HP_흡수율	
		HP_Absorb = 12, 

		//[SF, ITEM] FP_흡수율	
		FP_Absorb = 13, 

		//[SF, ITEM] HP_회복율	
		HP_Rev = 14, 

		//[SF] FP_회복속도율
		FP_Rev = 15, 

		//[SF] SP_회복속도율
		SP_Rev = 16, 

		//[SF] 방패방어력증가율	
		Shield_Def = 17,

		__END = 18
	};

	enum { NUM = __END };
};

struct _EFF_PLUS
{
	enum{

		//[SF, ITEM] 일반_계열별_명중
		GE_Hit_= 0, GH1 = 1, GH2 = 2, 

		//[SF, ITEM] 일반_회피	
		GE_Avd = 3,		

		//[SF, ITEM] 일반_계열별_사정거리
		GE_Att_Dist_ = 4, GAD1 = 5, GAD2 = 6,	//사정거리 체크는 하지않음..

		//[] 스킬_사정거리
		SK_Att_Dist = 7,	

		//[SF] 포스_사정거리
		FC_Att_Dist = 8,	

		//[SF] 일반_계열별_공격속도
		GE_Att_Spd_ = 9, GAS1 = 10, GAS2 = 11, 

		//[] 스킬_공격속도
		SK_Att_Spd = 12,	

		//[SF] 포스_공격속도
		FC_Att_Spd = 13, 

		//[SF] 일반_크리티컬_구간
		GE_CrtExt = 14, 

		//[SF, ITEM] 내성별증가도
		Part_Tol_ = 15, PT1 = 16, PT2 = 17, PT3 = 18, 

		//[ITEM] 스킬_레벨증가
		SK_LvUp = 19, 

		//[SF, ITEM] 이동속도_뛰기
		Move_Run_Spd = 20, 

		//[ITEM] 자신을 안보이기
		Transparency = 21,	

		//[SF, ITEM] 투명, 스텔스..다보기
		Detect = 22,	

		//[ITEM] 지속피해보조 안걸림
		Anti_CtDm = 23, 

		//[ITEM] 대상플레이어의 약한부위를 보기
		Know_WeakPoint = 24, 

		//[SF] FP_회복속도량
		FP_Rev_Add = 25, 

		//[SF]시야범위 감소
		View_Circle = 26,

		__END = 27
	};

	enum { NUM = __END };

};

struct _EFF_STATE
{
	enum{

		//[SF] 공격 스킬 잠금
		SK_Att_Lck = 0, 

		//[SF] 공격 포스 잠금
		FC_Att_Lck = 1, 

		//[SF] 지속이득보조 스킬 잠금
		SK_CtHp_Lck = 2, 

		//[SF] 지속이득보조 포스 잠금
		FC_CtHp_Lck = 3, 

		//[SF] 지속피해보조 포스 잠금
		FC_CtDm_Lck = 4, 

		//[SF] 스텔스
		Stealth = 5,	

		//[SF] 이동 잠금
		Move_Lck = 6,

		//[SF] 뛰기 잠금
		Run_Lck = 7, 		

		//[SF] 절대회피
		Abs_Avd = 8,	

		//[SF] 절대 크리티컬
		Abs_Crt = 9,			

		//[SF] 회복잠금
		Rev_Lck = 10,	

		//[SF] 대상방패무시
		Dst_No_Shd = 11,		

		//[SF] 대상의 방어력무시
		Dst_No_Def = 12,		

		//[SF] 대상을 스턴화	
		Dst_Make_Stun = 13,  

		//[SF] 반격기
		Res_Att = 14,	

		__END = 15
	};

	enum { NUM = __END };
};

struct _EFF_HAVE
{
	enum{

		//[RES] 도박에서의 승률이득
		Gamble_Prof = 0,		//나중..

		//[RES] 상점과의 거래에서의 이득
		Trade_Prof = 1,	

		//[RES] 경험치획득의 이득
		Exp_Prof = 2,		

		//[RES] 같은 종족에게 모두 채팅을 보낼수있음
		Chat_All_Race = 3,	

		//[RES] 자신의 레벨보다 상위의 아이템을 장착할수있음
		Equip_Lv_Up = 4,	

		__END = 5
	};

	enum { NUM = __END };
};

#endif