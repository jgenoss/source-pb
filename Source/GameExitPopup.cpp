#include "pch.h"
#include "GameExitPopup.h"
#include "BattleGui.h"
#include "StageBattle.h"
#include "UI/UIUtil.h"
#include "AwayInput.h"
#include "MessageBoxManager.h"
#include "MyRoomInfoContext.h"
#include "UI/UIBattleFrame.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/snprintf.h"

namespace detail
{
	void CbGiveupBattle(void* pThis,INT32 nParam)
	{
		if(UIBattleFrame::i() != nullptr)
			UIBattleFrame::i()->InputDisable(false);

		((gexit_popup*)pThis)->m_pExitMessageBox = nullptr;

		if( nParam == MBR_OK)
		{
			ISTEST_S
			{	
				CGameFramework::JumpToLogin();
				return;
			}
			ISTEST_E

			I3PRINTLOG(I3LOG_NOTICE,"||||||||||||>>>> Game Exit by User Force!!! (Lene) <<<<||||||||||||");

			CBattleGui::i()->SetEnableGui( false);
			i3GuiRoot::setGuiDisableInput(true);
			INT32 nArg = 0;
			GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &nArg );

			g_pFramework->SetNightVisionEnable(false);

			// Ȧ��Ī �����Ͽ� ��Ŷ�� �� �� �� �ֱ� ������ �ڽ��� ��������� ��Ŷ�� �ȹ޾Ƶ� ������ ���ư���.
			CAwayInput::i()->setOffAwayInput();

			CGameFramework::JumpToReady();

			//	 ����뿡 ������ ��� ���� �̵� �˾�â�� �������� ���� ����
			//	'���� �̵�' ĳ���� ��������� üũ (���� �ȵ� ���� ����)
			//	PhaseReadyRoom:OpenPopupUsingCashItem ���� �˾� ȣ�⿡ ���
			if (!MyRoomInfoContext::i()->getStage()->IsAiMode())
			{
				if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_FREE_MOVE) == true )
				{
					CInvenList::i()->SetUseBattleGiveupCashItem( true);
				}
			}
		}
	}


	void CbGiveupBattleUsingBattlefieldMove(void* pThis, INT32 nParam)
	{
		((gexit_popup*)pThis)->m_pExitMessageBox = nullptr;

		//	'���� �̵�' ���� �������� �����
		if( nParam == MBR_USER)		//	'�����ۻ��'
		{
			//	���� �̵� ������
			INT64 authFlag = CInvenList::i()->FindWareDBIdx_ByItemID( CASHITEM_ITEMID_BATTLEFIELD_MOVE);

			//	���������� ���� �̵� �������� ����Ͽ� ����뿡 ������ ��� ���� �̵� �˾�â�� �������� ���� ����
			//	PhaseReadyRoom:OpenPopupUsingCashItem ���� �˾� ȣ�⿡ ���
			if (!MyRoomInfoContext::i()->getStage()->IsAiMode())
				CInvenList::i()->SetUseBattleGiveupCashItem( true);

			//	������ ���
			GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &authFlag);

			// Ȧ��Ī �����Ͽ� ��Ŷ�� �� �� �� �ֱ� ������ �ڽ��� ��������� ��Ŷ�� �ȹ޾Ƶ� ������ ���ư���.
			CAwayInput::i()->setOffAwayInput();

			CGameFramework::JumpToReady();
		}
		//	'���� �̵�' ���� �������� ������. (�Ϲ� �г�Ƽ ���� �����ϱ� �ȳ� â ���)
		else if( nParam == MBR_OK)
		{
			gexit_popup::i()->OpenGiveupBattle(GAME_RCSTRING("STBL_IDX_EP_GAME_ENTER_LOBBY_Q"));
		}
	}
	
}


gexit_popup::gexit_popup() : m_pExitMessageBox(nullptr)
{

}

