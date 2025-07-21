/**
 * \file	Character\GameCharaAnimControl.h
 *
 * Declares the game chara animation control class.
 */


#if !defined( __GAME_CHARA_ANIMCALLBACK_H__)
#define __GAME_CHARA_ANIMCALLBACK_H__

#include "GameCharaDef.h"
#include "GameCharaControlBase.h"
#include "GameCharaAnimControlDefine.h"

#define ANIMTIME_MOVE_DURATION		0.666f			//	MOVE �ѱ���
#define ANIMTIME_MOVE_RIGHT_STEP	0.322f			//	MOVE ������ Ÿ�̹�
#define ANIMTIME_MOVE_LEFT_STEP		(ANIMTIME_MOVE_RIGHT_STEP * 2.0f - 0.01f)			//	MOVE �޹� Ÿ�̹�

class CGameCharaAnimControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaAnimControl, CGameCharaControlBase);

protected:
	void			_ProcessJumpEnd();
	REAL32			_PlayFootStepSound( REAL32 rDuration, REAL32 rElapseTime, REAL32 tm);

	void			_DinoChangeAIContext( const char * pszPVState );
	void			_CharaChangeAIContext( const char * pszPVState );
	
public:
	/** \brief �ִϸ��̼� �÷��� ������ �����ݴϴ�. 
		\note GameCharaAnimControl.h���� ENABLE_CHARA_ANIM_OUTPUT �� on/off �� �� �ֽ��ϴ�. */
	virtual void	AnimDebug( i3AIContext * pCtx, REAL32 timeStart = 0.0f);

	/** \brief AI Context�� �����մϴ�.
		\param[in] pszPVState Person View		*/
	virtual void	OnChangeAIContext( const char * pszPVState = "1PV:");
	
	// �� ��ü �ִϸ��̼��� ���� ���� ��Ű�⿡ ����ü ��ü �ִϸ��̼ǿ��� �����ϱ� �ٶ��ϴ�.
	virtual void	PlayStop( void);

	/** \brief ��ü �ִϸ��̼��� Play�մϴ�. */
	virtual bool	PlayLower( ID_LOWER_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief ��ü �ִϸ��̼��� Play�մϴ�. */
	virtual bool	PlayUpper( ID_UPPER_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief ���� ��ü �ִϸ��̼��� Play�մϴ�. */
	virtual bool	PlayDinoUpper( ID_UPPER_DINO_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief ���� ��ü �ִϸ��̼��� Play�մϴ�. */
	virtual bool	PlayDinoLower( ID_LOWER_DINO_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief ��� ��ü �ִϸ��̼��� Play�մϴ�. */
	virtual bool	PlayUpperDual( ID_UPPER_DUAL_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief ���� ������ ��ü �ִϸ��̼��� Play�մϴ�.
		\note Idle�� Move�� �ش�˴ϴ�. */
	virtual bool	PlayUpperReturn( void);

	/** \brief �ɱ�/���⿡ ���� Animation�� Play�մϴ�.
		\note Animation Context�� ���� �����ϰ� �˴ϴ�. */
	virtual bool	PlayBodyLowerSync( void);

	/** \brief �ڵ� ���ε带 üũ �մϴ�. */
	virtual void	PlayAutoReload( void);

	/************************************************************************************/
	/*							Animation Callback procedure							*/
	/** \brief Idle�� */
	virtual void	OnIdleRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief �̵� ������ */
	virtual void	OnMoveRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief ���� ������ */
	virtual void	OnAttackRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief ���� ���� */
	virtual void	OnAttackFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief ���� �غ� ���� ���� */
	virtual void	OnAttackReadyFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief �ٱ��� */
	virtual void	OnJumpStartRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief ������ */
	virtual void	OnJumpEndRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief ���� ���� */
	virtual void	OnJumpEndFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief ������ */
	virtual void	OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief ź�� ������ */
	virtual void	OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief ź�� ���� ���� */
	virtual void	OnLoadBulletFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ��Ʈ�׼��� */
	virtual void	OnFireBulletRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief ��Ʈ�׼� ���� */
	virtual void	OnFireBulletFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ���� ��ü�� */
	virtual void	OnWeaponSwapRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief �ǰ� ������ */
	virtual void	OnDamageRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief ��� ���� ���� */
	virtual void	OnDeathFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief Extension Ȱ��ȭ�� */
	virtual void	OnExtensionActivateRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief Extension ��Ȱ��ȭ �� */
	virtual void	OnExtensionDeactivateRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI Profile Idle A ������ */
	virtual void	OnUI_IdleARun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief UI Profile Idle A ���� */
	virtual void	OnUI_IdleAFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief UI Profile Idle B ������ */
	virtual void	OnUI_IdleBRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI Profile Idle B ���� */
	virtual void	OnUI_IdleBFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI Profile Idle Change ������ */
	virtual void	OnUI_IdleChangeRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI Profile Idle Change ���� */
	virtual void	OnUI_IdleChangeFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief UI Profile Idle Stand ������ */
	virtual void	OnUI_IdleStandRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief UI Profile Idle Stand ���� */
	virtual void	OnUI_IdleStandFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ���� Idle�� */
	virtual void	OnLeft_IdleRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ���� �̵� ������ */
	virtual void	OnLeft_MoveRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ���� ���� ������ */
	virtual void	OnLeft_AttackRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ���� �ٱ��� */
	virtual void	OnLeft_JumpStartRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ���� ������ */
	virtual void	OnLeft_JumpEndRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ������ Idle�� */
	virtual void	OnRight_IdleRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ������ �̵� ������ */
	virtual void	OnRight_MoveRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ������ ���� ������ */
	virtual void	OnRight_AttackRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ������ �ٱ��� */
	virtual void	OnRight_JumpStartRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief ������ ������ */
	virtual void	OnRight_JumpEndRun( i3AIContext * pCtx, REAL32 tm) {}
};

#endif
