#if !defined( __MULTICASTSYSTEM_H )
#define __MULTICASTSYSTEM_H

#include "Network/ClientP2PDef.h"

#define MULTICASTBUFFERSIZE		2048				//���� ���� ������ 

enum STAGEINFO_REQ_STATE
{
	STAGEINFO_REQ_STATE_NONE = 0,					//
	STAGEINFO_REQ_STATE_SEND,
	STAGEINFO_REQ_STATE_WAITSEND,
};

struct PACKET_INFO
{
	INT32 iPacketSize;		// Packet structure size
	INT32 iReserve;			// ���Ǵ� Size
	INT32 iMaxCount;		// network�� ������ �ִ� �ִ� count
	INT32 iDataMaxCount;	// original data count
}; 

class CCastPacket_Chara
{
public:
	CASTCHECK_USER		m_CastCheck_Chara				;
	N_PCINFO_DEF2		m_CastPacket_Chara_Def			;
	N_PCINFO_CHARA_STATE	m_CastPacket_Chara_State	;
	N_PCINFO_POSROTATION	m_CastPacket_Chara_PosRot	;
	N_PCINFO_ACTIONKEY	m_CastPacket_Chara_ActionKey	;
	N_PCINFO_ONLOADOBJECT	m_CastPacket_Chara_OnLoadObject;
	N_PCINFO_SYNCOBJECT	m_CastPacket_Chara_SyncObject	;
	N_PCINFO_RADIOCHAT	m_CastPacket_Chara_Radio		;
	N_PCINFO_WEAPON		m_CastPacket_Chara_Weapon		;
	N_PCINFO_LIFE		m_CastPacket_Chara_Life		;
	N_PCINFO_BULLET		m_CastPacket_Chara_Bullet		;
	INT32				m_CastPacket_Chara_HitCount	;		// ��Ʈ ī��Ʈ
	N_PCINFO_HIT		m_CastPacket_Chara_Hit			[MAX_FIREHIT_COUNT];
	N_PCINFO_SUFFER		m_CastPacket_Chara_Suffering	;		// �ǰ��Դϴ�.
	N_PCINFO_DEATH		m_CastPacket_Chara_Death		;
	INT32				m_CastPacket_Chara_DecentCount	;	// �ڻ� ��Ŷ�� ���� ī��Ʈ
	N_PCINFO_SUICIDEDMG	m_CastPacket_Chara_Decent		[MAX_SUICIDE_COUNT];
	N_PCINFO_MISSION	m_CastPacket_Chara_Mission		;
	N_PCINFO_DROPWEAPON m_CastPacket_Chara_DropW		;
	N_PCINFO_GETWEAPON	m_CastPacket_Chara_GetW		;

	void ResetCastPacket();
};

class CMulticastSystem : public i3::shared_ginst<CMulticastSystem>
{
	
protected:
	REAL32				m_rCheckSendTime;						// ���� Ÿ�� ����(�ʴ� 55ȸ�� ���ߴ� �뵵)
	REAL32				m_rCheckSendPingTime;					// Ping send check	
	INT32				m_nAnimObjReserveSize;					// �ִϸ��̼� ������Ʈ ���� ī��Ʈ���� ������ ������
	INT32				m_nStaticObjReserveSize;				// ���� ������Ʈ ���� ī��Ʈ���� ������ ������
	INT32				m_nMoveObjReserveSize;					// �̵� ������Ʈ ���� ī��Ʈ���� ������ ������
	INT32				m_nDynamicObjReserveSize;				// ���� ������Ʈ ���� ī��Ʈ���� ������ ������
	INT32				m_nDropedWeaponReserveSize;				// ����� ���� ���� ī��Ʈ���� ������ ������	
	UINT16				m_nSendPos;								// ��Ŷ�� ���� ������
	char 				m_SendBuffer[ MULTICASTBUFFERSIZE ];	// ��Ŷ ����

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���� ĳ���� ���� ����
	
	UINT32				m_nPreSend_CharaIdx;									// ������ ���� �ε���

	/*CCastPacket_Chara m_CastPacket_Chara[SLOT_MAX_COUNT];
	CCastPacket_Chara m_CastPacket_CharaForAI[MAX_COUNT_DOMI_DINO];*/

	CASTCHECK_USER		m_pCastCheck_Chara				[SLOT_MAX_COUNT];
	N_PCINFO_DEF2		m_pCastPacket_Chara_Def			[SLOT_MAX_COUNT];
	N_PCINFO_CHARA_STATE	m_pCastPacket_Chara_State	[SLOT_MAX_COUNT];
	N_PCINFO_POSROTATION	m_pCastPacket_Chara_PosRot	[SLOT_MAX_COUNT];
	UINT8				m_pCastPacket_Chara_ActionKeyCount[SLOT_MAX_COUNT];
	N_PCINFO_ACTIONKEY	m_pCastPacket_Chara_ActionKey	[SLOT_MAX_COUNT][MAX_ACTION_COUNT];
	N_PCINFO_ONLOADOBJECT	m_pCastPacket_Chara_OnLoadObject[SLOT_MAX_COUNT];
	N_PCINFO_SYNCOBJECT	m_pCastPacket_Chara_SyncObject	[SLOT_MAX_COUNT];
	N_PCINFO_RADIOCHAT	m_pCastPacket_Chara_Radio		[SLOT_MAX_COUNT];
	N_PCINFO_WEAPON		m_pCastPacket_Chara_Weapon		[SLOT_MAX_COUNT];
	N_PCINFO_LIFE		m_pCastPacket_Chara_Life		[SLOT_MAX_COUNT];
	N_PCINFO_BULLET		m_pCastPacket_Chara_Bullet		[SLOT_MAX_COUNT];
	INT32				m_pCastPacket_Chara_HitCount	[SLOT_MAX_COUNT];		// ��Ʈ ī��Ʈ
	N_PCINFO_HIT		m_pCastPacket_Chara_Hit			[SLOT_MAX_COUNT][MAX_FIREHIT_COUNT];
	N_PCINFO_SUFFER		m_pCastPacket_Chara_Suffering	[SLOT_MAX_COUNT];		// �ǰ��Դϴ�.
	N_PCINFO_DEATH		m_pCastPacket_Chara_Death		[SLOT_MAX_COUNT];
	INT32				m_pCastPacket_Chara_DecentCount	[SLOT_MAX_COUNT];	// �ڻ� ��Ŷ�� ���� ī��Ʈ
	N_PCINFO_SUICIDEDMG	m_pCastPacket_Chara_Decent		[SLOT_MAX_COUNT][MAX_SUICIDE_COUNT];
	N_PCINFO_MISSION	m_pCastPacket_Chara_Mission		[SLOT_MAX_COUNT];
	N_PCINFO_DROPWEAPON m_pCastPacket_Chara_DropW		[SLOT_MAX_COUNT];
	N_PCINFO_GETWEAPON	m_pCastPacket_Chara_GetW		[SLOT_MAX_COUNT];

