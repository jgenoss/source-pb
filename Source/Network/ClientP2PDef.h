#pragma once

#include "../../../Common/CommonSource/GameP2PDef.h"

struct GAMEINFO_CHARACTER
{
	bool				_bRecvFlag = false;					// �޾Ҵٸ� ���ú� üũ
	bool				_bSendFlag = false;					// ���� üũ (true�̸� �����ϴ�)
	bool				_bPosSendCheck = false;				// �̵����� ���ߴٸ� üũ�մϴ�. ������ �ʾҴٸ� �ּ� ��ũŸ�Ӹ�ŭ�� �����ϴ�.
	bool				_bReqStageInfo = false;				// only host (�����ϸ� �����ִ� �������� ���� üũ��)
	bool				_bPosStopSendCheck = false;			// �����ÿ� ����մϴ�.(only local)

	UINT8				_iPing = 0;						// 1�̸� �� ��Ŷ���� ó��
	REAL32				_rPingSendTime = 0.0f;

	REAL32				_rCharaStateSendTime = 0.0f;
	REAL32				_rWeaponSyncSendTime = 0.0f;
	REAL32				_rCharaPosSendTime = 0.0f;
	REAL32				_rMissionSyncSendTime = 0.0f;
	REAL32				_rLifePacketSendTime = 0.0f;		// for host
	REAL32				_rObjectSyncSendTime = 0.0f;		// for host

	REAL32				_rDomiObjectSendTime = 0.0f;
	UINT8				_iDomiObjectSendCnt = 0;
	UINT32				_iDomiObjectActivateFlag = 0;

	REAL32				_rSendTime = 0.0f;

	// ��Ʈ��ũ ó�� ����
	GAMEINFO_CHARA_NETDATA	_tNetData;
	INT32				_MaxThrowingIndex = 0;

	void	Reset()
	{
		_bRecvFlag				= false;
		_bSendFlag				= false;
		_bPosSendCheck			= false;
		_bReqStageInfo			= false;
		_bPosStopSendCheck		= false;
		_iPing					= 0;
		_tNetData.Reset();
		_rCharaStateSendTime	= 0.f;
		_rWeaponSyncSendTime	= 0.f;
		_rCharaPosSendTime		= 0.f;
		_rMissionSyncSendTime	= 0.f;
		_rLifePacketSendTime	= 0.f;
		_rObjectSyncSendTime	= 0.f;
		_rSendTime				= 0.f;
		_MaxThrowingIndex		= -1;
		_rDomiObjectSendTime = 0.0f;
		_iDomiObjectSendCnt = 0;
	}

	void	ProcessHostLost()
	{
		_bRecvFlag				= false;
		_bSendFlag				= false;
		_bReqStageInfo			= false;
		_tNetData._iInfoFlag	= 0;
	}

	void	ResetSendCount()
	{	
		_bSendFlag				= false;
		_tNetData._iInfoFlag	= 0;
		_tNetData._tCharaState.setFireAction(0);
		_tNetData._tActionKey.Reset();
		_tNetData._tRadioChat.setRadio(0);
		_tNetData._tFireData._tHitData.setHitCount( 0);
		_iPing											= 0;
		_tNetData._tSuicideDamageData.setCount( 0);
		_tNetData._tDropWeaponData.setSuccess(0);
		_tNetData._tGetWeaponData.setSuccess(0);

		_rSendTime										= 0.f;
	}

	void	ResetSendCountForHost()
	{
		_tNetData._tSufferData.setDeath( false);
		_tNetData._tSufferData.setDeathType( CHARA_DEATH_UN);
		_tNetData._tGetWeaponData.setSuccess(0);
		_tNetData._tDropWeaponData.setSuccess(0);
	}
} ;

class GAMEINFO_AI_DINOs_NETDATA : public GameNetworkInfoBase
{
public:
	INT32 i32Mask;
	GAMEINFO_AI_DINO		m_AI_Dino_Infos[ MAX_COUNT_DOMI_DINO];
	i3::vector<GAMEINFO_AI_DINO*> list;

