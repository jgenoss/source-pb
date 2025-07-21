#if !defined( __UI_LB_FlowTextBox_H__)
#define __UI_LB_FlowTextBox_H__

/*	Ư�� �ð����� ���ڸ� ����ִ� Ŭ����
 *		
 *	
 **/
class FlowTextBox
{
	i3UIEditBox* m_Edit;
	i3::rc_wstring m_wstr_text;

	REAL32 m_acc_time;
	INT32 m_letterPersecond;
	bool m_finish_text_show;
	REAL32 m_finish_show_time;

private:
	bool Update_Pre(REAL32 rDeltaSeconds) { return false; }
	bool Update_On(REAL32 rDeltaSeconds);
	bool Update_Post(REAL32 rDeltaSeconds);
	bool (FlowTextBox::*m_PF_Update)(REAL32 rDeltaSeconds);

private:
	INT32 letter_copy(i3::rc_wstring& dest, const i3::rc_wstring& src, INT32 letterCount);

public:
	FlowTextBox();
	virtual ~FlowTextBox();

	void SetControl(i3UIScene * pScene, const char* edit);
	void SetControl(i3UIControl* edit);
	i3UIEditBox* GetControl()	{ return m_Edit; }

	void SetText(const i3::rc_wstring & wstr_text);

	//LetterPerSecond : 1�� �� ��Ÿ���� ���ڼ�, finish_text_show : ��� ���� ���� �Ŀ� ǥ�� ����, finish_show_time : ���� �Ŀ� ǥ�� �ð� 
	//finish_text_show : false ��, finish_show_time �� ���õȴ�.
	void Start(INT32 LetterPerSecond, bool finish_text_show = true, REAL32 finish_show_time = 1.f);
	void Finish(bool finish_text_show = true, REAL32 finish_show_time = 1.f);
	bool OnUpdate(REAL32 rDeltaSeconds);

	static UINT32 GetLetterCount(const i3::wliteral_range& wTextRng); //text�� ���ؼ� �� ����Ʈ���� �˷��ش�.
};

#endif