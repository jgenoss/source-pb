#if !defined( __GAME_CHARA_INPUT_CONTROL_AI_H__)
#define __GAME_CHARA_INPUT_CONTROL_AI_H__

#include "GameCharaInputControl.h"

//�ڼ� ��/����/��
//�ȱ� ��/�ȱ�/�ٱ�
//�̵� ��/��/��/��/��
//���� ��/�Ʒ�/��/��
//���� �ֶ�/�ֽ�/����/������

//�����߿� ���Ҽ� �ִ��ൿ : ����/�ɾƼ�/����/�ɾƼ� �̵�/������ ��Ż/ȸ��

//���� �ൿ: �Ÿ��� ��������, ����ź ��ô, ����/�÷��ù� ��ô

//�����߿� �߻��Ҽ��ִ� ��Ȳ : ��ó�� ����ź �߰�, �������� ����, 


#define AI_ACTION_STAND			0x0000000000000000
#define AI_ACTION_CROUCH		0x0000000000000001
#define AI_ACTION_JUMP			0x0000000000000002

#define AI_ACTION_IDLE			0x0000000000000000
#define AI_ACTION_FOOTSTEP		0x0000000000000010
#define AI_ACTION_EVASION		0x0000000000000020
#define AI_ACTION_MOVE			0x0000000000000040


#define AI_ACTION_ATTACK		0x0000000000000100
#define AI_ACTION_STOPATTACK	0x0000000000000200
#define AI_ACTION_SWAPWEAPON0	0x0000000000001000
#define AI_ACTION_SWAPWEAPON1	0x0000000000002000
#define AI_ACTION_SWAPWEAPON2	0x0000000000004000
#define AI_ACTION_SWAPWEAPON3	0x0000000000008000
#define AI_ACTION_SWAPWEAPON4	0x0000000000010000
#define AI_ACTION_SWAPWEAPON5	0x0000000000020000

#define AI_ACTION_FIRE			0x0000000001000000
#define AI_ACTION_DROPWEAPON	0x0000000002000000
#define AI_ACTION_SPECIAL		0x0000000004000000
#define AI_ACTION_RELOAD		0x0000000008000000
#define AI_ACTION_SUBFUNC		0x0000000010000000
#define AI_ACTION_EXT_ACTIVATE	0x0000000020000000
#define AI_ACTION_STOPFIRE		0x0000000040000000

#define AI_MOVE_LEFT			0x0000000000000001
#define AI_MOVE_RIGHT			0x0000000000000002
#define AI_MOVE_FORWARD			0x0000000000000004
#define AI_MOVE_BACKWARD		0x0000000000000008

#define AI_STATE_MASK_HOLDPOSITION		0x80000000					// ���� ��ġ�� �̵��� ��
#define AI_STATE_MASK_THROW_CONSIDER	0x40000000

enum AI_STATE
{
	AI_STATE_NONE				= 0,
	AI_STATE_GOPATH				= (0 | AI_STATE_MASK_THROW_CONSIDER),								//PATH�� ��� �ൿ�� �Ѱ�� ���� ����� PATH POINT�� ã�ư�
	AI_STATE_KEEPWATCH			= (1 | AI_STATE_MASK_HOLDPOSITION | AI_STATE_MASK_THROW_CONSIDER),	//���
	AI_STATE_MOVE				= (2 | AI_STATE_MASK_THROW_CONSIDER),								//�̵�
	AI_STATE_FIGHT				= 3,																//����
	AI_STATE_ESCAPE				= 4,																//���� ��ġ ��Ż
	AI_STATE_THROW				= (5 | AI_STATE_MASK_HOLDPOSITION),			// (����ź, etc...) ������ ����
	AI_STATE_REMOVE_OBJ			= (6 | AI_STATE_MASK_HOLDPOSITION),

	AI_STATE_MAX_COUNT,
};

enum AI_MOVE_RETURN_VALUE
{
	AI_MOVE_RET_OK = 0,
	AI_MOVE_RET_ARRIVED,
	AI_MOVE_RET_BLOCKED,