	//��Ŷ ������ 
	PACKET_INFO			m_iPacketInfo[P2P_SUB_HEAD_COUNT]; 	
	INT32				m_pPreSend_Idx[P2P_SUB_HEAD_COUNT];//������ ���� �ε���

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���� ������Ʈ ���� ����
	// ����ź	
	bool				m_pbCastCheck_Grenade			[MAX_THROWWEAPON_COUNT];
	N_WEAPONINFO_THROW	m_pCastPacket_Grenade			[MAX_THROWWEAPON_COUNT];

	// Static Object (HP�� ����Ǵ� ������Ʈ �̵����� ����)
	struct _OBJINFO_FOR_MULTICAST 
	{
		T_OBJ		_CastCheck;
		char		_buffer[ 64];	// �ִ� 33����Ʈ
		UINT16		_writePos;

		void	Reset()
		{
			_CastCheck = 0;
			_writePos = 0;
			i3mem::FillZero( _buffer, sizeof(_buffer));
		}

		UINT16		CopyToPacket( char * pDest)
		{
			if( _writePos > 0)
				i3mem::Copy( pDest, _buffer, _writePos);
			return _writePos;
		}
	};
	_OBJINFO_FOR_MULTICAST	m_pCastPacket_Obj				[OBJ_TOTAL_COUNT];

	// Droped weapon	
	bool				m_pbCastCheck_DropedWeapon		[MAX_DROPEDWEAPON_COUNT];
	N_DROPEDWEAPONINFO	m_pCastPacket_DropedWeapon		[MAX_DROPEDWEAPON_COUNT];
	
	//NPC packet	
	bool				m_pbCastCheck_NPC				[MAX_NPC_COUNT];
	N_NPCINFO_DEF		m_pCastPacket_NPC				[MAX_NPC_COUNT];

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//������ ���������� ó�� ���� 
	// Client�� Stage info�� ��û�ϴ� ��쿡 �ʿ��� ó��	
	INT32				m_nStageInfoSendCount;
	STAGEINFO_REQ_STATE	m_nStageInfoReq;

	// �������� ���� ���忡�� ���������� ���� ������Ʈ�� �ε��� 
	INT32				m_nStageInfo_CharaPreSend;
	INT32				m_nStageInfo_MissionPreSend;

protected:
	// ���� �����͸� ���ۿ� �����մϴ�.
	void	_WriteGameInfo_Chara(			INT32 idx, GAMEINFO_CHARACTER		* pCharaInfo );
	void	_WriteGameInfo_CharaForAI(		INT32 idx, GAMEINFO_CHARACTER		* pCharaInfo );

	void	_WriteGameInfo_Grenade(			INT32 idx, GAMEINFO_THROW_WEAPON	* pGrenadeInfo);
	void	_WriteGameInfo_Object(			INT32 idx, GAMEINFO_OBJECT			* pObjectInfo);
	void	_WriteGameInfo_DropedWewapon(	INT32 idx, GAMEINFO_DROPED_WEAPON	* pDropedWeaponInfo);
	void	_WriteGameInfo_NPC(				INT32 idx, GAMEINFO_NPC				* pNPCInfo);

	//�Ϲ� ��Ŷ ������ ����� 
	void	_MergeSendData_Chara( void);
	bool	_MakePacket_Chara( UINT16 idx);
	UINT16	_MakePacket(INT32 iType, INT32 iIdx, char * pBuffer, INT32 iWritePos); 
	void	_MergeSendData( P2P_SUB_HEADTYPE type );
	
	//������ ���� ��Ŷ �����
	void	_ProcessMakeIntruderPacket( void);
	
	bool	_MakeStageInfo( void);
	bool	_MakeStageInfo_Mission( void);

	void	_ResetStageInfoBuff( void);

	void	_SendToUser( UINT8 idx );					
	void	_SendToBattle( void );

public:
	CMulticastSystem();
	virtual ~CMulticastSystem();

	void	Create( void );								//
	void	Destroy( void );							//
	void	ResetBuffer( INT32 idx);					// ������ ������ �����մϴ�.	
	void	Broadcast( REAL32 rDeltaTime);	//�ܺο��� ȣ��˴ϴ�. 	
	void	RequestIntrudeProcess( void);			//User���� �������� ������ �������� �մϴ�.	
};

#endif	// __MULTICASTSYSTEM_H
