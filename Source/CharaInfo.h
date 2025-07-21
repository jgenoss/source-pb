#if !defined ( _CHARA_INFO_H__)
#define _CHARA_INFO_H__

////////////////////////////////////////////////////////////////////////
// �� �ɸ��� ������ ���� ������ �����ϱ� ���� Ŭ�����Դϴ�.
// i3Registry���� �ɸ��� ���¿� ���� ������ ���� �����Դϴ�. - raja -
// �̰��� ���ӿ��� �Ӽ� ���·� ������ ���Դϴ�. ��Ÿ�ӽ��� ����
// �ٸ������� ����Ͻñ� �ٶ��ϴ�.
////////////////////////////////////////////////////////////////////////
#include "EquipInfo.h"
#include "GameCharaDef.h"
#include "GameCharaEquipDef.h"

#define		DINO_ATTACK_DAMAGE_TYPE_COUNT	6

namespace CHARA {
	enum eDAMAGEPART
	{
		eDAMAGEPART_ARM,
		eDAMAGEPART_BODY,
		eDAMAGEPART_FOOT,
		eDAMAGEPART_HAND,
		eDAMAGEPART_HEAD,
		eDAMAGEPART_LEG,
		eDAMAGEPART_NECK,
		eDAMAGEPART_OTHER,
		eDAMAGEPART_VITAL,

		eDAMAGEPART_COUNT
	};

	inline
	eDAMAGEPART		GetDamagePartByHit( CHARA_HIT_PART part)
	{
		switch( part)
		{
		case CHARA_HIT_JAW:
		case CHARA_HIT_HEAD:		return eDAMAGEPART_HEAD;	// �Ӹ�
		case CHARA_HIT_NECK	:
		case CHARA_HIT_LCLAVICLE :
		case CHARA_HIT_RCLAVICLE :
		case CHARA_HIT_BODY1 :
		case CHARA_HIT_BODY2 :
		case CHARA_HIT_BODY3 :
		case CHARA_HIT_ROOT	:		
		case CHARA_HIT_DEATH_TRAMPLE:
		case CHARA_HIT_DEATH_HOWL:	return eDAMAGEPART_BODY;	// ����
		case CHARA_HIT_RUARM:
		case CHARA_HIT_RDARM:
		case CHARA_HIT_LUARM:
		case CHARA_HIT_LDARM:		return eDAMAGEPART_ARM;		// ��
		case CHARA_HIT_LHAND:
		case CHARA_HIT_RHAND:		return eDAMAGEPART_HAND;	// ��
		case CHARA_HIT_LULEG:
		case CHARA_HIT_LDLEG:
		case CHARA_HIT_RULEG:
		case CHARA_HIT_RDLEG:		return eDAMAGEPART_LEG;		// �ٸ�
		case CHARA_HIT_LFOOT:
		case CHARA_HIT_RFOOT:		return eDAMAGEPART_FOOT;	// ��
		case CHARA_HIT_NECK_LOWER:	return eDAMAGEPART_NECK;	// ��	// �����
		case CHARA_HIT_VITAL_EYE:	return eDAMAGEPART_VITAL;	// �޼�
		}

		return eDAMAGEPART_OTHER;
	}

	enum eSEX_TYPE {
		eSEX_TYPE_MALE = 0,
		eSEX_TYPE_FEMALE,
		eSEX_TYPE_DINO,
		eSEX_TYPE_SKINY_MALE,	// ��ȹ�� ����, �ø����� ���� ����

		eSEX_TYPE_MAX
	};
};