	GAMEINFO_AI_DINOs_NETDATA() { Reset(); }

	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest)  override { I3ASSERT_0;};

	// �������� ������ �ʿ��ϴٸ� ���� �����ؾ� �մϴ�.
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;
};

struct GAMEINFO_DOMI_EVENT : i3::class_common_pool<GAMEINFO_DOMI_EVENT>
{
	INT8 evtType;
};
struct GAMEINFO_SENTRY_DUMMY_REPAIR : GAMEINFO_DOMI_EVENT
{
	INT8 idx;
};
struct GAMEINFO_SENTRYGUN_FIRE : GAMEINFO_DOMI_EVENT
{
	VEC3D	target;
	INT8	idx;
};
struct GAMEINFO_SUPPLY_USE : GAMEINFO_DOMI_EVENT
{
	INT8 customerIdx;
	INT8 idx;
};

struct GAMEINFO_DROPITEM_DROPPED : GAMEINFO_DOMI_EVENT
{
	VEC3D pos;
	INT8 type;
	INT8 idx;
};
struct GAMEINFO_DROPITEM_CATCHED : GAMEINFO_DOMI_EVENT
{
	INT8 idx;
	INT8 userIdx;
};
struct GAMEINFO_SP_SCORE : GAMEINFO_DOMI_EVENT
{
	INT32 totalScore;
	UINT16 totalSP;
	INT16 totalKill;
	INT8  userIdx;
};
struct GAMEINFO_DINO_DEATH : GAMEINFO_DOMI_EVENT
{
	INT16 sp[4];
	INT16 score[4];
	INT8  killerIdx;
	INT8  combo;
	bool  bHeadShot;
};
struct GAMEINFO_MUTANTTREX_DASH : GAMEINFO_DOMI_EVENT
{
	INT8 dash_info;
};
struct GAMEINFO_MUTANTTREX_HOWL : GAMEINFO_DOMI_EVENT
{
	INT8 mutantIdx;
	INT16 threshold; // �Ͽ︵ ��� �Ӱ�ġ(HP)
};

struct GAMEINFO_SPECIAL_EFFECT : GAMEINFO_DOMI_EVENT
{
	PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK data;
};

struct GAMEINFO_SKILL_OBJ
{
	INT16	hp;
	INT8	idx;
	INT8	installerIdx;
	INT8	lv;
};

struct GAMEINFO_DOMI_DROPITEM
{
	VEC3D	pos;
	INT8	type;
	bool	dropped;
	INT8	idx;
};

class GAMEINFO_DOMIOBJECT_CONT : public GameNetworkInfoBase
{
protected:
	GAMEINFO_SKILL_OBJ				skillObj;
	GAMEINFO_DOMI_DROPITEM			dropItem;

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest)  override {}
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize )  override { I3ASSERT_0; return 0;}
	virtual UINT16	Unpack( const char * pInPacket) override;

	const GAMEINFO_SKILL_OBJ * getSkillObjectInfo( void)			{ return &skillObj; }
	const GAMEINFO_DOMI_DROPITEM * getDropItemInfo( void)			{ return &dropItem; }
};

class GAMEINFO_DOMIEVENT_CONT : public GameNetworkInfoBase
{
public:

	i3::vector<GAMEINFO_DOMI_EVENT*> events;

	GAMEINFO_DOMIEVENT_CONT() {}
	~GAMEINFO_DOMIEVENT_CONT()
	{
		Clear();
	}
	void Clear()
	{
		for (size_t i=0; i<events.size(); i++)
		{
			I3_SAFE_DELETE( events[i]);
		}
		events.clear();
	}

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override {}
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override { I3ASSERT_0; return 0;}
	virtual UINT16	Unpack( const char * pInPacket) override;
};
//#endif

class GAMEINFO_SENTRYGUN_DEGREE : public GameNetworkInfoBase
{
public:
	INT8 idx;
	REAL32 theta;

	virtual void	Reset() override { idx = -1; theta = 0.f; }
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override {}
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override { I3ASSERT_0; return 0; }
	virtual UINT16	Unpack( const char * pInPacket) override;
};

