/**
 * \file	GameUDPHandler.h
 *
 * Declares the game udp handler class.
 */


#if !defined( __GAME_UDP_HANDLER_H__)
#define __GAME_UDP_HANDLER_H__

#include "Network/ClientP2PDef.h"

class UDPPacketReceiver;
class CMulticastSystem;
class UDPPacketSender;

class CGameUDPHandler : public i3::shared_ginst<CGameUDPHandler>
{
private:
	// UDP Packet Receiver
	UDPPacketReceiver *		m_pUDPReceiver;

	// ��Ʈ��ũassist��
	NET_ASSIST_INFO			m_AssistInfo[ SLOT_MAX_COUNT ];
	NET_ASSIST_SEND			m_SendAssist[ SLOT_MAX_COUNT];

	REAL32					m_fUDPConnectTime	[ USER_SLOT_COUNT ];	// �� �޴� �ð������� Ÿ��( �� �ð��� �ʰ��ð��� ������ MS�� �˷��ش� )

	REAL32					m_rElapsedTime;

private:
	const GAMEINFO_CHARACTER *	getGameInfo_Chara( INT32 userIdx, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER) const;
	GAMEINFO_CHARACTER *	setGameInfo_Chara( INT32 userIdx, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER);
	const GAMEINFO_CHARA_NETDATA *	getNetInfo_Chara( INT32 userIdx, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER) const;
	GAMEINFO_CHARA_NETDATA *	setNetInfo_Chara( INT32 userIdx, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER);

protected:

	REAL32 	getElapsedTime( void )	const	{return m_rElapsedTime;	}	// UDPHandler������ �̿�Ǵ� ���� �� �Լ��̹Ƿ� �̰��� �ű�
	void	setElapsedTime( REAL32 fTime )	{m_rElapsedTime = fTime;	}

	void	_P2PMergeRecvData( REAL32 rDeltatime );		// ���������� ó�� 

	void	_ProcessRecvPacket_Chara_HP( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara );

	// for Client
	void	_ProcessRecvPacket_Chara_ForClient( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara );
	// for Host
	void	_ProcessRecvPacket_Chara_ForHost( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara);

	/* AI ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_AI_Respawn( GAMEINFO_AI_DINO* buf);
	void	_ProcessRecvPacket_AI_HP( INT32 idx, GAMEINFO_AI_DINO * pRecvData, GAMEINFO_CHARACTER * pCharaInfo, CGameCharaBase * pChara);
	void	_ProcessRecvPacket_AI( INT32 idx, GAMEINFO_AI_DINO* pRecvData, GAMEINFO_CHARACTER * pCharaInfo, CGameCharaBase * pChara, REAL32 rDeltatime);
	

//#ifdef DOMI_UDP_OBJECT
	void	_ProcessRecvPacket_DomiObject(GAMEINFO_DOMIOBJECT_CONT* pRecvData);
	void	_ProcessRecvPacket_DomiEvent(GAMEINFO_DOMIEVENT_CONT* pRecvData);
	void	_ProcessRecvPacket_SentryGun_TakingAim(GAMEINFO_SENTRYGUN_DEGREE* pRecvData);
//#endif
	/** \brief ����ź ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Grenade( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime);

	/** \brief Static ������Ʈ ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Obj( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime);

	/** \brief ���忡 ������ ���� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_DropedWeapon( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime);

	/** \brief ���Կ� ĳ���� ������ ó���մϴ�. */
	bool	_ProcessRecvPacket_StageInfo_Chara( INT32 idx, RECV_P2P_DATA * pRecvData);

	///** \brief ���Կ� ������Ʈ ������ ó���մϴ�. */
	//void	_ProcessRecvPacket_StageInfo_Obj( i3Object * pObject, INT32 nState, REAL32 rLocalTime);

	/** \brief ���Կ� Animation ������Ʈ ������ ó���մϴ�. */
	void	_ProcessRecvPacket_StageInfo_Obj_Anim( INT32 idx, RECV_P2P_DATA * pRecvData);

	/** \brief ���Կ� Static ������Ʈ ������ ó���մϴ�. */
	void	_ProcessRecvPacket_StageInfo_Obj_Static( INT32 idx, RECV_P2P_DATA * pRecvData);

