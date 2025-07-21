#if !defined( __UI_POPUP_USEITEM_H__)
#define __UI_POPUP_USEITEM_H__

#include "UIPopupBase.h"
#if defined( ENABLE_UI2)
//	�����ۻ�� �˾� Ÿ��
enum USE_ITEM_POPUP_TYPE
{
	USE_ITEM_UNKNOWN = -1,
	USE_ITEM_QUERY = 0,				//	����� ��� ����
	USE_ITEM_COMPLETE,				//	��� �Ϸ�

	USE_ITEM_CHANGE_NICK,			//	�г��� ����
	USE_ITEM_CHANGE_CLAN_NAME,		//	Ŭ���̸� ����
	USE_ITEM_COLOR_NICK,			//	�÷� �г���
	USE_ITEM_COLOR_CLAN_NAME,		//	�÷� Ŭ���̸�
	USE_ITEM_COLOR_CROSSHAIR,		//	�÷� ũ�ν����
	USE_ITEM_DISGUISE_NICK,			//	�ӽ� �г���
	USE_ITEM_DISGUISE_RANK,			//	���� ���

};

enum USE_ITEM_STYLE
{
	USE_ITEM_STYLE_NORMAL = 0,
	USE_ITEM_STYLE_BUYING,		//	'�κ��丮���� Ȯ���� �� �ֽ��ϴ�' - ��� ����ϸ� �޽��� ��.
	USE_ITEM_STYLE_RESULT,		//	������ ��� ��� â
};

#define SELECT_COLOR_BOX_COUNT					10	//	���� ���� ���� ����
#define SELECT_CROSSHAIR_COLOR_BOX_COUNT		10	//	���� ���� ���� ����

class UIPopupUseItem : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUseItem);

