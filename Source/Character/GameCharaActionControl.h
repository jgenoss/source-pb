/**
 * \file	Character\GameCharaActionControl.h
 *
 * Declares the game chara action control class.
 */


#if !defined( __GAME_CHARA_ACTION_CONTROL_H__ )
#define __GAME_CHARA_ACTION_CONTROL_H__

#include "GameCharaControlBase.h"
#include "GameCharaActionControlDefine.h"
#include "GameCharaAnimControlDefine.h"

/*
Action ó�� ���� (State�� Anim�� ���� ���þ��� �۵��ϱ� ������ ��ũ�� �´� Action�� �����ϱ� ���ؼ��� State�� Anim�� ���ÿ� ����� �Ѵ�.)

ù°. Action�� �´� State�� �����Ѵ�.
��°. Action�� �´� Anim�� �÷����Ѵ�.
*/

class CGameCharaActionControl : public CGameCharaControlBase
{
	I3_ABSTRACT_CLASS_DEFINE( CGameCharaActionControl, CGameCharaControlBase );

	friend class CGameCharaActionContext;

private:
	/** \brief Jump �ִϸ��̼��� Play�մϴ�. */
	void			__Play_Animation_Jump( void);

	/** \brief ��ü�� �̵� �ִϸ��̼ǿ� ���� Scale Time�� ���մϴ�.
		\note ���� ������ �̵����� ���� �����˴ϴ�. */
	REAL32			_CalcRunUpperScaleTime( void);

	/** \brief ��ü�� �̵� �ִϸ��̼ǿ� ���� Scale Time�� ���մϴ�. */
	REAL32			_CalcRunDinoUpperScaleTime( void);

	/** \brief ���⿡ ���� Death Animation id�� ���մϴ�. */
	ID_LOWER_AI		_FindDeathAnim( CHARA_DEATH_TYPE deathType, VEC3D * pVec);

	/** \brief ���⿡ ���� DinoDeath Animation id�� ���մϴ�. */
	ID_LOWER_DINO_AI _FindDeathAnimForDino( CHARA_DEATH_TYPE deathType, VEC3D * pVec);

protected:
	REAL32			_CalcRunLowerScaleTime( ID_LOWER_AI nId, REAL32 rSpeed);

	REAL32			_CalcRunLowerDinoScaleTime( ID_LOWER_DINO_AI nId, REAL32 rSpeed);

	ID_UPPER_AI		_ValidReloadAnimationID( WeaponBase * pWeapon, CWeaponInfo * pWeaponInfo);

	enum LOWER_AI_TYPE	{ LOWER_AI_DEATH_A = 0, LOWER_AI_DEATH_B, LOWER_AI_DEATH_H, MAX_LOWER_AI_DEATH };
	enum LOWER_AI_DIR	{ LOWER_AI_DIR_FRONT = 0, LOWER_AI_DIR_LEFT, LOWER_AI_DIR_RIGHT, LOWER_AI_DIR_BACK };
	ID_LOWER_AI		GetLowerAnimID(LOWER_AI_TYPE Type, LOWER_AI_DIR Dir);
	ID_LOWER_DINO_AI GetLowerAnimIDForDino(LOWER_AI_TYPE Type, LOWER_AI_DIR Dir);

	/** \brief ��ü�� ���ؼ� Idle ���·� �����Ѵ�.*/
	bool	ActionLowerReturn( void);

	/** \brief ��ü�� ���ؼ� Idle ���·� �����Ѵ�. */
	bool	ActionUpperReturn( void);

	/** \brief �����ڼ��� Toggle�մϴ�. (��� ����) */
	void	ActionToggleIdle( void);

	/** \brief ���� ���� Idle ���·� �����մϴ�. */
	bool	ActionUpperLeftReturn( void);

	/** \brief ������ ���� Idle ���·� �����մϴ�. */
	bool	ActionUpperRightReturn( void);

	/** \brief ���� ���� Run ���·� �����մϴ�. */
	bool	ActionRunUpperLeft( void);

	/** \brief ������ ���� Run ���·� �����մϴ�. */
	bool	ActionRunUpperRight( void);

	void	_ActionFireDual( REAL32 startTime, REAL32 scale, bool bRight);

