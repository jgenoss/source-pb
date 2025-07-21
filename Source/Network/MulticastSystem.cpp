#include "pch.h"
#include "MulticastSystem.h"
#include "../GlobalVariables.h"
#include "ClientUDPSocket.h"
#include "../StageObject/ObjectWithWeapon.h"
#include "../StageObject/ObjectRidingHelicopter.h"
#include "../StageObject/ObjectRespawnHelicopter.h"
#include "../StageObject/ObjectWeaponBox.h"

#include "../InBattleDef.h"

#include "../StageBattle/UserContext.h"
#include "../StageBattle/NetworkContext.h"

#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "UserInfoContext.h"
#include "../StageBattle/ScoreContext.h"
#include "GameNetworkManager.h"
#include "GameMissionManager.h"
#include "../BattleSyncTimer.h"
#include "../Weapon/WeaponItemID.h"

#include "ClientStageInfoUtil.h"

//////////////////////////////////////////////////////////////////////////////////
//								Define											//
//////////////////////////////////////////////////////////////////////////////////
#if defined (LOCALE_KOREA)
	#define UDPMULTICASTTIME			0.0333f		// �ʴ� 15ȸ(CS) --> �ʴ� 30ȸ
#else
	#define UDPMULTICASTTIME			0.0666f//0.0416f //0.018f		// �ʴ� 55ȸ(CS) --> �ʴ� 15ȸ
#endif

#define UDPPINGSENDTIME				1.f

// �� ��Ŷ�� �ƽ������� ���� �� �ִ� ������Ʈ ī��Ʈ�Դϴ�.
#define UDPSEND_GRENADEOBJ_COUNT	6			// (Head(2) + 8) * 6 bytes	= 60
#define UDPSEND_C5OBJ_COUNT			2			// (Head(2) + 14) * 2 bytes = 32	// 92
#define UDPSEND_ANIMOBJ_COUNT		1			// (Head(2) + 7) * 1 bytes	= 9		// 101
#define UDPSEND_STATICOBJ_COUNT		4			// (Head(2) + 2) * 4 bytes	= 16	// 117
#define UDPSEND_MOVEOBJ_COUNT		1			// (Head(2) + 8) * 1 bytes	= 10	// 127
#define UDPSEND_DYNAMICOBJ_COUNT	2			// (Head(2) + 16) * 2 bytes	= 36	// 163
#define UDPSEND_WEAPONOBJ_COUNT		4			// (Head(2) + 16) * 4 bytes	= 72	// 235
#define UDPSEND_NPC_COUNT			2			// (Head(2) + 8) * 2 bytes	= 20	// 255

// CharaInfo 250 +
//   Object Info 255	= 505	// �� ��Ŷ���� ���� �� �ִ� �ƽ� ����� �˴ϴ�.
// �� ��Ŷ���� �Ʒ��� �뷮�� �ʰ����� �ʾҴٸ� �׸�ŭ�� ������ ����ϴ�. �̸� ���� �������Դϴ�.
#define PACKET_RESERVESIZE_CHARACTER		250

//////////////////////////////////////////////////////////////////////////////////

void CCastPacket_Chara::ResetCastPacket()
{
	m_CastCheck_Chara.Reset();
	m_CastPacket_Chara_Def.Reset();
	m_CastPacket_Chara_State.Reset();
	m_CastPacket_Chara_PosRot.Reset();
	m_CastPacket_Chara_ActionKey.Reset();
	m_CastPacket_Chara_OnLoadObject.Reset();
	m_CastPacket_Chara_SyncObject.Reset();
	m_CastPacket_Chara_Radio.Reset();
	m_CastPacket_Chara_Weapon.Reset();
	m_CastPacket_Chara_Life.Reset();
	m_CastPacket_Chara_Bullet.Reset();
	m_CastPacket_Chara_HitCount = 0;
	for(INT32 i = 0; i < MAX_FIREHIT_COUNT; i++)
	{
		m_CastPacket_Chara_Hit[ i].Reset();
	}
	m_CastPacket_Chara_Suffering.Reset();
	m_CastPacket_Chara_Death.Reset();
	m_CastPacket_Chara_DecentCount = 0;
	for(INT32 i = 0; i < MAX_SUICIDE_COUNT; i++)
	{
		m_CastPacket_Chara_Decent[ i].Reset();
	}
	m_CastPacket_Chara_Mission.Reset();
	m_CastPacket_Chara_DropW.Reset();
	m_CastPacket_Chara_GetW.Reset();
}

CMulticastSystem::CMulticastSystem()
{
	//�����ϱ����� �輺�鿡�� �̾߱� �ϼ���.
	//���ð��� ���ϰ� ����ϱ� ���ؼ� ��� ������ �ֽ��ϴ�. 

	m_iPacketInfo[P2P_SUB_HEAD_GRENADE].iDataMaxCount		= MAX_THROWWEAPON_COUNT;
	m_iPacketInfo[P2P_SUB_HEAD_OBJECT].iDataMaxCount		= OBJ_TOTAL_COUNT;
	m_iPacketInfo[P2P_SUB_HEAD_DROPEDWEAPON].iDataMaxCount	= MAX_DROPEDWEAPON_COUNT;
	m_iPacketInfo[P2P_SUB_HEAD_NPC].iDataMaxCount			= MAX_NPC_COUNT;
	
	m_iPacketInfo[P2P_SUB_HEAD_GRENADE].iMaxCount			= UDPSEND_GRENADEOBJ_COUNT;
	m_iPacketInfo[P2P_SUB_HEAD_DROPEDWEAPON].iMaxCount		= UDPSEND_WEAPONOBJ_COUNT;
	m_iPacketInfo[P2P_SUB_HEAD_NPC].iMaxCount				= UDPSEND_NPC_COUNT;

	m_iPacketInfo[P2P_SUB_HEAD_GRENADE].iPacketSize			= sizeof(N_GAME_SUBHEAD) + sizeof(N_WEAPONINFO_THROW);
	m_iPacketInfo[P2P_SUB_HEAD_OBJECT].iPacketSize			= sizeof(N_GAME_SUBHEAD) + sizeof(N_OBJINFO_STATE) + sizeof( N_OBJINFO_HP) + sizeof(N_OBJINFO_EVENTSENDER) + sizeof(N_POS)
			+ sizeof(N_ROT) + sizeof(N_OBJINFO_ANIMPATH) + sizeof(N_OBJINFO_ANIMKEY) + sizeof(N_OBJINFO_SHELLCOUNT) + sizeof(N_OBJINFO_RESPAWN) + sizeof(N_OBJINFO_OWNER_INDEX);
	m_iPacketInfo[P2P_SUB_HEAD_DROPEDWEAPON].iPacketSize	= sizeof(N_GAME_SUBHEAD) + sizeof(N_DROPEDWEAPONINFO);
	m_iPacketInfo[P2P_SUB_HEAD_NPC].iPacketSize				= sizeof(N_GAME_SUBHEAD) + sizeof(N_NPCINFO_DEF);

	m_iPacketInfo[P2P_SUB_HEAD_USER].iReserve				= PACKET_RESERVESIZE_CHARACTER; 
	m_iPacketInfo[P2P_SUB_HEAD_GRENADE].iReserve			= m_iPacketInfo[P2P_SUB_HEAD_USER].iReserve		+ ((m_iPacketInfo[P2P_SUB_HEAD_GRENADE].iPacketSize			) * m_iPacketInfo[P2P_SUB_HEAD_GRENADE].iMaxCount);
	m_iPacketInfo[P2P_SUB_HEAD_DROPEDWEAPON].iReserve		= m_iPacketInfo[P2P_SUB_HEAD_OBJECT].iReserve	+ ((m_iPacketInfo[P2P_SUB_HEAD_DROPEDWEAPON].iPacketSize	) * m_iPacketInfo[P2P_SUB_HEAD_DROPEDWEAPON].iMaxCount);
	m_iPacketInfo[P2P_SUB_HEAD_NPC].iReserve				= m_iPacketInfo[P2P_SUB_HEAD_DROPEDWEAPON].iReserve		+ ((m_iPacketInfo[P2P_SUB_HEAD_NPC].iPacketSize				) * m_iPacketInfo[P2P_SUB_HEAD_NPC].iMaxCount);
}