////////////////////////////////////////////////////////////////////////////////////
//	���� ĳ���� ����
////////////////////////////////////////////////////////////////////////////////////
class CCharaGlobal : public i3NamedElement
{
	I3_CLASS_DEFINE( CCharaGlobal, i3NamedElement);

private:
	REAL32		m_rInvincibleTime = 0.0f;	//	���� �ð�
	REAL32		m_rInvincibleTime_4vs4 = 0.0f;//4vs4��忡���� �����ð�
	REAL32		m_rAimingAngleMax = 0.0f;	//	�ִ� ���� ����
	REAL32		m_rAimingAngleMin = 0.0f;	//	�ּ� ���� ����
	REAL32		m_rSpeedReductionTime = 0.0f;		//�ǰݵȰ�� �̵��ð� ������ ���ӽð�
	REAL32		m_rSpeedReductionMaxRate = 0.0f;	//�ǰݵȰ�� ������������ �ִ� �̵��� ���� ����

	REAL32		m_rPartDamageRateInSuddenDeath[CHARA::eDAMAGEPART_COUNT] = { 0.0f, };	//	��쿡 ���� ������ ���� (%)
	REAL32		m_rMinDamage = 0.0f;

protected:
	UINT8 		m_u8EncryptKey = 0;

public:
	REAL32		GetInvincibleTime() const;
	void		SetInvincibleTime(REAL32 val);
	REAL32		GetInvincibleTime_4vs4() const;
	void		SetInvincibleTime_4vs4(REAL32 val);
	REAL32		GetAimingAngleMax() const;
	void		SetAimingAngleMax(REAL32 val);
	REAL32		GetAimingAngleMin() const;
	void		SetAimingAngleMin(REAL32 val);
	REAL32		GetSpeedReductionTime() const;
	void		SetSpeedReductionTime(REAL32 val);
	REAL32		GetSpeedReductionMaxRate() const;
	void		SetSpeedReductionMaxRate(REAL32 val);
	REAL32		GetInvincibleTime( STAGE_MODE type) const;

public:
	void	ReadInfo( i3RegKey * pKey);
	
	void	OnEncryptInfo( UINT8 u8Key);

	void		ReadDamageByModeInfo( i3RegKey * pKey);
	REAL32		GetDamageRateByMode( CHARA::eDAMAGEPART eParam) const;
	void		CalcDamageByPart(REAL32& rDamage);

	// ���� ������ ����.
private:
	INT32		m_nFallMinDamage = 0;			//	������ ��� �޴� �ּ� ������ ��
	REAL32		m_rFallDamageMinHeight = 0.0f;		//	������ ��� ������ �޴� �ּ� ����
	REAL32		m_rFallDamageSlope = 0.0f;			//	����

public:
	INT32	GetFallMinDamage() const;
	void	SetFallMinDamage(INT32 val);
	REAL32	GetFallDamageMinHeight() const;
	void	SetFallDamageMinHeight(REAL32 val);
	REAL32	GetFallDamageSlope() const;
	void	SetFallDamageSlope(REAL32 val);
};

class CCharaGlobal2 : public i3NamedElement
{
	I3_CLASS_DEFINE( CCharaGlobal2, i3NamedElement);

private:
	REAL32		m_rInvincibleTime = 0.0f;	//	���� �ð�
	REAL32		m_rAimingAngleMax = 0.0f;	//	�ִ� ���� ����
	REAL32		m_rInvincibleTime_4vs4 = 0.0f;//4vs4��忡���� �����ð�
	REAL32		m_rAimingAngleMin = 0.0f;	//	�ּ� ���� ����
	REAL32		m_rSpeedReductionMaxRate = 0.0f;	//�ǰݵȰ�� ������������ �ִ� �̵��� ���� ����
	REAL32		m_rSpeedReductionTime = 0.0f;		//�ǰݵȰ�� �̵��ð� ������ ���ӽð�
	
protected:
	UINT8		m_u8EncryptKey = 0;

public:
	REAL32		GetInvincibleTime() const;
	void		SetInvincibleTime(REAL32 val);
	REAL32		GetAimingAngleMax() const;
	void		SetAimingAngleMax(REAL32 val);
	REAL32		GetInvincibleTime_4vs4() const;
	void		SetInvincibleTime_4vs4(REAL32 val);
	REAL32		GetAimingAngleMin() const;
	void		SetAimingAngleMin(REAL32 val);

