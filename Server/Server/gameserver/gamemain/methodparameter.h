#ifndef _MethodParameter_h_
#define _MethodParameter_h_

#include "CharacterDB.h"

/*
ȿ��	EFF
����	EQP
����	HAV
*/

/*
�������..
GE : �Ϲݰ���
SK : ��ų����
FC : ��������
CtDm : ��������
CtHp : �����̵�
Tm : �ð�
Lv : ����
Avd : ȸ��
Rev : ȸ��
Mx : Max
Spd : �ӵ�
Att : Attack
Fc : ���ݷ�
Def : ���(��)
Lck : ���
EXP : ����ġ
Exp : ���õ�
Ext : ����
DC : ���η�
Abs : ����
Crt : ũ��Ƽ��
*/

//SF, ITEM, RES ..

struct _EFF_RATE
{
	enum {

		//[SF, ITEM] �Ϲ� ����������
		GE_AttFc_ = 0, GA1 = 1, GA2 = 2,	

		//[SF, ITEM] ��ų_����������	
		SK_AttFc = 3,				
		
		//[SF, ITEM] ����_����������		
		FC_AttFc = 4,				

		//[ITEM] �Ϲ�_����Ȯ��	
		GE_Stun = 5,				

		//[SF, ITEM] ����������	
		Part_Def = 6,		
		
		//[SF, ITEM] FP�Ҹ���	
		FP_Consum = 7,				

		//[ITEM] ����_�������غ���_�ð�_������
		FC_CtDm_Tm = 8,				

		//[ITEM] HP_�ִ�ġ��	
		HP_Mx = 9, 

		//[ITEM] FP_�ִ�ġ��	
		FP_Mx = 10, 

		//[ITEM] SP_�ִ�ġ��	
		SP_Mx = 11,
		
		//[SF, ITEM] HP_�����	
		HP_Absorb = 12, 

		//[SF, ITEM] FP_�����	
		FP_Absorb = 13, 

		//[SF, ITEM] HP_ȸ����	
		HP_Rev = 14, 

		//[SF] FP_ȸ���ӵ���
		FP_Rev = 15, 

		//[SF] SP_ȸ���ӵ���
		SP_Rev = 16, 

		//[SF] ���й���������	
		Shield_Def = 17,

		__END = 18
	};

	enum { NUM = __END };
};

struct _EFF_PLUS
{
	enum{

		//[SF, ITEM] �Ϲ�_�迭��_����
		GE_Hit_= 0, GH1 = 1, GH2 = 2, 

		//[SF, ITEM] �Ϲ�_ȸ��	
		GE_Avd = 3,		

		//[SF, ITEM] �Ϲ�_�迭��_�����Ÿ�
		GE_Att_Dist_ = 4, GAD1 = 5, GAD2 = 6,	//�����Ÿ� üũ�� ��������..

		//[] ��ų_�����Ÿ�
		SK_Att_Dist = 7,	

		//[SF] ����_�����Ÿ�
		FC_Att_Dist = 8,	

		//[SF] �Ϲ�_�迭��_���ݼӵ�
		GE_Att_Spd_ = 9, GAS1 = 10, GAS2 = 11, 

		//[] ��ų_���ݼӵ�
		SK_Att_Spd = 12,	

		//[SF] ����_���ݼӵ�
		FC_Att_Spd = 13, 

		//[SF] �Ϲ�_ũ��Ƽ��_����
		GE_CrtExt = 14, 

		//[SF, ITEM] ������������
		Part_Tol_ = 15, PT1 = 16, PT2 = 17, PT3 = 18, 

		//[ITEM] ��ų_��������
		SK_LvUp = 19, 

		//[SF, ITEM] �̵��ӵ�_�ٱ�
		Move_Run_Spd = 20, 

		//[ITEM] �ڽ��� �Ⱥ��̱�
		Transparency = 21,	

		//[SF, ITEM] ����, ���ڽ�..�ٺ���
		Detect = 22,	

		//[ITEM] �������غ��� �Ȱɸ�
		Anti_CtDm = 23, 

		//[ITEM] ����÷��̾��� ���Ѻ����� ����
		Know_WeakPoint = 24, 

		//[SF] FP_ȸ���ӵ���
		FP_Rev_Add = 25, 

		//[SF]�þ߹��� ����
		View_Circle = 26,

		__END = 27
	};

	enum { NUM = __END };

};

struct _EFF_STATE
{
	enum{

		//[SF] ���� ��ų ���
		SK_Att_Lck = 0, 

		//[SF] ���� ���� ���
		FC_Att_Lck = 1, 

		//[SF] �����̵溸�� ��ų ���
		SK_CtHp_Lck = 2, 

		//[SF] �����̵溸�� ���� ���
		FC_CtHp_Lck = 3, 

		//[SF] �������غ��� ���� ���
		FC_CtDm_Lck = 4, 

		//[SF] ���ڽ�
		Stealth = 5,	

		//[SF] �̵� ���
		Move_Lck = 6,

		//[SF] �ٱ� ���
		Run_Lck = 7, 		

		//[SF] ����ȸ��
		Abs_Avd = 8,	

		//[SF] ���� ũ��Ƽ��
		Abs_Crt = 9,			

		//[SF] ȸ�����
		Rev_Lck = 10,	

		//[SF] �����й���
		Dst_No_Shd = 11,		

		//[SF] ����� ���¹���
		Dst_No_Def = 12,		

		//[SF] ����� ����ȭ	
		Dst_Make_Stun = 13,  

		//[SF] �ݰݱ�
		Res_Att = 14,	

		__END = 15
	};

	enum { NUM = __END };
};

struct _EFF_HAVE
{
	enum{

		//[RES] ���ڿ����� �·��̵�
		Gamble_Prof = 0,		//����..

		//[RES] �������� �ŷ������� �̵�
		Trade_Prof = 1,	

		//[RES] ����ġȹ���� �̵�
		Exp_Prof = 2,		

		//[RES] ���� �������� ��� ä���� ����������
		Chat_All_Race = 3,	

		//[RES] �ڽ��� �������� ������ �������� �����Ҽ�����
		Equip_Lv_Up = 4,	

		__END = 5
	};

	enum { NUM = __END };
};

#endif