CMulticastSystem::~CMulticastSystem()
{

}

void CMulticastSystem::Create( void )
{	
	INT32 i, j;
	m_nAnimObjReserveSize		= 0;
	m_nStaticObjReserveSize		= 0;
	m_nMoveObjReserveSize		= 0;
	m_nDynamicObjReserveSize	= 0;
	m_nDropedWeaponReserveSize	= 0;

	m_rCheckSendTime			= 0.f;
	m_rCheckSendPingTime		= 0.f;
	m_nSendPos					= 0;

	i3mem::FillZero( m_SendBuffer,		sizeof(m_SendBuffer));
	i3mem::FillZero( m_pPreSend_Idx,	sizeof(m_pPreSend_Idx));	

	// ĳ���� ����
	m_nPreSend_CharaIdx		= 0;
	for( i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_pCastCheck_Chara[ i ].Reset();
		m_pCastPacket_Chara_Def[ i ].Reset();
		m_pCastPacket_Chara_State[ i ].Reset();
		m_pCastPacket_Chara_PosRot[ i ].Reset();
		m_pCastPacket_Chara_ActionKeyCount[i] = 0;
		for (j = 0; j < MAX_ACTION_COUNT; j++)
		{
			m_pCastPacket_Chara_ActionKey[i][j].Reset();
		}
		m_pCastPacket_Chara_OnLoadObject[ i ].Reset();
		m_pCastPacket_Chara_SyncObject[ i ].Reset();
		m_pCastPacket_Chara_Radio[ i ].Reset();
		m_pCastPacket_Chara_Weapon[ i].Reset();
		m_pCastPacket_Chara_Life[ i].Reset();
		m_pCastPacket_Chara_Bullet[ i ].Reset();
		m_pCastPacket_Chara_HitCount[ i ] = 0;
		for( j = 0; j < MAX_FIREHIT_COUNT; j++)
		{
			m_pCastPacket_Chara_Hit[ i][ j].Reset();
		}
		m_pCastPacket_Chara_Suffering[ i ].Reset();
		m_pCastPacket_Chara_Death[ i ].Reset();
		m_pCastPacket_Chara_DecentCount[ i] = 0;
		for( j = 0; j < MAX_SUICIDE_COUNT; j++)
		{
			m_pCastPacket_Chara_Decent[ i][ j].Reset();
		}
		m_pCastPacket_Chara_Mission[ i].Reset();
		m_pCastPacket_Chara_DropW[ i].Reset();
		m_pCastPacket_Chara_GetW[ i].Reset();
	}

	for( i = 0; i < MAX_THROWWEAPON_COUNT; i++ )
	{
		m_pbCastCheck_Grenade[ i ] = false;
		m_pCastPacket_Grenade[ i ].Reset();
	}

	for( i = 0; i < MAX_DROPEDWEAPON_COUNT; i++ )
	{
		m_pbCastCheck_DropedWeapon[ i ] = false;
		m_pCastPacket_DropedWeapon[ i ].Reset();
	}

	for( i = 0; i < OBJ_TOTAL_COUNT; i++ )
	{
		m_pCastPacket_Obj[ i ].Reset();
	}
	
	for( i = 0; i < MAX_NPC_COUNT; i++)
	{
		m_pbCastCheck_NPC[ i] = false;
		m_pCastPacket_NPC[ i].Reset();
	}	

	_ResetStageInfoBuff();
}

void CMulticastSystem::Destroy( void )
{

}

void CMulticastSystem::ResetBuffer( INT32 idx)
{
	m_pCastCheck_Chara[ idx].bCheckBroadcast= false;
	m_pCastCheck_Chara[ idx].nP2PInfoFlag	= 0x00;
}