void	gexit_popup::open()
{
#if defined (ENABLE_TEST_COMMAND)
	//	AI ����� ���
	g_pFramework->getCommandSet()->setShowSkill(false);
#endif // ENABLE_TEST_COMMAND

	if( g_pFramework->GetMessageBoxManager()->IsTopMsgBox(m_pExitMessageBox) == true ) return; //����â�� Ȱ��ȭ �����̹Ƿ� �����Ѵ�.

	if (!MyRoomInfoContext::i()->getStage()->IsAiMode())
	{
		/*������ �ߴ��ϰ� ������ �̵��Ͻðڽ��ϱ�?*/
		OpenGiveupBattle(GAME_RCSTRING("STR_TBL_INGAME_HUD_MESSAGE_GIVEUP_GAME"));
	}
	else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		/*������ �ߴ��ϰ� ������ �̵��Ͻðڽ��ϱ�?*/
		OpenGiveupBattle(GAME_RCSTRING("STR_TBL_INGAME_HUD_MESSAGE_GIVEUP_TUTORIAL"));
	}
	//	�Ϲ� ����� ���
	else
	{
		i3::rc_wstring strName;
		i3::stack_wstring wstrMessage;
	//	char szMessage[MAX_STRING_COUNT] = {0,};

		//	'���� �̵�' ���� ������
		INT32 itemFreeMove = CASHITEM_ITEMID_FREE_MOVE;

		//	'���� �̵�' ���� ������
		INT32 itemBattlefieldMove = CASHITEM_ITEMID_BATTLEFIELD_MOVE;

		//	'���� �̵�' ĳ���� ��������� üũ (���� �ȵ� ���� ����)
		//	�ֿ켱 üũ
		if( CInvenList::i()->IsUsingCashItem( itemFreeMove))
		{
			g_pShopItemInfoDataBase->GetItemName(itemFreeMove, strName);

			/*��� ���� {col:255,153,51,255}%s{/col} ���������� ���Ͽ�\n����� ���Ƽ�� �ο����� �ʽ��ϴ�.\n \n������ �̵��Ͻðڽ��ϱ�?*/
			i3::sprintf( wstrMessage, GAME_RCSTRING("STR_TBL_INGAME_HUD_USE_ITEM_TEXT"), strName);

			m_pExitMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(&detail::CbGiveupBattle), this);
		}
		//	'���� �̵�' ĳ������ �����ϰ� �ִ��� üũ
		else if( CInvenList::i()->IsUsingCashItem( itemBattlefieldMove))
		{	
			g_pShopItemInfoDataBase->GetItemName(itemBattlefieldMove, strName);

			/*{col:255,153,51,255}%s{/col} �������� ����Ͽ� �����ϸ�\n���Ƽ�� �ο����� �ʽ��ϴ�.\n \n�������� ����Ͻðڽ��ϱ�?*/
			i3::sprintf( wstrMessage, GAME_RCSTRING("STR_TBL_INGAME_HUD_USE_ITEM_TEXT2"), strName);

			// ������, ���, �����ۻ���� LuaScript���� �����մϴ�. User Ÿ������ ����. �ٸ� caption�� �ַ��� �ٸ� Ÿ������ ����.. �Ǵ� �ٸ��� �����ؾ���...
			m_pExitMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_USER, wstrMessage, nullptr, MAKE_CALLBACK(&detail::CbGiveupBattleUsingBattlefieldMove), this);
		}
		else
		{
			OpenGiveupBattle( GAME_RCSTRING("STBL_IDX_EP_GAME_ENTER_LOBBY_Q"));
		}
	}

	//alt + F4�� Ȱ��ȭ �� ���, ����â�� Ȱ��ȭ�� ���¿��� Ű�Է��� �ٷ� �Ǳ� ������ ���Ƿ� ��Ȱ��ȭ �Ѵ�.(�߿��� �ڵ�)
	//����, �Ʒ� �ڵ尡 ���� ��쿣 ����â�� Ȱ��ȭ ��� ��Ȱ��ȭ �ȴ�.
	i3InputKeyboard * pKeyboard = g_pFramework->getKeyboard();
	if( pKeyboard->GetStrokeState(I3I_KEY_F4) == true) 
		pKeyboard->SetStrokeState(I3I_KEY_F4, false); 

	i3UI::getGlobalIme()->KillFocus(); //ä��â�� ��Ŀ���� ���� ��찡 �־ ���Ƿ� ��Ŀ���� ���δ�.

	if(UIBattleFrame::i() != nullptr)
		UIBattleFrame::i()->InputDisable(true);
}	



// ����������Ʋ ����� close�� ������ִ°� ���ڴ�..
void	gexit_popup::ProcessDestroyStage()
{	
	if( m_pExitMessageBox != nullptr)
	{
		GameUI::CloseMsgBox( m_pExitMessageBox);
		m_pExitMessageBox = nullptr;
	}
}

void	gexit_popup::OpenGiveupBattle(const i3::rc_wstring& wstrMessage)		//	���� ���� �˾� // private�� ����..
{
	m_pExitMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(&detail::CbGiveupBattle), this);
}
