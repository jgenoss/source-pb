#pragma once


struct DesigApplyInfo_Chara
{
	REAL32		charaMoveSpeed = 0.0f;
	REAL32		charaJump = 0.0f;
	REAL32		charaAddSpeed = 0.0f;
	REAL32		charaFallDamageReduce = 0.0f;
};

void ResetDesigApplyInfo_Chara(DesigApplyInfo_Chara* info_chara);

struct DesigApplyInfo_Weapon
{
	DESIGNATION_APPLY_TYPE	applyType = DESIGNATION_APPLY_TYPE_ALL;		// ���� ȣĪ�ý��� ������ ���� ���ϴ� ���׶����� �ǻ���� ���� ����.

	REAL32		weaponFireSpeed = 0.0f;
	REAL32		weaponMoveSpeed = 0.0f;
	REAL32		weaponReloadTime = 0.0f;
	REAL32		weaponAttackRange = 0.0f;
	REAL32		weaponHorzReaction = 0.0f;
	REAL32		weaponVertReaction = 0.0f;
	REAL32		weaponDeclination = 0.0f;
	REAL32		weaponThrowSpeed = 0.0f;
	REAL32		weaponBombTime = 0.0f;
	REAL32		weaponBombRange = 0.0f;
	REAL32		weaponReaction = 0.0f;
	REAL32		weaponDPDRate = 0.0f;
	REAL32		weaponFirstShotRate = 0.0f;
	//	REAL32		weaponDamage;			// ���� �ʾ� ����..(�ڵ������ ��������)
	//	REAL32		weaponScoopeRange;		// ���� �ʾ� ����..(�ڵ������ ��������)
	//	REAL32		weaponCrossHairRestore;	// ���� �ʾ� ����..(�ڵ������ ��������)
};

void ResetDesigApplyInfo_Weapon(DesigApplyInfo_Weapon* info_weapon);

struct DesigApplyInfo
{
	DesigApplyInfo_Chara	desigChara;
	DesigApplyInfo_Weapon	desigWeapon;
};

void ResetDesigApplyInfo(DesigApplyInfo* info);
