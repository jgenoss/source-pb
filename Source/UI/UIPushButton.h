#if !defined( __UI_PUSH_BUTTON_H__)
#define __UI_PUSH_BUTTON_H__

#include "UIPopupBase.h"

//CreateRoom�� Ǫ�� ��ư ���� ��� enum.
namespace CR_PushButton
{
	enum BTN_VALUE
	{
		JUMPIN_IMPOSSIBLE = 0, JUMPIN_POSSIBLE, JUMPIN_MAX,
		TEAM_BALANCE_NONE = 0, TEAM_BALANCE_SKILL, TEAM_BALANCE_MEMBER, TEAM_BALANCE_MAX,
		AUTO_TEAM_CHANGE_NOUSE = 0, AUTO_TEAM_CHANGE_USE, AUTO_TEAM_CHANGE_MAX
	};

	enum BTN_TYPE
	{
		TYPE_JUMP_IN = 0,		// ������(����) ��ư
		TYPE_TEAM_BALANCE,		// ���뷱�� ��ư
		TYPE_AUTO_TEAM_CHANGE,	// �ڵ� �������� ��ư
		TYPE_MAX
	};
}

struct PushButton
{
	UINT8 ui8Value;											//��ư�� �к� ��. (ENUM)
	i3UIButton * pNormalBtn;
	i3::string	str_btn_name;

	PushButton(void)
	{
		ui8Value		= 0xFF;
		pNormalBtn		= nullptr;
		str_btn_name.clear();
	}
	
	void SteBtnInfo(UINT8 value, i3UIButton* btn, i3::string btn_name)
	{
		ui8Value		= value;
		pNormalBtn		= btn;
		str_btn_name	= btn_name;
	}

	void SetPushed()			{ pNormalBtn->UnSelectedNormalState();	}
	bool GetPushed(void)		{ return pNormalBtn->isPushFlag();		}
};

class PushButton_LIST
{
public:
	PushButton_LIST();
	~PushButton_LIST();

public:
	/** breif PushBtn���� ������ �س��� ��ư���� �� �ҷ���. ��, Ǫ�� ��ư�� ���� ����Ǿ�� �� */
	void	InitList(i3UIScene * pScene );

	void	PushBtn(i3::string str_btn_name, UINT8 btn_value);

	void	SetValue(UINT8 btn_value);
	UINT8	GetValue()						{ return m_ui8Value; }

private:
	i3::vector<PushButton*>	m_vbtnlist;								//������ �ִ� ��ư��
	UINT8					m_ui8Value;								//������ �ִ� ��ư�� �� Ȱ��ȭ �Ǿ� �ִ� ��ư �к� ��.
};

#endif
