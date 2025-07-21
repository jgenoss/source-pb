/**
 * \file	GameCharaBase.h
 *
 * Declares the game chara base class.
 */


#if !defined( CGAME_CHARA_BASE_H)
#define CGAME_CHARA_BASE_H

#include "GameCharaDef.h"
#include "GameCharaTextureDef.h"
#include "GameCharaWeaponControlDef.h"
#include "CharaInfo.h"
#include "Character/GameCharaAnimControlDefine.h"

class CGameCharaActionContext;
class CGameCharaAnimContext;
class CGameCharaBoneContext;
class CGameCharaCameraContext;
class CGameCharaCollisionContext;
class CGameCharaEquipContext;
class CGameCharaInputContext;
class CGameCharaMaterialContext;
class CGameCharaMoveContext;
class CGameCharaSoundContext;
class CGameCharaWeaponContext;
class CGameCharaUpdateContext;
class WeaponBase;
class CGameObjectWithWeapon;
class CGameCharaInputControl;
class CAbstractWeaponContext;
class CGameCharaHUDContext;
class CGameCharaMultiWeaponContext;

#define MASTER_RELOAD_SPEED_RATIO 1.25f

typedef UINT32	CHARA_FLAG;
#define CHARA_FLAG_RESETABLE			0x0000FFFF		// �������� ���µǴ� �÷��װ�
#define CHARA_FLAG_DAMAGED_BY_HOWL		0x00000001				// Howl������ Damage�� �Ծ� �������� ����������	(resetable)
#define CHARA_FLAG_DEATHBLOW			0x00000002				// DeathBlow ����	(resetable)
#define CHARA_FLAG_HELM_PROTECT			0x00000004				// ��� ���
#define CHARA_FLAG_ENABLE_EFFECT		0x00000008				// ����Ʈ �߻� ����	(resetable)
#define CHARA_FLAG_ON_OBJECT			0x00000020				// Object ž�� ���� (resetable)
#define CHARA_FLAG_LEFT_HAND			0x80000000				// �޼�����
#define CHARA_FLAG_IS_FEMALE			0x40000000				// ���� ĳ����


#pragma pack(push,4)