	REAL32		GetSpeedReductionMaxRate() const;
	void		SetSpeedReductionMaxRate(REAL32 val);
	REAL32		GetSpeedReductionTime() const;
	void		SetSpeedReductionTime(REAL32 val);

public:
	void		ReadInfo( i3RegKey * pKey);
	void		OnEncryptInfo( UINT8 u8Key);

	// ���� ������ ����.
private:
	INT32		m_nFallMinDamage = 0;			//	������ ��� �޴� �ּ� ������ ��
	REAL32		m_rFallDamageSlope = 0.0f;			//	����
	REAL32		m_rFallDamageMinHeight = 0.0f;		//	������ ��� ������ �޴� �ּ� ����

public:
	INT32 GetFallMinDamage() const;
	void SetFallMinDamage(INT32 val);
	REAL32 GetFallDamageMinHeight() const;
	void SetFallDamageMinHeight(REAL32 val);
	REAL32 GetFallDamageSlope() const;
	void SetFallDamageSlope(REAL32 val);
};


class CSpeedReduction
{
public:
	REAL32 GetRate() const;
	void SetRate(REAL32 val);
	REAL32 GetTime() const;
	void SetTime(REAL32 val);
private:
	REAL32 m_rRate = 0.0f;
	REAL32 m_rTime = 0.0f;
};

////////////////////////////////////////////////////////////////////////////////////
//	���� ĳ���� ����
////////////////////////////////////////////////////////////////////////////////////
class CCharaInfo :	public CEquipInfo
{
	I3_CLASS_DEFINE( CCharaInfo, CEquipInfo);

private:
	REAL32	m_arMoveSpeed[MMT_COUNT][MST_COUNT][MDT_COUNT] = { 0.0f, };
	REAL32	m_rJumpDelay = 0.0f;			// ���� ������ ������ �ð�
	REAL32  m_rAccelerationTime = 0.3f;	// �ְ�ӵ����� �����ϴµ� �ɸ��� �ð�
	INT32	m_iResParts[EQUIP::ePART_COUNT] = { 0 };

	T_ItemID	m_RepresentID = 0;			// Resource�� �����ϴ� ���� ID

	T_ItemID	m_TeamBlue_ItemID = 0;		// Blue���� ItemID
	T_ItemID	m_TeamRed_ItemID = 0;		// Red���� ItemID

public:
	REAL32	GetJumpDelay() const;
	void	SetJumpDelay(REAL32 val);
	REAL32	GetAccelerationTime() const;
	void	SetAccelerationTime(REAL32 val);
	REAL32	GetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, EMoveModeType eMMT = MMT_NORMAL ) const;
	void	SetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, REAL32 Value, EMoveModeType eMMT = MMT_NORMAL);
	T_ItemID	GetPartsItemID( EQUIP::ePART parts) const;

	T_ItemID	GetRepresentID( void) const;

	T_ItemID	GetTeamBlue_ItemID( void) const		{ return m_TeamBlue_ItemID; }
	T_ItemID	GetTeamRed_ItemID( void) const		{ return m_TeamRed_ItemID; }