	AI_MOVE_RET_MAX,
};

#define		MAX_PATH_DEPTH		50
#define		NORMAL_PATH_DEPTH	20

//������->���� �н��� �̵� ->�̵��� ���� ������ ���� -> ���� �������� �ٽ� �̵�
struct TMPOBJ_INFO;

class CGameCharaInputControlAI : public CGameCharaInputControl
{
	I3_CLASS_DEFINE( CGameCharaInputControlAI, CGameCharaInputControl);

	i3GamePath *	m_pGamePath = nullptr;

	PB_AI_DATA		m_AIData;
	REAL32			m_EvasionRate[AIET_COUNT];			// [ initialize at constructor]

	//state
	AI_STATE		m_nAIState = AI_STATE::AI_STATE_NONE;
	AI_STATE		m_nOldAIState = AI_STATE::AI_STATE_NONE;

	UINT64			m_nActionState = 0;
	UINT64			m_nMoveState = 0;

	//target
	INT32			m_CurrentTargetIdx = -1;
	VEC3D			m_vTargetDir;

	I3_POINTIDX		m_idxOldTargetPoint = -1;
	I3_POINTIDX		m_idxCurTargetPoint = -1;

	//
	REAL32			m_InterpolateTime = 0.2f;
	REAL32			m_rThetaTimer = 0.0f;
	REAL32			m_rOldTheta = 0.0f;
	REAL32			m_rTargetTheta = 0.0f;

	INT32			m_WayPointCount = 0;
	I3_POINTIDX		m_CurWayPointIndex = 0;
	I3_POINTIDX		m_WayPoint[ MAX_PATH_DEPTH];		// [initialize at constructor]

	//timer
	REAL32			m_rElapsedKeepWatchTime = 0.0f;
	REAL32			m_rElapsedTraceTime = 0.0f;	//���� �н����� �󸶳� ���� �ӹ������� ���. �� ���¿��� �����
	REAL32			m_rElapsedJumpTime = 0.0f;
	REAL32			m_rAIUpdateElapsedTime = 10.0f;

	//����ź ��ô��
	REAL32			m_rAfterThrowTimer = 0.0f;

	REAL32			m_rElapsedAutoFireTime = 0.0f;	//���� Ÿ�̸�

	REAL32			m_rEvasionStateTime = 0.0f;	//ȸ�� Ÿ�̸�
	
	bool			m_bMoveFlag[2] = { false, false };

	//flag
	bool			m_bLeft = false;				//�԰��� ȸ�Ǳ������ �¿츦 �ν��ϱ����� �÷���

	i3::vector<TMPOBJ_INFO*> m_RemoveObjectList;		//�ǰ����� ������Ʈ ����Ʈ

	//
	AI_GRENADE_PATTERN	m_GrenadePattern = AI_GRENADE_RANDOM;		// ����ź ��ô ���� (�ٷ� ������ : ���� �� ������)
	REAL32				m_timeGrenadeThrow = 0.0f;
	INT32				m_SmokeProbability = 100;		// ����ź�� ��ô�ϴ� Ȯ�� (0~100)
	bool				m_bThrowSmoke = false;
	WEAPON_SLOT_TYPE	m_ThrowingWeapon = WEAPON_SLOT_UNKNOWN;

	VEC3D			m_vThrowPosition;
	bool			m_bForceEvadeSelfThrow = false;		//�ڽ��� ����ź�� �������� �־� ������ ���ϵ��� ��

	// �������
	i3::rc_wstring	m_nickname;
	INT32			m_rank = 0;

	INT32			m_primaryWeaponName = 0;
	INT32			m_secondaryWeaponName = 0;
	INT32			m_meleeWeaponName = 0;
	INT32			m_throwWeaponName = 0;
	INT32			m_itemWeaponName = 0;
	INT32			m_redCharaName = 0;	
	INT32			m_blueCharaName = 0;
	INT32			m_headCharaName = 0;
	INT32			m_beretCharaName = 0;

	INT32			m_hp = 0;
	INT32			m_respawnTime = 0;
	