struct PHYSIX_SHAPESET_INFO
{
	INT32				_iBoneIndex = -1;
	INT32				_iParentShapeIdx = CHARA_HIT_UNKNOWN;
	REAL32				_rDistanceToParentBone = 0.0f;
	i3PhysixShapeSet *	_pShapeSet = nullptr;
};

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Class : CGameCharaBase
//	Desc: �⺻���� AI ����, Chara State�� Animation ó���� �մϴ�.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CGameCharaBase : public i3Chara
{
	I3_CLASS_DEFINE( CGameCharaBase, i3Chara);

	friend class CGameCharaAnimContext;

	//--------------------------------------------------------------------------------------------------------//
	//													Variables
	//--------------------------------------------------------------------------------------------------------//
protected:
	bool				m_bReturnedInstance = false;
	CHARA_VIEW_TYPE		m_ViewType = CHARA_VIEW_3PV;			//	1 or 3��Ī �ִϸ��̼�
	CHARA_VIEW_TYPE		m_oldViewType = CHARA_VIEW_3PV;

	UINT32				m_uStateMaskKey = 0;
	UINT32				m_EnableStateMask = 0;	// ĳ���Ͱ� �� �� ���� ���µ��� �����մϴ�.
	CHARA_FLAG			m_CharaFlag = 0;

	// Action Variables
	VAR_LR(REAL32, 7)				m_fJumpDelayElapsedTime = 0.0f;		// ���� ������ Ÿ��
	VAR_LR(REAL32, 8)				m_rRespawnElapseTime = 0.0f;			// ������ Ÿ�� ����
	VAR_LR(REAL32, 9)				m_rInvincibleElapseTime = 0.0f;		// ���� Ÿ��
	VAR_LR(REAL32, 10)				m_rContactTime = 0.0f;					// Physix�� ���� ���� Ÿ�̸��Դϴ�. contact�� �߻��ϸ� ����� �մϴ�.
	REAL32				m_rAttackIdleTime = 0.0f;				// Attack �� Idle���� �ð� - ����
	REAL32				m_rAttackIdleTimeLeft = 0.0f;			// Attack �� Idle���� �ð� - ��󹫱⿡ �ʿ�
	REAL32				m_rPendingActionTime = 0.0f;			// ���� Ű�ݺ��������� �ִϸ��̼� Ƣ�����������
	
	
	VAR_LR(GLOBAL_CHARA_INFO*, 11)		m_pCharaInfo = nullptr;		//	������ ����Ǵ� ���� ĳ���� ��
	VAR_LR(CCharaInfo*, 12)		 	m_pDBInfo = nullptr;			//	������� �ʴ� ĳ���� ������ ���̽�
	CGameCharaMultiWeaponContext *	m_pMultiWeaponContext = nullptr;		// MultiWeapon Context
	VAR_LR(CGameCharaUpdateContext*, 13)			m_pUpdateContext = nullptr;		// Update Context
	VAR_LR(CGameCharaInputContext*,  14)			m_pInputContext = nullptr;		// Input Context
	VAR_LR(CGameCharaActionContext *, 15)			m_pActionContext = nullptr;		// Action Context
	VAR_LR(CGameCharaAnimContext *, 16)			m_pAnimContext = nullptr;			// Anim Context
	VAR_LR(CGameCharaWeaponContext *, 17)			m_pWeaponContext = nullptr;		// Weapon Context
	VAR_LR(CGameCharaMoveContext *, 18)			m_pMoveContext = nullptr;			// Move Context
	VAR_LR(CGameCharaMaterialContext *, 19)		m_pMaterialContext = nullptr;		// Volume Light context
	VAR_LR(CGameCharaEquipContext *, 20)			m_pEquipContext = nullptr;		// Equip Context
	VAR_LR(CGameCharaCameraContext *, 21)			m_pCameraContext = nullptr;		// Camera Context
	VAR_LR(CGameCharaBoneContext *, 22)			m_pBoneContext = nullptr;			// Bone Context
	VAR_LR(CGameCharaSoundContext *, 23)			m_pSoundContext = nullptr;		// Sound Context
	VAR_LR(CGameCharaCollisionContext *, 24)		m_pCollisionContext = nullptr;	// Collision Context
	VAR_LR(CGameCharaHUDContext *, 25)				m_pHUDContext = nullptr;
	
	REAL32				m_rDamageHowlEffectiveTime = 0.0f;		// Howl������ Damage������.
	REAL32				m_rElapsedDeathBlow = 0.0f;			// DeathBlow ����

	// ���� ������
	PHYSIX_SHAPESET_INFO	m_PhysixShapeSetInfo[ CHARA_HIT_MAXCOUNT];
 
	// Ÿ�� �ִ� ������Ʈ
	i3Object *			m_pAttachedObject = nullptr;

	// ������⿡ ���� ��Ÿ��(�̽ð������� �߻縦 ���մϴ�.)
	REAL32				m_rDropWeaponTimer = 0.0f;

	REAL32				m_rMultiWeaponTimer = 0.0f;

	// ���� ������
	REAL32				m_rJumpParalysis = 0.0f;
	REAL32				m_rWalkParalysis = 0.0f;
	REAL32				m_rRunParalysis = 0.0f;
	REAL32				m_rCrouchParalysis = 0.0f;

	bool				m_bLoadMagChg = true;

private:
	VAR_LR(UINT32, 234) m_CharaStateVal[CHARA_STATE_MAX] = { 0 };
	VAR_LR(UINT32, 235) m_CharaStateKey[CHARA_STATE_MAX] = { 0 };


	void _ResetCharaStateMask(void)
	{
		for ( int i = 0 ; i < CHARA_STATE_MAX ; i ++ )
		{
			m_CharaStateKey[i] = ( i3Math::Rand() % (UINT_MAX - 1) ) + 1;
			m_CharaStateVal[i] = 0;
		}
	}

	UINT32	GetCharaStateKey(INT32 Idx) const
	{
		return m_CharaStateKey[Idx];
	}

public:

	/** \brief ĳ���� ���� ����ũ�� flag�� �߰��մϴ�. */
	void				addCharaStateMask( UINT32 StateIdx)
	{
		m_CharaStateVal[StateIdx] = GetCharaStateKey(StateIdx);
	}

	/** \brief ĳ���� ���� ����ũ���� flag�� ����ϴ�. */
	void				removeCharaStateMask( UINT32 StateIdx)
	{
		m_CharaStateVal[StateIdx] = 0;
	}

	/** \brief ĳ���� ���� ����ũ�� Ȯ���մϴ�. */
	/* �ε��� ó���� �ٲ�����Ƿ� ���ÿ� �ΰ��� �̻��� ����ũ ó�� �Ұ����մϴ� */
	bool				isCharaStateMask( UINT32 StateIdx) const
	{
		I3ASSERT_RETURN(StateIdx < CHARA_STATE_MAX, false);
			
		return m_CharaStateVal[StateIdx] == GetCharaStateKey(StateIdx);
	}
	//------------------------------------------------------------------------------------------------------------//
	//													Functions
	//------------------------------------------------------------------------------------------------------------//
protected:
	// Physix
	/** \brief Actor���� Pair�� �����մϴ�. */
	void				_PHYSIX_SetActorPairs( i3PhysixShapeSet * pShapeSet, UINT32 nFlag );

	/** \brief �⺻ �������� �����մϴ�.
		\note damp, spring, limit�� */
	void				_PHYSIX_SetDefaultVariable( void );

	/** \brief ���� ������ ���� Linear velocity�� �����մϴ�. */
	void				_PHYSIX_SetLinearVelocity( CHARA_DAMAGEINFO * pInfo );

	/** \brief Actor pair�� �ʱ�ȭ�մϴ�. */
	void				_PHYSIX_InitActorPairs( void);

	/** \brief �ǰݵ� �Ÿ��� ���մϴ�. */
	REAL32				_CalcDamageLength( const CHARA_DAMAGEINFO * pDamageInfo);

	/** \brief �Ÿ��� ���� DamageRate�� ���մϴ�. */
	REAL32				_CalcDistanceRate( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength, CGameCharaBase* pKiller);

	/** \brief helmet protection�� �˻��մϴ�. */
	void				_CheckHelmetProtection( CHARA_DAMAGEINFO * pDamageInfo, bool &bHelmetDrop, bool &bDamaged);
	
	/** \brief Howl�� ���� Damage�� Update�մϴ�. */
	void				_UpdateDamagedByHowl( REAL32 rDeltaSeconds);

	/** \brief DeathBlow�� ���� Update�� �մϴ�. */
	void				_UpdateDeathBlowState( REAL32 rDeltaSeconds);

	/** \brief �ǰ� ���带 Play�մϴ�. */
	void				_PlaySoundForCharaHit( CHARA_DAMAGEINFO * pDamageInfo, VEC3D * pPos, bool bListener);

	/** \brief AIContext�� Update�մϴ�. */
	void				_UpdateOnlyAI( REAL32 tm);

	/** \brief AIContext �ܿ� Update�մϴ�. */
	void				_UpdateAllExceptAI( REAL32 tm);

	/**\brief CharaResType�� ���� Control�� �����Ѵ�. */
	void				_SetControlsByCharaType( void);

	const char *		_GetWeaponBodyUpperName( CHARA_ACTION_BODYUPPER bodyUpper, const CWeaponInfo * pWeaponInfo);


	void				_PrepareCommonAI( void);

	/** \brief ��ü Bone Count */
	virtual INT32		_GetUpperBoneCount( void) const		{ return CHARA_UPPER_BONE_COUNT; }

	/** \brief ��ü Bone Count */
	virtual INT32		_GetLowerBoneCount( void) const		{ return CHARA_LOWER_BONE_COUNT; }

	void				_PresetResName( void);

	/** \brief ���� ���������� ���� */
	virtual void		_PlayDownfallSound( void);


public:
	bool				isReturnedInstance( void) const				{ return m_bReturnedInstance; }

	void				addCharaFlag( CHARA_FLAG flag)				{ m_CharaFlag |= flag; }
	void				removeCharaFlag( CHARA_FLAG flag)			{ m_CharaFlag &= ~flag; }
	bool				isCharaFlag( CHARA_FLAG flag) const			{ return (m_CharaFlag & flag) == flag; }

	/** \brief �޼����� ��带 Ȯ���մϴ�. */
	bool				isLeftHand(void) const						{ return isCharaFlag( CHARA_FLAG_LEFT_HAND); }

	/** \brief ���� ĳ���� ������ Ȯ���մϴ�. */
	// �ش� �÷��׷� �Ǻ��ϴ� ���� ���ڴ� ���� ��, �ø���, �ִϸ��̼ǿ� ���õȰ͸� ����ؾ��Ѵ�.
	bool				isFemale( void) const						{ return isCharaFlag(CHARA_FLAG_IS_FEMALE); }	

	void				AddOnObjectState();
	void				RemoveOnObjectState();

	/** \brief ĳ���� ���� ���� ����ũ�� ��ȯ�մϴ�. */
	UINT32				getEnableStateMask( void) const				{ return m_EnableStateMask; }

	/** \brief ĳ���� ���� ���� ����ũ�� flag�� �߰��մϴ�. */
	void				addEnableStateMask( UINT32 mask);	
	
	/** \brief ĳ���� ���� ���� ����ũ���� flag�� ����ϴ�. */
	void				removeEnableStateMask( UINT32 mask);
	
	/** \brief ĳ���� ���� ���� ����ũ�� Ȯ���մϴ�. */
	bool				isEnableStateMask( UINT32 mask) const		{ return (m_EnableStateMask & mask) != 0; }
	
	// 1��Ī �� üũ(��Ʈ��ũ ĳ���͵� �� �� �ִ�, ����)
	// 1��Ī ���� �ݵ��� FPS����Դϴ�. �̰��� ���ٸ� ���װ� �˴ϴ�.
	bool				is1PV( void) const							{ return (m_ViewType == CHARA_VIEW_1PV); }
	bool				is3PV( void) const							{ return (m_ViewType == CHARA_VIEW_3PV); }

	/** \brief �޼����� ��带 �����մϴ�. */
	void				setLeftHand( bool bFlag);

	REAL32				getJumpDelayElapsedTime( void) const		{ return m_fJumpDelayElapsedTime; }
	void				setJumpDelayElapsedTime( REAL32 tm)			{ m_fJumpDelayElapsedTime = tm; }
	
	/** \brief ����� ���� �ð��� ��ȯ�մϴ�. */
	REAL32				getInvincibleTime( void) const				{ return m_rInvincibleElapseTime; }

	/** \brief ���� �ð��� �����մϴ�. */
	void				setInvincibleTime( REAL32 tm)				{ m_rInvincibleElapseTime = tm; }

	/** \brief ���� ������� Ȯ���մϴ�. */
	bool				isInvincible( void) const					{ return (m_rInvincibleElapseTime > 0.0f);		}

	const CCharaInfo *			getDBInfo( void) const						{ return m_pDBInfo;					}
	const GLOBAL_CHARA_INFO *	getCharaInfo(void) const					{ return m_pCharaInfo;				}
	void						setCharaInfo( GLOBAL_CHARA_INFO * pInfo)	{ m_pCharaInfo = pInfo; }
	GLOBAL_CHARA_INFO *			setCharaInfo( void)							{ return m_pCharaInfo; }

	/** \brief Team�� ��ȯ�մϴ�. */
	CHARACTER_TEAM_TYPE		getCharaTeam( void) const				{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, CHARACTER_TEAM_NONE); return m_pCharaInfo->GetTeam(); }

	TEAM_TYPE			getTeam( void) const
	{
		if( getCharaTeam() & CHARACTER_TEAM_RED)	return TEAM_RED;
		return TEAM_BLUE;
	}

	/** \brief Network Index�� ��ȯ�մϴ�. */
	INT32				getCharaNetIndex( void) const				{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, -1); return (INT32) m_pCharaInfo->GetNetIdx();}

	/** \brief ���� HP�� ��ȯ�մϴ�. */
	INT32				getCurHP( void) const						{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, 0 ); return (INT32)(m_pCharaInfo->GetHP()); }

	/** \brief ĳ�� �������� ����� ���� HP�� ��ȯ�մϴ� */
	INT32				getCurHPAppliedCashItem( void) const;

	/** \brief ���� HP�� ��ȯ�մϴ�. */
	REAL32				getCurHP_Real( void) const					{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, 0.f ); return (m_pCharaInfo->GetHP()); }

	/** \brief ���� HP�� 100%�������� ��ȯ�մϴ�. */
	INT32				getCurHP_Percent( void) const;

	/** \brief �� HP�� ��ȯ�մϴ�. */
	INT32				getFullHP( void) const;
	

	/** \brief �� HP�� �����մϴ�. */
	void				setFullHP( void)					{ I3ASSERT_RETURN( m_pCharaInfo != nullptr );	m_pCharaInfo->SetHP((REAL32) getFullHP());	}

	
	// ĳ���� ���� üũ
	bool				isAIToClient( void) const			{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return m_pCharaInfo->GetAIToClient(); return false;}
	bool				isAI( void) const					{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_AI); return false; }
	bool				isLocal( void) const				{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) 	return (m_pCharaInfo->GetType() == CHARA::TYPE_PLAYER); return false;}
	bool				isNetwork( void) const				{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_PLAYER_NET) || (m_pCharaInfo->GetType() == CHARA::TYPE_DOMI_FULLBODY_AI); return false;	}
	bool				isNPC( void) const					{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_NPC); return false;}
	bool				isProfile( void) const				{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_PROFILE); return false;}
	bool				is1PVProfile( void) const			{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_FIRST_VIEW_PLAYER); return false; }
	
	/** \brief ���� �浹 �� �ð��� ��ȯ�մϴ�.*/
	REAL32				getContactTime( void) const			{ return m_rContactTime; }

	/** \brief ���� �浹 �� �ð��� �����մϴ�. */
	void				setContactTime( REAL32 tm)			{ m_rContactTime = tm; }

	/** \brief ĳ������ ���� ���� ������ ��ȯ�մϴ�. */
	const PHYSIX_SHAPESET_INFO * getPhysixShapeSetInfo( INT32 part) const	{ I3_BOUNDCHK( part, CHARA_HIT_MAXCOUNT); return &m_PhysixShapeSetInfo[ part]; }
	PHYSIX_SHAPESET_INFO * setPhysixShapeSetInfo( INT32 part)				{ I3_BOUNDCHK( part, CHARA_HIT_MAXCOUNT); return &m_PhysixShapeSetInfo[ part]; }

	/** \brief ���� �ڼ� ���� �ð��� ��ȯ�մϴ�. */
	REAL32				getAttackIdleTime( void) const			{ return m_rAttackIdleTime; }
	REAL32				getAttackIdleTimeLeft( void) const		{ return m_rAttackIdleTimeLeft; }

	/** \brief ���� �ڼ� ���� �ð��� �����մϴ�. */
	void				setAttackIdleTime( REAL32 tm)			{ m_rAttackIdleTime = tm; }
	void				setAttackIdleTimeLeft( REAL32 tm)		{ m_rAttackIdleTimeLeft	= tm; }
	
	REAL32				getActionPendingTime(void) const		{ return m_rPendingActionTime;}
	void				setActionPendingTime( REAL32 tm)		{ m_rPendingActionTime = tm;}

	/** \brief ������ �ð��� ��ȯ�մϴ�. */
	REAL32				getRespawnElapseTime( void) const		{ return m_rRespawnElapseTime; }

	/** \brief ������ �ð��� �����մϴ�. */
	void				setRespawnElapseTime( REAL32 tm)		{ m_rRespawnElapseTime = tm; }

	/** \brief �ǰ� ���� �ð��� ��ȯ�մϴ�. */
	REAL32				getDamageElapseTime( void ) const;
	REAL32				getDeathTime(void) const;

	/** \brief character buffer type�� ��ȯ �մϴ�.. */
	CHARA::BUFFER_TYPE	getBufferType( void) const					{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, CHARA::BUFFER_USER); return CHARA::Type2BufferType( m_pCharaInfo->GetType());}

	/** \brief Ÿ�� �ִ� ������Ʈ�� ��ȯ�մϴ�. */
	i3Object *			getAttachedObject( void) const				{ return m_pAttachedObject; }

	/** \brief Ÿ�� �ִ� ������Ʈ�� �����մϴ�. */
	void				setAttachedObject( i3Object * pObj)			{ m_pAttachedObject = pObj; }

	/** \brief ���� ������ ���� �ð��� ��ȯ�մϴ�.
		\note �ش� �ð����� �ݱ⸦ �� �� �����ϴ�. */
	REAL32				getDropWeaponTimer( void) const				{ return m_rDropWeaponTimer; }

	/** \brief ���߹��� HUD�� ���� ���� �ð��� ��ȯ�մϴ�.
		\note �ش� �ð����� �߻縦 �� �� �����ϴ�. */
	REAL32				getMultiWeaponTimer( void) const			{ return m_rMultiWeaponTimer; }
	void				setMultiWeaponTimer( REAL32 time)			{ m_rMultiWeaponTimer = time; }

	/** \brief ���� ������ ���� �ð��� �����մϴ�. */
	void				setDropWeaponTimer( REAL32 tm)				{ m_rDropWeaponTimer = tm; }

	bool				getDamagedByHowl(void) const				{ return isCharaFlag( CHARA_FLAG_DAMAGED_BY_HOWL); }
	void				setDamagedByHowl(bool bDamaged)				{ bDamaged?addCharaFlag(CHARA_FLAG_DAMAGED_BY_HOWL):removeCharaFlag(CHARA_FLAG_DAMAGED_BY_HOWL); }
	void				setDamagedByHowlTime( REAL32 time)			{ m_rDamageHowlEffectiveTime = time;}

	void				SetDeathblowState(bool bEnable);
	bool				isDeathblowState(void) const				{ return isCharaFlag( CHARA_FLAG_DEATHBLOW); }

	bool				isAttachedProtectParts(void);
	EQUIP::ePART		getProtectPartsType(void);

	/** \brief ��ǥ ���� ��� ����� Ȯ���մϴ�. */
	bool				isEnableTargetChase( void) const;
	REAL32				getTargetChaseRange( void) const;	// ��ǥ ���� �Ÿ�

	// revision 53646 HK417 ������� �߰�
	REAL32				getJumpParalysis(void) const				{ return m_rJumpParalysis;	}
	REAL32				getWalkParalysis(void) const				{ return m_rWalkParalysis;	}
	REAL32				getRunParalysis(void) const					{ return m_rRunParalysis;	}
	REAL32				getCrouchParalysis(void) const				{ return m_rCrouchParalysis; }

	CGameCharaInputContext *		getInputContext( void) const	{ return m_pInputContext; }
	CGameCharaWeaponContext*		getWeaponContext( void) const	{ return m_pWeaponContext; }
	CGameCharaUpdateContext *		getUpdateContext( void) const	{ return m_pUpdateContext; }
	CGameCharaActionContext *		getActionContext( void) const	{ return m_pActionContext; }
	CGameCharaAnimContext *			getAnimContext( void) const		{ return m_pAnimContext; }
	CGameCharaMoveContext *			getMoveContext( void) const		{ return m_pMoveContext; }
	CGameCharaMaterialContext *		getMaterialContext( void) const	{ return m_pMaterialContext; }
	CGameCharaEquipContext *		getEquipContext( void) const	{ return m_pEquipContext; }
	CGameCharaCameraContext *		getCameraContext( void) const	{ return m_pCameraContext; }
	CGameCharaBoneContext *			getBoneContext( void) const		{ return m_pBoneContext; }
	CGameCharaSoundContext *		getSoundContext( void) const	{ return m_pSoundContext; }
	CGameCharaCollisionContext *	getCollisionContext( void) const{ return m_pCollisionContext; }
	CGameCharaHUDContext *			getHUDContext( void) const		{ return m_pHUDContext; }
	CGameCharaMultiWeaponContext *	getMultiWeaponContext( void) const { return m_pMultiWeaponContext; }
	
	void ToggleRenderBone() {}