	/****************************************************************************/
	/*							 internal inline								*/
	/****************************************************************************/
	inline bool		isBody( CHARA_ACTION_BODYLOWER body)	{ return getActionContext()->isBody( body); }
	inline bool		isBody2( CHARA_ACTION_BODYUPPER body)	{ return getActionContext()->isBody2( body); }
	inline bool		isLower( CHARA_ACTION_LOWER lower)		{ return getActionContext()->isLower( lower); }
	inline bool		isUpper( CHARA_ACTION_UPPER upper)		{ return getActionContext()->isUpper( upper); }
	inline void		setLower( CHARA_ACTION_LOWER lower)		{ getActionContext()->setLower( lower); }
	inline void		setUpper( CHARA_ACTION_UPPER upper)		{ getActionContext()->setUpper( upper); }
	inline void		setUpperLeft( CHARA_ACTION_UPPER upper)	{ getActionContext()->setUpperLeft( upper); }

	void		_Animation_DinoUpper( ID_UPPER_DINO_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	inline void		_Animation_DualUpper( ID_UPPER_DUAL_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f)
	{
		getActionContext()->_Play_Anim_DualUpper( aiID, startTime, scale);
	}

	inline void		_Animation_Lower( ID_LOWER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f)
	{
		I3_BOUNDCHK( aiID, ID_LOWER_AI_MAX);
		getActionContext()->_Play_Anim_Lower( aiID, startTime, scale);
	}

	void		_Animation_LowerForDino( ID_LOWER_DINO_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	inline void		_Animation_Upper( ID_UPPER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f)
	{
		I3_BOUNDCHK( aiID, ID_UPPER_AI_MAX);
		getActionContext()->_Play_Anim_Upper( aiID, startTime, scale);
		//I3PRINTLOG(I3LOG_NOTICE,"Chara rTimeScale: %f", scale);
	}


	inline void		_ApplyActionBody( CHARA_ACTION_BODYLOWER state )
	{
		getActionContext()->ApplyActionBody( state);
	}

	inline void		_ApplyActionBody2( CHARA_ACTION_BODYUPPER state)
	{
		getActionContext()->ApplyActionBody2( state);
	}

public:
	/** \brief �⺻ �ڼ��� �����մϴ�. */
	virtual bool	OnActionReset( void);

	/** \brief �Ͼ�⸦ �����մϴ�. */
	virtual bool	OnActionStandup( void);

	/** \brief �ɱ⸦ �����մϴ�. */
	virtual bool	OnActionCrouch( void);

	virtual bool	OnActionDinoRunLower( VEC3D * pDir)	{ return true; }

	/** \brief ��ü�� ���� �޸��� ������ �����մϴ�. */
	virtual bool	OnActionRunLower( VEC3D * pDir)	{ return true; }

	/** \brief ��ü�� ���� �ȱ� ������ �����մϴ�. */
	virtual bool	OnActionWalkLower( VEC3D * pDir) { return true; }

	/** \brief ��ü�� ���� �̵� ������ �����մϴ�. */
	virtual bool	OnActionRunUpper( void);

	/** \brief ���� ��ü�� ���� �̵� ������ �����մϴ�. */
	virtual bool	OnActionRunDinoUpper( void);
	
	/** \brief ���� �ȱ� ��ü�� ���� �̵� ������ �����մϴ�. */
	virtual bool	OnActionWalkDinoUpper( void);

	virtual bool	OnActionSteadyDinoLower( void);

    /** \brief ��ü�� ���� �⺻ �ڼ��� �����մϴ�. */
	virtual bool	OnActionSteadyLower( void);

	/** \brief ��ü�� ���� �⺻ �ڼ��� �����մϴ�. */
	virtual bool	OnActionSteadyUpper( void);

	/** \brief ���� ��ü�� ���� �⺻ �ڼ��� �����մϴ�. */
	virtual bool	OnActionSteadyDinoUpper( void);

	/** \brief �ٱ� ������ �����մϴ�. */
	virtual bool	OnActionJumpStart( void);

	/** \brief ��ü�� ���� ���� �ϷḦ �����մϴ�. */
	virtual bool	OnActionJumpLowerFinish( void);

	/** \brief ��ü�� ���� ���� �ϷḦ �����մϴ�. */
	virtual bool	OnActionJumpUpperFinish( void);

	/* \brief �������� ���� �޾� �����ϴ� �ൿ�� ���� �մϴ�. */
	virtual bool	OnActionForceJumpStart( VEC3D * pDir, REAL32 rSpeed, REAL32 rJumpPower );

	/** \brief �ǰ� ������ �����մϴ�. */
	virtual bool	OnActionDamage( CHARA_HIT_PART part, VEC3D * pDir)	{ return true; }

	/** \brief ��� ������ �����մϴ�.  */
	virtual bool	OnActionDeath( INT32 iKillIdx, CHARA_DEATH_TYPE type, VEC3D * pDir);	

	/** \brief ProFileA ������ �����մϴ�. */
	virtual bool	OnActionProfileIdleA( void)	{ return true; }

	/** \brief ProfileB ������ �����մϴ�. */
	virtual bool	OnActionProfileIdleB( void) { return true; }

	/** \brief Profile Idle_Change (���� ����) ������ �����մϴ�. */
	virtual bool	OnActionProfileIdleChange(bool bIsSameWeapon)	{return true; }

	/** \brief Profile Idle_Stand (���) ������ �����մϴ�. */
	virtual bool	OnActionProfileIdleStand(void) {return true; }

	/** \brief ���� ������ �����մϴ�.
		\param[in] iSlot ���� ���� ��ȣ
		\param[in] order ���� ���� ����		*/
	virtual bool	OnActionFire( INT32 iSlot = -1, WEAPON::FIRE_ORDER order = WEAPON::PRIMARY_FIRE);

	/** \brief ������ ���� ������ �����մϴ�.
		\param[in] iSlot ���� ���� ��ȣ
		\param[in] order ���� ���� */
	bool			OnActionFireDino( INT32 iSlot, WEAPON::FIRE_ORDER order);

	/** \brief ����� ���� ������ �����մϴ�.
		\param[in] order ���� ���� */
	bool			OnActionFireHuman( WEAPON::FIRE_ORDER order);

	/** \brief ���� �غ� ������ �����մϴ�.
		\note ����ź �迭�� �ش�˴ϴ�. */
	virtual bool	OnActionReadyToFire( void);

	/** \brief Extension ������ �����մϴ�.
		\param[in] bActivate Ȱ��ȭ ���� */
	virtual bool	OnActionExtensionWeapon( bool bActivate);

	/** \brief ���� ������ �����մϴ�. */
	virtual bool	OnActionActiveZoom( void);

	/** \brief ���� ������ ����մϴ�. */
	virtual bool	OnActionCancelZoom( void);	

	/** \brief ���� ��ü ������ �����մϴ�. */
	virtual bool	OnActionSwapWeapon( void);

	/** \brief źâ ���� ������ �����մϴ�. */
	virtual bool	OnActionLoadMagazine( void);

	/** \brief ��� źâ�� �� ������ �����մϴ�. */
	virtual bool	OnActionLoadMagazineDual( void);

	/** \brief ��ǿ� ź���� �����ϴ� ������ �����մϴ�. */
	virtual bool	OnActionLoadBullet( void);

	/** \brief ��ǿ��� ź���� �����ϴ� ������ �����մϴ�.
		\note ��ź �迭�� �ش�˴ϴ�. */
	virtual bool	OnActionRemoveShell( void);

	/** \brief ���� 3�� �ش��ϴ� ���� ������ �����մϴ�. */
	virtual bool	OnActionLoad_B_Magazine( void);

	/** \brief ���� 3�� �ش��ϴ� ���� �غ� ������ �����մϴ�. */
	virtual bool	OnActionLoadMag_Ready( void);

	/** \brief �߻��� ��Ʈ�׼� ������ �����մϴ�. */
	virtual bool	OnActionFireBullet( void);

	// Dual ����
	// ���� ������ ���� ó��
	virtual bool	OnActionJumpUpperLeftFinish( void);

	/** \brief ���� ��ü ���� �ڼ��� �����մϴ�. */
	virtual bool	OnActionDinoRoar( void);

	/** \brief ��ü�� ���� �⺻ �ڼ��� �����մϴ�. */
	virtual bool	OnActionDinoIdle( void);

	/** \brief ��ü�� ���� �⺻ �ڼ��� �����մϴ�. */
	virtual bool	OnActionDinoStun( void);

	
protected:
	REAL32			m_rStartStandUpTime = 0.0f;

	// �����̼���(����) ���� �߰�
	REAL32			GetScaleTimeByWalkType(REAL32 fVel, VEC3D* pMoveDir);

protected: 
	// To Version
	REAL32			_CalcReloadTimeScaleToVersion(WeaponBase * pWeapon, REAL32 rAnimDuration);
	REAL32			_CalcSwapTimeScaleToVersion(void);

};

#endif
