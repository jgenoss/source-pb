#include "pch.h"
#include "ClientTCPSocket.h"
#include "MedalManager.h"
#include "MedalSetBase.h"
#include "ui/UIHudManager.h"
#include "MedalContext.h"
#include "UserInfoContext.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_GET_MEDAL_INFO_ACK : return "PROTOCOL_GET_MEDAL_INFO_ACK";
		case PROTOCOL_GET_CUR_MEDAL_SET_INFO_ACK : return "PROTOCOL_GET_CUR_MEDAL_SET_INFO_ACK";
		case PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK : return "PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK";
		case PROTOCOL_MISSION_COMPLETE_ACK : return "PROTOCOL_MISSION_COMPLETE_ACK";
		case PROTOCOL_GET_MEDAL_REWARD_ACK : return "PROTOCOL_GET_MEDAL_REWARD_ACK";
		case PROTOCOL_SET_NOTIFY_MEDAL_ACK : return "PROTOCOL_SET_NOTIFY_MEDAL_ACK";
		case PROTOCOL_SET_REFRESH_MEDAL_SET_ACK : return "PROTOCOL_SET_REFRESH_MEDAL_SET_ACK";
		case PROTOCOL_CHEAT_MEDAL_COMMAND_ACK : return "PROTOCOL_CHEAT_MEDAL_COMMAND_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_MEDAL_XXX �������� �̸� ��������̼�~");
			return "PROTOCOL_MEDAL_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingMedal( i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Medal.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Medal.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Medal_Function( void)
{
	RegisterParser( PROTOCOL_GET_MEDAL_INFO_ACK,		&ClientTCPSocket::__Parse_Medal_Normal_All_Info	);
	RegisterParser( PROTOCOL_GET_CUR_MEDAL_SET_INFO_ACK,&ClientTCPSocket::__Parse_Medal_Set_Cur_All_Info);
	RegisterParser( PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK,&ClientTCPSocket::__Parse_Medal_Set_Com_All_Info);
	RegisterParser( PROTOCOL_MISSION_COMPLETE_ACK,		&ClientTCPSocket::__Parse_Medal_Mission_Complete);
	RegisterParser( PROTOCOL_GET_MEDAL_REWARD_ACK,		&ClientTCPSocket::__Parse_Medal_Get_Reward		);
	RegisterParser( PROTOCOL_SET_NOTIFY_MEDAL_ACK,		&ClientTCPSocket::__Parse_Medal_Notify_Register	);
	RegisterParser( PROTOCOL_SET_REFRESH_MEDAL_SET_ACK, &ClientTCPSocket::__Parse_Medal_Refresh_Set	);
}

void ClientTCPSocket::__Parse_Medal_Normal_All_Info( i3NetworkPacket * pPacket )
{
	PACKET_H_GET_MEDAL_INFO_ACK pMedalAck;
	MEDAL pMedal[MAX_MEDAL_COUNT];

	MedalContext::i()->SetReceivedMyMedal(0,false);
	pPacket->ReadData(&pMedalAck, sizeof(PACKET_H_GET_MEDAL_INFO_ACK));
	if(EV_SUCCESSED(pMedalAck.m_i32Rv))
	{
		if(pMedalAck.m_ui16Idx + pMedalAck.m_ui16Count > MAX_MEDAL_COUNT)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Normal Medal Idx range over.");
			return;
		}

		INT32 i = 0;
		for(i=pMedalAck.m_ui16Idx; i<pMedalAck.m_ui16Idx + pMedalAck.m_ui16Count; i++)
		{
			pPacket->ReadData(&pMedal[i], sizeof(MEDAL));
			MedalSetBase * pBase = CMedalManager::i()->getMedalSet(MEDAL_SET_TYPE_NORMAL);
			pBase->setMedalInfo(pMedal[i]);
		}
		if( i == pMedalAck.m_ui16TotalCount )
			MedalContext::i()->SetReceivedMyMedal(0,true);
		GameEventSender::i()->SetEvent(EVENT_MEDAL_GET_SET_CUR_INFO);
	}
}