private:
	/** \brief ���� �� ��� �����Ѵ�.
		\desc ��) MainThread������ ȣ��Ǿ� �Ѵ�. */
	void				__CreateEquipment( bool bInBattle, bool bLoadWeapon);

	/** \brief Resource Instance�� �����մϴ�.
		\desc PhysX Shape�� �̶��� �����Ѵ�. ������ �����͸� �����ؾ���.
			��) MainThread������ ȣ��Ǿ� �Ѵ�. */
	void				__Instantiate( void);
	
public:
	CGameCharaBase( void);
	virtual ~CGameCharaBase( void);

	virtual void		Create( GLOBAL_CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo);

	/** \brief �ε� �� ���� �Լ� */
	virtual void		CreateFirst( const CCharaInfo * pDBInfo, bool bInLobby = false);

	/** \brief �ε� �� ���� �Լ�(InMain) */
	virtual void		CreateLater( const CCharaInfo * pDBInfo, bool bLoadWeapon = true);

	void				CreateInLobby( const CCharaInfo * pDBInfo);
	void				CreateLaterInLobby( void);
	
	virtual void		Destroy( void);
	virtual void		OnInitVariable();
	virtual void		OnUpdate( REAL32 tm) override;

	/** \brief AIState�� ����Ǹ� ȣ��˴ϴ�.
		\see i3Chara::OnChangeAIState */
	virtual void		OnChangeAIState( i3AIContext * pCtx);
	
	/** \brief ���ҽ��� bind�մϴ�.
		\note ĳ������ Bone�� ���� List�� �ۼ��մϴ�.
		\see i3Chara::OnBindResource */
	virtual void		OnBindResource( void) override;

	/** \brief ĳ���� Instance�� ��ȯ�մϴ�.
		\note ���ҽ��� ���ؼ� ��ȯ�� �ʿ��� ��� �� �Լ����� ó���Ǿ� ��.
			��) ����, ��� */
	virtual void		ReturnInstance( void) override;

	/** \brief ĳ���� ������ �ʱ�ȭ�մϴ�.
		\note Respawn�� ȣ��˴ϴ�. */
	virtual void		ResetChara( void);	

	/** \brief View Type�� ��ȯ�մϴ�. */
	CHARA_VIEW_TYPE		getViewType( void)						{ return m_ViewType; }

	/** \brief ���� View Type�� ��ȯ�մϴ�.
		\note Cmd_ChangeView�� ȣ���ϸ� ���� ���� ���� ����˴ϴ�. */
	CHARA_VIEW_TYPE		getOldViewType( void)					{ return m_oldViewType; }

	/** \brief View Controller�� �����մϴ�. */
	void				ChangeControl_View( CHARA_VIEW_TYPE viewType );

	/** \brief Loca Controller�� �����մϴ�. */
	void				ChangeControl_Local( void);

	/** \brief Network Controller�� �����մϴ�. */
	void				ChangeControl_Network( void);

	void				ChangeControl_Fullbody( void);

	/** \brief NPC Controller�� �����մϴ�. */
	void				ChangeControl_NPC( void);

	/** \brief Profile Controller�� �����մϴ�. */
	void				ChangeControl_Profile( void);

	/** \brief AI Controller�� �����մϴ�. */
	void				ChangeControl_AI( void);

	/** \brief ���� */
	void				MoveStop( void);

	/** \brief ������ ��ġ�� �����մϴ� */
	void				forceSetPos( const VEC3D * pPos);
	
	/** \brief Event message�� ó���մϴ�.
		\see i3GameObj::OnEvent */
	virtual bool		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code) override;

	/** \brief �ð��� ����մϴ�. */
	void				ProcessTimer( REAL32 rDeltatime);

	/** \brief Profile�� View character�� �����մϴ�. */
	void				SetViewerChara( bool bEnable = true);

	/** \brief Helmet ������ ����մϴ�. */
	bool				CalcHelmetPenetrate( const CHARA_DAMAGEINFO * pDamageInfo ) const;

	/** \brief �ǰ� ó���Դϴ�. */
	INT32				OnHit( CHARA_DAMAGEINFO * pDamageInfo, CGameCharaBase* pKiller = nullptr);

	/** \brief LOD�� ����մϴ�. */
	void				calcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio = 1.0f);
	

	// �ùķ��̼� �� Update
	void				PostUpdate(REAL32 rDeltaTime);

	/** \brief Damage�� ����մϴ�. */
	INT32				CalcDamage( const CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength, CGameCharaBase* pKiller);

	/** \brief ������ ���� Damage�� ����մϴ�. */
	void				CalcDamageByPart( CHARA_DAMAGEINFO * pDamageInfo, REAL32& rDamage);

	virtual bool		NET_HitForHost( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamage);
	void				NET_HitForClient( CHARA_DAMAGEINFO * pDamageInfo);
	void				NET_HitForClientNoHelmet( CHARA_DAMAGEINFO * pDamageInfo);

	/** \brief HP�� ����մϴ�. */
	void				CalcHitPoint( REAL32 rDamage, CHARA_DAMAGEINFO* pDamageInfo);


	// Medical Kit �߰�  - 2011.06.21
	bool				RecoveryHP(CHARA_DAMAGEINFO* pDamageInfo);

	// �Ʒ��� AI ĳ����
	void				BindAI(AI_BIND_DATA* pData);
	
	//--------------------------------------------------------------------------------------------------//
	//																									//
	//										Ability variables											//
	//																									//
	//	ĳ���� �ɷ�ġ�� ���� �������Դϴ�.
	//--------------------------------------------------------------------------------------------------//

	//-------------------------------------------------------------------------------------------------//
	// ��� ��ȣ ���� �ð� - �ڸ��� 2008. 7. 4