	/** \brief ���Կ� Static ������Ʈ ������ ó���մϴ�. */
	void	_ProcessRecvPacket_StageInfo_Obj_Move( INT32 idx, RECV_P2P_DATA * pRecvData);

	/** \brief ���Կ� Dynamic ������Ʈ ������ ó���մϴ�. */
	bool	_ProcessRecvPacket_StageInfo_Obj_Dynamic( INT32 idx, RECV_P2P_DATA * pRecvData);

	/** \brief ���Կ� Mission ������ ó���մϴ�.
		\note C4 ������ �޾� C4�� ��ġ�մϴ�... */
	bool	_ProcessRecvPacket_StageInfo_Mission( RECV_P2P_DATA * pRecvData );

	/** \brief ���Կ� Controlled ������Ʈ ������ ó���մϴ�.
		\note RPG7 �ڽ��� ������ Ÿ���� ����ȭ ��ŵ�ϴ�. */
	bool	_ProcessRecvPacket_StageInfo_ControledObject( INT32 idx, RECV_P2P_DATA * pRecvData);
	

	/** \brief ������Ʈ ��� ��Ŷ�� ó���մϴ�.
		\note ������Ʈ ž�� ����� ���� ����ȭ ��Ŷ�Դϴ�. */
	void	_ProcessRecvPacket_Chara_ActionForObjectSync( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief ĳ���� Action ���� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_ActionState( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	void	_ProcessRecvPacket_Dino_FullBody(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief ĳ���� ��ġ �� ���� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_PosRotation(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara, bool bRotated = false );

	/** \brief ������Ʈ ��� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_ActionKey( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief ������Ʈ ž�� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_OnLoadObject( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief ���� ���� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_WeaponSync( GAMEINFO_CHARA_WEAPON * pRecvBuf, GAMEINFO_CHARA_STATE * pRecvCharaStateBuf, CGameCharaBase * pChara);

	/** \brief Radio chat ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_RadioChat( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief �̼� ���� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_MissionDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara );

	/** \brief �̼� ���� ��Ŷ�� ó���մϴ�. (for host) */
	void	_ProcessRecvPacket_Chara_MissionDataForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief �ڻ� ��Ŷ�� ó���մϴ�.
		\note ����, ������Ʈ�� ���� ����� ó���˴ϴ�. */
	void	_ProcessRecvPacket_Chara_Suicide( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief ���� �ݱ� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_GetWeaponForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief ���� �ݱ� ��Ŷ�� ó���մϴ�. (for host) */
	void	_ProcessRecvPacket_Chara_GetWeaponForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief ���� ������ ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_DropWeaponForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief ���� ������ ��Ŷ�� ó���մϴ�. (for host) */
	void	_ProcessRecvPacket_Chara_DropWeaponForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief ���̷�Ʈ ���� PickUp ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_DirectPickUp(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief ���� �߻� ��Ŷ�� ó���մϴ�. */
	bool	_ProcessRecvPacket_Chara_FireData( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief ���� �߻� ��Ŷ�� ó���մϴ�.
		\note ������Ʈ ž�½� */
	void	_ProcessRecvPacket_Chara_FireDataOnObject( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameObjectWithWeapon* pObj);

	/** \brief ĳ���� ��Ʈ ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_FireNHitData( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara );
	
	/** \brief ������Ʈ ��Ʈ ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_FireNHitDataOnObject( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameObjectWithWeapon* pObj);

	/** \brief ĳ���� ��� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_DeathDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara );
	
	/** \brief ĳ���� �ǰ� ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_SufferDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief ����ź ��ô ��Ŷ�� ó���մϴ�. */
	void	_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf);

	// �� ó��
	UINT8	_GetPingValue(REAL32 rPingTime); 
	void	_CalcPing_CS( REAL32 rDeltatime ); 
	void	_CalcPing_DediCation( REAL32 rDeltaTime ); 

	// UDP Ÿ�� �ƿ� ó��
	void	_CalcUDPTimeOut( REAL32 rDeltatime );

	/** Game Event�� Net���� ������ Ȯ���մϴ�. */
	void	_CheckGameDataToSend( CGameCharaBase * pChara);
	void	_CheckGameDataToSendForHost(void);
	void	_CheckGameDataToSendForAi(void);

public:
	CGameUDPHandler();
	virtual ~CGameUDPHandler();

	void	UDPConnect( bool bHost);
	void	UDPDisconnect( void);
	void	ChangeHost( bool bHost);

	void	InitRound( void);
	void	InitUserInfo( INT32 userIdx);
	void	StartBattle( INT32 userIdx);
	void	ResetRecvTime( INT32 userIdx);
	void	RespawnChara( INT32 userIdx);
	void	RequestIntrudeProcess( void);


	/** \brief ���� ���� event�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] pTargetPos ������
		\param[in] pWeapon ���� */
	void	WriteGameEvent_Fire( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon);

#ifdef DOMI_STING_POISON_GRENADE
	void	WriteGameEvent_Fire_DomiDinoGrenade( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon);
#endif

	/** \brief Ÿ�� event�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nHitWho ���� ĳ���� index
		\param[in] nHitPart ���� ����
		\param[in] rDamage ���ݷ� */
	void	WriteGameEvent_Hit( CGameCharaBase * pChara, const CHARA_DAMAGEINFO * pDamageInfo, bool bByObj);

	/** \brief ���� ������ event�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] iWeaponSlot ���� ���� ��ȣ
		\param[in] iLoadedBullet ���� źȯ ��
		\param[in] iDulaLoadedBullet Dual źâ�� ���� źȯ ��
		\param[in] iTotalBullet �� źȯ ��*/
	void	WriteGameEvent_DropWeapon( CGameCharaBase * pChara, WEAPON_SLOT_TYPE iWeaponSlot, INT32 iLoadedBullet, INT32 iDualLoadedBullet, INT32 iTotalBullet);

	/** \brief ���� �ݱ� event�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] pWeapon ���� */
	void	WriteGameEvent_PickupWeapon( CGameCharaBase * pChara, WeaponBase * pWeapon);

	/** \brief ���� �ݰ� �ٷ� ������ event�� �����մϴ�.
	\param[in] pChara ĳ���� ����
	\param[in] pDropWeapon ������ ����
	\param[in] GetItemID ���� ������ ����*/
	void	WriteGameEvent_DirectPickupWeapon(CGameCharaBase * pChara, T_ItemID GetItemID);

	/** \brief �̼� ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nState �̼� ����
		\param[in] nArea �̼� ���� */
	void	WriteGameEvent_Mission( CGameCharaBase * pChara, NET_MISSION_STATE nState, BOMB_AREA nArea, T_ItemID ItemId 
#ifdef ALTER_DOMI_INSTALL_OBJ
		, DOMI_TYPE ObjType, INT16 ObjIdx
#else
		, INT32 ObjIdx,REAL32 fTime
#endif
		);

	/** \brief �ڻ� Damge�� �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nDamage ���ݷ�
		\param[in] iByObjectIdx Object�� ���� ����� Object Index
		\note object index�� Damage�� �ִ� ��� Object Index�� �����˴ϴ�. default�� 0xFF��
		\param[in] KillerIdx ����ź�� ���� ��� ��� ���� Index. */
	void	WriteGameEvent_SuicideDamage( CGameCharaBase * pChara, CHARA_DEATH_TYPE type,
		CHARA_HIT_PART part, UINT16 nDamage, VEC3D * pDir,
		WEAPON_CLASS_TYPE WeaponClass, INT32 WeaponNum, UINT16 iByObjectIdx,
		INT32 KillerIdx, WEAPON_SLOT_TYPE WeaponUsage);

	/** \brief RadioChat ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nRadio radio chat ������ */
	void	WriteGameEvent_RadioChat( CGameCharaBase * pChara, UINT8 nRadio);

	/** \brief Object ��뿡 ���� ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nObjIdx Object Index
		\param[in] nActionKey ��� ���� */
	void	WriteGameEvent_UseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nActionKey);

	/** \brief Object ��뿡 ���� ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] state */
	void	WriteGameEvent_UsurpationObject( CGameCharaBase * pChara, UINT8 nstate);
	

	/** \brief Object Sync�� ������ �����մϴ�.
		\param[in] pChara ĳ���� ����
		\param[in] nObjIdx Object Index
		\param[in] nSyncActionKey Sync ���� key
		\note for Only Host */
	void	WriteGameEvent_UseObjectSync( CGameCharaBase * pChara, i3Object * pObj, UINT8 nSyncActionKey);

	/** \brief ĳ���� �ǰݿ� ���� ������ �����մϴ�.
		\note for Only Host
		\param[in] pChara ĳ���� ����
		\param[in] type �״� Ÿ��
		\param[in] nShooter ���� ĳ���� index
		\param[in] nHitPart ���� ����		*/
	void	WriteGameEvent_SufferingDamage( CGameCharaBase * pChara, CHARA_DEATH_TYPE type, INT32 nShooter, CHARA_HIT_PART nHitPart, bool bIsNeedDamageMotion);

	/** \brief ���忡 ������ ����ź ������ �����մϴ�.
		\note BindNetGrenadeInfo �� ȣ��Ǳ����� ���� ȣ���ϸ� �ȵ˴ϴ�.
		\param[in] idx Network index
		\param[in] pThrowWeaponInfo ���� structrue		*/
	void	WriteGameEvent_ThrowWeaponData( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo);

#ifdef DOMI_STING_POISON_GRENADE
	void	WriteGameEvent_DomiDinoThrowWeaponData( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo );
#endif

//#ifdef DOMI_UDP_OBJECT
	void	WriteGameEvent_DomiSkillObject_Install(INT32 customerIdx, INT8 objectIdx);
	void	WriteGameEvent_DomiSkillObject_Repair(INT32 customerIdx, INT8 objectIdx);
	void	WriteGameEvent_DomiSkillObject_Upgrade(INT32 customerIdx, INT8 objectIdx, INT8 lv);
	void	WriteGameEvent_DomiSkillObject_SupplyUse(INT32 customerIdx, INT8 objectIdx);
	void	WriteGameEvent_DomiDropItem_Catched(INT32 userIdx, INT8 objectIdx);
//#endif

	/** \brief ������Ʈ ������ �����մϴ�.
		\note for Only Host
		\param[in] idx network index
		\param[in] nHP HP
		\param[in] pPos ��ġ
		\param[in] pRot ȸ��		*/
	void	WriteGameEvent_ObjectData( INT32 idx, INT32 EventSender, UINT16 nHP, VEC3D * pPos, QUATERNION * pRot );


	/** \brief ���忡 ������ ���� ����
		\param[in] idx network index
		\param[in] pPos ��ġ
		\param[in] pMtxRot ȸ��*/
	void	WriteGameEvent_DropedWeaponData( INT32 idx, VEC3D * pPos, MATRIX * pMtxRot);

	/** \brief animation object ������ �����մϴ�.
		\note �ִϸ��̼� ������Ʈ�� Ű�����Ӹ� �����մϴ�.
			rBroadcastTime ���� ��ũ�� ���ߵ��� �մϴ�.
		\param[in] idx network index
		\param[in] nHP HP
		\param[in] rKeyFrame key��			*/
	void	WriteGameEvent_AnimObjectData( INT32 idx, INT32 EventSender, UINT16 nHP, REAL32 rKeyFrame);

	/** \brief Outpost ���� object ������ �����մϴ�.
		\note �ִϸ��̼� ������Ʈ�� Ű�����Ӹ� �����մϴ�.
		\param[in] idx network index
		\param[in] nPathKey Path key��
		\param[in] nHP HP
		\param[in] rKeyFrame key��
		\param[in] bSend ���� Send ����
				bSend�� false�̸� sync time���� ������ ������ �˴ϴ�.*/
	void	WriteGameEvent_AnimObjectDataForOutpost( INT32 idx, INT32 EventSender, UINT8 nPathKey, UINT16 nHP, REAL32 rKeyFrame, bool bSend);
	
	// ������ ����ź�� ��Ʈ��ũ ���ۿ� ����մϴ�.
	void	BindNetGrenadeInfo( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos);
	// ������ C5�� ��Ʈ��ũ ���ۿ� ����մϴ�.
	void	BindNetC5Info( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos);

	// ��ġ �ٿ� ����� UDP �� ���𿡰� ����
	void	WriteGameEvent_TouchDown( CGameCharaBase* pChara, UINT16 idx );


	void	P2PUpdate( REAL32 rDeltatime );			// P2P ������Ʈ	

};


#endif
