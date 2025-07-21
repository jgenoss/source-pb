#if !defined( __UI_POPUP_CLAN_BASE_H__)
#define __UI_POPUP_CLAN_BASE_H__

#include "UIPopupBase.h"

struct SearchInfo {
	bool bDuplicateName;               // �ߺ�����
	bool bDuplicateAddress;            // �ߺ�����
	i3::rc_wstring wstrClanName; // Ŭ�� �̸� �� URL ���� ����
	i3::rc_wstring wstrClanURL;  // Ŭ�� �̸� �� URL ���� ����
};

class UIPopupClanBase : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupClanBase, UIPopupBase);
	CStrFilterPtr		m_StringFilter;
	CStrFilterPtr		m_StringFilter2;

public:
	UIPopupClanBase(void);
	virtual ~UIPopupClanBase(void);

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	bool	_OnPopupCheckOverlapping( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName, bool bDuplicate);			// virtual ����..
	bool	_OnPopupCheckOverlappingAddress( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName, bool bDuplicate);  // virtual ����..

	bool	_CheckNickNameOverLap( const i3::wliteral_range& wTextRng) const;
	bool	_CheckClanAddressOverLap(const i3::wliteral_range& wTextRng) const;
	bool	_CheckClanAddressValidate(const i3::wliteral_range& wTextRng) const;

	// "��/�� ����Ͻ� �� �ֽ��ϴ�." ���� �Լ�
	void    UseName( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName);
};

#endif