private:
	REAL32				m_rEndHelmProtectionTime = 0.0f;

public:
	REAL32				getHelmetProtectionTime( void)		{ return m_rEndHelmProtectionTime; }
	void				setHelmetProtectionTime( REAL32 tm)	{ m_rEndHelmProtectionTime = tm; }

	void				SetHelmProtectionFlag(bool bFlag)	{ bFlag?addCharaFlag(CHARA_FLAG_HELM_PROTECT):removeCharaFlag(CHARA_FLAG_HELM_PROTECT); }
	bool				IsHelmProtection()					{ return isCharaFlag(CHARA_FLAG_HELM_PROTECT); }
	void				SetHelmetProtection( bool bEnable);

	void				DropEquip( VEC3D * pDir = nullptr);
	void				HelmetEffect( CHARA_DAMAGEINFO * pInfo);

private:
	REAL32				m_rTotalAbility[ CHARA::eABILITY_COUNT];	// ������ + ��� + ��ų�� ���� �ɷ�ġ ��	// ����� �߰������� ����� �̷�� �����Ѵ�.
	REAL32				m_rMoveOffset = 0.0f;								// m_arMoveOffset�� ����� ������ �ʾƼ�..�׳� ���ϳ��� ����..ȣĪ�������� �ؼ��Ѵ�..
		
	INT32				m_iFullHP = 0;						// �� HP
	REAL32				m_rMovementSpeedRate[MST_COUNT][MDT_COUNT];	// �̵� �ӵ��� % 
	REAL32				m_rDefenceRate[ CHARA::eDAMAGEPART_COUNT];	// ���� %

	REAL32				m_rAccelerationTimeOffset = 0.0f;
	REAL32				m_rAccelerationTimeOffset_RunSkill = 0.0f;
	
	REAL32				m_rFallDamageMinOffset = 0.0f;
