#pragma once

// ��ų�ý��� ����ü ����
//sever �ڵ� ���� �մϴ�. 
// ��ų �ý��� ���� ī�װ� ��ų ����Ʈ

#include "../../LeucoShell/Include/Client/Lib/LeucoShell.h"

enum REQUISTE
{
	REQUISTE_UNKNOWN	= -1,
	REQUISTE_CLASS		= 0,
	REQUISTE_TIME
};

enum SKILL_MAINCATEGORY
{
	MAINCATEGORY_UNKNOWN	= -1,
	MAINCATEGORY_ASSAULT	=0,
	MAINCATEGORY_SMG,
	MAINCATEGORY_RIFLE,
	MAINCATEGORY_SHOTGUN,
	MAINCATEGORY_MG,
	MAINCATEGORY_MAX,
};

enum SKILL_WEAPONTYPE
{
	SKILL_WEAPONTYPE_UNKNOWN	= -1,
	SKILL_WEAPONTYPE_ASSAULT	=0,
	SKILL_WEAPONTYPE_SMG,
	SKILL_WEAPONTYPE_RIFLE,
	SKILL_WEAPONTYPE_SHOTGUN,
	SKILL_WEAPONTYPE_MG,
	SKILL_WEAPONTYPE_HANDGUN,
	SKILL_WEAPONTYPE_KNIFE,
	SKILL_WEAPONTYPE_THROWING,
	SKILL_WEAPONTYPE_MAX,
};

enum SKILL_CATEGORYLIST
{
	CATEGORYLIST_UNKNOWN	= -1,
	CATEGORYLIST_COMMON		= 0,
	CATEGORYLIST_MAIN,
	CATEGORYLIST_ASSIST,
	CATEGORYLIST_MAX,
};

// ����� ������ �ʴ� Get/SetWeaponThrowSpeed�� ������� �����մϴ�...(2015.05.22.����)

class SKILLSYSTEM_APPLY_INFO
{
private:
	VAR_LR(INT32, 249)				_iCategoryIdx;				//��ų ī�װ�
	VAR_LR(INT32, 250)				_iSkillIdx;					//��ų �ε���
	VAR_LR(SKILL_TYPE, 251)			_iSkillType;				//��ų Ÿ��
	VAR_LR(INT32, 252)				_iSkillLevel;				//��ų ����
	VAR_LR(SKILL_WEAPONTYPE,253)	_iWeaponType;				//���� Ÿ��
	VAR_LR(bool,254)				_bActivate;					//��ų Ȱ��ȭ 

	// Weapon
	VAR_LR(REAL32, 255) _fWeaponMoveSpeed;				// �̵��ӵ�
	VAR_LR(REAL32, 256) _fWeaponFireSpeed;				// �߻�ӵ�
	VAR_LR(REAL32, 257) _fWeaponReloadTime;				// ������ �ð�
	VAR_LR(REAL32, 258) _fWeaponHorizontalReaction;		// ����ݵ�
	VAR_LR(REAL32, 259) _fWeaponVerticalReaction;		// �����ݵ�
	VAR_LR(REAL32, 260) _fWeaponDeclination;				// ��ǥ����
	VAR_LR(REAL32, 261) _fWeaponAttackRange;				// �����Ÿ�
	VAR_LR(REAL32, 262) _fWeaponDamage;					// ������
	VAR_LR(REAL32, 263) _fWeaponScoopeRange;				// �������þ�
	VAR_LR(REAL32, 264) _fWeaponCrossHairRestore;		// ũ�ν������ð�
	VAR_LR(REAL32, 265) _fWeaponBombTime;				// ���������ð�
	VAR_LR(REAL32, 266) _fWeaponBombRange;				// ���Ĺ���
	VAR_LR(REAL32, 267) _fWeaponBombLength;				// �������մ°Ÿ� (��ô�ӵ��� �����ϰ� ������!)
	VAR_LR(REAL32, 268) _fWeaponChange;					// ���ⱳü�ӵ�
	VAR_LR(REAL32, 269) _fWeaponPenetrate;				// ��������(���)

	// character
	VAR_LR(REAL32, 271) _fCharaWalkSpeed;				// ĳ���� �ȱ�ɷ�
	VAR_LR(REAL32, 272) _fCharaMoveSpeed;				// ĳ���� �̵��ɷ�
	VAR_LR(REAL32, 273) _fCharaAddSpeed;					// ĳ���� ���ӵ�
	VAR_LR(REAL32, 274) _fCharaJump;						// ĳ���� ����������
	VAR_LR(REAL32, 275) _fCharaFallDown;					// ĳ���� ����
	VAR_LR(REAL32, 276) _fCharaDefense;					// ĳ���� ����
	VAR_LR(REAL32, 277) _fCharaHelmetDefense;			// ĳ���� ��� ����

	// �߰�.
	VAR_LR(REAL32, 278)	_fWeaponReaction;			// �ѱ� �ݵ�( ������ ����)
	VAR_LR(REAL32, 279)	_fFirstShot_Rate;			// ��ź ��Ȯ��(CorssHair�� �ּҷ� ���� ���� ũ��)
	VAR_LR(REAL32, 280)	_fDPD_Rate;					// ���Ÿ� ������ (DPD_Rate�� (Damage per Distance))


public:
	SKILLSYSTEM_APPLY_INFO()
	{
		Reset();
		setCategory(-1);
		setIndex(-1);
		setLevel(0);
		seActivate(false);
	}

