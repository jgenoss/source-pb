#if !defined( __UI_LB_EditBox_H__)
#define __UI_LB_EditBox_H__

/*	MFC TextControl �� ����ϰ� ���� Ŭ����
 *		���� i3UIEditBox�� �����
 **/
class Editbox
{
	i3UIEditBox* m_Edit;
	i3::wliteral_range m_wTextRng;

public:
	Editbox();
	virtual ~Editbox();

	void SetControl(i3UIScene * pScene, const char* edit);
	void SetControl(i3UIControl* edit);
	i3UIEditBox* GetControl()	{ return m_Edit; }

	void Clear();

	void SetText(const i3::wstring & text);
	void SetText(const i3::rc_wstring& text);
	void SetText(const i3::stack_wstring& text);
	void SetText(const i3::wliteral_range& text);
	void SetText(const wchar_t* text);

	void SetText(int value);
	void SetText(REAL32 value);

	i3::wliteral_range ExportText() const;

	void SetAlphaRealNumericOnly();
	void SetAlphaNumericOnly();
	void SetTextOnly();

	void EnableReadOnly(bool enable);

	bool IsAlphaNumericOnly() const;
	bool IsTextOnly() const;
	bool IsReadOnly() const;

	void Hide();
	void Show();
	void Enable(bool enable);
	void SetFocus() const;
	bool IsFocused() const;

public:
	bool OnKey_Enter();//����� ��� true ��ȯ
	bool OnKey_Escape(); //����� ��� true ��ȯ
};

#endif