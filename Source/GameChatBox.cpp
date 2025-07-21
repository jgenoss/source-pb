#include "pch.h"
#include "GameChatBox.h"
#include "GlobalVariables.h"
#include "CommunityContext.h"
#include "GameGUI.h"

#include "UI/UIMainFrame.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIHUDIngameChat.h"

#include "i3Base/string/ext/generic_string_ncopy.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/remove_blank_from_tail.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "i3Base/RemoveColorString.h"

#include "UserInfoContext.h"

//I3_CLASS_INSTANCE( CGameChatBox, i3GameObjBase);
I3_CLASS_INSTANCE( CGameChatBox);

CGameChatBox::CGameChatBox(void) :	m_ChatBufferTypeList(MAX_CHAT_BUFFER), 
									m_ChatBufferList(MAX_CHAT_BUFFER), 
									m_ChatBackupBufferList(MAX_CHAT_BUFFER), 
									m_WhisperBufferList(5),
									m_MegaPhoneBufferList(MAX_MEGAPHONE_BUFFER),
									m_HistoryBufferList(MAX_CHAT_HISTORY_COUNT), 
									m_HistoryChatModeList(MAX_CHAT_HISTORY_COUNT)
{

	m_pSceneNode = i3Node::new_object();	

	m_nCurrentBufferIndex = 0;
	m_nBackUpBufferIndex = 0;
	m_nDeleteBufferIndex = 0;
	m_nCurrentWhisperBufferIndex = 0;
	m_nCurrentMegaPhoneBufferIndex = 0;
	m_nWhisperBufferIndex = 0;
	m_nMegaPhoneBufferIndex = 0;
	m_nBufferCount = 0;
	m_nBackUpBufferCount = 0;
	m_nWhisperBufferCount = 0;
	m_nMegaPhoneBufferCount = 0;
	m_fBackUpBufferRatio = 0.0f;

	m_bWhisperEnable = false;
	m_bRingBufferFull = false;

	m_nCurrentHistoryBufferIndex = 0;
	m_nHistoryBufferIndex = 0;
	m_nHistoryBufferCount = 0;
	m_nHistoryBufferOffset = 0;

}

CGameChatBox::~CGameChatBox(void)
{

}

void CGameChatBox::ResetChatBuffer()
{
	INT32 i;

	m_nCurrentBufferIndex = 0;
	m_nBackUpBufferIndex = 0;
	m_nDeleteBufferIndex = 0;
	m_nBufferCount = 0;
	m_nBackUpBufferCount = 0;
	m_fBackUpBufferRatio = 0.0f;
	m_bRingBufferFull = false;

	for(i=0; i<MAX_CHAT_BUFFER; i++)
	{
		m_ChatBufferList[i].clear();
		m_ChatBackupBufferList[i].clear();
	}

	// UI�� �̺�Ʈ�� ������.
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	if( pUIFrame != nullptr)
	{
		pUIFrame->UpdateChatBuffer();
	}

	if(g_pFramework->GetStageBattle())
	{
		UIBattleFrame::i()->UpdateChatBuffer();
	}

}

void CGameChatBox::ResetWhisperBuffer(void)
{
	INT32 i = 0;

	m_nCurrentWhisperBufferIndex = 0;
	m_nWhisperBufferIndex = 0;
	m_nWhisperBufferCount = 0;

	for(i=0; i<5; i++)
	{
		m_WhisperBufferList[i].clear();
	}
}

void CGameChatBox::ResetMegaPhoneBuffer(void)
{
	INT32 i = 0;

	m_nCurrentMegaPhoneBufferIndex = 0;
	m_nMegaPhoneBufferIndex = 0;
	m_nMegaPhoneBufferCount = 0;

	for(i=0; i<MAX_MEGAPHONE_BUFFER; i++)
	{
		m_MegaPhoneBufferList[i].clear();
	}
}	


const i3::rc_wstring& CGameChatBox::GetBufferText( INT32 index) const
{
	// ���� ��ȯ�� index�� Buffer�� ũ�⸦ �Ѿ�ٸ�, nullptr�� ��ȯ�մϴ�.
	if( index < 0 || index >= MAX_CHAT_BUFFER)
	{
		return m_emptyString;
	}

	return m_ChatBufferList[index];				// �� ���ľ� ��. (13.02.15.����)
}