void ClientTCPSocket::__Parse_Medal_Set_Cur_All_Info( i3NetworkPacket * pPacket )
{
	PACKET_H_GET_CUR_MEDAL_SET_INFO_ACK pMedalAck;
	CUR_MEDAL_SET pMedal[MAX_CUR_MEDAL_SET_COUNT];

	pPacket->ReadData(&pMedalAck, sizeof(PACKET_H_GET_CUR_MEDAL_SET_INFO_ACK));
	if(EV_SUCCESSED(pMedalAck.m_i32Rv))
	{
		INT32 i = 0;
		if(pMedalAck.m_ui16Idx + pMedalAck.m_ui16Count > MAX_CUR_MEDAL_SET_COUNT)
		{
			I3PRINTLOG(I3LOG_FATAL, "Current Medal Idx range over.");
			return;
		}

		for(i=pMedalAck.m_ui16Idx; i<pMedalAck.m_ui16Idx+pMedalAck.m_ui16Count; i++)
		{
			pPacket->ReadData(&pMedal[i], sizeof(CUR_MEDAL_SET));
			if(pMedal[i].m_ui8Type == MEDAL_SET_TYPE_NORMAL || pMedal[i].m_ui16Idx == INIT_MEDAL_NUMBER)
			{
				// ���� m_ui8Type�� MEDAL_SET_TYPE_NORMAL �̰ų� m_ui16Idx �� INIT_MEDAL_NUMBER �̸� ���� �޴� ��Ʈ �Դϴ�.
				continue;
			}
			MedalSetBase * pBase = CMedalManager::i()->getMedalSet(pMedal[i].m_ui8Type);
			pBase->setMedalInfo(pMedal[i]);
		}
		if( i == pMedalAck.m_ui16TotalCount )
			MedalContext::i()->SetReceivedMyMedal(1,true);
		GameEventSender::i()->SetEvent(EVENT_MEDAL_GET_SET_COM_INFO);
	}
}

void ClientTCPSocket::__Parse_Medal_Set_Com_All_Info( i3NetworkPacket * pPacket )
{
	PACKET_H_GET_COM_MEDAL_SET_INFO_ACK MedalAck;
	COM_MEDAL_SET Medal[MAX_COM_MEDAL_SET_COUNT];

	pPacket->ReadData( &MedalAck, sizeof(PACKET_H_GET_COM_MEDAL_SET_INFO_ACK));
	if(EV_SUCCESSED( MedalAck.m_i32Rv))
	{
		INT32 i = 0;
		if( MedalAck.m_ui16Idx + MedalAck.m_ui16Count > MAX_COM_MEDAL_SET_COUNT)
		{
			I3PRINTLOG(I3LOG_FATAL, "Complete Medal Idx range over.");
			return;
		}

		for(i=MedalAck.m_ui16Idx; i< MedalAck.m_ui16Idx + MedalAck.m_ui16Count; i++)
		{
			pPacket->ReadData( &Medal[i], sizeof(COM_MEDAL_SET));
			MedalSetBase * pBase = CMedalManager::i()->getMedalSet( Medal[i].m_ui8Type);
			pBase->setMedalInfo( Medal[i]);
		}
		if( i == MedalAck.m_ui16TotalCount )
			MedalContext::i()->SetReceivedMyMedal(2,true);
	}
}

void ClientTCPSocket::__Parse_Medal_Mission_Complete( i3NetworkPacket * pPacket )
{
	I3TRACE("ClientTCPSocket::__Parse_Medal_Mission_Complete\n");
	PACKET_H_MISSION_COMPLETE_ACK	pMedalAck;
	MEDAL_STATE					completeMedal[MAX_MEDAL_COUNT];

	pPacket->ReadData(&pMedalAck, sizeof(PACKET_H_MISSION_COMPLETE_ACK));
	INT32 completeCnt = pMedalAck.m_ui16CompleteMedalCount;

	for(INT32 i=0; i < completeCnt; i++)
	{
		pPacket->ReadData(&completeMedal[i], sizeof(MEDAL_STATE));

		INT32 _type = MEDAL_SET_TYPE_NORMAL;
		if(completeMedal[i].m_ui8MedalType != MEDAL_TYPE_NORMAL)
			_type = CMedalManager::i()->getMedalSetTypeByIndex(completeMedal[i].m_ui16MedalIdx);

		if (g_pEnvSet->GetUIVersion() != UFV_1_5) continue;
		
		I3ASSERT( _type != -1 );
		if(_type == -1) 	continue;

		// �޴� �Ϸ� ó��
		CMedalManager::i()->ProcessCompleteMedalInfo(completeMedal[i]);

		//���� ���â
		completeMedal[i].m_ui8MedalType = (UINT8)_type;
		if(HUD::instance() != nullptr)
			HUD::instance()->SetParameter(HUD::GAME_RESULT, HUD::GRS_Medal, (INT32)&completeMedal[i] );
	}
}

