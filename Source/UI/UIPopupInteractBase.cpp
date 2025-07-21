#include "pch.h"
#include "UIPopupInteractBase.h"
#include "i3Base/string/ext/mb_to_utf16.h"

//I3_ABSTRACT_CLASS_INSTANCE( UIPopupInteractBase, UIPopupBase);
I3_CLASS_INSTANCE( UIPopupInteractBase);

UIPopupInteractBase::UIPopupInteractBase()
{
	m_SelectedIndex = 0;
}

UIPopupInteractBase::~UIPopupInteractBase()
{
}

/*virtual*/ bool UIPopupInteractBase::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( pArg1 != nullptr)
	{
		// pArg1�� wchar_t Ÿ������ ���� �ٲ��� ���� ������ �����Ѵ�.
		// TODO : ������ �ٲ���� �ֱ� ������ ���߿� ��Ȯ��..(2014.07.10.����)

		// ��Ȯ�� ��� �� Ŭ������ Arg1�� ���� ���̰� ���� ������ �ϴ�...(2014.09.01.����)
		// Unicode�� ��ȯ �� �� Ŭ���� Send Message�� ���� ����մϴ�. (2015.05.22 -> Jinsik.kim)
		i3::safe_string_copy(m_wstrNick, (const wchar_t*)pArg1, NET_NICK_NAME_SIZE);

		//i3::mb_to_utf16((const char*)pArg1, m_wstrNick);
//		i3::safe_string_copy( m_szNick, (const char*) pArg1, NET_NICK_NAME_SIZE );
	}
	else
		m_wstrNick.clear(); 

	if( pArg2 != nullptr)
		m_SelectedIndex = *static_cast<INT32*>(pArg2);
	else
		m_SelectedIndex = -1;

	return true;
}