	void		Reset();

	INT32		getCategory( void) const;
	void		setCategory( INT32 idx);

	INT32		getIndex( void) const;
	void		setIndex( INT32 idx);

	SKILL_TYPE	getSkillType( void) const;
	void		setSkillType( SKILL_TYPE idx);
	void		getSkillTypeStr( char* strtemp) const;

	INT32		getLevel( void) const;
	void		setLevel( INT32 idx);

	bool		getActivate( void) const;
	void		seActivate( bool idx);

	void		setSkillData( INT32 _category, INT32 _idx, INT32 _level);
	REAL32		getSkillValue(void) const;
	REAL32		getSkillValue( WEAPON_CLASS_TYPE weapontype, SKILL_TYPE skilltype) const;
	REAL32		getSkillValue(  SKILL_TYPE skilltype) const;
	REAL32		getDataValue( SKILL_TYPE skilltype) const;

	// ����Ÿ�� 
	SKILL_WEAPONTYPE getSkillWeaponType( void);
	bool		isWeaponType( WEAPON_CLASS_TYPE _weapon) const;
	void		getWeaponTypeStr( char* strtemp) const;
	void		setWeaponType( SKILL_WEAPONTYPE type);

	//���� ���� ���� ��
	REAL32		getWeaponMoveSpeed( void) const;
	void		setWeaponMoveSpeed( REAL32 rVal);

	//���� �����
	REAL32		getWeaponFireSpeed( void) const;
	void		setWeaponFireSpeed( REAL32 rVal);

	//���� �������ӵ�
	REAL32		getWeaponReloadTime( void) const;
	void		setWeaponReloadTime( REAL32 rVal);

	//���� ���� ����ݵ�  �Ⱦ��� ��������
	REAL32		getWeaponHorizontalReaction( void) const;
	void		setWeaponHorizontalReaction( REAL32 rVal);

	//���� ���� �����ݵ� �Ⱦ��� ��������
	REAL32		getWeaponVerticalReaction( void) const;
	void		setWeaponVerticalReaction( REAL32 rVal);

	//���� ���߷�
	REAL32		getWeaponDeclination( void) const;
	void		setWeaponDeclination( REAL32 rVal);

	//���� �����Ÿ�
	REAL32		getWeaponAttackRange( void) const;
	void		setWeaponAttackRange( REAL32 rVal);

	//���� ������
	REAL32		getWeaponDamage( void) const;
	void		setWeaponDamage( REAL32 rVal);

	//���� �������Ÿ�  �Ⱦ��� ��������
	REAL32		getWeaponScoopeRange( void) const;
	void		setWeaponScoopeRange( REAL32 rVal);

	//���� �Ⱦ���
	REAL32		getWeaponCrossHairRestore( void) const;
	void		setWeaponCrossHairRestore( REAL32 rVal);

	//���� ��ô�� ���߽ð�����
	REAL32		getWeaponBombTime( void) const;
	void		setWeaponBombTime( REAL32 rVal);

	//���� ����ź ���߹���
	REAL32		getWeaponBombRange( void) const;
	void		setWeaponBombRange( REAL32 rVal);

	//���� ��ü �ӵ�
	REAL32		getWeaponChange( void) const;
	void		setWeaponChange( REAL32 rVal);

	//���� ��� ����� 
	REAL32		getWeaponPenetrate( void) const;
	void		setWeaponPenetrate( REAL32 rVal);

	//���� ��ô�� ������ �Ÿ�
	REAL32		getWeaponBombLength( void) const;
	void		setWeaponBombLength( REAL32 rVal);

	//���� �ѱ�ݵ�����
	REAL32		getWeaponReaction( void) const;
	void		setWeaponReaction( REAL32 rVal);

	//ĳ���� �̵��ӵ�
	REAL32		getCharaMoveSpeed( void) const;
	void		setCharaMoveSpeed( REAL32 rVal);

	//ĳ���� �ȱ�ӵ�
	REAL32		getCharaWalkSpeed( void) const;
	void		setCharaWalkSpeed( REAL32 rVal);

	//ĳ���� ���ӵ�
	REAL32		getCharaAddSpeed( void) const;
	void		setCharaAddSpeed( REAL32 rVal);

	//ĳ���� ����
	REAL32		getCharaJump( void) const;
	void		setCharaJump( REAL32 rVal);

	//ĳ���� ����
	REAL32		getCharaDefense( void) const;
	void		setCharaDefense( REAL32 rVal);

	//ĳ���� ��� ����
	REAL32		getCharaHelmetDefense( void) const;
	void		setCharaHelmetDefense( REAL32 rVal);

	//���� ĳ���� ���ϵ�����
	REAL32		getCharaFallDown( void) const;
	void		setCharaFallDown( REAL32 rVal);

	//���� ��ź ��Ȯ�� ���
	REAL32		getFirstShotRate( void) const;
	void		setFirstShotRate( REAL32 rVal);

	//���� ���Ÿ� ������ ���
	REAL32		getDPDRate( void) const;
	void		setDPDRate( REAL32 rVal);

};
