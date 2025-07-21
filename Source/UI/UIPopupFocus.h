#if !defined( __UI_POPUP_FOCUS_H__)
#define __UI_POPUP_FOCUS_H__


class UIPopupFocusCallback
{
public:
	virtual void	ProcessInputDisableMainUI(bool bEnable) = 0;
	virtual void	ProcessInputDisableFloatWindow(bool bEnable) = 0;
	virtual ~UIPopupFocusCallback() {}
};

//
// �����͸� ó���ϸ�, ���� �˾���ü���������� ���� �ʽ��ϴ�.
//
class UIPopupBase;
class CPopupBase;

class UIPopupFocus
{
public:
	UIPopupFocus();
	~UIPopupFocus();

	void	SetCallback( UIPopupFocusCallback* cb);
	void	ClearCallback();

	void	AddFocusPopup( UIPopupBase* popup);
	void	RemoveFocusPopup( UIPopupBase* popup);
	
	bool	IsActivePopup( UIPopupBase* popup) const;
	bool	IsAnyPopupOpened() const { return m_active_popup_list.size() > 0; }
	bool IsTopPopup(UIPopupBase* popup) const;

	UIPopupBase* GetLastestPopup() const;

private:
	i3::vector<UIPopupBase*>	m_active_popup_list;
	UIPopupFocusCallback*	m_cb;
};




#endif