private:

	void				__ResetDesigOffsetDatas(void);

	/** \brief �������� ���� �ð� ��� */
	void				__ResetInvincibleTime( void);

	/** \brief �������� ���� ���̷� ��� */
	void				__ResetJumpHeightRate( void);

	/** \brief �������� ������ �ӷ� ��� */
	void				__ResetThrowSpeed( void);

	/** \brief �������� �� HP�� ��� */
	void				__ResetFullHP( void);

	/** \brief �������� �̵��ӵ� ��� */
	void				__ResetMovementSpeed( void);

	/** \brief �������� ���� ���*/
	void				__ResetDefenceRate( void);
	
	/** \brief �������� ���� ��ü �ӵ��� ��� */
	void				__ResetWeaponSwapTimeRate( void);

public:
	void				SetMoveSpeedAddOffset(REAL32 rMoveOffset);

	REAL32				getAbility( CHARA::eABILITY type)							{ return m_rTotalAbility[ type];	}

	void				SetAccelerationTimeOffset(REAL32 rAccelerationTimeOffset)	{ m_rAccelerationTimeOffset = rAccelerationTimeOffset; }
	
	REAL32				GetMoveSpeedAddOffset(EMoveSpeedType eMST, EMoveDirectionType eMDT) const;
	REAL32				GetJumpDelay() const						{ I3ASSERT_RETURN(m_pDBInfo != nullptr, 0.0f ); return m_pDBInfo->GetJumpDelay(); }
	REAL32				GetAccelerationTimeAddOffset() const;

	REAL32				GetDamageRateByDefence(CHARA_HIT_PART hitpart = CHARA_HIT_UNKNOWN) const;
	REAL32				GetHelmetProtectionRate() const;
	REAL32				GetWeaponSwapTimeScale( void) const;

	virtual void				HP_Recovery(REAL32 rRecoveryHP = 0.0f) { I3ASSERT_0; }
	REAL32						GetDinoDamage(REAL32 rDamage);
	REAL32						GetDinoFullHP();

	virtual REAL32				GetDinoSkillDuration(INT32 idx) const		{ return 0.0f; }
	virtual REAL32				GetDinoSkillCoolTime(INT32 idx) const		{ return 0.0f; }
	virtual REAL32				GetDinoSkillMovementRate(INT32 idx) const	{ return 0.0f; }

	// ���� ������ ������ ����.
	INT32				GetFallMinDamage() const;
	REAL32				GetFallDamageMinHeight() const;

	void				SetFallDamageMinOffset(REAL32 rOffset) { m_rFallDamageMinOffset = rOffset; }
	REAL32				GetFallDamageMinOffset() const { return m_rFallDamageMinOffset; }

	REAL32				GetFallDamageSlope() const;

	/** \brief ��ô���� �ӵ� */
	REAL32		GetThrowSpeed( WEAPON_SLOT_TYPE nSlot );
	REAL32		GetExThrowSpeed(WEAPON_SLOT_TYPE nSlot);

	//ũ��Ƽ�� ������

	INT32				m_hitCount;
	INT32				GetHitCount()				{ return m_hitCount; }
	void				SetHitCount(INT32 val)		{ m_hitCount = val;  }
	