private:
	INT32	m_nHP = 0;					// ĳ���� �⺻ ü��
	//Jump
	REAL32	m_rNewTempJumpHeight = 0.0f;	
	REAL32  m_rNewTempClunchJumpHeight = 0.0f;	
	REAL32	m_rJumpHeight = 0.0f;			// ���� ������ ����
	REAL32	m_rJumpTime = 0.0f;			// ������ ü���ð�( ������ ��ġ�� ���� ���̰� �Ǵµ� �ɸ��� �ð�?)
	REAL32	m_rJumpPenaltyTime = 0.0f;		// �������� �̽ð��� ������� ������쿣 ������ ���Ƽ�� ����
	REAL32	m_rJumpPenaltyRate = 0.0f;		// ���Ƽ�� ����ɶ� �ܰ躰�� ������ ������ ����ġ
	//Weapon
	REAL32	m_rWeaponSwap = 0.0f;			// ���� ��ü �ð�( ���⿡ ���� Ʋ������ ������?)
	REAL32	m_rScopeWaveVelocity = 0.0f;	// ��鸲 �ӵ�
	REAL32	m_rScopeWaveRange = 0.0f;		// ��鸲 ���� (%)
	REAL32	m_rReloadSpeed = 0.0f;			// �����ӵ� (%)
	REAL32  m_rShoulderSpeed = 0.0f;		// ���� �ӵ�

	//����ġ ����
	REAL32	m_rAccelRateBackVsFront = 0.0f;// �������ӵ������� �������ӵ��� ����

	//Height
	VEC4D	m_vCamIKHeight;			//�ڼ������� ī�޶��� �㸮����x:stand y:sitmove z:sit
	//FPS Camera�� Position
	VEC3D	m_vFPSCamPos;
	//	Damage Rate by body parts	// ĳ���� ������ ������ ����
	REAL32		m_rPartDamageRate[CHARA::eDAMAGEPART_COUNT] = { 0.0f };

	INT32		m_nCharaSexType = 0;

	//////////////////////////////////////////////////////////////////////////
	//	Add NPC info
	REAL32		m_rNPCAttackDelay = 0.0f;
	REAL32		m_rNPCInvincibleTime = 0.0f;
	REAL32		m_rNPCRespawnTime = 0.0f;
	REAL32		m_rNPCSearchRandomValue = 0.0f;

	// Clunch���� ����
	REAL32		m_rClunchJumpHeight = 0.0f;
	REAL32		m_rClunchJumpCheckTime = 0.0f;

public:
	INT32	GetHP() const;
	CHARA::eSEX_TYPE	GetCharaSexType() const;
	CHARA::eSEX_TYPE	GetCharaSexTypePHYSIX() const;
	CHARA::eSEX_TYPE	GetCharaSexTypeDISPLAY() const;
	void	SetHP(INT32 val);
	REAL32	GetJumpHeight() const;
	void	SetJumpHeight(REAL32 val);
	
	REAL32	GetNewTempJumpHeight() const;
	void	SetNewTempJumpHeight(REAL32 val);
	REAL32	GetNewTempClunchJumpHeight() const;
	void	SetNewTempClunchJumpHeight(REAL32 val);

	REAL32	GetJumpTime() const;
	void	SetJumpTime(REAL32 val);

	REAL32	GetJumpPenaltyTime() const;
	void	SetJumpPenaltyTime(REAL32 val);
	REAL32	GetJumpPenaltyRate() const;
	void	SetJumpPenaltyRate(REAL32 val);
	REAL32	GetWeaponSwap() const;
	void	SetWeaponSwap(REAL32 val);
	REAL32	GetScopeWaveVelocity() const;
	void	SetScopeWaveVelocity(REAL32 val);
	REAL32	GetScopeWaveRange() const;
	void	SetScopeWaveRange(REAL32 val);
	REAL32	GetReloadSpeed() const;
	void	SetReloadSpeed(REAL32 val);
	REAL32	GetShoulderSpeed() const;
	void	SetShoulderSpeed(REAL32 val);
	REAL32	GetAccelRateBackVsFront() const;
	void	SetAccelRateBackVsFront(REAL32 val);
	void	GetCamIKHeight( VEC4D * pOut) const;
	void	SetCamIKHeight(const VEC4D & val);
	void	GetFPSCamPos( VEC3D * pOut) const;
	void	SetFPSCamPos(const VEC3D & val);

	REAL32	GetPartDamageRate( CHARA::eDAMAGEPART damagePart) const;
	void	SetPartDamageRate( CHARA::eDAMAGEPART damagePart, REAL32 val);

	REAL32	GetNPCAttackDelay() const;
	void	SetNPCAttackDelay(REAL32 val);

	REAL32 GetNPCInvincibleTime() const;
	void SetNPCInvincibleTime(REAL32 val);
	REAL32 GetNPCRespawnTime() const;
	void SetNPCRespawnTime(REAL32 val);
	REAL32 GetNPCSearchRandomValue() const;
	void SetNPCSearchRandomValue(REAL32 val);
	REAL32 GetClunchJumpHeight() const;
	void SetClunchJumpHeight(REAL32 val);
	REAL32 GetClunchJumpCheckTime() const;
	void SetClunchJumpCheckTime(REAL32 val);


	//���
public:
	virtual void	ReadInfo( i3RegKey * pKey) override;
	virtual void	OnEncryptInfo( UINT8 u8Key) override;

	void	ReadDamageRateInfo( i3RegKey * pKey);

	// ������ ���� �̵� �ӵ� ���� ���� �� �ð�
private:
	CSpeedReduction  m_rSpeedReduction[CHPS_COUNT];
public:
	const CSpeedReduction* GetSpeedReduction(CHARA_HIT_PART_SIMPLE ePart) const;

#if defined( AI_MOVE_TO_WEAPON)
	////////////////////////////////
	// AI State�� ����� �����Կ� ���� �ʿ����� �Լ� �� �����.
	// 
protected:
	char		m_szAnimName[64] = { 0 };

public:
	const char *	getAnimationName(void)			{ return (const char *) m_szAnimName; }
#endif

	virtual UINT32		GetDinoWeaponID(INT32 idx)	const				{ return 0; }
	virtual void		SetDinoWeaponID(INT32 idx, T_ItemID itemID)		{}

	// ���� ����
	virtual REAL32		GetDinoHP_Ratio(INT32 idx) const				{ return 0.f; }
	virtual void		SetDinoHP_Ratio(INT32 idx, REAL32 val)			{}

	virtual REAL32		GetDinoCCRadius() const							{ return 0.f; }
	virtual REAL32		GetDinoCCTall() const							{ return 0.f; }
	virtual REAL32		GetDinoCCStepOffset() const						{ return 0.f; }

	virtual void 		SetDinoCCTall(REAL32 val)						{}
	virtual void 		SetDinoCCStepOffset(REAL32 val)					{}
	virtual void 		SetDinoCCRadius(REAL32 val)						{}

	virtual void 		SetCharaSexType(INT32 val)						{ m_nCharaSexType = val; }

	virtual REAL32		getFootStepTrembleAmp(void) const				{ return 0.f; }
	virtual void		SetFootStepTrembleAmp(REAL32 val)				{}

	virtual INT32		getShapeIndexCCSkill( UINT8 idx) const			{ return 0; }
	virtual void		getCCSkillDescription( i3::rc_wstring& out, UINT8 idx) const	{}

	// ��ٸ� �̵���.
private:
	REAL32 m_rLadderWalkBackSpeed = 0.0f;
	REAL32 m_rLadderWalkFrontSpeed = 0.0f;
	REAL32 m_rLadderWalkSideSpeed = 0.0f;
	REAL32 m_rLadderWalkDiagonalSpeed = 0.0f;
	REAL32 m_rLadderRunBackSpeed = 0.0f;
	REAL32 m_rLadderRunFrontSpeed = 0.0f;
	REAL32 m_rLadderRunSideSpeed = 0.0f;
	REAL32 m_rLadderRunDiagonalSpeed = 0.0f;

public:
	REAL32 GetLadderWalkBackSpeed() const;
	REAL32 GetLadderWalkFrontSpeed() const;
	REAL32 GetLadderWalkSideSpeed() const;
	REAL32 GetLadderWalkDiagonalSpeed() const;
	REAL32 GetLadderRunBackSpeed() const;
	REAL32 GetLadderRunFrontSpeed() const;
	REAL32 GetLadderRunSideSpeed() const;
	REAL32 GetLadderRunDiagonalSpeed() const;
};


// ġ�� ������ ���� CCharaInfo�� ���� ���� �ϳ� �� �����ؼ� 
// ���� �ð����� �˻��Ѵ�.