	bool			m_bAttack_Death_Player = false;
	bool			m_bAttacking_Death_Player = false;

protected:
	bool			isWeaponExist( WEAPON_SLOT_TYPE type);
	WEAPON_SLOT_TYPE	_GetAvailableWeapon(void);
	bool			_SwapAvailableWeapon();

public:
	CGameCharaInputControlAI();
	virtual ~CGameCharaInputControlAI();
	
	virtual void			OnFirstUpdate( REAL32 rDeltatime) override;
	virtual void			OnUpdate( REAL32 rDeltatime) override;
	virtual void			OnLastUpdate( REAL32 rDeltatime) override;

	void					OnResetAI();
	void					BindAI(AI_BIND_DATA* pData);

	UINT64					getActionState(void)			{	return m_nActionState;}
	UINT64					getMoveState(void)				{	return m_nMoveState;}

	I3_POINTIDX				getCurTargetPoint(void)			{	return m_idxCurTargetPoint;}

	//Ž��
	INT32					FindStartPosition(void);										//���� ��ġ���� ���� ����� �̵������� pointã��
	INT32					SelectNextPath( INT32 oldPoint, INT32 curPoint);				//���� �̵��� path���
	INT32					GetClosestPoint( VEC3D * pvTargetPos);							//���� ��ġ�� ���� ����� point���ϱ�
	void					FindNextTargetPosition(void);									//���� �̵��� path�� ��� ����
	void					FindTargetPosition(I3_POINTIDX idxStart, I3_POINTIDX idxEnd, INT32 nDepth = NORMAL_PATH_DEPTH);	// ���� ��ġ���� ����2��ġ�� �̵������� ��� ã��

	INT32					GetClosestDirectionPoint( VEC3D * pvTargetPos, VEC3D * pvDirection);	//������ġ�� ����� POINT�߿� ���� ������ DIRECTION�� ������ ��ġ ã��
	INT32					GetClosestDirectionPoint( INT32 idxStart, VEC3D * pvDirection);			//������ ����Ʈ�� ������ ����Ʈ�� ���� ������ DIRECTION�� ������ ��ġ ã��

	//��ǥ�� ���ϴ� ���� ���ϱ�
	REAL32					GetAttackDirection( CGameCharaBase * pTargetChara, VEC3D * pvStart, VEC3D * pvDir, bool bForce = true);

	bool					MakeKeyInput( VEC3D * pvDir, MATRIX * pOwnerMtx);

	//�̵�
	AI_MOVE_RETURN_VALUE	MoveTo( VEC3D * vPos, REAL32 rDeltaTime);		//�̵�, �������� 0.7m�̳��� ��������� false����
	virtual bool			MoveToTarget( i3GameObj* pTarget, REAL32 rMinLength, REAL32 rDeltaSeconds);

	bool					SelectWeapon();

	bool					IsThisNegativeByWatchTime();
	AI_STATE				OnArrived_FindNext(void);

	void					SeeTargetPosition( VEC3D * pvPos, REAL32 rTime = 0.2f, bool bRho = false);			// ��� ��ġ �ٶ󺸱�
	void					SeeTargetPosition(i3::pack::VEC3D * pvPos, REAL32 rTime = 0.2f, bool bRho = false);			// ��� ��ġ �ٶ󺸱�
	void					SeeTarget( VEC3D *pvDir, REAL32 rTime = 0.2f, bool bRho = true);	// ��� ���� �ٶ󺸱�

	void					OnInterpolateTheta( REAL32 rDeltaSeconds);	// �ɸ��� theta interpolate

	//Ž��
	INT32					FindTarget();																	//���� ����( �ּ� 20m) �����Ÿ� ���� �� Ž��

	//���� ��� ����
	bool					FightWidth( INT32 idx);

	//������ ����
	void					SelectFightPose();							// ������ �ൿ���� ����
	void					ActivateExtension( bool bActivate);
	REAL32					GetTargetRate(void);
	