private:
	REAL32				m_rJumpHeightOffset = 0.0f;

public:
	void				SetJumpHeightOffset( REAL32 offset)				{ m_rJumpHeightOffset = offset; }
	REAL32				GetJumpHeightOffset() const						{ return m_rJumpHeightOffset; }

	//--------------------------------------------------------------------------------------------------//
	//				������Ʈ ���� ����
	//--------------------------------------------------------------------------------------------------//
public:
	VEC3D m_vSavedPos;
	CGameObjectWithWeapon* m_pObjectForControlWeapon = nullptr;

public:
	void				OnObject(CGameObjectWithWeapon* pObj);
	void				OffObject(); 
	bool				IsOnObject() { return isCharaFlag( CHARA_FLAG_ON_OBJECT); }

	CGameObjectWithWeapon*			GetOnObject() { return m_pObjectForControlWeapon; }
	void				SaveNowPos();
	VEC3D*				LoadSavedPos();


	//--------------------------------------------------------------------------------------------------//
	//		Ŭ�� ��ũ ����
	//--------------------------------------------------------------------------------------------------//
private:
	INT32				m_iClanMark = 0;
	i3Texture*			m_pClanMarkTexture = nullptr;
	i3Texture*			m_pPBMarkTexture = nullptr;
	i3TextureBindAttr*  m_arClanMarkTexBindAttr[AVT_LOD_COUNT];
	i3AttrSet*			m_arClanMarkAttrSet[AVT_LOD_COUNT];
	
public:
	static i3BlendEnableAttr*	m_spBlendEnableAttr;
	static i3BlendModeAttr*		m_spBlendModeAttr;

	void				CharaChangeClanMark(UINT32 iClanMark);
	bool				IsPBMark(UINT32 uiMark);
	void				InitClanMark();
	void				CreateClanMarkTex();

	//--------------------------------------------------------------------------------------------------//
	//		���� ����
	//--------------------------------------------------------------------------------------------------//
	bool				IsDino() const					{ if(m_pCharaInfo != nullptr) return (m_pCharaInfo->IsDino() || m_pCharaInfo->IsDomiDino()); else return false; }

	 //���� ��� ����� (����,����)
	bool				IsRaptorBase() const			{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsRaptorBase();	else return false; }
	bool				IsRaptor() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsRaptor();	else return false; }
	bool				IsTank() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsTank();	else return false; }
	bool				IsAcid() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsAcid();	else return false; }
	bool				IsElite() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsElite();	else return false; }
	bool				IsSting() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsSting();	else return false; }
	bool				IsRex() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsRex();	else return false; }
	bool				IsCCRaptor() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsCCRaptor(); else return false; }
	bool				IsCCSting() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsCCSting(); else return false; }
	bool				IsCCAcid() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsCCAcid(); else return false; }
	bool				IsRaptorMercury() const			{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsRaptorMercury(); else return false; }
	bool				IsStingMars() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsStingMars(); else return false; }
	bool				IsAcidVulcan() const			{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsAcidVulcan(); else return false; }
#ifdef DOMI_DINO
	bool				IsDomiDino() const				{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiDino(); else return false;			}
	bool				IsDomiRaptor() const			{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptor(); else return false;			}
	bool				IsDomiRaptorGreen() const		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptorGreen(); else return false;	}
	bool				IsDomiRaptorBlue() const		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptorBlue(); else return false;		}
	bool				IsDomiAcid() const				{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiAcid(); else return false;			}
	bool				IsDomiAcidNotBomb() const		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiAcidNotBomb(); else return false;	}
	bool				IsDomiAcidItem() const			{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiAcidItem(); else return false;	}
	bool				IsDomiSting() const				{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiSting(); else return false;			}
	bool				IsDomiMutantRex() const			{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiMutantRex(); else return false;		}
	bool				IsDomiTank() const				{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiTank(); else return false;			}
	bool				IsDomiRaptorBoneBase() const	{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptorBoneBase(); else return false; }			
	bool				IsDomiRaptorMercury() const 	{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptorMercury(); else return false;	}
	bool				IsDomiStingMars() const 		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiStingMars(); else return false;		}
	bool				IsDomiAcidVulcan() const		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiAcidVulcan(); else return false;		}
#endif
	bool				IsControlledDino() const	{ return ( IsDino() && (isLocal() || isAI()) ); }

	//
	virtual void		setHpTagMaxHp( REAL32 max) {}
	virtual REAL32		getHpTagMaxHp() const { return 0.f;}


	virtual WeaponBase*			GetCharaWeapon( WEAPON_SLOT_TYPE slot) const;
	virtual WeaponBase*			GetCurrentCharaWeapon() const;
	virtual void				SetDinoWeaponType( WEAPON::FIRE_ORDER eType) { I3ASSERT_0; }

	
	//--------------------------------------------------------------------------------------------------//
	//		Battle Use Item Use ����
	//--------------------------------------------------------------------------------------------------//
	void				SetGravityRecovery(bool Enable);
	

	//////////////////////////////////////////////////////////////////////////////////////
	//									Action	Interface								//
	//////////////////////////////////////////////////////////////////////////////////////