class CCharaInfo2 :	public i3NamedElement
{
	I3_CLASS_DEFINE( CCharaInfo2, i3NamedElement);

private:
	REAL32  m_rAccelerationTime = 0.3f;	// �ְ�ӵ����� �����ϴµ� �ɸ��� �ð�
	REAL32	m_rJumpDelay = 0.0f;			// ���� ������ ������ �ð�
	REAL32	m_arMoveSpeed[MMT_COUNT][MST_COUNT][MDT_COUNT] = { 0.0f, };
	INT32	m_iResParts[EQUIP::ePART_COUNT] = { 0 };

public:
	REAL32	GetAccelerationTime() const;
	void	SetAccelerationTime(REAL32 val);

	REAL32	GetJumpDelay() const;
	void	SetJumpDelay(REAL32 val);

	REAL32	GetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, EMoveModeType eMMT = MMT_NORMAL ) const;
	void	SetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, REAL32 Value, EMoveModeType eMMT = MMT_NORMAL );
	T_ItemID	GetPartsItemID( EQUIP::ePART parts);

private:
	//Jump
	REAL32	m_rJumpHeight = 0.0f;			// ���� ������ ����
	INT32	m_nHP = 0;					// ĳ���� �⺻ ü��
	REAL32	m_rJumpTime = 0.0f;			// ������ ü���ð�( ������ ��ġ�� ���� ���̰� �Ǵµ� �ɸ��� �ð�?)
	REAL32	m_rJumpPenaltyTime = 0.0f;		// �������� �̽ð��� ������� ������쿣 ������ ���Ƽ�� ����
	INT32	m_nCharaSexType = 0;
public:
	REAL32	GetJumpHeight() const;
	void	SetJumpHeight(REAL32 val);
	INT32	GetHP() const;
	void	SetHP(INT32 val);
	REAL32	GetJumpTime() const;
	void	SetJumpTime(REAL32 val);
	REAL32	GetJumpPenaltyTime() const;
	void	SetJumpPenaltyTime(REAL32 val);

	// ������ ���� �̵� �ӵ� ���� ���� �� �ð�
private:
	CSpeedReduction  m_rSpeedReduction[CHPS_COUNT];
public:
	const CSpeedReduction* GetSpeedReduction(CHARA_HIT_PART_SIMPLE ePart) const;

private:
	REAL32	m_rJumpPenaltyRate = 0.0f;		// ���Ƽ�� ����ɶ� �ܰ躰�� ������ ������ ����ġ
	//Weapon
	REAL32	m_rWeaponSwap = 0.0f;			// ���� ��ü �ð�( ���⿡ ���� Ʋ������ ������?)

	REAL32	m_rScopeWaveRange = 0.0f;		// ��鸲 ���� (%)

	REAL32  m_rShoulderSpeed = 0.0f;		// ���� �ӵ�
	//Height
	VEC4D	m_vCamIKHeight;			//�ڼ������� ī�޶��� �㸮����x:stand y:sitmove z:sit
	REAL32	m_rReloadSpeed = 0.0f;			// �����ӵ� (%)
	//����ġ ����
	REAL32	m_rAccelRateBackVsFront = 0.0f;// �������ӵ������� �������ӵ��� ����
	REAL32	m_rScopeWaveVelocity = 0.0f;	// ��鸲 �ӵ�
	//FPS Camera�� Position
	VEC3D	m_vFPSCamPos;
	//	Damage Rate
	REAL32	m_rPartDamageRate[CHARA::eDAMAGEPART_COUNT] = { 0.0f };

	//////////////////////////////////////////////////////////////////////////
	//	Add NPC info
	REAL32		m_rNPCAttackDelay = 0.0f;

	REAL32		m_rNPCRespawnTime = 0.0f;
	REAL32		m_rNPCInvincibleTime = 0.0f;
	REAL32		m_rClunchJumpHeight = 0.0f;
	REAL32		m_rNPCSearchRandomValue = 0.0f;
	// Clunch���� ����
	
	REAL32		m_rClunchJumpCheckTime = 0.0f;

	T_ItemID	m_ITEMID = 0;

