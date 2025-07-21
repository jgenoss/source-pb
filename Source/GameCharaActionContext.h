/**
 * \file	GameCharaActionContext.h
 *
 * Declares the game chara action context class.
 */


#if !defined( __GAME_CHARA_ACTION_CONTEXT_H__)
#define __GAME_CHARA_ACTION_CONTEXT_H__

#include "GameCharaContextBase.h"

class CGameCharaActionControl;
class CGameCharaActionControl1PV;
class CGameCharaActionControl3PV;

class CGameCharaActionContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaActionContext, CGameCharaContextBase);

	friend class CGameCharaBase;
	friend class CGameCharaAnimContext;
	
	friend class CGameCharaActionControl;
	friend class CGameCharaActionControl1PV;
	friend class CGameCharaActionControl3PV;
	
protected:
	// ActionControl
	CGameCharaActionControl *			m_pCurrentActionControl = nullptr;
	CGameCharaActionControl1PV *		m_p1PVActionControl = nullptr;
	CGameCharaActionControl3PV *		m_p3PVActionControl = nullptr;

	CHARA_ACTION_BODYLOWER				m_ActionBody = CHARA_BODYLOWER_STAND;		//	Stand or Sit State
	CHARA_ACTION_BODYUPPER				m_ActionBody2 = CHARA_BODYUPPER_ATTACKIDLE;		//	AttackIdle or Idle or Zoom or Grenade State
	CHARA_ACTION_LOWER					m_ActionLower = CHARA_LOWER_NONE;		//	��ü State
	CHARA_ACTION_UPPER					m_ActionUpper = CHARA_UPPER_NONE;		//	��ü State

	CHARA_ACTION_UPPER					m_ActionUpperLeft = CHARA_UPPER_NONE;	// �¿� �и�
	CHARA_ACTION_COMMON					m_ActionCommon = CHARA_ACTION_COMMON_NONE;
	NPC_ACTION_STATE					m_NPCActionState = NPC_ACTION_NONE;

public:
	/** \brief ��ü ���¸� ��ȯ�մϴ�. */
	CHARA_ACTION_BODYLOWER	getBodyLower( void)						{ return m_ActionBody;				}

	/** \brief ��ü ���¸� �����մϴ�.*/
	void				setBodyLower( CHARA_ACTION_BODYLOWER body)	{ m_ActionBody = body; }

	/** \brief ��ü ���¸� ��ȯ�մϴ�. */
	CHARA_ACTION_BODYUPPER	getBodyUpper( void)						{ return m_ActionBody2;				}

	/** \brief ��ü ���¸� �����մϴ�. */
	void				setBodyUpper( CHARA_ACTION_BODYUPPER body2)	{ m_ActionBody2 = body2; }

	/** \brief ��ü �׼��� ��ȯ�մϴ�. */
	CHARA_ACTION_LOWER	getLower( void)								{ return m_ActionLower;				}

	/** \brief ��ü �׼��� �����մϴ�. */
	void				setLower( CHARA_ACTION_LOWER lower)			
	{ 
        m_ActionLower = lower; 
	}

	/** \brief ��ü �׼��� ��ȯ�մϴ�. */
	CHARA_ACTION_UPPER	getUpper( void)								{ return m_ActionUpper;				}	

	/** \brief ��ü �׼��� �����մϴ�. */
	void				setUpper( CHARA_ACTION_UPPER upper)			{ m_ActionUpper = upper; }

	/** \brief ��ü Left �׼��� ��ȯ�մϴ�. */
	CHARA_ACTION_UPPER	getUpperLeft( void)							{ return m_ActionUpperLeft; }

	/** \brief ��ü Left �׼��� �����մϴ�. */
	void				setUpperLeft( CHARA_ACTION_UPPER upper)		{ m_ActionUpperLeft = upper; }

	/** \brief common �׼��� ��ȯ�մϴ�. */
	CHARA_ACTION_COMMON	getCommon( void)							{ return m_ActionCommon;				}

	/** \brief Common �׼��� �����մϴ�. */
	void				setCommon( CHARA_ACTION_COMMON act)			{ m_ActionCommon	= act;				}

	/** \brief ��ü ���¸� Ȯ���մϴ�. */
	bool				isBody( CHARA_ACTION_BODYLOWER nState)		{ return (m_ActionBody == nState);	}

	/** \brief ��ü ���¸� Ȯ���մϴ�. */
	bool				isBody2( CHARA_ACTION_BODYUPPER nState)		{ return (m_ActionBody2 == nState);	}

	/** \brief ��ü �׼��� Ȯ���մϴ�. */
	bool				isLower( CHARA_ACTION_LOWER nState)			{ return (m_ActionLower == nState);	}

	/** \brief ��ü �׼��� Ȯ���մϴ�. */
	bool				isUpper( CHARA_ACTION_UPPER nState)			{ return (m_ActionUpper == nState);	}

	/** \brief ������Ʈ�� ź ���� Ȯ���մϴ�. */
	bool				isEnableOnObject()							{ return (m_ActionUpper == CHARA_UPPER_IDLE || m_ActionUpper == CHARA_UPPER_MOVE || m_ActionUpper == CHARA_UPPER_DAMAGE); }

	/** \brief Common �׼��� Ȯ���մϴ�. */
	bool				isCommon( CHARA_ACTION_COMMON nState)		{ return (m_ActionCommon == nState);	}

