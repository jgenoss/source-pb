#if !defined( __UI_CLAN_FUNCTION_H__)
#define __UI_CLAN_FUNCTION_H__


namespace ucf
{
	// ���ϵ� ��ü�� �θ� �ٴ� �Ϳܿ� ���۷���ī������ �߰� �������� �����Ƿ�, ������ ������ �ʿ�����ϴ�.

	/// �غ��Լ���������, ������ �����Ҽ� ����..
	i3UIImageBoxEx* CreateImageBoxExUV( i3UIControl* pParent, i3Texture *pTex, INT32 u, INT32 v, INT32 w, INT32 h );
	bool			AdjustControlPosToParentCenter(i3UIControl* ctrl);
	

//	CLAN_MARK_SMALL =  0,		//	���� ������
//	CLAN_MARK_MEDIUM = 1,		//	�߰� ������
	
	void 			SelectClanMark(i3UIControl* parent, CLAN_MARK_SIZE type, CLAN_DETAIL_INFO* info, i3UIImageBoxEx*& inout);
	void			SelectClanMark(i3UIControl* parent, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag, i3UIImageBoxEx*& inout);

	i3UIImageBoxEx* CreateClanMark( i3UIControl* pParent, CLAN_MARK_SIZE type, CLAN_DETAIL_INFO* info);
	i3UIImageBoxEx*	CreateClanMark(	i3UIControl* pParent, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag);
	i3UIImageBoxEx*	CreateClanMark(	i3UIControl* pParent, CLAN_MARK_SIZE type, INT32 nBackImageIdx, 
									INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);
	
	i3UIImageBoxEx*	RemoveImageBoxExFromParent( i3UIImageBoxEx* ctrl);		// ū �ǹ̴� ������, ���� ���߱� ���� ����. ctrl�� ������ �޸� �Ұŵɼ� �ֽ��ϴ�.
																			// �θ�Ŭ���� �ҰŽ� �Բ� ���ŵǹǷ� �� ȣ���� �ʿ�� �����ϴ�..
	
	void		MakeComboboxString(i3::wstring& inout, const i3::wliteral_range& input);
	
	class ComboString
	{
	public: 
		ComboString() {}
		ComboString& operator << (const i3::wliteral_range& wRng) { ucf::MakeComboboxString(m_wstr_result, wRng); return *this; }
		operator const i3::wstring& () const { return m_wstr_result; }
		operator i3::wstring& ()  { return m_wstr_result; }
	private:
		i3::wstring m_wstr_result;				// �� ���� �� �ƴ�.
	};
	
}

#endif