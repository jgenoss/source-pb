#if !defined( _POPUP_CLAN_CREATE_H__)
#define	_POPUP_CLAN_CREATE_H__

#include "PopupBase.h"

#define CREATE_CLANMARK_COMBINATION_MAX		2
#define CREATE_CLANMARK_SHAPE_SLOT_MAX		5
#define CREATE_CLANMARK_COLOR_SLOT_MAX		10

class CPopupClanCreate : public CPopupBase
{
	I3_CLASS_DEFINE( CPopupClanCreate);

	enum { MAX_STRING = 1024 };

protected:

	//Create Popup
	i3GuiStatic *	m_pClanCreateRoot;
	i3GuiStatic	*	m_pClanCreate;

	//	Ŭ�� ������ �ȳ� 
	i3GuiEditBox	*	m_pClanCreateCaption;				// Ŭ�� ����
	i3GuiButton		*	m_pClanCreateOk;					// Ŭ������ Ȯ��
	i3GuiButton		*	m_pClanCreateCancel;				// Ŭ������ ���
	i3GuiEditBox	*	m_pClanCreateGuideValue;			// Ŭ������ ����

	//	Ŭ�� ���� �Է�
	i3GuiStatic	*	m_pClanInputInfo;						// Ŭ������ �Է�
	i3GuiEditBox		*	m_pClanInputInfoCaption;		// Ŭ�� �⺻���� �Է�
	i3GuiButton		*	m_pClanInputInfoOk;					// Ŭ�� �⺻���� �Է� Ȯ��
	i3GuiButton		*	m_pClanInputInfoCancel;				// Ŭ�� �⺻���� �Է� ���

	i3GuiEditBox	*	m_pClanInputInfoClanName;			// Ŭ����
	i3GuiEditBox	*	m_pClanInputInfoClanNameEdit;		// Ŭ���� �Է�
	i3GuiButton		*	m_pClanInputInfoClanNameCheck;		// Ŭ���� �ߺ� üũ
	i3GuiEditBox	*	m_pClanInputInfoClanNameGuide;		// (�ѱ� 8��, ���� 16��)
	i3GuiEditBox	*	m_pClanInputInfoClanIntroduce;		// Ŭ���Ұ�( �ѱ� 120��, ...)
	i3GuiEditBox	*	m_pClanInputInfoClanIntroduceValue;	// Ŭ���Ұ��� �Է�
	i3GuiEditBox	*	m_pClanInputInfoClanAddressGuideStatic;		//	Ŭ�� ����Ʈ �ּ� �Է� �ȳ�
	i3GuiEditBox	*	m_pClanInputInfoClanAddressStatic;	//	Ŭ�� ����Ʈ �⺻ �ּ�
	i3GuiEditBox	*	m_pClanInputInfoClanAddressEdit;	//	Ŭ�� ����Ʈ ���� �ּ� �Է�
	i3GuiStatic     *   m_pClanInputInfoClanAddressBox;
	i3GuiButton		*	m_pClanInputInfoClanAddressCheck;	//	Ŭ�� ����Ʈ �ּ� �ߺ� �˻� ��ư
		
	//	Ŭ�� ��ũ ���
	i3GuiStatic * m_pCreateClanMarkGui;						//	Ŭ�� ��ũ �˾�
	i3GuiEditBox	* m_pCreateClanMarkCaption;				//	'Ŭ�� ��ũ ����'

	i3GuiEditBox	* m_pCreateClanMarkSelectStatic;					//	'SELECT'
	i3GuiEditBox	* m_pCreateClanMarkBackColorStatic;					//	'���� ����'
	i3GuiEditBox	* m_pCreateClanMarkShapeColorStatic;				//	'��� ����'

	i3GuiButton		* m_pCreateClanMarkButtonOk;						//	'Ȯ��'
	i3GuiButton		* m_pCreateClanMarkButtonCancel;					//	'���'
	i3GuiButton		* m_pCreateClanMarkButtonOverlappingCheck;			//	'�ߺ��˻�'

	i3GuiStatic		* m_pCreateClanMarkConfirmImage;					//	���� ��ũ �̹���

	i3GuiButton		* m_pCreateClanMarkPrev[CREATE_CLANMARK_COMBINATION_MAX];		//	�̹��� ���� ��ư
	i3GuiButton		* m_pCreateClanMarkNext[CREATE_CLANMARK_COMBINATION_MAX];		//	�̹��� ���� ��ư