void ClientTCPSocket::__Parse_Medal_Get_Reward( i3NetworkPacket * pPacket )
{
	PACKET_H_GET_MEDAL_REWARD_ACK AckHeader;
	pPacket->ReadData(&AckHeader, sizeof(PACKET_H_GET_MEDAL_REWARD_ACK) );

	// ������ ����Ʈ / ����ġ / ������ ���� ����
	{
		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		MyUserInfoBasic.m_ui32Exp	= AckHeader.m_ui32Exp;
		MyUserInfoBasic.m_ui32Point	= AckHeader.m_ui32Point;
		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

		MedalContext::i()->setMaster( AckHeader.m_ui16MasterMedal );
	}

	for(UINT8 i = 0 ; i < AckHeader.m_ui8Count ; ++i)
	{
		PACKET_D_GET_MEDAL_REWARD AckData;
		pPacket->ReadData(&AckData, sizeof(PACKET_D_GET_MEDAL_REWARD) );

		CMedalManager::i()->setMedalRewardResult(&AckData);

		if( EV_FAILED(AckData.m_i32Rv) )
		{
			I3PRINTLOG(I3LOG_QA,"Medal GetReward fail : %d", AckData.m_i32Rv);
			switch(AckData.m_i32Rv)
			{
			case EVENT_ERROR_PBV15_MEDAL_REWARD_HACK :// �޴� ���� ��û �� ��ŷ.
				break;
			case EVENT_ERROR_PBV15_MEDAL_REWARD_FIND_FAIL_MEDAL :// �޴� ���� ��ȸ ����.
				break;
			case EVENT_ERROR_PBV15_MEDAL_REWARD_FIND_FAIL_ITEM :// ���� - ������ ������ ����.
				break;
			case EVENT_ERROR_PBV15_MEDAL_REWARD_NETWORK :// ���� - ������ ��Ʈ��ũ ����
				break;
			case EVENT_ERROR_PBV15_MEDAL_REWARD_NO_USER :// ���� - ���� ���� ����.
				break;
			case EVENT_ERROR_PBV15_MEDAL_REWARD_DB_SAVE_BUFFER_FULL:// �޴� Reward DB Save Buffer Full
				break;
			case EVENT_ERROR_PBV15_MEDAL_REWARD_SAVE_SP_FAIL_NOUSER:// �޴� Reward Save Sp Fail. ���� ����.
				break;
			default:
				break;
			}
		}
	}

	if( AckHeader.m_bLastPacket )
	{
		if (AckHeader.m_ui8Count == 1)
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_MEDAL_GET_REWARD);
		else
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_MEDAL_GET_ALL_REWARD);
	}
}

void ClientTCPSocket::__Parse_Medal_Notify_Register( i3NetworkPacket * pPacket )
{
	PACKET_SET_NOTIFY_MEDAL_ACK		MedalAck;
	pPacket->ReadData(&MedalAck, sizeof(PACKET_SET_NOTIFY_MEDAL_ACK));
	if(EV_SUCCESSED(MedalAck.m_i32Rv))
	{
		for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
		{
			CMedalManager::i()->setNotifyMedal(i, &MedalAck.m_aNotifyMedal[i]);
		}
	}

	//GameEventReceiver::i()->PushReceivedGameEvent(EVENT_MEDAL_NOTIFY_REGISTER, pMedalAck.m_i32Rv);
}

void ClientTCPSocket::__Parse_Medal_Refresh_Set( i3NetworkPacket * pPacket)
{
	PACKET_SET_REFRESH_MEDAL_SET_ACK	MedalAck;
	pPacket->ReadData(&MedalAck, sizeof(PACKET_SET_REFRESH_MEDAL_SET_ACK));

	CUR_MEDAL_SET * pMedal = MedalAck.m_aCurMedalSet;

	for(INT32 idx = 0; idx < MAX_CUR_MEDAL_SET_COUNT; ++idx)
	{
		if( pMedal[idx].m_ui8Type == MEDAL_SET_TYPE_NORMAL ||pMedal[idx].m_ui16Idx == INIT_MEDAL_NUMBER)
			continue;

		MedalSetBase * pBase = CMedalManager::i()->getMedalSet(pMedal[idx].m_ui8Type);
		pBase->resetMedalInfo();
		pBase->setMedalInfo(pMedal[idx]);
	}
}