const i3::rc_wstring& CGameChatBox::GetBackUpBufferText( INT32 index) const
{
	// ���� ��ȯ�� index�� Buffer�� ũ�⸦ �Ѿ�ٸ�, nullptr�� ��ȯ�մϴ�.
	if( index < 0 || index >= MAX_CHAT_BUFFER)
	{
		return m_emptyString;
	}

	return m_ChatBackupBufferList[index];		
}

const i3::rc_wstring& CGameChatBox::GetWhisperBufferText( INT32 index) const
{
	return m_WhisperBufferList[index];
}

const i3::rc_wstring& CGameChatBox::GetMegaPhoneBufferText( INT32 index) const
{
	return m_MegaPhoneBufferList[index];
}

void CGameChatBox::GetHistoryBufferText( INT32 index, i3::rc_wstring& out_str, INT32 * pMode)
{
	if( index == 1)		// ���� Ű�� ������ ���
	{
		m_nHistoryBufferOffset++;

		if( m_nHistoryBufferOffset <= m_nHistoryBufferCount)
		{	
			if( m_nHistoryBufferOffset != 1)
			{
				m_nCurrentHistoryBufferIndex--;
				if( m_nCurrentHistoryBufferIndex < 0) m_nCurrentHistoryBufferIndex = m_nHistoryBufferCount - 1;
			}

			out_str = m_HistoryBufferList[m_nCurrentHistoryBufferIndex];

			*pMode = m_HistoryChatModeList[ m_nCurrentHistoryBufferIndex ];		//	m_szHistoryChatMode[m_nCurrentHistoryBufferIndex];
		}
		else
		{
			m_nHistoryBufferOffset = m_nHistoryBufferCount;					
			
			out_str = m_HistoryBufferList[m_nCurrentHistoryBufferIndex];

			*pMode = m_HistoryChatModeList[m_nCurrentHistoryBufferIndex];
		}
	}
	else				// �Ʒ��� Ű�� ������ ���
	{
		m_nHistoryBufferOffset--;

		if( m_nHistoryBufferOffset > 0)
		{			
			m_nCurrentHistoryBufferIndex++;

			if( m_nCurrentHistoryBufferIndex >= m_nHistoryBufferCount) m_nCurrentHistoryBufferIndex = 0;

			out_str =  m_HistoryBufferList[m_nCurrentHistoryBufferIndex];

			*pMode = m_HistoryChatModeList[m_nCurrentHistoryBufferIndex];
		}
		else
		{
			m_nHistoryBufferOffset = 0;

			out_str.clear();
		}
	}
}

void CGameChatBox::ResetHistoryBufferIndex(void)
{
	if( m_nHistoryBufferIndex - 1 > 0)
	{
		m_nCurrentHistoryBufferIndex = m_nHistoryBufferIndex - 1;
	}
	else
	{
		if( m_nHistoryBufferIndex < m_nHistoryBufferCount)
		{
			m_nCurrentHistoryBufferIndex = 0;
		}
		else
		{
			m_nCurrentHistoryBufferIndex = m_nHistoryBufferCount - 1;
		}
	}

	m_nHistoryBufferOffset = 0;
}

//
// �� ��ƾ�� ���� ���� ���ľ� �Ѵ�...�ֳ��ϸ� ���ϴ� ���� �˻������ ù�ε����� 0�ΰ��� ã�°��ε�
// 0�� ���ٸ� ������ ã�� �ظű� �����̴�... 0�� ���ٸ� ��� �����Ҽ� �ֵ��� �ؾ��Ѵ�..(13.02.19.����)
// �Լ��� �߸� ������ ������ ���δ�.... �׳� �ܼ� ���Լ��� �ٲٴ°� ����..
// 
// ParsingText�� ���� ȣ��ȴٸ� ( �� ��) .... GAME_STRING�������� �̸� �ε��� �����ϴ°� ����...
//