public:
	UIPopupUseItem();
	virtual ~UIPopupUseItem();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);

	virtual void ProcessGameEvent( INT32 event, INT32 arg);
	virtual bool OnEntranceStart(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual bool OnExitStart();

	void OnCancel(void);
	void OnUseItem(void);

	void OnOpenChangeNick(void);
	BOOL OnChangeNameDuplicate(void);
	BOOL OnChangeNameOk(void);

	void OnChangeColorSelect(INT32 idx);
	void OnChangeCrosshairColorSelect(INT32 idx);

	void OnChangeRankPopupButton(void);
	void OnChangeRankPopupList(UINT32 idxItem);

	static void CbQueryOk(void * pThis, INT32 nParam);
	static void CbChangeNickWarnning(void * pThis, INT32 nParam);		//	�г��� ����� �ӽ� �г��� ������̸� ��� �˾� ó��

protected:
	bool	_Open( USE_ITEM_POPUP_TYPE type);
	void _InitQueryPopup(void);
	void _InitCompletePopup(void);
	void _UseItem(INT32 nItemFlag);
	void _SetInputLock(BOOL bValue);
	void _SetEnablePopup(USE_ITEM_POPUP_TYPE type);
	BOOL _SetCurrentUseItemName(i3GuiEditBox * pControl);
	BOOL _CheckItem(INT32 nItemFlag);

	void _QueryOK(void);
	void _UseItemOk(void);

	void _InitChangeNickPopup(void);
	void _ChangeNickOk(void);
	void _ChangeNickCheck(void);

	void _InitChangeClanNamePopup(void);
	void _ChangeClanNameOk(void);
	void _ChangeClanNameCheck(void);

	void _InitColorNickPopup(void);
	void _ChangeColorNickOk(void);

	void _InitColorClanNamePopup(void);
	void _ChangeColorClanNameOk(void);

	void _InitColorCrosshairPopup(void);
	void _ChangeColorCrosshairOk(void);

	void _InitDisguiseNickPopup(void);
	void _ChangeDisguiseNickOk(void);

	void _InitDisguiseRankPopup(void);
	void _ChangeDisguiseRankOk(void);



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

	//	�г��� ���� �˾�
	i3GuiStatic * m_pChangeNameRoot;
	i3GuiEditBox * m_pChangeNameCaption;
	i3GuiButton * m_pChangeNameButtonOk;
	i3GuiButton * m_pChangeNameButtonCancel;
	i3GuiEditBox * m_pChangeNameItemStatic;
	i3GuiEditBox * m_pChangeNameItemValue;
	i3GuiEditBox * m_pChangeNameBeforeStatic;
	i3GuiEditBox * m_pChangeNameBeforeValue;
	i3GuiEditBox * m_pChangeNameUserStatic;
	i3GuiEditBox * m_pChangeNameUserEdit;
	i3GuiEditBox * m_pChangeNameCheckStatic;
	i3GuiButton * m_pChangeNameCheck;
	i3GuiEditBox * m_pChangeNameNoticeValue;
	BOOL m_bChangeNameChecked;

	//	�÷� ���� �˾�
	i3GuiStatic * m_pChangeColorRoot;
	i3GuiEditBox * m_pChangeColorCaption;
	i3GuiButton * m_pChangeColorButtonOk;
	i3GuiButton * m_pChangeColorButtonCancel;
	i3GuiEditBox * m_pChangeColorItemStatic;
	i3GuiEditBox * m_pChangeColorItemValue;
	i3GuiEditBox * m_pChangeColorCurrentStatic;
	i3GuiEditBox * m_pChangeColorSelectStatic;
	i3GuiEditBox * m_pChangeColorApplyStatic;
	i3GuiEditBox * m_pChangeColorApplyValue;
	i3GuiEditBox * m_pChangeColorNoticeValue;
	i3GuiStatic * m_pChangeColorCurrentColorBox;
	i3GuiStatic * m_pChangeColorSelectColorBoxFrame;
	i3GuiStatic * m_pChangeColorSelectColorBox[SELECT_COLOR_BOX_COUNT];

	//	��� ���� �˾�
	i3GuiStatic * m_pChangeRankRoot;
	i3GuiEditBox * m_pChangeRankCaption;
	i3GuiButton * m_pChangeRankButtonOk;
	i3GuiButton * m_pChangeRankButtonCancel;
	i3GuiEditBox * m_pChangeRankItemStatic;
	i3GuiEditBox * m_pChangeRankItemValue;
	i3GuiEditBox * m_pChangeRankCurrentStatic;
	i3GuiEditBox * m_pChangeRankCurrentValue;
	i3GuiEditBox * m_pChangeRankSelectStatic;
	i3GuiEditBox * m_pChangeRankSelectCombo;
	i3GuiEditBox * m_pChangeRankNoticeValue;
	i3GuiButton * m_pChangeRankSelectButton;
	i3GuiPopupList * m_pChangeRankSelectList;

	//	�÷� ũ�ν���� �˾�
	i3GuiStatic * m_pChangeCrosshairColorRoot;
	i3GuiEditBox * m_pChangeCrosshairColorCaption;
	i3GuiButton * m_pChangeCrosshairColorButtonOk;
	i3GuiButton * m_pChangeCrosshairColorButtonCancel;
	i3GuiEditBox * m_pChangeCrosshairColorItemStatic;
	i3GuiEditBox * m_pChangeCrosshairColorItemValue;
	i3GuiEditBox * m_pChangeCrosshairColorCurrentStatic;
	i3GuiEditBox * m_pChangeCrosshairColorSelectStatic;
	i3GuiEditBox * m_pChangeCrosshairColorNoticeValue;
	i3GuiStatic * m_pChangeCrosshairColorCurrentColorBox;
	i3GuiStatic * m_pChangeCrosshairColorSelectColorBoxFrame;
	i3GuiStatic * m_pChangeCrosshairColorSelectColorBox[SELECT_CROSSHAIR_COLOR_BOX_COUNT];
	i3GuiStatic * m_pChangeCrosshairColorCrossImage;
	i3GuiStatic * m_pChangeCrosshairColorCrossDotImage;
	i3GuiStatic * m_pChangeCrosshairColorDotImage;
	i3GuiStatic * m_pChangeCrosshairColorCircleImage;

	//	��� Ȯ�� �˾�
	i3GuiStatic * m_pCompleteRoot;
	i3GuiEditBox * m_pCompleteCaption;		//	�����ۻ��
	i3GuiButton * m_pCompleteButtonOk;		//	'Ȯ��'
	i3GuiEditBox * m_pCompleteResultStatic;	//	��� ������ �̸�
	i3GuiEditBox * m_pCompleteResultValue;	//	������ ����� ����� ���
	i3GuiEditBox * m_pCompleteNoticeValue;	//	������ ����� ���
	i3GuiStatic * m_pCompleteCrosshair_Cross;
	i3GuiStatic * m_pCompleteCrosshair_CrossDot;
	i3GuiStatic * m_pCompleteCrosshair_Dot;
	i3GuiStatic * m_pCompleteCrosshair_Circle;

	USE_ITEM_POPUP_TYPE m_Type;
	INT32				m_ItemID;
	INT64				m_AuthWareDBIndex;
	USE_ITEM_STYLE		m_Style;
	INT32				m_nSelected;

	char m_szChangeName[MAX_STRING_COUNT];
	CStringFilter *		m_pStringFilter;
	CStringFilter *		m_pStringFilter2;
};

#endif

#endif	// ENABLE_UI2