	//������ �̵�
	void					MoveOnFight( REAL32 rDistance, REAL32 rDeltaSeconds);
	void					ProcessEvasion( REAL32 rDeltaSeconds);

	// ������ ����
	void					Attack( INT32 idx, REAL32 rDeltaSeconds);			// �ɸ��� �ε����� ��������
	void					Attack( VEC3D * pVDir, REAL32 rDeltaTime);			// ��� �������� ����
	void					Attack( i3GameObj * pTargetObj, REAL32 rDeltaTime);	//��� object�� �ٶ󺸰� ����

	//
	bool					DestroyIIObject( REAL32 rDeltaSeconds);
	INT32					_CheckObjectCollision( i3::vector<TMPOBJ_INFO*>& List, REAL32 rRange, REAL32 rDeltaSeconds);
	void					OnRemoveObject( REAL32 rDeltaSeconds);
	//���� ����
	void					StopAttack( void);
	AI_STATE				AfterFight(void);



	// ����ź ��ô ����
	REAL32					CalcRhoForThrow( const VEC3D * pTargetPos, VEC3D * pLookPos);
	void					ProcessGrenadeForBusyPattern( REAL32 tm, WEAPON_SLOT_TYPE type);
	bool					ProcessGrenadeForLazyPattern( VEC3D * pLastPos);

	void					OnHit( INT32 idx);

	INT32					IsThrowWasSafeZone(INT32 * pIdx);			//�ڽ��� ���� ����ź�� �ڽſ��� ������ ��ġ�� ���������� üũ
	void					EvadeSelfThrow( REAL32 rDeltaTime);	//�ڽ��� ���� ����ź�� ���� �ʵ��� ����

	///////////////////////////////////////////////////////////////////////
	//AI STATE
	void	OnGoPath(REAL32 rDeltaSeconds, bool bFirst);

	void	OnKeepWatch(REAL32 rDeltaSeconds, bool bFirst);

	void	OnMove(REAL32 rDeltaSeconds, bool bFirst);

	void	OnFight( REAL32 rDeltaSeconds, bool bFirst);

	void	OnEscape( REAL32 rDeltaSeconds, bool bFirst);

	void	OnThrow( REAL32 tm, bool bFirst);

	///////////////////////////////////////////////////////////////////////
	//Processing
	void	SetMoveDir( VEC3D * pVecOut);
	// ���� ������ / �ݱ� ó��
	void	ProcessDropWeapon( REAL32 rDeltatime);
	// �ɱ� / �Ͼ�� ó�� ( return BodyChangeflag)
	bool	ProcessCrouch( REAL32 rDeltatime);
	// �̵� ó��
	void	ProcessMovement( REAL32 rDeltatime, bool bChangeBody);

	// ���� ó��
	void	ProcessWeapon( REAL32 rDeltatime);
	void	ProcessDinoAttack( REAL32 rDeltatime);

	// Extension
	void	ProcessExtension( void);

	AI_STATE	ProcessAI( REAL32 rDeltatime);
	void		ProcessInput( REAL32 rDeltatime);


	// �������
	const i3::rc_wstring& getNickname(void) const	{ return m_nickname;	}
	INT32		getRank(void)					{ return m_rank;	}
	INT32		getPrimaryWeaponName(void)		{ return m_primaryWeaponName;	}
	INT32		getSecondaryWeaponName(void)	{ return m_secondaryWeaponName;	}
	INT32		getMeleeWeaponName(void)		{ return m_meleeWeaponName;	}
	INT32		getThrowWeaponName(void)		{ return m_throwWeaponName;	}
	INT32		getItemWeaponName(void)			{ return m_itemWeaponName;	}
	INT32		getRedCharaName(void)			{ return m_redCharaName;	}
	INT32		getBlueCharaName(void)			{ return m_blueCharaName;	}
	INT32		getHeadCharaName(void)			{ return m_headCharaName;	}
	INT32		getBeretCharaName(void)			{ return m_beretCharaName;	}
	INT32		getHp(void)						{ return m_hp;				}
	INT32		getRespawnTime(void)			{ return m_respawnTime;		}
};

#endif
