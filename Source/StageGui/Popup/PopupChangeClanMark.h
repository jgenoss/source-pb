#ifndef __POPUP_CLAN_MARK_H
#define __POPUP_CLAN_MARK_H

#include "PopupBase.h"
#include "ClanDef.h"

#define CHANGE_CLANMARK_COMBINATION_MAX		2
#define CHANGE_CLANMARK_SHAPE_SLOT_MAX		5
#define CHANGE_CLANMARK_COLOR_SLOT_MAX		10

class CPopupChangeClanMark : public CPopupBase
{
	I3_CLASS_DEFINE( CPopupChangeClanMark);

public:
	CPopupChangeClanMark(void);
	virtual ~CPopupChangeClanMark(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual void OnGameEvent( INT32 event, INT32 arg);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	void OnOK(void);
	void OnCancel(void);
	void OnOverlappingCheck(void);
	void OnPrevBackImage(void);
	void OnNextBackImage(void);
	void OnPrevShapeImage(void);
	void OnNextShapeImage(void);
	void OnChangeBackColor(INT32 nColorIdx);
	void OnChangeShapeColor(INT32 nColorIdx);

protected:
	i3GuiStatic * m_pGui;
	i3GuiEditBox * m_pCaption;		//	'Ŭ�� ��ũ ����'

	i3GuiEditBox * m_pSelectStatic;		//	'SELECT'
	i3GuiEditBox * m_pBackColorStatic;	//	'���� ����'
	i3GuiEditBox * m_pShapeColorStatic;	//	'��� ����'

	i3GuiButton * m_pButtonOk;			//	'Ȯ��'
	i3GuiButton * m_pButtonCancel;		//	'���'
	i3GuiButton * m_pButtonOverlappingCheck;	//	'�ߺ��˻�'

	i3GuiStatic * m_pClanMarkConfirmImage;		//	���� ��ũ �̹���

	i3GuiButton * m_pClanMarkPrev[CHANGE_CLANMARK_COMBINATION_MAX];		//	�̹��� ���� ��ư
	i3GuiButton * m_pClanMarkNext[CHANGE_CLANMARK_COMBINATION_MAX];		//	�̹��� ���� ��ư

	i3GuiStatic * m_pClanMarkSelectImage[CHANGE_CLANMARK_COMBINATION_MAX][CHANGE_CLANMARK_SHAPE_SLOT_MAX];	//	��ũ ���� �̹��� 
	i3GuiStatic * m_pBackColorBox[CHANGE_CLANMARK_COLOR_SLOT_MAX];			//	�̹��� ��� ����
	i3GuiStatic * m_pShapeColorBox[CHANGE_CLANMARK_COLOR_SLOT_MAX];		//	�̹��� ��� ����
	i3GuiStatic * m_pBackColorSelected;								//	��� ���� ���� ��Ŀ��
	i3GuiStatic * m_pShapeColorSelected;							//	��� ���� ���� ��Ŀ��

	INT64 m_nAuthFlag;

	i3Texture *		m_pTexClanMarkFinal;
	i3Texture *		m_pTexClanMarkSelect[CHANGE_CLANMARK_COMBINATION_MAX][CHANGE_CLANMARK_SHAPE_SLOT_MAX];

	INT32 m_nBackImageIdx;		//	1 base (0 unknown)
	INT32 m_nShapeImageIdx;		//	1 base (0 unknown)
	INT32 m_nBackColorIdx;		//	1 base (0 unknown)
	INT32 m_nShapeColorIdx;		//	1 base (0 unknown)

	BOOL m_bWaitingForCheck;

protected:
	void _InitPopup(UINT32 nFlag);
	void _CleanupTexture(void);
	void _ClanMarkUserSelectImageUpdate(INT32 nShapeIdx);
	void _ClanMarkUserSelectImage(INT32 LayerType, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx);
	void _ClanMarkUserFinalImage(void);
};

#endif