public:
	CGameCharaActionContext() {}
	virtual ~CGameCharaActionContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV() override;
	virtual void	OnChange3PV() override;

	virtual void	OnChangeLocal() override;
	virtual void	OnChangeNetwork() override;
	virtual void	OnChangeNPC() override;
	virtual void	OnChangeAI() override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief ��ü ���¸� �����ϰ� View type�� �����մϴ�. */
	void			ApplyActionBody( CHARA_ACTION_BODYLOWER state);

	/** \brief ��ü ���¸� �����ϰ� view type�� �����մϴ�. */
	void			ApplyActionBody2( CHARA_ACTION_BODYUPPER state);


public:
	/** \brief ��� �ִϸ��̼��� �����Ѵ�.
		\note �⺻ �ִϸ��̼����� �����˴ϴ�. */
	void	_All_Stop();

	/** \brief Dual upper animation�� �����Ѵ�.
		\param[in] aiID ID_UPPER_DUAL_AI index
		\param[in] startTime ���� �ð�
		\param[in] scale �ִϸ��̼� �ӵ� */
	void	_Play_Anim_DualUpper( ID_UPPER_DUAL_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief ��ü animation�� �����Ѵ�.
		\param[in] aiID ID_LOWER_AI index
		\param[in] startTime animation ���� �ð�
		\param[in] scale animation scale ��	*/
	void	_Play_Anim_Lower( ID_LOWER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief ��ü animation�� �����Ѵ�.
		\param[in] aiID ID_UPPER_AI index
		\param[in] startTime animation ���� �ð�
		\param[in] scale animation scale �� */
	void	_Play_Anim_Upper( ID_UPPER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief Body State�� �����մϴ�. */
	void	_Play_Body_Sync();

	/** \brief Ceremony animation�� �մϴ�. */
	bool	_Play_Ceremony( void);

	/** \brief �ɱ� animation�� �Ѵ�. */
	void	_Play_Crouch( void);

	/** \brief Damage ������ �Ѵ�.
		\param[in] part ���� ����
		\param[in] pDir ���� ����
		\param[in] bFalling ���� ������ ����	*/
	void	_Play_Damage( CHARA_HIT_PART part = CHARA_HIT_UNKNOWN, VEC3D * pDir = nullptr, bool bFalling = false);

	/** \brief Death ������ �Ѵ�.
		\param[in] iKillIdx Killer index
		\param[in] type CHARA_DEATH_TYPE
		\param[in] pDir ���� ���� 	*/
	void	_Play_Death( INT32 iKiilIdx, CHARA_DEATH_TYPE type, VEC3D * pDir);

	/** \brief Dual�� �޼��� idle�� �����Ѵ�. */
	void	_Play_Dual_Idle_Left( void);

	/** \brief Dual�� �������� idle�� �����Ѵ�. */
	void	_Play_Dual_Idle_Right( void);

	/** \brief Dual�� �޼տ� ���ؼ� Jump ������ �����Ѵ�. */
    void	_Play_Dual_Jump_Left();

	/** \brief Dual�� �����տ� ���ؼ� Jump ������ �����Ѵ�. */
    void	_Play_Dual_Jump_Right();

	/** \brief Dual�� �޼տ� ���ؼ� Jump End ������ �����Ѵ�. */
    void	_Play_Dual_JumpEnd_Left();

	/** \brief Dual�� �����տ� ���ؼ� Jump End ������ �����Ѵ�. */
    void	_Play_Dual_JumpEnd_Right();

	/** \brief Dual�� �޼տ� ���ؼ� Run ������ �����Ѵ�. */
    void	_Play_Dual_Run_Left( void);
	
	/** \brief Dual�� �����տ� ���ؼ� Run ������ �����Ѵ�. */
    void	_Play_Dual_Run_Right( void);

	/** \brief ���� Extension ������ �����Ѵ�.
		\param[in] bEnable Extension Ȱ�� ����	*/
	bool	_Play_Extension( bool bEnable);

	/** \brief Jump ������ �����Ѵ�. */
	bool	_Play_JumpStart( void);

	/** \brief Jump ������ �����Ѵ�. */
	bool	_Play_JumpStartForDino( void);

	/** \brief ���� ������ �����Ѵ�. */
	void	_Play_JumpEnd( void);

	/** \brief ���� ������ �����Ѵ�. */
	void	_Play_JumpEndForDino( void);

	/* \brief force jump�� �����Ѵ�. */
	void    _Play_ForceJump( VEC3D *pDir, REAL32 rSpeed, REAL32 rJumpPower );

	/** \brief ��ü�� �⺻ �� �������� �����Ѵ�. */
	void	_Play_Lower_Idle();

	/** \brief Dino��ü�� �⺻ �� �������� �����Ѵ�. */
	void	_Play_DinoLower_Idle();

	/** \brief ��ü�� ���ؼ� ���� ������ �����Ѵ�. */
    void	_Play_Lower_Jump();

	/** \brief ��ü�� ���ؼ� Idle �Ǵ� �ٱ� �������� �����Ѵ�. */
	void	_Play_Lower_Return( void);

	/** \brief ��ü�� ���ؼ� �ٱ� ������ �����Ѵ�.
		\param[in] pDir �̵�����		*/
    void	_Play_Lower_Run( VEC3D * pDir);

	/** \brief ��ü�� ���ؼ� �ٱ� ������ �����Ѵ�.
		\param[in] pDir �̵�����		*/
	void	_Play_Dino_Lower_Run( VEC3D * pDir);

	/** \brief ��ü�� ���ؼ� �ȱ� ������ �����Ѵ�.
		\param[in] pDir �̵����� */
    void	_Play_Lower_Walk( VEC3D * pDir);

	/** \brief Profile ���� A�� �Ѵ�. */
	void	_Play_Profile_Idle_A( void);

	/** \brief Profile ���� B�� �Ѵ�. */
	void	_Play_Profile_Idle_B( void);

	/** \brief Profile Change ������ �Ѵ�. */
	void	_Play_Profile_Idle_Change( bool bIsSameWeapon);

	/** \brief Profile Stand ������ �Ѵ�. */
	void	_Play_Profile_Idle_Stand( void);

	/** \brief �Ͼ�� ������ �Ѵ�. */
    void	_Play_StandUp();

	/** \brief ���� ��ȯ ������ �Ѵ�. */
	void	_Play_Upper_ChangeWeapon();

	/** \brief ���� ���� ������ �Ѵ�.
		\param[in] iSlot ���� ����
		\param[in] order ���� ���		*/
    bool	_Play_Upper_Fire( INT32 iSlot = -1, WEAPON::FIRE_ORDER order = WEAPON::PRIMARY_FIRE);

	/** \brief ��ü�� ���ؼ� �⺻ �ڼ��� �����Ѵ�. */
	bool	_Play_Upper_Idle();

	/** \brief ��ü�� ���ؼ� ���� ������ �����Ѵ�. */
	void	_Play_Upper_JumpEnd( void);

	/** \brief ������ ������ �Ѵ�.
		\param[in] type ������ ���� */
    bool	_Play_Upper_Reload( ACTION_LOAD_MAG_TYPE type = ACTION_LOAD_MAG);

	/** \brief ��ü�� ���ؼ� Idle �Ǵ� �̵� �������� �����Ѵ�. */
	bool	_Play_Upper_Return( void);

	/** \brief ��ü�� ���ؼ� �̵� �������� �����Ѵ�. */
	bool	_Play_Upper_Run();
	
	bool	_Play_Upper_Walk();//EscapeMission: ������ �ȱ� ��ü������ ���� ���� �߰�

	/** \brief �� ������ �Ѵ�. */
	bool	_Play_Zoom();

	/** \brief ���� �����ϴ� �������� �����Ѵ�. */
	bool	_Play_ZoomCancel( void);

	/** \brief Body state�� �����Ѵ�. */
	void	_Toggle_BodyState( void);

	/** \brief Animation�� ������ ���� ��Ų��. */
	void	_Stop_Anim( void);
};

#endif