CHAT_SYS_TYPE CGameChatBox::ParsingText( const i3::wliteral_range& wTextRng) 
{
	CHAT_SYS_TYPE ret = CHAT_SYS_TYPE_NONE;

	if( i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM3"))  ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM4"))  ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM2"))  ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM1")) ) 
	{
		// ��ȸ�� �� ê��!!	
		ret = CHAT_SYS_TYPE_ONETIME_TEAM;/*/T *//*/t *//*/�� *//*/�� */
	}
	else if( i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE3"))  ||
			 i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE4"))  ||
			 i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE2"))  ||
			 i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE1")) ) 
	{
		// ä��â Ŭ����!!	
		ret = CHAT_SYS_TYPE_CLEAR;/*/C*//*/c*//*/��*//*/�����*/
	}
	else if( i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_HELP"))  ||
			 i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_HELP2")) )
	{
		// ä�� ���� ǥ��!!	
		ret = CHAT_SYS_TYPE_HELP;/*/? *//*/?*/
	}
	else if( i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER3")) ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER4"))||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER2")) ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER1")) ) 
	{
		// �ӼӸ�!!			
		if( FindTarget( wTextRng))
		{
			if(	!m_wstrChatTarget.empty() )	
			{
				ret = CHAT_SYS_TYPE_WHISPER;/*/W *//*/w *//*/�� *//*/�� */
			}
		}
	}
	else if( i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY3"))  ||
			 i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY4"))  ||
			 i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY2"))  ||
			 i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY1")) ) 
	{
		FindTarget( wTextRng );/*/R *//*/r *//*/�� *//*/�� */

		// �亯!!	
		ret = CHAT_SYS_TYPE_REPLY;
	}
	else if( i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_MEGAPHONE1"))  ||
			 i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_MEGAPHONE2")) )
	{
		ret = CHAT_SYS_TYPE_MEGAPHONE;
	}
	
	return ret;
}


//
// ���� FindTarget�Լ��� while���ڿ� ó���δ� ù ���鿡������ Ÿ�ٴг����� ��� �����ϰ�, �ι�° ���鿡�� �г��� Ȯ���� ������ ���ڿ���
// �Ӹ� �ؽ�Ʈ�� �ִ� ����..(13.02.15.����)
//
// �̺κ��� �Ϲ�ȭ�ϸ�, ���� ù������ ã��, �� ��ġ���� �ٽ� �ι�° ������ ã���� �ȴ�.. ( ���� �̺κ��� �űԷ�ƾ���� ��ü�Ѵ�..)
// 

bool CGameChatBox::FindTarget( const i3::wliteral_range& wTextRng)
{
	m_wstrHistoryText.clear();

	i3::wliteral_range::const_iterator it_target = i3::find(wTextRng.begin(), wTextRng.end(), L' '); 
	
	if ( it_target == wTextRng.end() )
	{
		m_wstrChatTarget.clear();
		return false;
	}
	++it_target;
	i3::wliteral_range::const_iterator it_text = i3::find( it_target, wTextRng.end(), ' ');
	
	m_wstrChatTarget.assign(it_target, it_text);	

	if ( it_text == wTextRng.end() )
		return false;
	
	++it_text;
	m_wstrHistoryText.assign(it_text, wTextRng.end() );

	return true;
}

void CGameChatBox::SetChatReplyTarget(const i3::rc_wstring& wstrTarget)
{
	m_wstrChatReplyTarget = wstrTarget; 
}

void CGameChatBox::SetHelpText(void)
{
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP1"));	//"{col:80,255,60,255}F2: ��ü ä�� ���� ����{/col}"
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP2"));	//"{col:80,255,60,255}F3: �� ä�� ���� ����{/col}"
	PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_HELP_CHANGE_CLANMODE"));/*{col:255,120,0,255}F4: Ŭ�� ä�� ���� ����{/col}*/
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP3"));	//"{col:80,255,60,255}/w, /��: �ӼӸ�{/col}"
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP4"));	//"{col:80,255,60,255}/r, /��: �亯{/col}"
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP5"));	//"{col:80,255,60,255}/t, /��: 1ȸ�� �� ä�� �Է�{/col}"
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP6"));	//"{col:80,255,60,255}/c, /�����: ä�� ���� �����{/col}"
}

void CGameChatBox::PutUserChatting( const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng,  const bool bIsGM, UINT16 nType, UINT8 NickColor)
{
//	char szTemp[MAX_STRING_COUNT + MAX_STRING_COUNT + 10] = {0,};

	i3::stack_wstring wstrTemp;

	i3::stack_wstring wstrNickColor;
	i3::stack_wstring wstrStringColor;
	//bool isGM = false;
	
	// revision 61128
	i3::wstring wstrClearString = wStringRng;
//	char szResult[256] = {0,};
	bool bcolor = ::RemoveColorString(wStringRng, wstrClearString);		// ::RemoveColorSting �� �Ű������δ� i3::string �� �� �� ����
//	i3::safe_string_copy(szResult,szClearString,sizeof(szResult));		// �׷��� i3String::Concat �� �Ű������� char * �̹Ƿ� �ε����ϰ� �̷��� ��ƾ���� �ۼ��ϰ� �Ǿ���
																		// i3String �� char * �� i3::string ȣȯ �۾��� �̷������ RemoveColorString ���ٷ� �ۼ� ������
	if( bcolor == false)
		wstrClearString = wStringRng;

	//if (i3::generic_is_niequal(wstrNick , GAME_RCSTRING("STR_TBL_CHAT_GM_TEXT") , 2) )
	//{
	//	isGM = true; 
	//}
	if( CCommunityContext::i()->CheckBlockUser(wstrNick) )
	{
		return;
	}

	//ä�� ������ ���Ѵ�.
	if( bIsGM == false)
	{
		// Ŀ�´�Ƽ �ɼ��� ������ ���� ä�ó����� ������Ʈ���� �����մϴ�.
		switch( g_pEnvSet->m_nChatType)
		{
		// ��ü����
		case 1:
			// ��� ä�ø޼����� �����ϰ� �����մϴ�.
			return;
			break;

		// �Ʊ����
		case 2:
			if( nType != CHATTING_TYPE_TEAM && nType != CHATTING_TYPE_CLAN)
			{
				return;
			}
			break;
		// ��ü���
		default:
			break;
		}
	}

	//ä�� ������ ���Ѵ�.
	if( bIsGM )
	{
		wstrNickColor = L"247,200,60,255";/*GM*/

		switch( nType)
		{
		case CHATTING_TYPE_ALL:		wstrStringColor = L"255,255,255,255";		break;
		case CHATTING_TYPE_TEAM:	wstrStringColor = L"98, 180, 253, 255";	break;
		case CHATTING_TYPE_CLAN:	wstrStringColor = L"80, 255, 60, 255";	break;
		case CHATTING_TYPE_LOBBY:	wstrStringColor = L"255,255,255,255";		break;
		default:					wstrStringColor = L"255,255,255,255";		break;
		}
	}
	else if( nType == CHATTING_TYPE_CLAN )
	{
		wstrNickColor = L"80, 255, 60, 255";
		wstrStringColor = L"80, 255, 60, 255";
	}
	else if( nType == CHATTING_TYPE_TEAM )
	{
		wstrNickColor = L"98, 180, 253, 255";
		wstrStringColor = L"98, 180, 253, 255";
	}
	else if( nType == CHATTING_TYPE_ALL 
		|| nType == CHATTING_TYPE_LOBBY 
		|| nType == CHATTING_TYPE_CLAN_MEMBER_PAGE  )
	{
		const I3COLOR* clr_nick = GetColorCashItem(NickColor);
		const I3COLOR* clr_string = GetColorCashItem(0);
		
		i3::sprintf(wstrNickColor, L"%d,%d,%d,%d", clr_nick->r, clr_nick->g, clr_nick->b, clr_nick->a);
		i3::sprintf(wstrStringColor, L"%d,%d,%d,%d", clr_string->r, clr_string->g, clr_string->b, clr_string->a);
	}
	else if( nType == CHATTING_TYPE_RADIO )	//���� �޼���
	{
		const I3COLOR* clr_nick = GetColorCashItem(NickColor);
		const I3COLOR* clr_string = GetColorCashItem(0);
		i3::sprintf(wstrNickColor, L"%d,%d,%d,%d", clr_nick->r, clr_nick->g, clr_nick->b, clr_nick->a);
		i3::sprintf(wstrStringColor, L"%d,%d,%d,%d", clr_string->r, clr_string->g, clr_string->b, clr_string->a);

		wstrClearString = wStringRng;
	}
	else
	{
		const I3COLOR* clr_nick_string = GetColorCashItem(0);

		i3::sprintf(wstrNickColor, L"%d,%d,%d,%d", clr_nick_string->r, clr_nick_string->g, clr_nick_string->b, clr_nick_string->a);
		wstrStringColor = wstrNickColor;
	}

	// Nick Name�� �ִٸ� ���� �Է��մϴ�.
	if( !wstrNick.empty() )
	{
		if( i3::generic_is_equal( wstrNickColor,wstrStringColor) )
		{
			wstrTemp = L"{col:";
			i3::generic_string_cat( wstrTemp, wstrNickColor);
			i3::generic_string_cat( wstrTemp, L"}[");
			i3::generic_string_cat( wstrTemp, wstrNick);
			i3::generic_string_cat( wstrTemp, L"] ");
			i3::generic_string_cat( wstrTemp, wstrClearString);
			i3::generic_string_cat( wstrTemp, L"{/col}");
		}
		else
		{
			wstrTemp = L"{col:";

			i3::generic_string_cat( wstrTemp, wstrNickColor);
			i3::generic_string_cat( wstrTemp, L"}[");
			i3::generic_string_cat( wstrTemp, wstrNick); 
			i3::generic_string_cat( wstrTemp, L"] {/col}");

			i3::generic_string_cat( wstrTemp, L"{col:");
			i3::generic_string_cat( wstrTemp, wstrStringColor);
			i3::generic_string_cat( wstrTemp, L"}");
			i3::generic_string_cat( wstrTemp, wstrClearString);
			i3::generic_string_cat( wstrTemp, L"{/col}");
		}
	}
	else
	{
		wstrTemp = wstrClearString;
	}

	// ���� �Է��� ä���̶��, ä��Ÿ�Կ� ���� ����ϴ°� �ƴ϶�, ��� ����ؾ���. �׷��⶧���� ������ ��µǴ� nType = 0���� ����.
	_PutStringBuffer(wstrTemp, nType);
}

void CGameChatBox::PutUserWhisper( const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng, const bool bIsGM)
{
	i3::wstring wstrClearString;
	//i3::string szResult;
	::RemoveColorString(wStringRng, wstrClearString);					// ä�� ��Ŷ ���� �� �÷� Ű���� ���� ��ƾ ���� ( 2013_0628 ������ )
	//i3::safe_string_copy(szResult,szClearString,sizeof(szResult));

	i3::stack_wstring strTemp;

	const wchar_t* litNickColor = L"237, 86, 250, 255";
	const wchar_t* litStringColor = litNickColor;	// "237, 86, 250, 255";

	if( UserInfoContext::i()->CheckAuthLevel())
	{
		litNickColor = L"255,128,64,255";  /*GM*/
		litStringColor = L"237, 86, 250, 255";
	}

	// Ŀ�´�Ƽ �ɼ��� ������ ���� �ӼӸ��� ������Ʈ���� �����մϴ�.
	switch( g_pEnvSet->m_nWhisperType)
	{
	// ��ü����
	case 1:
		// ��� �ӼӸ��� �����ϰ� �����մϴ�.
		return;
		break;

	// ģ��&Ŭ�����
	case 2:
		// �۾��ؾ��մϴ�~!
		break;
	// ��ü���
	default:
		break;
	}

	// Nick Name�� �ִٸ� ���� �Է��մϴ�.

	if( litNickColor == litStringColor )			// ���ͷ��� �׳� ������ �񱳷� ������...(13.02.15.����)
	{
		i3::sprintf(strTemp, L"{col:%s}[%s] %s{/col}", litNickColor, wstrNick, wstrClearString);
	}
	else
	{
		i3::sprintf(strTemp, L"{col:%s}[%s]{/col} {col:%s}%s{/col}", litNickColor, wstrNick, litStringColor, wstrClearString);
	}

	if( m_bWhisperEnable)	_PutStringBuffer(strTemp, (UINT16)CHATTING_TYPE_WHISPER);
	else					_PutStringWhisperBuffer(strTemp);

}

void CGameChatBox::PutUserMegaPhone( const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng)
{
	i3::wstring wstrClearString;
	::RemoveColorString(wStringRng, wstrClearString);

	i3::stack_wstring strTemp;

	const wchar_t* litNickColor = L"253, 139, 248, 255";
	
	i3::sprintf(strTemp, L"{col:%s}[%s] %s{/col}", litNickColor, wstrNick, wstrClearString);
	
	_PutStringMegaPhoneBuffer(strTemp);
}

void CGameChatBox::PutSystemChatting(const i3::rc_wstring& str, UINT16 nType)
{
	// ! praptor - ���� �ٲٰų� Ư���� �ٹ��� �ϱ����ؼ��� ����ڸ� �����ϰ�,
	// ����ϴ� �κ� - �̸��׸� ReadyRoom���� ����ڸ� ã�Ƴ��� ����Ͽ����Ѵ�.
	// ���� �Ϲ� ü�ð��� �г����� ���ܵǾ��ٴ� �� �̿ܿ� �������� ����.

	_PutStringBuffer(str, nType);
}

void CGameChatBox::PutHistory(  const i3::rc_wstring& str, INT32 ChatMode)
{
		
	if( str.empty() ) return;

	I3TRACE( "History Buffer TextInput: Idx = %d\n", m_nHistoryBufferIndex);

	// ���� ���Ǵ� Buffer�� ���� �ֽ��ϴ�.
	m_HistoryBufferList[m_nHistoryBufferIndex] = str;
	m_HistoryChatModeList[ m_nHistoryBufferIndex ] = UINT16(ChatMode);


	// Buffer Count�� ������ŵ�ϴ�.
	if( m_nHistoryBufferCount < MAX_CHAT_HISTORY_COUNT)
	{
		m_nHistoryBufferCount++;
	}

	// Buffer Index�� ������ŵ�ϴ�.
	m_nHistoryBufferIndex++;
	if( m_nHistoryBufferIndex >= MAX_CHAT_HISTORY_COUNT)
	{
		m_nHistoryBufferIndex = 0;				
	}

	m_nCurrentHistoryBufferIndex = m_nHistoryBufferIndex;
}

void CGameChatBox::_PutStringBuffer(const i3::rc_wstring& str, UINT16 nType)
{
	if( str.empty() )	return;

	//I3TRACE( "TextInput: Idx = %d\n", m_nCurrentBufferIndex);

	// Buffer�� Type
	m_ChatBufferTypeList[m_nCurrentBufferIndex] = nType;

	// ���� ���Ǵ� Buffer�� ���� �ֽ��ϴ�.
	m_ChatBufferList[ m_nCurrentBufferIndex ] = str;
	// Backup�� Buffer�� ���� �ֽ��ϴ�.
	m_ChatBackupBufferList[ m_nCurrentBufferIndex ] = str;
	
	// Buffer Count�� ������ŵ�ϴ�.
	m_nBufferCount++;
	m_nBackUpBufferCount++;

	// Buffer�� ���� Index Count�� ������ŵ�ϴ�.
	if( m_nCurrentBufferIndex == m_nBackUpBufferIndex)
	{
		// ������ġ�� ���� ��(m_nCurrentBufferIndex)�̶��, �ڵ� ��ũ�� �Ǿ�� ��.
		m_nCurrentBufferIndex++;
		m_nBackUpBufferIndex++;
	}
	else
	{
		// ������ġ�� ���� ��(m_nCurrentBufferIndex)�� �ƴ϶��, �ڵ� ��ũ�� ���� ����.
		m_nCurrentBufferIndex++;

		// ���۰� �������� ��ȯ�ǰ� �������, ȭ�鿡 ���̴� ����� �ε����� ����Ǿ�� �մϴ�.
		if( m_bRingBufferFull) m_nBackUpBufferIndex++;
	}

	// ���� Buffer�� Index�� Buffer�� ũ�⸦ �Ѿ�ٸ�, Index�� �ٽ� �ʱ�ġ�� �����մϴ�.(��ȯ�ǵ���!!)
	if(m_nCurrentBufferIndex >= MAX_CHAT_BUFFER)
	{
		m_bRingBufferFull = true;
		m_nCurrentBufferIndex = 0;		
	}

	if(m_nBackUpBufferIndex >= MAX_CHAT_BUFFER)
	{
		m_nBackUpBufferIndex = 0;		
	}

	if(m_nBackUpBufferCount >= MAX_CHAT_BUFFER)
	{
		m_nBackUpBufferCount = MAX_CHAT_BUFFER;
	}

	// UI�� �̺�Ʈ�� ������.
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	if( pUIFrame )
	{
		pUIFrame->UpdateChatBuffer();
	}

	CStageBattle*pStageBattle = g_pFramework->GetStageBattle();
	if(pStageBattle)
	{
		if( UIBattleFrame::i() != nullptr)
			UIBattleFrame::i()->UpdateChatBuffer();
	}
}

void CGameChatBox::_PutStringWhisperBuffer(const i3::rc_wstring& str)
{

	if( str.empty() ) return;

	I3TRACE( "BackUp TextInput: Idx = %d\n", m_nWhisperBufferIndex);

	// ���� ���Ǵ� Buffer�� ���� �ֽ��ϴ�.
	m_WhisperBufferList[m_nWhisperBufferIndex] = str;

	// Buffer Count�� ������ŵ�ϴ�.
	if( m_nWhisperBufferCount < 5)
	{
		m_nWhisperBufferCount++;
	}
	else
	{
		m_nCurrentWhisperBufferIndex++;
		if( m_nCurrentWhisperBufferIndex >= 5)
		{
			m_nCurrentWhisperBufferIndex = 0;
		}
	}

	// Buffer Index�� ������ŵ�ϴ�.
	m_nWhisperBufferIndex++;
	if( m_nWhisperBufferIndex >= 5)
	{
		m_nWhisperBufferIndex = 0;		
	}
}

void CGameChatBox::_PutStringMegaPhoneBuffer(const i3::rc_wstring& str)
{

	if( str.empty() ) return;

	I3TRACE( "BackUp TextInput: Idx = %d\n", m_nWhisperBufferIndex);

	// ���� ���Ǵ� Buffer�� ���� �ֽ��ϴ�.
	m_MegaPhoneBufferList[m_nMegaPhoneBufferIndex] = str;

	// Buffer Count�� ������ŵ�ϴ�.
	if( m_nMegaPhoneBufferCount < MAX_MEGAPHONE_BUFFER)
	{
		m_nMegaPhoneBufferCount++;
	}
	else
	{
		m_nCurrentMegaPhoneBufferIndex++;
		if( m_nCurrentMegaPhoneBufferIndex >= MAX_MEGAPHONE_BUFFER)
		{
			m_nCurrentMegaPhoneBufferIndex = 0;
		}
	}

	// Buffer Index�� ������ŵ�ϴ�.
	m_nMegaPhoneBufferIndex++;
	if( m_nMegaPhoneBufferIndex >= MAX_MEGAPHONE_BUFFER)
	{
		m_nMegaPhoneBufferIndex = 0;		
	}

	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	if( pUIFrame )
	{
		pUIFrame->UpdateMegaPhoneBuffer();
	}
}


void CGameChatBox::CalcRatio( INT32 nExceptionValue)
{	
	INT32 nCount = 0;
	INT32 nStartIdx = m_nCurrentBufferIndex;
	INT32 nEndIdx = m_nBackUpBufferIndex;

	while( nStartIdx != nEndIdx)
	{
		nCount++;

		nStartIdx--;
		if( nStartIdx < 0) nStartIdx = MAX_CHAT_BUFFER-1;
	}

	if( nCount > 0)
	{
		if( (m_nBackUpBufferCount - nExceptionValue) > 0)
		{
			m_fBackUpBufferRatio = ((REAL32)nCount) / (REAL32)(m_nBackUpBufferCount - nExceptionValue);
		}
		else
		{
			m_fBackUpBufferRatio = 0.0f;
		}
	}
	else
	{
		m_fBackUpBufferRatio = 0.0f;
	}
}

void CGameChatBox::DeleteBufferIndex()
{
	m_ChatBufferList[ m_nDeleteBufferIndex ].clear();
	
	//I3TRACE( "TextInput Delete: Idx = %d\n", m_nDeleteBufferIndex);

	m_nDeleteBufferIndex++;

	if(m_nDeleteBufferIndex >= MAX_CHAT_BUFFER)
	{
		m_nDeleteBufferIndex = 0;
	}

	m_nBufferCount--;
	if( m_nBufferCount < 0) m_nBufferCount = MAX_CHAT_BUFFER-1;
}

void CGameChatBox::ResetBackupBuffer(void)
{
	m_nBackUpBufferIndex = m_nCurrentBufferIndex;
	m_fBackUpBufferRatio = 0.0f;
}

void CGameChatBox::IncreaseBackupBuffer( INT32 nExceptionValue)
{
	if( m_nBackUpBufferIndex != m_nCurrentBufferIndex)
	{
		m_nBackUpBufferIndex++;
		if( m_nBackUpBufferIndex >= MAX_CHAT_BUFFER)
		{
			m_nBackUpBufferIndex = 0;
		}
	}

	CalcRatio(nExceptionValue);
}

void CGameChatBox::DecreaseBackupBuffer( INT32 nExceptionValue)
{
	INT32 i = 0;
	INT32 nStopIdx = m_nCurrentBufferIndex;

	INT32 nLimit = m_nBackUpBufferCount - nExceptionValue;
	if( nLimit < 0) return;

	for( i=0; i<nLimit; i++)
	{
		nStopIdx--;
		if( nStopIdx < 0) nStopIdx = MAX_CHAT_BUFFER-1;
	}

	if( m_nBackUpBufferIndex !=  nStopIdx)
	{
		m_nBackUpBufferIndex--;
		if( m_nBackUpBufferIndex < 0)
		{
			m_nBackUpBufferIndex = MAX_CHAT_BUFFER-1;
		}
	}

	CalcRatio( nExceptionValue);

	I3TRACE("Decrese Index: %d\n", m_nBackUpBufferIndex);
}

void CGameChatBox::CalcBackupBufferByRatio( REAL32 fVal, INT32 nExceptionValue)
{
	// ��ũ���� �з��� ���� ������ ó������ �ʴ´�
	if (nExceptionValue >= m_nBackUpBufferCount)
	{
		return;
	}

	INT32 nResultIdx = 0;
	INT32 nIdxArray[MAX_CHAT_BUFFER] = {0,};
	INT32 nStartIdx = m_nCurrentBufferIndex;

	m_fBackUpBufferRatio = 1.0f - fVal;
	fVal = 1.0f - fVal;

	for(INT32 i = 0; i < m_nBackUpBufferCount; i++)
	{
		nIdxArray[i] = nStartIdx;
		nStartIdx--;

		if (nStartIdx < 0)
		{
			nStartIdx = MAX_CHAT_BUFFER-1;
		}
	}

	INT32 nTemp = m_nBackUpBufferCount - nExceptionValue;

	if (nTemp < 0)
	{
		nTemp = MAX_CHAT_BUFFER + nTemp;
	}

	nResultIdx = (INT32)(nTemp * fVal);

	m_nBackUpBufferIndex = nIdxArray[nResultIdx];
}

void CGameChatBox::SetEnable( bool bTrue, REAL32 tm)
{
	i3GameNode::SetEnable( bTrue, tm);
}


void CGameChatBox::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GameObjBase::OnUpdate( rDeltaSeconds );
}
