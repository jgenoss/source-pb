#include "pch.h"
#include "PBCmd_UseItem.h"

CMD_RESULT_FLAG PBCmd_UseItem :: RunCommand(REAL32 DeltaSec)
{
	UINT32 listItemCount = CInvenList::i()->GetItemCount(INVEN::DB_CASHITEM, 0);
	for( UINT32 i = 0; i < listItemCount; ++i)
	{//
		const INVEN_BUFFER* info = CInvenList::i()->GetItem( INVEN::DB_CASHITEM, i, 0);
		if( info->_ui32ItemID== m_itemID)
		{
			if(info->_ui32ItemArg <= 0)//����� �������� �κ��丮�� ���°��
			{
				//�ӽ÷α� 
				setLogContext("�����۾�� ��������");
				SaveLogFile("UseItemCmd.log",true);

				return CMD_RESULT_FAIL;
			}
		}
	}

	m_timeBuffer += DeltaSec;
	if(m_timeBuffer >= 0.033f)
	{
		m_timeBuffer = 0.0;
		setLogContext("LogCheck");//TCP����� �α׳��涧 ���������� ����ϱ����Ѱ�
		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &m_itemDBIdx);
		return CMD_RESULT_OK;
	}

	return CMD_RESULT_LOOP;
}

/*virtual*/ CMD_RESULT_FLAG PBCmd_UseItem::ExecuteCommand( i3::vector<i3::string> &params)
{
	if( params.size() > 0)
	{
		m_itemID = atoi( params.at(0).c_str());
		m_itemDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID(m_itemID); 

		if(m_itemDBIdx < 0)
			return CMD_RESULT_FAIL;

		return CMD_RESULT_OK;
	}

	return CMD_RESULT_FAIL;
}

