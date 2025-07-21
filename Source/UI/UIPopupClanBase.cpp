#include "pch.h"
#include "UIPopupClanBase.h"

#include "UIUtil.h"
#include "i3Base/string/ext/contain_char.h"

I3_CLASS_INSTANCE( UIPopupClanBase);//, UIPopupBase);

UIPopupClanBase::UIPopupClanBase(void)
{

}

UIPopupClanBase::~UIPopupClanBase(void)
{

}

void UIPopupClanBase::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);
	if( m_StringFilter == nullptr)
	{
		m_StringFilter.reset(i3::make_pscoped<CStrFilter>());
		m_StringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if( m_StringFilter2 == nullptr)
	{
		m_StringFilter2.reset(i3::make_pscoped<CStrFilter>()); 
		m_StringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

bool UIPopupClanBase::_CheckNickNameOverLap( const i3::wliteral_range& wTextRng) const
{
	if( i3::generic_string_size(wTextRng) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_NAME"));	//	"Ŭ������ �����ּ���"
		return false;
	}

	{
		bool GoodNickName = true;

		INT32 idxBlank = i3::contain_char(wTextRng, L' ');

		if( -1 != idxBlank )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Contained Blank");
			GoodNickName = false;
		}
		else if( false == m_StringFilter2->IsFilterNickOK( wTextRng ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilter");
			GoodNickName = false;
		}
		else if( false == m_StringFilter->IsFilterMatchOK( wTextRng ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilterSame");
			GoodNickName = false;
		}
		else if( false == g_pFramework->IsFilterNickOK( wTextRng ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilter");
			GoodNickName = false;
		}
		else if( false == g_pFramework->CheckValidCharacterByLocale( wTextRng ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected CheckValidCharacterByLocale");
			GoodNickName = false;
		}

		if( !GoodNickName )
		{
			//	�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�"
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));

			return false;
		}
	}

	return true;
}

bool	UIPopupClanBase::_CheckClanAddressOverLap(const i3::wliteral_range& wTextRng) const
{
#ifdef CLAN_ADDRESS_ENABLE
	if( i3::generic_string_size(wTextRng) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	//	"Ŭ�� ����Ʈ �ּҸ� �����ּ���."

		return false;
	}

	{
		INT32 idxBlank = i3::contain_char(wTextRng, L' ');

		if( -1 != idxBlank || 
			false == m_pStringFilter2->IsFilter( wTextRng ) || 
			false == m_pStringFilter->IsFilterSame( wTextRng ) || 
			false == g_pFramework->IsFilter( wTextRng ) ) 
		{
			//	"�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�."
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
			return false;
		}

		//	check step 5
		{
			CStringFilter* pStringFilter = CStringFilter::new_object();
			pStringFilter->CreateFilter(STR_FILTER_URL);

			if( pStringFilter->IsFilterSame( wTextRng ) == false ) {
				//	"�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�."
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
				I3_SAFE_RELEASE( pStringFilter );
				return false;
			}

			I3_SAFE_RELEASE( pStringFilter );
		}
	}

	return true;

#else
	// m_bCheckedAddress = true;

	return true;

#endif
}

bool	UIPopupClanBase::_CheckClanAddressValidate(const i3::wliteral_range& wTextRng) const
{
#ifdef CLAN_ADDRESS_ENABLE
	if( wTextRng[0] == 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	//	"Ŭ�� ����Ʈ �ּҸ� �����ּ���."
		return false;
	}

	const wchar_t* wszText  = wTextRng.c_str();

	while(*pszText)
	{
		//	Ŭ�� �ּҴ� ���� �ҹ��ڿ� ���ڷθ� �Է� �����ϹǷ� ������ üũ�Ѵ�. (0 ~ 9 && a ~ z)
		if( (*pszText >= '0' && *pszText <= '9') || (*pszText >= 'a' && *pszText <= 'z')) 
		{ 
			pszText++;

			continue;
		}
		else
		{
			//	"Ŭ�� ����Ʈ URL��\n���� �ҹ��ڿ� ���ڸ� ��� �����մϴ�."
			//	"����Ʈ �ּ� ����"
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ONLY_ENGLISH_OR_NUMERIC"), 
				&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_ERROR_AZIT"));

			return false;
		}
	}	

	return true;
#endif
	
	return true;
}

void UIPopupClanBase::UseName( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName)
{
	i3::sprintf(wstrSaveChar, L"%s\n\n%s", wstrName , GAME_RCSTRING("STBL_IDX_CLAN_CAN_USE"));
}

bool UIPopupClanBase::_OnPopupCheckOverlapping( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName, bool bDuplicate)
{
	if( bDuplicate)
	{
		// "��/�� �̹� ������̰ų� ����Ҽ� ���� �̸��Դϴ�. \n �ٸ� �̸��� ����� �ֽʽÿ�."
		i3::sprintf(wstrSaveChar, L"%s\n\n%s",  wstrName, GAME_RCSTRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));
		return false;
	}
	else
	{
		// "��/�� ����Ͻ� �� �ֽ��ϴ�."
		UseName(wstrSaveChar, wstrName);
		return true;
	}
}

// 
bool UIPopupClanBase::_OnPopupCheckOverlappingAddress(  i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName, bool bDuplicate)
{
	if( bDuplicate)
	{
		//	"��/�� �̹� ������̰ų� ����Ҽ� ���� �ּ��Դϴ�.\n �ٸ� �ּҸ� ������ �ּ���."
		i3::sprintf(wstrSaveChar, L"%s\n\n%s", wstrName , GAME_RCSTRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));
		return false;
	}
	else
	{
		//	"��/�� ����Ͻ� �� �ֽ��ϴ�."
		UseName(wstrSaveChar, wstrName);
		return true;
	}
}
/*
bool	UIPopupClanBase::_OnEventDuplicateName( const char * pszName, INT32 Arg)
{
	char szTemp[MAX_PATH] = ""; 

	// Ŭ�� ����ȭ��
	if( Arg == 0)		//	Ŭ������ �ߺ����� �ʴ´�
	{	
		//	"��/�� ����Ͻ� �� �ֽ��ϴ�."
		sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s", pszName, GAME_RCSTRING("STBL_IDX_CLAN_CAN_USE"));
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  szTemp);
		return true;
	}
	else		//	Ŭ������ �ߺ��ȴ�.
	{
		_OnPopupCheckOverlapping(szTemp, pszName, true);
		return false;
	}		
}

bool	UIPopupClanBase::_OnEventDuplicateAddress( const char * pszAddress, INT32 Arg)
{
	char szTemp[MAX_PATH] = ""; 

	// Ŭ�� ����ȭ��
	if( Arg == 0)
	{
		//	"��/�� ����Ͻ� �� �ֽ��ϴ�."
		sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s" , 
			pszAddress, GAME_RCSTRING("STBL_IDX_CLAN_CAN_USE"));
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  szTemp);
		return true;
	}
	else
	{			
		_OnPopupCheckOverlappingAddress(szTemp, pszAddress, true);
		return false;
	}
}
*/