	i3GuiStatic		* m_pCreateClanMarkSelectImage[CREATE_CLANMARK_COMBINATION_MAX][CREATE_CLANMARK_SHAPE_SLOT_MAX];	//	��ũ ���� �̹��� 
	i3GuiStatic		* m_pCreateClanMarkBackColorBox[CREATE_CLANMARK_COLOR_SLOT_MAX];		//	�̹��� ��� ����
	i3GuiStatic		* m_pCreateClanMarkShapeColorBox[CREATE_CLANMARK_COLOR_SLOT_MAX];		//	�̹��� ��� ����
	i3GuiStatic		* m_pCreateClanMarkBackColorSelected;							//	��� ���� ���� ��Ŀ��
	i3GuiStatic		* m_pCreateClanMarkShapeColorSelected;							//	��� ���� ���� ��Ŀ��

	i3Texture *		m_pTexClanMarkFinal;
	i3Texture *		m_pTexClanMarkSelect[CREATE_CLANMARK_COMBINATION_MAX][CREATE_CLANMARK_SHAPE_SLOT_MAX];

	INT32 m_nClanMarkBackImageIdx;		//	1 base (0 unknown)
	INT32 m_nClanMarkShapeImageIdx;		//	1 base (0 unknown)
	INT32 m_nClanMarkBackColorIdx;		//	1 base (0 unknown)
	INT32 m_nClanMarkShapeColorIdx;		//	1 base (0 unknown)

	//	Ŭ�� �̸� �ߺ� �˻�
    i3GuiStatic	*	m_pOverlappingCheck;
	i3GuiEditBox	*	m_pOverlappingCheckCaption;			// Ŭ���� �ߺ� �˻�
	i3GuiButton		*	m_pOverlappingCheckOk;				// Ŭ���� �ߺ��˻� Ȯ��
	i3GuiButton		*	m_pOverlappingCheckCancel;			// Ŭ���� �ߺ��˻� ���
	i3GuiButton		*	m_pOverlappingCheckCheck;			// �ߺ��˻� ��ư

	i3GuiEditBox	*	m_pOverlappingCheckInput;			// �ٸ� Ŭ���� �Է�
	i3GuiEditBox	*	m_pOverlappingCheckInsertedName;	// �ߺ����� �Ǹ�� Ŭ����
	i3GuiEditBox	*	m_pOverlappingCheckInsertedNameGuide;	// �� �̹� ������̰ų� ����Ҽ� ���� �̸��Դϴ�....
	i3GuiEditBox	*	m_pOverlappingCheckInputEdit;		// �ٸ� Ŭ���� �Է�

	//	Ŭ�� �ּ� �ߺ� �˻�
	i3GuiStatic	*	m_pOverlappingAddress;					//	����Ʈ �ּ� �ߺ� �˻� �˾�
	i3GuiEditBox	*	m_pOverlappingAddressCaption;		// ����Ʈ �ߺ� �˻�
	i3GuiButton		*	m_pOverlappingAddressOk;			// ����Ʈ �ߺ��˻� Ȯ��
	i3GuiButton		*	m_pOverlappingAddressCancel;		// ����Ʈ �ߺ��˻� ���
	i3GuiButton		*	m_pOverlappingAddressCheck;			// �ߺ��˻� ��ư

	i3GuiEditBox	*	m_pOverlappingAddressInputStatic;	// �ٸ� ����Ʈ �Է�
	i3GuiEditBox	*	m_pOverlappingAddressInsertedName;	// �ߺ����� �Ǹ�� ����Ʈ
	i3GuiEditBox	*	m_pOverlappingAddressInsertedNameGuide;	// �� �̹� ������̰ų� ����Ҽ� ���� ����Ʈ�Դϴ�....
	i3GuiEditBox	*	m_pOverlappingAddressURL;			// http://cafe.plaync.co.kr.ncpb
	i3GuiEditBox	*	m_pOverlappingAddressInputEdit;		// �ٸ� ����Ʈ �Է�

	//	Ŭ�� ����
	i3GuiStatic	*	m_pSubScription;
	i3GuiEditBox	*	m_pSubScriptionCaption;				// Ŭ�����Կ�û
	i3GuiButton		*	m_pSubScriptionOk;					// Ŭ�� ���� Ȯ��
	i3GuiButton		*	m_pSubScriptionCancel;				// Ŭ�� ���� ���

	i3GuiEditBox	*	m_pSubScriptionReceiver;			// To. 
	i3GuiEditBox	*	m_pSubScriptionReceiverValue;		// ��� Ŭ����
	i3GuiEditBox	*	m_pSubScriptionSenderNick;			// �г���
	i3GuiStatic		*	m_pSubScriptionSenderRank;			// �߽��� ���
	i3GuiEditBox	*	m_pSubScriptionSenderNickValue;		// �߽��� �г��� 

