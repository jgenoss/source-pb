#ifndef __POPUP_DELETE_ITEM_H
#define __POPUP_DELETE_ITEM_H

#if !defined( ENABLE_UI2)

#include "PopupBase.h"

//	�����ۻ�� �˾� Ÿ��
enum DELETE_ITEM_POPUP_TYPE
{
	DELETE_ITEM_UNKNOWN = -1,
	DELETE_ITEM_QUERY = 0,				//	����� ��� ����
	DELETE_ITEM_COMPLETE,				//	��� �Ϸ�

};

enum DELETE_ITEM_STYLE
{
	DELETE_ITEM_STYLE_NORMAL = 0,
	DELETE_ITEM_STYLE_BUYING,		//	'�κ��丮���� Ȯ���� �� �ֽ��ϴ�' - ��� ����ϸ� �޽��� ��.
	DELETE_ITEM_STYLE_RESULT,		//	������ ��� ��� â
};

#define SELECT_COLOR_BOX_COUNT					10	//	���� ���� ���� ����
#define SELECT_CROSSHAIR_COLOR_BOX_COUNT		10	//	���� ���� ���� ����

class CPopupDeleteItem : public CPopupBase
{
	I3_CLASS_DEFINE( CPopupDeleteItem);

public:
	CPopupDeleteItem(void);
	virtual ~CPopupDeleteItem(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual void OnGameEvent( INT32 event, INT32 arg);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	void OnCancel(void);
	void OnDeleteItem(void);

protected:
	BOOL _Open( DELETE_ITEM_POPUP_TYPE type);
	void _InitQueryPopup(void);
	void _InitCompletePopup(void);
	void _DeleteItem(INT32 nItemFlag);
	void _SetInputLock(BOOL bValue);
	void _SetEnablePopup(DELETE_ITEM_POPUP_TYPE type);
	BOOL _SetCurrentDeleteItemName(i3GuiEditBox * pControl);
	BOOL _CheckItem(INT32 nItemFlag);
	void _QueryOK(void);
	void _DeleteItemOk(void);
	void _GetInvenUseDay(UINT32 Arg, char * pOut);
	
	static void CbQueryOk(void * pThis, INT32 nParam);


protected:
	i3GuiStatic * m_pRoot;

	//	�����ۻ�� �˾�
	i3GuiStatic * m_pQueryRoot;
	i3GuiEditBox * m_pQueryCaption;		//	'�����ۻ��'
	i3GuiButton * m_pQueryButtonOk;		//	'Ȯ��'
	i3GuiButton * m_pQueryButtonCancel;	//	'���'
	i3GuiStatic * m_pQueryItemImage;	//	��� ������ �̹���
	i3GuiEditBox * m_pQueryItemStatic;	//	'��� ������ :'
	i3GuiEditBox * m_pQueryItemValue;	//	��� ������ �̸�
	i3GuiEditBox * m_pQueryAttrStatic;	//	'�Ӽ� :'
	i3GuiEditBox * m_pQueryAttrValue;	//	��� ������ �Ӽ�
	i3GuiEditBox * m_pQueryNotiveValue;		//	��� ������ ����
	i3GuiEditBox * m_pQueryStatic;			//	'�������� ��� �Ͻðڽ��ϱ�?'
	i3GuiEditBox * m_pQueryStatic2;			//	'��� ������ �������� �������� �ʽ��ϴ�.'

	DELETE_ITEM_POPUP_TYPE m_Type;
	INT32				m_ItemID;
	INT64				m_AuthWareDBIndex;
	DELETE_ITEM_STYLE		m_Style;
	INT32				m_nSelected;

	char m_szChangeName[MAX_STRING_COUNT];
	CStringFilter *		m_pStringFilter;
	CStringFilter *		m_pStringFilter2;
};

#endif	// ENABLE_UI2

#endif