//��� �÷��� Ȯ���մϴ�. ����
struct BROADCASTINFO
{
	REAL32						_rDeltatime;
	GAMEINFO_CHARACTER *		_pCharaInfoList;
	GAMEINFO_CHARACTER *		_pCharaInfoForAI_List;
	GAMEINFO_THROW_WEAPON *		_pGrenadeInfoList;
	GAMEINFO_OBJECT *			_pObjectInfoList;
	GAMEINFO_DROPED_WEAPON *	_pDropedWeaponInfoList;
    GAMEINFO_NPC *				_pNPCInfoList;
	INT32						_iTotalObjectCount;			// ������ ��ü ������Ʈ ��
	UINT8						_iRoundNum;					// ���� ���� �� 2009.06.09 �߰� dfly79
};

///////////////////////////////////////////////////////////////////////////////////////////
// ��ũ ������ �������Դϴ�. ( Recv���� ��� )
struct NET_ASSIST_INFO
{
	// ���������̼ǿ�
	REAL32	_fRecvPosTime;			// �̵��� ���� Recv Ÿ��
									// ( Recv �������κ��� _vRecvPosition���� ���������̼��ϱ� ���� Ÿ�̸�, Recv�ϸ� �ʱ�ȭ )
	VEC3D	_vPostPosition;			// Recv������ ���� ��ġ( �� ��ġ�� �������� ���� ��Ŷ�� ���������� ���������̼��Ѵ�. )
	VEC3D	_vRecvPosition;			// Recv�� ���� ������( �� ��ġ�� �̵��ϰ� �˴ϴ� )
	REAL32	_fLength;				// ���� ��ġ�� ���� ������ ��ġ�� ���� �Ÿ�

	UINT32	_uThrowCount;	// ��ŷ ���� count
	
	REAL32	_fRecvThetaRhoTime;		//
	VEC3D	_vPostDir;				// FirePacket���� Recv������ ���� ���� ( Theta, Rho�� ���ϴµ� ���˴ϴ�. )
	VEC3D	_vRecvDir;				// FirePacket���� Recv�� Ÿ�� ������ ( Theta, Rho�� ���ϴµ� ���˴ϴ�. )
	 
	REAL32	_fPostTheta, _fRecvTheta;// SyncPacket������ Theta
	REAL32	_fPostRho,	_fRecvRho;	//	Rho

	UINT32	_nPrvMoveKey;

	UINT8	_nMissionLoaded;
	VEC3D	_vMissionLoadPos;		// Mission Object install position // only intruder

	// ��ô ������ only client
	GRENADE_THROW_STATE		_nThrowState;		// Send���� ���˴ϴ�. ���� ����ź�� ����(CN)
	WEAPON_SLOT_TYPE		_nThrowSlotIdx;		// Send���� ���˴ϴ�. Slot Index(CN)
	VEC3D					_vThrowDir;

	INT32	_iSendGrenadeCount;

	void	Reset()
	{
		_fRecvPosTime		= 0.f;
		_fLength			= 0.f;
		_uThrowCount		= 0xFFFFF0FF & i3Math::Rand();
		_fRecvThetaRhoTime	= 0.f;
		_fPostTheta			= 0.f;
		_fRecvTheta			= 0.f;
		_fPostRho			= 0.f;
		_fRecvRho			= 0.f;
		_nPrvMoveKey		= 0;
		_nMissionLoaded		= 0;
		_nThrowState		= GRENADE_THROW_NONE;
		_nThrowSlotIdx		= WEAPON_SLOT_PRIMARY;
		i3Vector::Zero( &_vPostPosition );
		i3Vector::Zero( &_vRecvPosition );
		i3Vector::Zero( &_vPostDir );
		i3Vector::Zero( &_vRecvDir );
		i3Vector::Zero( &_vMissionLoadPos );
		_iSendGrenadeCount = 0;
	}

} ;

struct NET_ASSIST_SEND	// Send�� ���۸��� �ʿ��� ��� ���
{
	UINT8	_SendCountForHitByObj;		// ���� Ƚ��
	i3::vector<CHARA_DAMAGEINFO*>		_CharaDamageInfoList;
};