	i3GuiEditBox	*	m_pSubScriptionSenderWin;			// �¼� :
	i3GuiEditBox	*	m_pSubScriptionSenderWinValue;		// �߽��� �¼�
	i3GuiEditBox	*	m_pSubScriptionSenderScore;			// ����	:
	i3GuiEditBox	*	m_pSubScriptionSenderScoreValue;	// �߽��� ����

	i3GuiEditBox	*	m_pSubScriptMessageStatic;			// ���� ��û �޽���*( 100���̳�)

	i3GuiEditBox	*	m_pSubScriptMessageValue;			// ���Խ�û �޽��� ���� �Է�

	CStringFilter *		m_pStringFilter;
	CStringFilter *		m_pStringFilter2;

public:
	CPopupClanCreate(void);
	virtual ~CPopupClanCreate();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual void OnGameEvent( INT32 event, INT32 arg);
	virtual void OnExitKey(void);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);
	
	void	CreateClanMarkOverlappingConfirm(void);
	void	CreateClanMarkNoSignConfirm(void);

	void	StepToCreateClan(void);

	//	�ݹ�
	static void cbSuccessCreateClan(void* pThis, INT32 nParam);
	static void cbQueryCreateClan(void* pThis, INT32 nParam);
	static void cbQuaryJoinClan(void* pThis, INT32 nParam);
	static void cbCancelRequest(void* pThis, INT32 nParam);
	static void cbCreateClanMarkOverlapping(void * pThis, INT32 nParam);
	static void cbCreateClanMarkNoSign(void * pThis, INT32 nParam);

protected:
	BOOL	m_bWaitingConfirmSubscripting;
	BOOL	m_bWaitingConfirmCreateClan;
	BOOL	m_bWaitingIsDuplicateName;
	BOOL	m_bWaitingIsDuplicateAddress;
	BOOL	m_bWaitingIsDuplicateMark;
	BOOL	m_bCheckingName;
	BOOL	m_bCheckingAddress;
	BOOL	m_bCheckedName;
	BOOL	m_bCheckedAddress;
	BOOL	m_bCheckedClanMark;
	BOOL	m_bCheckedClanIntro;
	BOOL	m_bCreateConfirm;
	BOOL	m_bJumpToClanMarkStep;	

	INT32	m_nCurClanIndex;

	void	_CleanupTexture(void);
	void	_CreateConfirm(void);
	void	_CreateCancel(void);
	void	_InputOK(void);
	void	_InputCancel(void);
	void	_InputCheckNick(void);
	void	_InputCheckClanAddress(void);	
	void	_OverLappingOK(void);
	void	_OverLappingCancel(void);
	void	_OverLappingCheck(void);
	void	_OverLappingAddressOK(void);
	void	_OverLappingAddressCancel(void);
	void	_OverLappingAddressCheck(void);
	void	_SubscriptionOK(void);
	void	_SubScriptionCancel(void);
	void	_SubscriptionSendJoin(void);
	void	_SubscriptionCancelRequest(void);
	void	_CreateClanMarkOK(void);
	void	_CreateClanMarkCancel(void);
	void	_CreateClanMarkOverlappingCheck(void);
	void	_CreateClanMarkPrevBackImage(void);
	void	_CreateClanMarkNextBackImage(void);
	void	_CreateClanMarkPrevShapeImage(void);
	void	_CreateClanMarkNextShapeImage(void);
	void	_CreateClanMarkUserSelectImage(INT32 nShapeIdx, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx);
	void	_CreateClanMarkUserFinalImage(void);
	void	_CreateClanMarkUserSelectImageUpdate(INT32 nShapeIdx);
	void	_CreateClanMarkChangeBackColor(INT32 nColorIdx);
	void	_CreateClanMarkChangeShapeColor(INT32 nColorIdx);

	BOOL	_CheckNickNameOverLap( const char * pszText);
	BOOL	_CheckClanAddressOverLap(const char * pszText);
	BOOL	_CheckClanAddressValidate(const char * pszText);

	void	_SendCreateClan(void);

	//
	void	_ResetSubscriptionPage();
	void	_ResetClanCreatePage();

	//
	void	_OnPopupSubScriptionPage(void);
	void	_OnPopupClanCreate( void);
	void	_OnPopupInputInfo(void);
	void	_OnPopupCheckOverlapping( const char * pszName, BOOL bDuplicate);
	void	_OnPopupCheckOverlappingAddress( const char * pszName, BOOL bDuplicate);
	void	_OnPopupCreateClanMark(void);

	void	_OnEventDuplicateName( INT32 Arg);
	void	_OnEventDuplicateAddress( INT32 Arg);
	void	_OnEventSubScriptionPage( INT32 Arg);
	void	_OnEventCreateClan( INT32 Arg);
};

#endif	// #define	_POPUP_CLAN_CREATE_H__
