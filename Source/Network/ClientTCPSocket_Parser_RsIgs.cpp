#include "pch.h"
#include "ClientTCPSocket.h"
#include "Shop.h"
#include "IGShopContext.h"
#include "IGShopItemController.h"

#include "UserInfoContext.h"

#include "Designation/DesigUtil.h"


namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_FIELDSHOP_OPEN_ACK : return "PROTOCOL_FIELDSHOP_OPEN_ACK";
		case PROTOCOL_FIELDSHOP_GOODSLIST_ACK : return "PROTOCOL_FIELDSHOP_GOODSLIST_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_RSIGS_XXX �������� �̸� ��������̼�~");
			return "PROTOCOL_RSIGS_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingRsIgs(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_RsIgs.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_RsIgs.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_RsIgs_Function( void)
{
	//RegisterParser( PROTOCOL_RS_ENTER_ACK, &ClientTCPSocket::__Parse_RouletteShop_Enter);
	//RegisterParser( PROTOCOL_RS_LEAVE_ACK, &ClientTCPSocket::__Parse_RouletteShop_Leave);
	//RegisterParser( PROTOCOL_RS_ITEM_INFO_ACK, &ClientTCPSocket::__Parse_RouletteShop_ItemInfo);
	//RegisterParser( PROTOCOL_RS_ROULETTE_RESULT_ACK, &ClientTCPSocket::__Parse_RouletteShop_RouletteResult);
	RegisterParser( PROTOCOL_FIELDSHOP_OPEN_ACK, &ClientTCPSocket::__Parse_FieldShop_Open);
	RegisterParser( PROTOCOL_FIELDSHOP_GOODSLIST_ACK, &ClientTCPSocket::__Parse_FieldShop_GetGoodsList);
}



void ClientTCPSocket::__Parse_FieldShop_Open( i3NetworkPacket * pPacket)
{
	// ���д� ������ �ƴ�, ���ο� ����� �޴´�. �̰��� �ٸ� ��Ŷ�̴�..
	// �̰�� ��Ŷ������ �𸥴ٰ� �����ϰ�, ��Ϲ����� ó���ϵ��� �����Ѵ�..
	// 
	UINT8 res;
	UINT8  ui8RecvShopInfo;	// ���� ������ ���� ����. 0 : ����X , 1 : ����O
	pPacket->ReadData(&ui8RecvShopInfo, sizeof(UINT8));
	CShop* pShop = CShop::i();
	if( 0 == ui8RecvShopInfo && true == pShop->GetFirstShopFileCheck() )
	{
		// ���� �����͸� �ѹ��� �������� �ʾҴٸ� ShopFile�� ���� ���� �����͸� �ε��մϴ�.
		pShop->LoadShopFile();
	}
	else if ( 1 == ui8RecvShopInfo)
	{
		DesigUtil::ClearDesigItemInfosFromServer();
	}

	pPacket->ReadData(&res,				sizeof(UINT8));
	if (res == 1)
	{
		IGShopContext* cont = g_pFramework->GetIGShopContext();

		if (cont->IsMainShopChange())
			cont->UpdateGoodsList();
		
		cont->EnteredServer();
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_OPEN_IGSHOP);		// �����ÿ��� �ٷ� ���� ���� ����
	}
}

void ClientTCPSocket::__Parse_FieldShop_GetGoodsList( i3NetworkPacket * pPacket)
{
	// �Ϲ������� ù �κ� ���Խ�, �������� ���½� ���� �ϳ��� ���� ������ �б⸦ ���ؾ��Ѵ�..
	// ���ı����� ����, �������� �ֽ� ������ǰ����� ���� �����Ҷ��� �ü� �ֱ� ������ �� ó���Ѵ�...
	FieldShopInfo fsgi;
	pPacket->ReadData(&fsgi, sizeof(FieldShopInfo));
	
	IGShopContext* cont = g_pFramework->GetIGShopContext();
	cont->ReceiveAndUpdateGoodsList(fsgi);				// ������ ����...
	
	if (cont->IsIGShopOpenning() == true)
	{
		cont->EnteredServer();					// ���������� ���� �˸��� UI�� ��� �̺�Ʈ�� ������..
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_OPEN_IGSHOP);
	}
}