public:
	// ���� ó�� �� ��Ŷ ���� - �ڸ��� 2008. 7. 23
	void				DeathCharaAndWritePacket( const CHARA_DAMAGEINFO * pDamageInfo);
	void				DeathCharaByFastObject(VEC3D* pDir);
public:
	bool			isLoadMagChg()							{ return m_bLoadMagChg;}
	void			SetLoadMagChg(bool bMagChg)				{ m_bLoadMagChg = bMagChg;}
public:
	/** \brief Crosshair�� �����մϴ�. */
	bool		Cmd_ApplyCrosshair( void);

	/* \brief ���� ���� ����
	\param[in] fireOrder ���� Ÿ��
	\param[in] pPos ���� ��ġ
	\param[in] pTargetPos ��ǥ ��ġ
	\return bool	*/
	bool		Cmd_Attack( WEAPON::FIRE_ORDER fireOrder = WEAPON::PRIMARY_FIRE,
		VEC3D * pPos = nullptr, VEC3D * pTarget = nullptr, bool bOnlyAction = false, INT32 iSlot = WEAPON_SLOT_PRIMARY);

	/** \brief 1/3��Ī �並 �����Ѵ�.
	\param[in] type CHARA_VIEW_TYPE
	*/
	void		Cmd_ChangeView( CHARA_VIEW_TYPE type, bool bPlayAnimSync = true);

	/** \brief ���� ���� ����
		\param[in] slot ���� ���� ��ȣ	*/
	bool		Cmd_ChangeWeapon( WEAPON_SLOT_TYPE slot, bool bDirect = false);
	
	/** \brief ��� �����Ѵ�. */
	void		Cmd_CreateEquip( CHARA_RES_ID charaResType, T_ItemID itemID, T_ItemID subitemID = 0);

	/** \brief ��� �����Ѵ�. */
	void		Cmd_DeleteEquip( EQUIP::ePART part);

	/** \brief ���⸦ �����Ѵ�. */
	bool		Cmd_CreateWeapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid);

	// ���� ����(���⺯�� ����)
	bool		Cmd_CreateWeapon_Only( WEAPON_SLOT_TYPE slot, T_ItemID Itemid);

	// �ɱ� �׼� ����
	bool		Cmd_Crouch( void);

	// Damage ���� ����
	void		Cmd_Damage( CHARA_DAMAGEINFO * pInfo);

	/** \brief �ڽſ��� Damage�� �޴� ���
		\note falling damage
		\param[in] nDamage Damage ��
		\param[in] iObjectIdx ������Ʈ�� ���� Damage�̸� ������Ʈ Idx 0xFFFF�̸� ������Ʈ�� �ƴ�
		\param[in] bFalling ���� ����		*/
	void		Cmd_DamageSelf( INT32 nDamage, UINT16 iObjectIdx = 0xFFFF, bool bFalling = false);


	/** \brief Death ���·� �����Ѵ�. */
	void		Cmd_Death( INT32 iShooterIdx, CHARA_DEATH_TYPE type, VEC3D * pHitDir);

	/** \brief �ش� ������ ���⸦ �����Ѵ�. */
	void		Cmd_DeleteWeapon( WEAPON_SLOT_TYPE slot);

	/** \brief �ش� ������ ���⸦ ����. */
	void		Cmd_DetachWeapon( WEAPON_SLOT_TYPE slot);

	/** \brief ���⸦ ĳ���Ϳ��� �����ϴ�. 
		\note ���̳� �������� �ִ� ��� �ڵ����� ����. 
	*/
	void		Cmd_DetachWeapon( WeaponBase * pWeapon);

	/** \brief ��� ����� ������. */
	bool		Cmd_DropEquip( EQUIP::ePART equipType, VEC3D * pPos);

	/** \brief ���⸦ ����� ������.
		\param[in] iSlot ���� ��ȣ
		\param[in] pPos ��ġvector
		\param[in] bNextWeapon ���� ����� �ڵ� ��ü ����
				false �̸� ���� ����� ��ü�ȴ�. */
	bool		Cmd_DropWeapon( WEAPON_SLOT_TYPE iSlot, VEC3D * pPos, bool bNextWeapon = true, bool bDirectPickup = false);

	// TeamTag Ȱ�� ������ �����Ѵ�.
	void		Cmd_Enable_TeamTag( bool bEnable);

	// Buff Tag Ȱ�� ������ �����Ѵ�.
	void		Cmd_Enable_Buff( INT32 bAttack, INT32 bSpeed);

	/** \brief ���� Extension�� Ȱ��ȭ�Ѵ�.	*/
	bool		Cmd_Extension( bool bEnable);

	/** \brief Hit Effect�� �Ѹ��ϴ�. */
	bool		Cmd_HitEffect( CHARA_DAMAGEINFO * pDamageInfo);

	/** \brief ĳ���� ���� �� Weapon�� Idle�� �����Ѵ�. */
	void		Cmd_Idle( void);

	/** \brief ĳ���� ��ü�� Idle�� ���� */
	void		Cmd_Idle_Lower( void);

	/** \brief ĳ���� ��ü�� idle�� ���� */
	void		Cmd_Idle_Upper( void);

	// revision 29547
	/// ��ü�� Move�϶��� ���� idle�� ����. ����ڵ���� ��ũ������ ���δ�
	void		Cmd_Idle_Upper_Move( void);

	/** \brief Camera�� Idle�� ���� */
	void		Cmd_Idle_Camera( void);

	/** \brief ��ź ��ġ
		\param[in] bEnable ��ġ ���� false�̸� ����Ѵ�.	*/
	bool		Cmd_Install( bool bEnable = true);

	/** �ܿ� ��� �̼ǹ��� ��ġ
		\param[in] bEnable ��ġ ���� false�̸� ����Ѵ�.	*/
	bool		Cmd_RunAwayInstall( bool bEnable = true);

	/** \brief ĳ���� ���� ����
		\param[in] bEnable true : �������� false : �Ϲݻ��� */
	void		Cmd_Invincible( bool bEnable, REAL32 InvincibleElapseTime = 0.f);

	// ���� �׼� ����
	void		Cmd_JumpEnd( void);

	/** \brief ���� ���� */
	void		Cmd_JumpStart( void);

	/** \brief ���� ���� ���⸸ �����Ѵ�. */
	void		Cmd_Only_AttackWeapon( WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE, VEC3D * pPos = nullptr, VEC3D * pTarget = nullptr);

	/** \brief ĳ������ ���� ��ȯ �׼Ǹ� ���մϴ�. */
	void		Cmd_Only_ChangeWeaponAction( void);

	/** \brief ĳ������ Damage �׼Ǹ� ���մϴ�. */
	void		Cmd_Only_DamageAction( CHARA_HIT_PART part, VEC3D * pDir);

	/** \brief Observer�� ����� ȣ��*/
	void		Cmd_Observer( bool bEnable = true);

	/** \brief Observer Mode���� Zoom ��� Ȱ��ȭ */
	void		Cmd_Observer_Zoom( void);

	/** \brief ���� �ݱ� */
	void		Cmd_PickupWeapon( WeaponBase * pWeapon, UINT8 nSyncNumber = 0, bool bCheckDropList = true, bool bPushWeaponList = true);

	/** \brief Profile ���� Ceremony�� �Ѵ�. */
	void		Cmd_Profile_Ceremony( void);

	/** \brief Profile ���� A�� �����Ѵ�.
		\note 1.0 ���� ����	*/
	void		Cmd_Profile_Idle_A( void);

	/** \brief Profile ���� B�� �����Ѵ�.
		\note 1.0 ���� ���� */
	void		Cmd_Profile_Idle_B( void);

	/** \brief Profile ���� Idle_Change�� �����Ѵ�. */
	void		Cmd_Profile_Idle_Change( bool bIsSameWeapon);

	/** \brief Profile ���� Idle_Stand�� �����Ѵ�. */
	void		Cmd_Profile_Idle_Stand( void);

	/** \brief ������ ������ �Ѵ�. */
	bool		Cmd_Reload( ACTION_LOAD_MAG_TYPE type = ACTION_LOAD_MAG);

	// Camera, Weapon, Action�� Idle�� �ʱ�ȭ�Ѵ�.
	void		Cmd_Reset( void);

	/** \brief ĳ���͸� Respawn ��Ų��.
		\param[in] pPos Respawn ��ġ
		\param[in] rTheta Theta
	*/
	void		Cmd_Respawn( VEC3D * pPos, REAL32 rTheta, bool bInvincible = true);

	/** \brief ��ü�� ���� ���·� �����Ѵ�. */
	bool		Cmd_Return_Upper( void);

	/** \brief ��ü�� ���� ���·� �����Ѵ�. */
	void		Cmd_Return_Lower( void);

	/** \brief �޸��� �׼� ����
		\note ��.��ü ��� �����˴ϴ�. 	*/
	void		Cmd_Run( VEC3D * pDir, bool bUpper = true, bool bLower = true);

	/** \brief �г����� ǥ���մϴ�. */
	void		Cmd_SetCharaNick( bool bFriendly);

	/** \brief clan Texture�� �����մϴ�. */
	bool		Cmd_SetClanTexture( INT32 nClanTexIdx);

	/** \brief ĳ���� ������ �����մϴ�.	*/
	void		Cmd_SetColor( COLORREAL * pDiff = nullptr, COLORREAL * pSpec = nullptr, REAL32 alpha = -1.f);

	/** \brief ĳ���� ������ �����մϴ�. */
	void		Cmd_SetPhysics( void);

	/** \brief �ӵ� ���� ȿ�� */
	void		Cmd_SpeedReduction( CHARA_HIT_PART part, INT32 rCurHP, INT32 nAfterHP);

	// ���� �׼� ����
	bool		Cmd_StandUp( void);

	/** \brief ���� ��ũ�� ����ϴ�.
		\note ���� �ٲ�� ���Ⱑ ������ �ٲ��ش�.
			�׼��� ���ϱ� ���� ȣ���Ͽ� ���⸦ �����մϴ�.
			��Ʈ��ũ�� ��� ���� ��ü�� �׼��� ���ÿ� �̷���� �� �ִ�.		*/
	void		Cmd_SyncWeapon( void);

	/** \brief */
	void		Cmd_ToggleBodyState( void);

	/** \brief Extension ���¸� Toggle�Ѵ�. */
	bool		Cmd_ToggleExtensionWeapon( void);

	/** \brief �ܱ���� Toggle�Ѵ�. */
	void		Cmd_ToggleZoom( void);


	/** \brief �ܱ���� Toggle�Ѵ�. */
	bool		Cmd_ToggleZoomForDSR( void);

	/** \brief ĳ���͸� ȸ����Ų��. */
	void		Cmd_Turn( REAL32 deg = 180.f);

	// Character�� Visible �� Controller Shape�� Ȱ��ȭ ������ �����Ѵ�.
	void		Cmd_Visible( bool bVisible);
	

	/** \brief ��ź ��ü
		\param[in] bEnable ��ü ����. false�̸� ����Ѵ�.*/
	bool		Cmd_Uninstall( bool bEnable = true);

	/** \brief Domination Skill Use
		\param[in] bEnable ��ü ����. false�̸� ����Ѵ�.*/
	bool		Cmd_UseDomiSkill();
	bool		Cmd_CancelDomiSkill();

	/** \brief �ȱ� ���� */
	void		Cmd_Walk( VEC3D * pDir);

	/** \brief ��ü�� �ȱ� ������ �Ѵ�. */
	void		Cmd_WalkLower( VEC3D * pDir);

	/** \brief ���� ������ �Ѵ�. */
	void		Cmd_Roar( void);

	/** \brief ���� ������ �Ѵ�. */
	void		Cmd_Dino_Idle( void);

	/** \brief ���� ������ �Ѵ�. */
	void		Cmd_Dino_Stun( void);

	
	// Ʃ�丮�� ����
public:
	void		WarpChara(VEC3D* pPos);
	
	// Animation Load for background system
	void		PreloadAnimationByWeapon( T_ItemID itemID);

	// Network Attack ���� ����
private:
	REAL32				m_iNetworkAttackCount = 0.0f;

public:
	void				ResetNetworkAttackCount() { m_iNetworkAttackCount = 0.f; }
	void				NetworkAttackCountPlus(REAL32 tm) { m_iNetworkAttackCount += tm; }
	bool				IsNetworkAttackReturnUpper();

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
public:
	INT64				m_fDebugtimeStart = 0;
	INT64				m_fDebugtimeEnd = 0;
	
	REAL32				m_rDebugFireTimer = 0.0f;
	REAL32				m_rDebugMoveTime = 0.0f;
	REAL32				m_rDebugElp = 0.0f;
	VEC3D				m_vDebugPos;
	VEC3D				m_vDebugOriginalPos;
	VEC3D				m_vDebugTargetPos;

	virtual void		DumpChara( void);
#endif

public:
	/** \brief Eye Enable */
	virtual void	SetEyeEnable( bool enable) { }

// To Version
protected:
	REAL32 _CalcDamageToVersion(const CHARA_DAMAGEINFO * pDamageInfo, REAL32 rValidDamageRate);
};

#endif