void CMulticastSystem::Broadcast( REAL32 rDeltaTime )
{
	INT32 i;

	const GAMEINFO_CHARACTER * _pCharaInfoList = BattleSlotContext::i()->getGameInfo_Chara(0, CHARA::BUFFER_USER); 
	const GAMEINFO_THROW_WEAPON * _pGrenadeInfoList = BattleObjContext::i()->getGameInfo_ThrowWeapon(0, 0);
	const GAMEINFO_OBJECT *		_pObjectInfoList = BattleObjContext::i()->getGameInfo_Object(0);
	const GAMEINFO_DROPED_WEAPON *	_pDropedWeaponInfoList =  BattleObjContext::i()->getGameInfo_DropedWeapon(0);
	INT32 _iTotalObjectCount = BattleObjContext::i()->getObjectCountInfo()->m_pTotalObjectList.size();	// ������ ��ü ������Ʈ ��

	UINT8 _iRoundNum = ScoreContext::i()->getCurrentRound( BattleServerContext::i()->IsUseDedicationMode());

	///////////////////////////////////////////////////////////////////////////////
	//1. 1�� ��Ŷ ���۸� ������. ���⿡�� ���� ��Ŷ�� ���� �����ϰ� �ֽ��ϴ�. 
	//ĳ����
	{
		//ĳ���� 
		for( i = 0; i < SLOT_MAX_COUNT; i++)
			_WriteGameInfo_Chara( i, const_cast<GAMEINFO_CHARACTER*>(&_pCharaInfoList[ i]) );
		//����ź 
		for( i = 0; i < MAX_THROWWEAPON_COUNT; i++)
		{
			if( _pGrenadeInfoList[ i]._pWeaponBase != nullptr)
			{
				_WriteGameInfo_Grenade( i, const_cast<GAMEINFO_THROW_WEAPON*>(&_pGrenadeInfoList[ i]) );
			}
		}	
		//������Ʈ
		for( i = 0; i < _iTotalObjectCount; i++)
			_WriteGameInfo_Object( i, const_cast<GAMEINFO_OBJECT*>(&_pObjectInfoList[ i]) );
		//����� ����
		for( i = 0; i < g_pWeaponManager->GetDropListCount(); i++)
		{
			WeaponBase * pWeapon = g_pWeaponManager->GetDropWeapon( i );
			_WriteGameInfo_DropedWewapon( pWeapon->getNetworkIdx(), const_cast<GAMEINFO_DROPED_WEAPON*>(&_pDropedWeaponInfoList[ pWeapon->getNetworkIdx()]) );
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	//2.  Broadcast�� �մϴ�.
	bool bPingSend = false;
	m_rCheckSendTime += rDeltaTime; 
	if( m_rCheckSendTime > UDPMULTICASTTIME)	//��Ŷ�� ������ �ִ����� üũ�մϴ�. 
	{		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//1. �����͸� ���� ���ۿ� ī��

		// �ش��� �ؿ��� �����մϴ�.
		m_nSendPos = sizeof( N_GAME_HEADER );

		// Ping
		m_rCheckSendPingTime += m_rCheckSendTime;
		if( m_rCheckSendPingTime > UDPPINGSENDTIME )
		{
			REAL32 * pTime = (REAL32*) &m_SendBuffer[ m_nSendPos];
			*pTime = BattleSyncTimer::i()->GetTimeElapsed();
			m_nSendPos += sizeof( REAL32);

			bPingSend = true;
		}

		//�켱���� ���� �ͺ��� ���մϴ�. 
		_MergeSendData_Chara();
		_MergeSendData( P2P_SUB_HEAD_GRENADE );
		_MergeSendData( P2P_SUB_HEAD_OBJECT );
		_MergeSendData( P2P_SUB_HEAD_DROPEDWEAPON );
		_MergeSendData( P2P_SUB_HEAD_NPC );

		// ������ ������ ��Ŷ
		_ProcessMakeIntruderPacket();

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//2. Send
		if( m_nSendPos > sizeof( N_GAME_HEADER ) + sizeof( REAL32) )
		{
			// Host �ش��� �����մϴ�.
			I3_BOUNDCHK( BattleSlotContext::i()->getMySlotIdx(), SLOT_MAX_COUNT);
			UINT8 ui8MySlotIdx = (UINT8) BattleSlotContext::i()->getMySlotIdx();

			UINT8 ui8MyRespawnCnt = BattleSlotContext::i()->getNetCharaInfo(ui8MySlotIdx)->getMyRespawnCount();
			I3ASSERT_RETURN( ui8MyRespawnCnt < 255);

			REAL32 timeElapsed = BattleSyncTimer::i()->GetTimeElapsed();

			UDP_UTIL::BuildHead( &m_SendBuffer[ 0], UDPPROTOCOL_CS_INFO, ui8MySlotIdx,
				timeElapsed, _iRoundNum, m_nSendPos, bPingSend, 0, ui8MyRespawnCnt, (UINT8)UserInfoContext::i()->GetMyTouchDownCnt());

			if( bPingSend)
			{
				BattleServerContext::i()->SetPingInfo()->m_rPingSendTime = timeElapsed;
				m_rCheckSendPingTime = 0.f;
			}

			if( BattleServerContext::i()->IsUseBattleServer() )
			{	
				_SendToBattle( ); 
			}
		}
		m_rCheckSendTime = 0.f;
	}

	return; 
}

void CMulticastSystem::_WriteGameInfo_CharaForAI( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo )
{
	INT32 i;

	//1. Send�� �ʿ䰡 �ִ��� �˻��մϴ�.
	if( !pCharaInfo->_bSendFlag )
		return;

	m_pCastCheck_Chara[ idx].bCheckBroadcast = true;

	//2. Send�ؾ��� �����͸� �����մϴ�. 
	// default data	
	//���� ��Ŷ �ε� ���� �߿��մϴ�.
	UINT32	iInfoFlag = pCharaInfo->_tNetData._iInfoFlag;

	if( iInfoFlag & P2P_INFO_FLAG_CHARASTATE )
	{
		pCharaInfo->_tNetData._tCharaState.Pack( (char*)&m_pCastPacket_Chara_State[ idx], sizeof(N_PCINFO_CHARA_STATE) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_CHARASTATE;
	}

	if( iInfoFlag & P2P_INFO_FLAG_CHARAPOS )
	{
		pCharaInfo->_tNetData._tPosRotData.Pack( (char*) &m_pCastPacket_Chara_PosRot[ idx], sizeof(N_PCINFO_POSROTATION) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_CHARAPOS;
	}

	if( iInfoFlag & P2P_INFO_FLAG_ACTIONKEY )
	{
		pCharaInfo->_tNetData._tActionKey.Pack( &m_pCastPacket_Chara_ActionKeyCount[idx], (char*)m_pCastPacket_Chara_ActionKey[ idx], sizeof(N_PCINFO_ACTIONKEY) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_ACTIONKEY;
	}

	if( iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT )
	{
		pCharaInfo->_tNetData._tTakingObject.Pack( (char*) &m_pCastPacket_Chara_OnLoadObject[ idx], sizeof(N_PCINFO_ONLOADOBJECT) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_ONLOADOBJECT;
	}

	if( iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE )
	{
		pCharaInfo->_tNetData._tSyncObject.Pack( (char*) &m_pCastPacket_Chara_SyncObject[ idx], sizeof(N_PCINFO_SYNCOBJECT) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_SYNCOBJECTSTATE;
	}

	if( iInfoFlag & P2P_INFO_FLAG_RADIO )
	{
		pCharaInfo->_tNetData._tRadioChat.Pack( (char*) &m_pCastPacket_Chara_Radio[ idx], sizeof(N_PCINFO_RADIOCHAT) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_RADIO;
	}

	if( iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON && (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
	{
		pCharaInfo->_tNetData._tWeaponData.Pack( (char*) &m_pCastPacket_Chara_Weapon[ idx], sizeof(N_PCINFO_WEAPON) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
	}

	if( iInfoFlag & P2P_INFO_FLAG_CS_LIFE )
	{
		pCharaInfo->_tNetData._tHP.Pack( (char*) &m_pCastPacket_Chara_Life[ idx], sizeof(N_PCINFO_LIFE) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_CS_LIFE;
		// GlobalFunc::PB_TRACE("P2P_INFO_FLAG_CS_LIFE idx %d life %d", idx, m_pCastPacket_Chara_Life[ idx].iLife);
	}

	// ĳ���Ͱ� �׾��ٸ� �ʿ���� �����ʹ� ������ �ʽ��ϴ�.
	if( !(iInfoFlag & P2P_INFO_FLAG_CS_DEATH))
	{
		// ���Ͽ� ���� ����
		if( iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE )
		{
			INT32 Count = pCharaInfo->_tNetData._tSuicideDamageData.getCount();
			m_pCastPacket_Chara_DecentCount[ idx] = Count;

			for( i = 0; i < Count; i++)
			{
				pCharaInfo->_tNetData._tSuicideDamageData.Pack_Single( (char*) &m_pCastPacket_Chara_Decent[ idx][ i], sizeof(N_PCINFO_SUICIDEDMG), i );
			}

			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_SUICIDEDAMAGE;
		}

		//�̼� ������ ������ �̼� ������ ������. 
		if( iInfoFlag & P2P_INFO_FLAG_MISSION )
		{
			pCharaInfo->_tNetData._tMissionData.Pack( (char*) &m_pCastPacket_Chara_Mission[ idx], sizeof(N_PCINFO_MISSION) );
			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_MISSION;
		}

		//Get Weapon
		if( iInfoFlag & P2P_INFO_FLAG_GETWEAPON )
		{
			pCharaInfo->_tNetData._tGetWeaponData.Pack( (char*) &m_pCastPacket_Chara_GetW[ idx], sizeof(N_PCINFO_GETWEAPON) );
			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_GETWEAPON;
		}

		// Fire
		if( iInfoFlag & P2P_INFO_FLAG_FIRE )
		{
			pCharaInfo->_tNetData._tFireData.Pack( (char*) &m_pCastPacket_Chara_Bullet[ idx], sizeof(N_PCINFO_BULLET) );

			if( isNetThrowWeapon( &pCharaInfo->_tNetData._tWeaponData) )
				m_pCastPacket_Chara_Bullet[ idx].Id	= pCharaInfo->_tNetData._tFireData.getNetSlotIndex();

			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_FIRE;
		}

		// Hit
		if( (iInfoFlag & P2P_INFO_FLAG_HIT) ||
			(iInfoFlag & P2P_INFO_FLAG_HITBYCHARA) )
		{
			GAMEINFO_HIT * pHitInfo = &pCharaInfo->_tNetData._tFireData._tHitData;

			UINT8 nHitCount = (UINT8) pHitInfo->getHitCount();

			//Set Data
			for( i = 0; i < nHitCount; i++ )
			{
				pHitInfo->Pack_SingleHit( (char*) &m_pCastPacket_Chara_Hit[ idx][ i], sizeof(N_PCINFO_HIT), i );
			}
			m_pCastPacket_Chara_HitCount[ idx] = nHitCount;
			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_HIT;
		}

		// Suffering
		if( iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
		{	
			pCharaInfo->_tNetData._tSufferData.Pack_Suffer( (char*) &m_pCastPacket_Chara_Suffering[ idx]);

			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_CS_SUFFERING;
		}
	}
	else
	{
		// Death
		pCharaInfo->_tNetData._tSufferData.Pack( (char*) &m_pCastPacket_Chara_Death[ idx], sizeof(N_PCINFO_DEATH) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag	|= P2P_INFO_FLAG_CS_DEATH;
	}

	//Drop
	if( iInfoFlag & P2P_INFO_FLAG_DROPWEAPON )
	{
		pCharaInfo->_tNetData._tDropWeaponData.Pack( (char*) &m_pCastPacket_Chara_DropW[ idx], sizeof(N_PCINFO_DROPWEAPON) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_DROPWEAPON;
	}

	// ���� ������ ���� ���ۿ� ���� �� �������� ���۸� �ʱ�ȭ
	pCharaInfo->ResetSendCount();
	pCharaInfo->ResetSendCountForHost();
}


void CMulticastSystem::_WriteGameInfo_Chara( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo )
{

	//1. Send�� �ʿ䰡 �ִ��� �˻��մϴ�.
	if( !pCharaInfo->_bSendFlag )
		return;

	m_pCastCheck_Chara[ idx].bCheckBroadcast = true;

	//2. Send�ؾ��� �����͸� �����մϴ�. 
	// default data	
	//���� ��Ŷ �ε� ���� �߿��մϴ�.
	UINT32	iInfoFlag = pCharaInfo->_tNetData._iInfoFlag;
	
	if( iInfoFlag & P2P_INFO_FLAG_CHARASTATE )
	{
		pCharaInfo->_tNetData._tCharaState.Pack( (char*) &m_pCastPacket_Chara_State[ idx], sizeof(N_PCINFO_CHARA_STATE) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_CHARASTATE;
	}

	if( iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT )
	{
		pCharaInfo->_tNetData._tTakingObject.Pack( (char*) &m_pCastPacket_Chara_OnLoadObject[ idx], sizeof(N_PCINFO_ONLOADOBJECT) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_ONLOADOBJECT;
	}

	if( iInfoFlag & P2P_INFO_FLAG_CHARAPOS )
	{
		pCharaInfo->_tNetData._tPosRotData.Pack( (char*) &m_pCastPacket_Chara_PosRot[ idx], sizeof(N_PCINFO_POSROTATION) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_CHARAPOS;
	}

	if( iInfoFlag & P2P_INFO_FLAG_ACTIONKEY )
	{
		pCharaInfo->_tNetData._tActionKey.Pack(&m_pCastPacket_Chara_ActionKeyCount[idx], (char*)m_pCastPacket_Chara_ActionKey[idx], sizeof(N_PCINFO_ACTIONKEY) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_ACTIONKEY;
	}

	if( iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE )
	{
		pCharaInfo->_tNetData._tSyncObject.Pack( (char*) &m_pCastPacket_Chara_SyncObject[ idx], sizeof(N_PCINFO_SYNCOBJECT) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_SYNCOBJECTSTATE;
	}

	if( iInfoFlag & P2P_INFO_FLAG_RADIO )
	{
		pCharaInfo->_tNetData._tRadioChat.Pack( (char*) &m_pCastPacket_Chara_Radio[ idx], sizeof(N_PCINFO_RADIOCHAT) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_RADIO;
	}

	if( iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON && (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
	{
		pCharaInfo->_tNetData._tWeaponData.Pack( (char*) &m_pCastPacket_Chara_Weapon[ idx], sizeof(N_PCINFO_WEAPON) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
	}

	if( iInfoFlag & P2P_INFO_FLAG_CS_LIFE )
	{
		pCharaInfo->_tNetData._tHP.Pack( (char*) &m_pCastPacket_Chara_Life[ idx], sizeof(N_PCINFO_LIFE) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_CS_LIFE;
		// GlobalFunc::PB_TRACE("P2P_INFO_FLAG_CS_LIFE idx %d life %d", idx, m_pCastPacket_Chara_Life[ idx].iLife);
	}

	// ĳ���Ͱ� �׾��ٸ� �ʿ���� �����ʹ� ������ �ʽ��ϴ�.
	if( !(iInfoFlag & P2P_INFO_FLAG_CS_DEATH))
	{
		// ���Ͽ� ���� ����
		if( iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE )
		{
			INT32 Count = pCharaInfo->_tNetData._tSuicideDamageData.getCount();
			m_pCastPacket_Chara_DecentCount[ idx] = Count;

			for(INT32 i = 0; i < Count; i++)
			{
				pCharaInfo->_tNetData._tSuicideDamageData.Pack_Single( (char*) &m_pCastPacket_Chara_Decent[ idx][ i], sizeof(N_PCINFO_SUICIDEDMG), i );
			}

			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_SUICIDEDAMAGE;
		}

		//�̼� ������ ������ �̼� ������ ������. 
		if( iInfoFlag & P2P_INFO_FLAG_MISSION )
		{
			pCharaInfo->_tNetData._tMissionData.Pack( (char*) &m_pCastPacket_Chara_Mission[ idx], sizeof(N_PCINFO_MISSION) );
			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_MISSION;
		}
	
		//Get Weapon
		if( iInfoFlag & P2P_INFO_FLAG_GETWEAPON )
		{
			pCharaInfo->_tNetData._tGetWeaponData.Pack( (char*) &m_pCastPacket_Chara_GetW[ idx], sizeof(N_PCINFO_MISSION) );
			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_GETWEAPON;
		}

		// Fire
		if( iInfoFlag & P2P_INFO_FLAG_FIRE )
		{
			pCharaInfo->_tNetData._tFireData.Pack( (char*) &m_pCastPacket_Chara_Bullet[ idx], sizeof(N_PCINFO_BULLET) );

			if( isNetThrowWeapon( &pCharaInfo->_tNetData._tWeaponData) )
				m_pCastPacket_Chara_Bullet[ idx].Id	= pCharaInfo->_tNetData._tFireData.getNetSlotIndex();

			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_FIRE;
			//GlobalFunc::PB_TRACE("_WriteGameInfo_Chara P2P_INFO_FLAG_FIRE");
		}

		// Hit
		if( (iInfoFlag & P2P_INFO_FLAG_HIT) ||
			(iInfoFlag & P2P_INFO_FLAG_HITBYCHARA) )
		{
			GAMEINFO_HIT * pHitInfo = &pCharaInfo->_tNetData._tFireData._tHitData;
			UINT8 nHitCount = (UINT8) pHitInfo->getHitCount();
			
			//Set Data
			for( INT32 i = 0; i < nHitCount; i++ )
			{
				pHitInfo->Pack_SingleHit( (char*) &m_pCastPacket_Chara_Hit[ idx][ i], sizeof(N_PCINFO_HIT), i );
			}
			m_pCastPacket_Chara_HitCount[ idx] = nHitCount;
			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_HIT;
		}

		// Suffering
		if( iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
		{	
			pCharaInfo->_tNetData._tSufferData.Pack_Suffer( (char*) &m_pCastPacket_Chara_Suffering[ idx] );

			m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_CS_SUFFERING;
		}
	}
	else
	{
		// Death
		pCharaInfo->_tNetData._tSufferData.Pack_Death( (char*) &m_pCastPacket_Chara_Death[ idx]);
		m_pCastCheck_Chara[ idx].nP2PInfoFlag	|= P2P_INFO_FLAG_CS_DEATH;
	}

	//Drop
	if( iInfoFlag & P2P_INFO_FLAG_DROPWEAPON )
	{
		pCharaInfo->_tNetData._tDropWeaponData.Pack( (char*) &m_pCastPacket_Chara_DropW[ idx], sizeof(N_PCINFO_DROPWEAPON) );
		m_pCastCheck_Chara[ idx].nP2PInfoFlag |= P2P_INFO_FLAG_DROPWEAPON;
	}

	// ���� ������ ���� ���ۿ� ���� �� �������� ���۸� �ʱ�ȭ
	pCharaInfo->ResetSendCount();
	pCharaInfo->ResetSendCountForHost();
}

void CMulticastSystem::_WriteGameInfo_Grenade( INT32 idx, GAMEINFO_THROW_WEAPON * pGrenadeInfo)
{
	// broadcast ���� üũ
	if( !pGrenadeInfo->_bSend)
		return;

	pGrenadeInfo->_tNetData.Pack( (char*) &m_pCastPacket_Grenade[ idx], sizeof(N_WEAPONINFO_THROW) );
	m_pbCastCheck_Grenade[ idx] = true;

	pGrenadeInfo->_bSend = false;

	bool bProcess = true;

	if( BattleServerContext::i()->IsUseDedicationMode() )
	{
		// �ڽ��� �͸� ó���ϵ��� �Ѵ�.
		if( ((idx < BattleSlotContext::i()->getMySlotIdx() * MAX_THROWWEAPON_SLOP) &&
			((idx >= (BattleSlotContext::i()->getMySlotIdx()+1) * MAX_THROWWEAPON_SLOP))))
		{
			bProcess = false;
		}
	}

	UINT8 tState = pGrenadeInfo->_tNetData.GetState();
	
	if( bProcess)
	{
		// �� ó��
		if( tState & NET_GRENADE_STATE_BOUND)
		{
			// ��Ŷ�� ����� �ٽ� �ö��� ���·� ����
			tState		&= ~NET_GRENADE_STATE_BOUND;
			tState		|= NET_GRENADE_STATE_FLY;
		}

		// ��Ŷ�� ����� �������� ��ȯ, �ȱ׷��� ��� �����Ե˴ϴ�.
		if( tState & NET_GRENADE_STATE_CLICKFIRE)
		{
			tState		&= ~NET_GRENADE_STATE_CLICKFIRE;
			tState		|= NET_GRENADE_STATE_SLEEP;
		}
		if( tState & NET_GRENADE_STATE_SETTLED )
		{
			tState		&= ~NET_GRENADE_STATE_SETTLED;
			tState		|= NET_GRENADE_STATE_SLEEP;
		}
		if( tState & NET_GRENADE_STATE_C5_SETTLED)
		{
			tState		&= ~NET_GRENADE_STATE_C5_SETTLED;
			tState		|= NET_GRENADE_STATE_SLEEP;
		}
	}

	// ���� ���´� ���� �ʿ䰡 ���� �˴ϴ�. �ʱ�ȭ
	if( tState & NET_GRENADE_STATE_EXPLOSION )
	{
		if( pGrenadeInfo->_tAssistData._SendCount <= 0)
		{
			pGrenadeInfo->Reset();
		}
		else
		{
			pGrenadeInfo->_tAssistData._fSendCountElapsedTime += g_pViewer->getDeltaTime();
			if( pGrenadeInfo->_tAssistData._fSendCountElapsedTime > 0.1f)
			{
				pGrenadeInfo->_tAssistData._SendCount--;
				pGrenadeInfo->_bSend = true;
				pGrenadeInfo->_tAssistData._fSendCountElapsedTime = 0.f;
			}
		}
	}
	pGrenadeInfo->_tNetData.SetState(tState);
}


void CMulticastSystem::_WriteGameInfo_Object( INT32 idx, GAMEINFO_OBJECT * pObjectInfo)
{
	// broadcast ���� üũ
	if( !pObjectInfo->_bBroadcast)
		return;

	_OBJINFO_FOR_MULTICAST * pBuffer = &m_pCastPacket_Obj[ pObjectInfo->_nInfoSlotIdx];
	pBuffer->_CastCheck = 0;
	pBuffer->_writePos = 0;

	switch( pObjectInfo->_nType )
	{
	case I3_NETWORK_GAMEOBJECT_TYPE_NONE :		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_STATIC :
		if( !i3::same_of<CGameObjectWeaponBox*>(pObjectInfo->_pObjectRef))
		{
			pObjectInfo->_tNetData.addObjectType( UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER);
			pBuffer->_writePos = (UINT16)pObjectInfo->_tNetData.PackPacket( pBuffer->_buffer, sizeof(pBuffer->_buffer) );
			pBuffer->_CastCheck = UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER;
		}
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_MOVE :
		{
			pObjectInfo->_tNetData.addObjectType( UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_POSITION);
			pBuffer->_writePos = (UINT16)pObjectInfo->_tNetData.PackPacket( pBuffer->_buffer, sizeof(pBuffer->_buffer) );
			pBuffer->_CastCheck = UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_POSITION;
		}
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC :
		{
			pObjectInfo->_tNetData.addObjectType( UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_POSITION | UDP_OBJECT_TYPE_ROTATION);
			pBuffer->_writePos = (UINT16)pObjectInfo->_tNetData.PackPacket( pBuffer->_buffer, sizeof(pBuffer->_buffer) );
			pBuffer->_CastCheck = UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_POSITION | UDP_OBJECT_TYPE_ROTATION;
		}
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_ANIM :
		{
			pObjectInfo->_tNetData.addObjectType( UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY);
			pBuffer->_writePos = (UINT16)pObjectInfo->_tNetData.PackPacket( pBuffer->_buffer, sizeof(pBuffer->_buffer) );
			pBuffer->_CastCheck = UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY;
		}
		break;
	default :
		I3TRACE( "[CMulticastSystem::_WriteGameInfo_Object()] - invalid Object type." );
		break;
	}

	pObjectInfo->_bBroadcast	= false;
}

void CMulticastSystem::_WriteGameInfo_DropedWewapon( INT32 idx, GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo)
{
	if( !pDropedWeaponInfo->_bSend)
		return;

	pDropedWeaponInfo->_tNetData.Pack( (char*) &m_pCastPacket_DropedWeapon[ idx], sizeof(N_DROPEDWEAPONINFO) );
	m_pbCastCheck_DropedWeapon[ idx]	= true;

	// process after send
	switch( pDropedWeaponInfo->getState() )
	{
	case  NET_WEAPONDROPED_REMOVE :
		if( CStageInfoUtil::IsDeletableDroppedWeapon(*MyRoomInfoContext::i()->getStage()) )
		{// �������� �����Դϴ�.
			pDropedWeaponInfo->Reset();
		}
		break;
	case NET_WEAPONDROPED_START :
	case NET_WEAPONDROPED_FLY :
		break;
	case NET_WEAPONDROPED_BOUND :
		// �ٿ�� ������ ������ �ٽ� �ö��� ���·� �ٲ��ݴϴ�.
		pDropedWeaponInfo->setState( NET_WEAPONDROPED_FLY);
		break;
	case NET_WEAPONDROPED_SLEEP :
		break;
	}

	pDropedWeaponInfo->_bSend = false;
}

void CMulticastSystem::_WriteGameInfo_NPC( INT32 idx, GAMEINFO_NPC * pNPCInfo)
{
	if( pNPCInfo->Pack( (char*) &m_pCastPacket_NPC[ idx], sizeof(N_NPCINFO_DEF) ) > 0)
	{
		m_pbCastCheck_NPC[ idx]	= true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Character ��Ŷ�� ó���մϴ�.
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define P2P_INFO_FLAG_SUICIDEDAMAGE		0x01	// ���� ������ ( CN -> CS )
//#define P2P_INFO_FLAG_MISSION				0x02	// Mission
//#define P2P_INFO_FLAG_GETWEAPON			0x04	// ���⸦ ��
//#define P2P_INFO_FLAG_DROPWEAPON			0x08	// ���⸦ ���
//#define P2P_INFO_FLAG_FIRE				0x10	// �߻� ��Ŷ
//#define P2P_INFO_FLAG_HIT					0x20	// �߻� + Hit *N �� ����
//#define P2P_INFO_FLAG_CS_DEATH			0x40	// ���常 ����
//#define P2P_INFO_FLAG_CS_SUFFERING		0x80	// ���常 ����
/*
bool CMulticastSystem::_MakePacket_Chara( UINT16 idx, bool bAI)
{
	INT32 i;
	UINT16 nLimitSize = (UINT16) m_iPacketInfo[P2P_SUB_HEAD_USER].iReserve;

	if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_DEF, m_nSendPos) == false)
		return false;

	INT32	nHeadPos = m_nSendPos;

	m_nSendPos += sizeof( N_GAME_SUBHEAD);

	UINT16	iInfoFlag = 0;
	CCastPacket_Chara* pCastPacket = nullptr;

	if(bAI)
	{
		I3ASSERT(idx<MAX_COUNT_DOMI_DINO);
		pCastPacket = &m_CastPacket_CharaForAI[idx];
	}
	else
	{
		I3ASSERT(idx<SLOT_MAX_COUNT);
		pCastPacket = &m_CastPacket_Chara[idx];
	}

	I3ASSERT(pCastPacket != nullptr);

	{	
		// ������ ��Ŷ ������ ���� ������Ʈ���� �����Ǿ��� ��Ŷ�� �����ϵ��� �մϴ�.
		pCastPacket->m_CastPacket_Chara_Def.iInfoFlag = pCastPacket->m_CastCheck_Chara.nP2PInfoFlag;
		if( pCastPacket->m_CastPacket_Chara_Def.iInfoFlag & P2P_INFO_FLAG_DROPWEAPON )
		{	// P2P_INFO_FLAG_DROPWEAPON �� P2P_INFO_FLAG_FIRE �� �Բ� ������ ���� �ȵ˴ϴ�.
			pCastPacket->m_CastPacket_Chara_Def.iInfoFlag &= ~P2P_INFO_FLAG_FIRE;
		}
		iInfoFlag = pCastPacket->m_CastPacket_Chara_Def.iInfoFlag;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_Def.CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CHARASTATE, m_nSendPos, nLimitSize) == false) return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_State.CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_CHARAPOS )
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CHARAPOS, m_nSendPos, nLimitSize) == false ) return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_PosRot.CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_ACTIONKEY )
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_ACTIONKEY, m_nSendPos, nLimitSize) == false) return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_ActionKey.CopyToPacket( &m_SendBuffer[ m_nSendPos]);

		// �ʱ�ȭ
		// ���� ���忡�� ��� ������ �ȴ�.
		pCastPacket->m_CastPacket_Chara_ActionKey.Reset();
	}

	if( iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_ONLOADOBJECT, m_nSendPos, nLimitSize) == false)return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_OnLoadObject.CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_SYNCOBJECTSTATE, m_nSendPos, nLimitSize) == false)	return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_SyncObject.CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_RADIO)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_RADIO, m_nSendPos, nLimitSize) == false)return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_Radio.CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_SYNCWEAPON, m_nSendPos, nLimitSize) == false) return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_Weapon.CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_CS_LIFE)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CS_LIFE, m_nSendPos, nLimitSize) == false)	return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_Life.CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}
	
	if( iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
	{
		UINT8 * pCount = (UINT8*) &m_SendBuffer[ m_nSendPos];
		I3ASSERT( pCastPacket->m_CastPacket_Chara_DecentCount <= MAX_SUICIDE_COUNT);
		*pCount = (UINT8) pCastPacket->m_CastPacket_Chara_DecentCount;
		m_nSendPos += sizeof( UINT8);

		for( i = 0; i < pCastPacket->m_CastPacket_Chara_DecentCount; i++)
		{
			if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_SUICIDEDAMAGE, m_nSendPos, nLimitSize) == false )return false;
			m_nSendPos += pCastPacket->m_CastPacket_Chara_Decent[ i].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
		}
	}

	if( iInfoFlag & P2P_INFO_FLAG_MISSION)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_MISSION, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos +=  pCastPacket->m_CastPacket_Chara_Mission[ i].CopyToPacket( &m_SendBuffer[ m_nSendPos] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_GETWEAPON, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_GetW[ i].CopyToPacket( &m_SendBuffer[ m_nSendPos] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_DROPWEAPON)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_DROPWEAPON, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_DropW[ i].CopyToPacket( &m_SendBuffer[ m_nSendPos] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_FIRE)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_FIRE, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_Bullet[ i].CopyToPacket( &m_SendBuffer[ m_nSendPos] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_HIT)
	{
		if( (INT32)(m_nSendPos + sizeof( UINT8) + (sizeof( N_PCINFO_HIT) * pCastPacket->m_CastPacket_Chara_HitCount[ i] )) >= nLimitSize )
			return false;

		UINT8 * pHitCount = (UINT8*) &m_SendBuffer[ m_nSendPos];
		I3ASSERT( pCastPacket->m_CastPacket_Chara_HitCount[ i] <= MAX_FIREHIT_COUNT);
		*pHitCount = (UINT8) pCastPacket->m_CastPacket_Chara_HitCount[ i];
		m_nSendPos += sizeof( UINT8);

		for(INT32 j = 0; j < pCastPacket->m_CastPacket_Chara_HitCount[ i]; j++)
		{
			m_nSendPos += pCastPacket->m_CastPacket_Chara_Hit[ i][ j].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
		}
	}

	// only client
	if( iInfoFlag & P2P_INFO_FLAG_CS_DEATH)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CS_DEATH, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_Death[ i].CopyToPacket( &m_SendBuffer[ m_nSendPos ] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CS_SUFFERING, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += pCastPacket->m_CastPacket_Chara_Suffering[ i].CopyToPacket( &m_SendBuffer[ m_nSendPos ] );
		//I3TRACE( "Multicast P2P_INFO_FLAG_CS_SUFFERING.\n");
	}

	// header ����
	UDP_UTIL::BuildSubHead( &m_SendBuffer[ nHeadPos], P2P_SUB_HEAD_USER, idx, (UINT16)(m_nSendPos-nHeadPos));

	// ���۸� �ʱ�ȭ�մϴ�.
	pCastPacket->m_CastPacket_Chara_HitCount[ i] = 0;
	pCastPacket->m_CastPacket_Chara_DecentCount[ i] = 0;
	pCastPacket->m_CastCheck_Chara[ i].Reset();

	return true;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Character ��Ŷ�� ó���մϴ�.
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define P2P_INFO_FLAG_SUICIDEDAMAGE		0x01	// ���� ������ ( CN -> CS )
//#define P2P_INFO_FLAG_MISSION				0x02	// Mission
//#define P2P_INFO_FLAG_GETWEAPON			0x04	// ���⸦ ��
//#define P2P_INFO_FLAG_DROPWEAPON			0x08	// ���⸦ ���
//#define P2P_INFO_FLAG_FIRE				0x10	// �߻� ��Ŷ
//#define P2P_INFO_FLAG_HIT					0x20	// �߻� + Hit *N �� ����
//#define P2P_INFO_FLAG_CS_DEATH			0x40	// ���常 ����
//#define P2P_INFO_FLAG_CS_SUFFERING		0x80	// ���常 ����
bool CMulticastSystem::_MakePacket_Chara( UINT16 idx)
{
	INT32 i;
	UINT16 nLimitSize = (UINT16) m_iPacketInfo[P2P_SUB_HEAD_USER].iReserve;

	if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_DEF, m_nSendPos) == false)
		return false;

	INT32	nHeadPos = m_nSendPos;

	m_nSendPos += sizeof( N_GAME_SUBHEAD);

	UINT32	iInfoFlag = 0;

	{	
		// ������ ��Ŷ ������ ���� ������Ʈ���� �����Ǿ��� ��Ŷ�� �����ϵ��� �մϴ�.
		m_pCastPacket_Chara_Def[ idx].iInfoFlag = m_pCastCheck_Chara[ idx].nP2PInfoFlag;		// UINT16 �����մϴ�.(2012.09.28.����)
		if( m_pCastPacket_Chara_Def[ idx].iInfoFlag & P2P_INFO_FLAG_DROPWEAPON )
		{	// P2P_INFO_FLAG_DROPWEAPON �� P2P_INFO_FLAG_FIRE �� �Բ� ������ ���� �ȵ˴ϴ�.
			m_pCastPacket_Chara_Def[ idx].iInfoFlag &= ~P2P_INFO_FLAG_FIRE;
		}
		iInfoFlag = m_pCastPacket_Chara_Def[ idx].iInfoFlag;							// UINT16 �����մϴ�.(2012.09.28.����)
		m_nSendPos += m_pCastPacket_Chara_Def[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CHARASTATE, m_nSendPos, nLimitSize) == false) return false;
		m_nSendPos += m_pCastPacket_Chara_State[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_ONLOADOBJECT, m_nSendPos, nLimitSize) == false)return false;
		m_nSendPos += m_pCastPacket_Chara_OnLoadObject[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_CHARAPOS )
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CHARAPOS, m_nSendPos, nLimitSize) == false ) return false;
		m_nSendPos += m_pCastPacket_Chara_PosRot[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if (iInfoFlag & P2P_INFO_FLAG_ACTIONKEY)
	{
		UINT8* pCount = (UINT8*)&m_SendBuffer[m_nSendPos];
		if (m_nSendPos + sizeof(UINT8) + (m_pCastPacket_Chara_ActionKeyCount[idx] * sizeof(N_PCINFO_ACTIONKEY)) > nLimitSize) return false;
		*pCount = m_pCastPacket_Chara_ActionKeyCount[idx];
		m_nSendPos += sizeof(UINT8);
		for (i = 0; i < m_pCastPacket_Chara_ActionKeyCount[idx]; i++)
		{
			m_nSendPos += m_pCastPacket_Chara_ActionKey[idx][i].CopyToPacket(&m_SendBuffer[m_nSendPos]);			
			m_pCastPacket_Chara_ActionKey[idx][i].Reset();
		}
		m_pCastPacket_Chara_ActionKeyCount[idx] = 0;
	}

	if( iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_SYNCOBJECTSTATE, m_nSendPos, nLimitSize) == false)	return false;
		m_nSendPos += m_pCastPacket_Chara_SyncObject[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_RADIO)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_RADIO, m_nSendPos, nLimitSize) == false)return false;
		m_nSendPos += m_pCastPacket_Chara_Radio[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON && (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_SYNCWEAPON, m_nSendPos, nLimitSize) == false) return false;
		m_nSendPos += m_pCastPacket_Chara_Weapon[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}

	if( iInfoFlag & P2P_INFO_FLAG_CS_LIFE)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CS_LIFE, m_nSendPos, nLimitSize) == false)	return false;
		m_nSendPos += m_pCastPacket_Chara_Life[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
	}
	
	if( iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
	{
		UINT8 * pCount = (UINT8*) &m_SendBuffer[ m_nSendPos];
		I3ASSERT( m_pCastPacket_Chara_DecentCount[ idx] <= MAX_SUICIDE_COUNT);
		*pCount = (UINT8) m_pCastPacket_Chara_DecentCount[ idx];
		m_nSendPos += sizeof( UINT8);

		for( i = 0; i < m_pCastPacket_Chara_DecentCount[ idx]; i++)
		{
			if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_SUICIDEDAMAGE, m_nSendPos, nLimitSize) == false )return false;
			m_nSendPos += m_pCastPacket_Chara_Decent[ idx][ i].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
		}
	}

	if( iInfoFlag & P2P_INFO_FLAG_MISSION)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_MISSION, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += m_pCastPacket_Chara_Mission[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_GETWEAPON, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += m_pCastPacket_Chara_GetW[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_DROPWEAPON)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_DROPWEAPON, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += m_pCastPacket_Chara_DropW[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_FIRE)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_FIRE, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += m_pCastPacket_Chara_Bullet[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos] );
	}

	if( (iInfoFlag & P2P_INFO_FLAG_HIT) ||
		(iInfoFlag & P2P_INFO_FLAG_HITBYCHARA) )
	{
		if( (INT32)(m_nSendPos + sizeof( UINT8) + (sizeof( N_PCINFO_HIT) * m_pCastPacket_Chara_HitCount[ idx] )) >= nLimitSize )
			return false;

		UINT8 * pHitCount = (UINT8*) &m_SendBuffer[ m_nSendPos];
		I3ASSERT( m_pCastPacket_Chara_HitCount[ idx] <= MAX_FIREHIT_COUNT);
		*pHitCount = (UINT8) m_pCastPacket_Chara_HitCount[ idx];
		m_nSendPos += sizeof( UINT8);

		for( i = 0; i < m_pCastPacket_Chara_HitCount[ idx]; i++)
		{
			m_nSendPos += m_pCastPacket_Chara_Hit[ idx][ i].CopyToPacket( &m_SendBuffer[ m_nSendPos]);
		}
	}

	// only client
	if( iInfoFlag & P2P_INFO_FLAG_CS_DEATH)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CS_DEATH, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += m_pCastPacket_Chara_Death[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos ] );
	}

	if( iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
	{
		if( UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG_CS_SUFFERING, m_nSendPos, nLimitSize) == false )return false;
		m_nSendPos += m_pCastPacket_Chara_Suffering[ idx].CopyToPacket( &m_SendBuffer[ m_nSendPos ] );
		//I3TRACE( "Multicast P2P_INFO_FLAG_CS_SUFFERING.\n");
	}

	// header ����
	UDP_UTIL::BuildSubHead( &m_SendBuffer[ nHeadPos], P2P_SUB_HEAD_USER, idx, (UINT16)(m_nSendPos-nHeadPos));

	// ���۸� �ʱ�ȭ�մϴ�.
	m_pCastPacket_Chara_HitCount[ idx] = 0;
	m_pCastPacket_Chara_DecentCount[ idx] = 0;
	m_pCastCheck_Chara[ idx ].Reset();

	return true;
}

UINT16 CMulticastSystem::_MakePacket(INT32 iType, INT32 iIdx, char * pBuffer, INT32 iWritePos)
{
	UINT16 iWriteSize = 0; 
	bool * pWriteBuffer = nullptr;
	
	//������ �˻�
	// �ִ� ����� ������ ������ �������� �Ѵ�.
	if( (iWritePos + m_iPacketInfo[iType].iPacketSize) >= MULTICASTBUFFERSIZE )
		return 0;

	//��Ŷ ����
	iWriteSize = sizeof( N_GAME_SUBHEAD);

	I3_BOUNDCHK( iIdx, 65536);

	UINT16 netIdx = (UINT16) iIdx;

	switch( iType )
	{
	case P2P_SUB_HEAD_USER			: break; 
	case P2P_SUB_HEAD_GRENADE		: 
		{
			pWriteBuffer = &m_pbCastCheck_Grenade[iIdx];
			iWriteSize += m_pCastPacket_Grenade[ iIdx ].CopyToPacket( &pBuffer[iWritePos+iWriteSize] );		
		}
		break;
	case P2P_SUB_HEAD_DROPEDWEAPON	: 
		{
			pWriteBuffer = &m_pbCastCheck_DropedWeapon[iIdx];
			iWriteSize += m_pCastPacket_DropedWeapon[ iIdx ].CopyToPacket( &pBuffer[iWritePos+iWriteSize] );	
		}
		break;
	case P2P_SUB_HEAD_OBJECT	:
		{
			if( (UINT32)iIdx < BattleObjContext::i()->getObjectCountInfo()->m_pTotalObjectList.size())
			{
				i3Object * pObj = BattleObjContext::i()->getObjectCountInfo()->m_pTotalObjectList.at( iIdx);
				I3ASSERT( pObj != nullptr);
				netIdx = (UINT16) pObj->getNetIndex();
				iWriteSize += m_pCastPacket_Obj[ iIdx ].CopyToPacket( &pBuffer[iWritePos+iWriteSize] );
				UDP_UTIL::BuildSubHead( &pBuffer[ iWritePos], (P2P_SUB_HEADTYPE) iType, netIdx, iWriteSize);
				return iWriteSize;
			}
			else
				return 0;
		}
		break; 
	
	case P2P_SUB_HEAD_NPC			: 
		{
			pWriteBuffer = &m_pbCastCheck_NPC[iIdx];
			iWriteSize += m_pCastPacket_NPC[ iIdx ].CopyToPacket( &pBuffer[iWritePos+iWriteSize] );			
		}
		break; 
	default :
		I3PRINTLOG(I3LOG_FATAL,  "Invalid Function Call....!!");
		break; 
	}

	if(pWriteBuffer == nullptr)
	{	
		I3TRACE("[CMulticastSystem::_MakePacket] Unknown Packet Type"); 
		I3ASSERT_0; 
		return 0;
	}

	// Head ����
	UDP_UTIL::BuildSubHead( &pBuffer[ iWritePos], (P2P_SUB_HEADTYPE) iType, netIdx, iWriteSize);

	*pWriteBuffer = false; 
	return iWriteSize; 
}

bool CMulticastSystem::_MakeStageInfo()
{
	return _MakeStageInfo_Mission();		//Mission information
}

void CMulticastSystem::_ResetStageInfoBuff( void)
{
	m_nStageInfoSendCount			= 0;
	m_nStageInfoReq					= STAGEINFO_REQ_STATE_NONE;
	m_nStageInfo_CharaPreSend		= 0;
	m_nStageInfo_MissionPreSend		= 0;
}


//��ź�� ����մϴ�. ���� �̸� ���� �ʿ�.
bool CMulticastSystem::_MakeStageInfo_Mission( void)
{
	if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) )
		return true;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	// check installed
	if( pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED )
		return true;

	if( m_nStageInfo_MissionPreSend == 1)
		return true;

	if( UDP_UTIL::IsPackSize( P2P_SUB_HEAD_STAGEINFO_MISSION, m_nSendPos) == false)
		return false;

	WeaponBase * pWeapon = nullptr;
#if defined( USE_EVENT_SHUTTLEX)
	pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SHUTTLEX));
#else
	pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4) );
#endif

	if( pWeapon == nullptr )	return true;

	I3_BOUNDCHK( pMissionMng->getWhoInstalled(), SLOT_MAX_COUNT);

	GAMEINFO_MISSION_INTRUDER netData;
	netData.setInstallMode( 1);
	netData.setInstallerIndex( pMissionMng->getWhoInstalled());
	netData.setInstallPosition( pWeapon->GetPos());

	UINT16 prePos = m_nSendPos;
	m_nSendPos += sizeof( N_GAME_SUBHEAD);
	m_nSendPos += netData.Pack( &m_SendBuffer[ m_nSendPos], sizeof(m_SendBuffer)-m_nSendPos );
	UDP_UTIL::BuildSubHead( &m_SendBuffer[ prePos], P2P_SUB_HEAD_STAGEINFO_MISSION, (UINT16)pMissionMng->getWhoInstalled(), (UINT16)(m_nSendPos - prePos) );

	m_nStageInfo_MissionPreSend = 1;

	UDPTRACE( "send intrude stageinfo mission ok.\n");

	return true;
}

void CMulticastSystem::_MergeSendData_Chara( void)
{
	INT32	i, idx;
	UINT16	nTempWritePos;

	for( i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		idx = (m_nPreSend_CharaIdx + i) % SLOT_MAX_COUNT;			// �����ϴ� ������ġ  
		if( !m_pCastCheck_Chara[ idx ].bCheckBroadcast )continue;	// ���� ��Ŷ�� ���� ���� �Դϴ�. 

		nTempWritePos = m_nSendPos;	// ���ϱ����� ������		
		if( !_MakePacket_Chara( (UINT16) idx) )
		{
			// ������ �ʰ��Ǹ� ��ŵ
			m_nSendPos = nTempWritePos;		// ���� ������� ����Ʈ �������� �����ݴϴ�.
			m_nPreSend_CharaIdx = idx;		// ������� ���� ���� �ε������� ���� ���带 �����ϵ��� �մϴ�.
			break; 
		}
	}
}

void CMulticastSystem::_MergeSendData( P2P_SUB_HEADTYPE iType )
{
	INT32		i, nObjLoofCount, idx;
	INT32		nLimitCount;
	bool	*	pCastChecker = nullptr;
	INT32	*	pSendObjIndex = nullptr;

	{//������ �ִ� �ִ� ��Ŷ ī��Ʈ�� ����Ѵ�. 
		INT32	nReserveCount;

		nReserveCount = m_iPacketInfo[iType].iReserve - m_nSendPos;

		if( nReserveCount > 0 )
		{
			// ���� countrk�ִ� ���
			// �ռ� Packet���� �����Ƿ� ���� TYpe�� ���� ��Ŷ�� �� ���� �� �ִ�.
			nReserveCount /= (m_iPacketInfo[iType].iPacketSize);
		}
		else
		{	// ���� count�� ����.
			// ������ �ִ� ������ŭ�� ������.
			nReserveCount = 0;
		}

		nLimitCount =  nReserveCount + m_iPacketInfo[iType].iMaxCount;
	}
	pSendObjIndex	= &m_pPreSend_Idx[iType];				//������ �ִ� �ε����� �˻��մϴ�. 
	nObjLoofCount	= m_iPacketInfo[iType].iDataMaxCount;	//���� ī��Ʈ�� �޽��ϴ�. 

	switch( iType )
	{
	case P2P_SUB_HEAD_NPC			: pCastChecker	= m_pbCastCheck_NPC;			break; 
    case P2P_SUB_HEAD_GRENADE		: pCastChecker	= m_pbCastCheck_Grenade;		break; 
	case P2P_SUB_HEAD_DROPEDWEAPON	: pCastChecker	= m_pbCastCheck_DropedWeapon;	break;
	case P2P_SUB_HEAD_OBJECT		: pCastChecker	= nullptr;			break;	
	default :
		I3TRACE( "MulticastSystem MurgeObject type invalid..\n" );
		return;
		break; 
	}	
		
	for( i = 0; i < nObjLoofCount; i++ )
	{
		idx = ((*pSendObjIndex) + i) % nObjLoofCount;
		if( pCastChecker != nullptr )
		{
			if( pCastChecker[ idx ] == false ) continue;
		}
		else
		{
			if( m_pCastPacket_Obj[idx]._CastCheck == 0) continue;
		}

		UINT16 iWriteSize = _MakePacket(iType, idx, &m_SendBuffer[0], m_nSendPos);         
		if( iWriteSize == 0 )
		{
			*pSendObjIndex = idx;	// ���� ���忡�� �� �ε��� ���� ����
			break; 
		}
		m_nSendPos = m_nSendPos + iWriteSize;
		if( i >= nLimitCount )
		{	
			*pSendObjIndex = idx;	// ���� ���忡�� �� �ε��� ���� ����
			break; 
		}
	}
	return; 
}

//User���� �������� ������ �������� �մϴ�.
void CMulticastSystem::RequestIntrudeProcess( void)
{
	_ResetStageInfoBuff();

	m_nStageInfoReq			= STAGEINFO_REQ_STATE_SEND;
	m_nStageInfoSendCount	= 3;
}

void CMulticastSystem::_ProcessMakeIntruderPacket( void)
{
	INT32 i;

	if( m_nStageInfoReq == STAGEINFO_REQ_STATE_SEND)
	{
		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if( (NetworkContext::i()->UdpHoleState[i] != UDP_STATE_DONE) &&
				(NetworkContext::i()->UdpHoleState[i] != UDP_STATE_NONE) )
			{
				return;
			}
		}

		if( _MakeStageInfo())
		{
			m_nStageInfoSendCount--;
			m_nStageInfo_CharaPreSend		= 0;
			m_nStageInfo_MissionPreSend		= 0;

			if( m_nStageInfoSendCount <= 0)
			{
				_ResetStageInfoBuff();
			}
		}
	}
}

void CMulticastSystem::_SendToBattle( void )
{
	ClientUDPSocket *	pUDPSocket = CGameNetworkManager::i()->GetUDPSocket();
	INT32 Rv;	

	Rv = pUDPSocket->SendBattleServer(m_SendBuffer, m_nSendPos, 0xFF);

	if( Rv == SOCKET_ERROR )
	{		
		i3Net::GetError("Error\n");
		return;
	}
}

void CMulticastSystem::_SendToUser( UINT8 idx )
{
	//char szTrace[128] = {0,};
	UINT32	nIP = 0;
	UINT16	nPort = 0;
	INT32	Rv;	

	ClientUDPSocket *	pUDPSocket = CGameNetworkManager::i()->GetUDPSocket();

	const UDP_TYPE NetType			= NetworkContext::i()->UdpHoleType[idx];
	const NET_UDP_INFO * pNetInfo	= &NetworkContext::i()->NetInfo[idx];

	switch( NetType )
	{
	case UDP_TYPE_REAL	: 
		nIP = pNetInfo->m_nRealIP;
		nPort = pNetInfo->m_nRealPort;
		break; 
	case UDP_TYPE_NAT	: 
		nIP = pNetInfo->m_nLocalIP;
		nPort = pNetInfo->m_nLocalPort;
		break; 
	default: 
		/*sprintf_s( szTrace, "Invalid NetType. %d", idx );
		I3PRINTLOG(I3LOG_NOTICE, szTrace);*/
		return;
		break; 
	}
	Rv = pUDPSocket->Send( m_SendBuffer, m_nSendPos, nIP, nPort );

	if( Rv == SOCKET_ERROR )
	{		
		i3Net::GetError("Error\n");
		return;
	}
	
	return; 
}