protected:
	UINT8 		m_u8EncryptKey = 0;

public:


	REAL32	GetJumpPenaltyRate() const;
	void	SetJumpPenaltyRate(REAL32 val);

	REAL32	GetWeaponSwap() const;
	void	SetWeaponSwap(REAL32 val);

	REAL32	GetScopeWaveRange() const;
	void	SetScopeWaveRange(REAL32 val);

	REAL32	GetShoulderSpeed() const;
	void	SetShoulderSpeed(REAL32 val);

	void	GetCamIKHeight( VEC4D * pOut) const;
	void	SetCamIKHeight(const VEC4D & val);

	REAL32	GetReloadSpeed() const;
	void	SetReloadSpeed(REAL32 val);

	REAL32	GetAccelRateBackVsFront() const;
	void	SetAccelRateBackVsFront(REAL32 val);

	REAL32	GetScopeWaveVelocity() const;
	void	SetScopeWaveVelocity(REAL32 val);

	void	GetFPSCamPos( VEC3D * pOut) const;
	void	SetFPSCamPos(const VEC3D & val);

	REAL32	GetPartDamageRate( CHARA::eDAMAGEPART damagePart) const;
	void	SetPartDamageRate( CHARA::eDAMAGEPART damagePart, REAL32 val);

	REAL32	GetNPCAttackDelay() const;
	void	SetNPCAttackDelay(REAL32 val);

	REAL32	GetNPCRespawnTime() const;
	void	SetNPCRespawnTime(REAL32 val);

	REAL32	GetNPCInvincibleTime() const;
	void	SetNPCInvincibleTime(REAL32 val);

	REAL32	GetClunchJumpHeight() const;
	void	SetClunchJumpHeight(REAL32 val);

	REAL32	GetNPCSearchRandomValue() const;
	void	SetNPCSearchRandomValue(REAL32 val);

	REAL32	GetClunchJumpCheckTime() const;
	void	SetClunchJumpCheckTime(REAL32 val);


	void		SetITEMID( T_ItemID val);
	T_ItemID	GetITEMID( void);

	//���
public:
	void	ReadInfo( i3RegKey * pKey);

	void	ReadDamageRateInfo( i3RegKey * pKey);
	void	OnEncryptInfo( UINT8 u8Key);
	
	// ��ٸ� �̵���.
private:
	REAL32 m_rLadderWalkBackSpeed = 0.0f;
	REAL32 m_rLadderWalkFrontSpeed = 0.0f;
	REAL32 m_rLadderWalkSideSpeed = 0.0f;
	REAL32 m_rLadderWalkDiagonalSpeed = 0.0f;
	REAL32 m_rLadderRunBackSpeed = 0.0f;
	REAL32 m_rLadderRunFrontSpeed = 0.0f;
	REAL32 m_rLadderRunSideSpeed = 0.0f;
	REAL32 m_rLadderRunDiagonalSpeed = 0.0f;

public:
	REAL32 GetLadderWalkBackSpeed() const;
	REAL32 GetLadderWalkFrontSpeed() const;
	REAL32 GetLadderWalkSideSpeed() const;
	REAL32 GetLadderWalkDiagonalSpeed() const;
	REAL32 GetLadderRunBackSpeed() const;
	REAL32 GetLadderRunFrontSpeed() const;
	REAL32 GetLadderRunSideSpeed() const;
	REAL32 GetLadderRunDiagonalSpeed() const;
};


class CUITemplateInfo_Chara : public i3NamedElement
{
	I3_CLASS_DEFINE(CUITemplateInfo_Chara, i3NamedElement);

public:
	void ReadInfo(i3RegKey* rk);
	INT32 GetType() const;
	INT32 GetLoopLoad() const;
	
	const i3::string& GetName() const;

private:
	INT32 m_type = -1;
	INT32 m_LoopLoad = 0;
	i3::string m_name;		
};

#endif