#include "pch.h"
#include "GameDefine.h"
#include "ClanDef.h"
#include "ClanContext.h"
#include "GlobalVariables.h"
#include "PopupClanCreate.h"
#include "GameContext.h"
#include "ClanMark.h"

I3_CLASS_INSTANCE( CPopupClanCreate, CPopupBase);

CPopupClanCreate::CPopupClanCreate(void)
{
	m_pStringFilter = NULL;
	m_pStringFilter2 = NULL;

	//Create Popup
	m_pClanCreate						= NULL;
	m_pClanCreateCaption				= NULL;	// Ŭ�� ����
	m_pClanCreateOk						= NULL;	// Ŭ������ Ȯ��
	m_pClanCreateCancel					= NULL;	// Ŭ������ ���
	m_pClanCreateGuideValue				= NULL;	// Ŭ������ ����

	m_pClanInputInfo					= NULL;	// Ŭ������ �Է�
	m_pClanInputInfoCaption				= NULL;	// Ŭ�� �⺻���� �Է�
	m_pClanInputInfoOk					= NULL;	// Ŭ�� �⺻���� �Է� Ȯ��
	m_pClanInputInfoCancel				= NULL;	// Ŭ�� �⺻���� �Է� ���

	m_pClanInputInfoClanName			= NULL;	// Ŭ����
	m_pClanInputInfoClanNameEdit		= NULL;	// Ŭ���� �Է�
	m_pClanInputInfoClanNameCheck		= NULL;	// Ŭ���� �ߺ� üũ
	m_pClanInputInfoClanNameGuide		= NULL;	// (�ѱ� 8��, ���� 12��)
	m_pClanInputInfoClanIntroduce		= NULL;	// Ŭ���Ұ�( �ѱ� 120��, ...)
	m_pClanInputInfoClanIntroduceValue	= NULL;	// Ŭ���Ұ��� �Է�

	m_pClanInputInfoClanAddressGuideStatic = NULL;
	m_pClanInputInfoClanAddressStatic = NULL;
	m_pClanInputInfoClanAddressEdit = NULL;
	m_pClanInputInfoClanAddressBox = NULL;
	m_pClanInputInfoClanAddressCheck = NULL;

	m_pOverlappingCheck					= NULL;
	m_pOverlappingCheckCaption			= NULL;	// Ŭ���� �ߺ� �˻�
	m_pOverlappingCheckOk				= NULL;	// Ŭ���� �ߺ��˻� Ȯ��
	m_pOverlappingCheckCancel			= NULL;	// Ŭ���� �ߺ��˻� ���
	m_pOverlappingCheckCheck			= NULL;	// �ߺ��˻� ��ư

	m_pOverlappingCheckInput			= NULL;	// �ٸ� Ŭ���� �Է�
	m_pOverlappingCheckInsertedName		= NULL;	// �ߺ����� �Ǹ�� Ŭ����
	m_pOverlappingCheckInsertedNameGuide= NULL;	// �� �̹� ������̰ų� ����Ҽ� ���� �̸��Դϴ�....
	m_pOverlappingCheckInputEdit		= NULL;	// �ٸ� Ŭ���� �Է�

	m_pOverlappingAddress				= NULL;

	m_pCreateClanMarkGui = NULL;
	m_pCreateClanMarkCaption = NULL;
	m_pCreateClanMarkSelectStatic = NULL;
	m_pCreateClanMarkBackColorStatic = NULL;
	m_pCreateClanMarkShapeColorStatic = NULL;
	m_pCreateClanMarkButtonOk = NULL;
	m_pCreateClanMarkButtonCancel = NULL;
	m_pCreateClanMarkButtonOverlappingCheck = NULL;
	m_pCreateClanMarkConfirmImage = NULL;

	i3mem::FillZero(m_pCreateClanMarkPrev, sizeof(m_pCreateClanMarkPrev));
	i3mem::FillZero(m_pCreateClanMarkNext, sizeof(m_pCreateClanMarkNext));

	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
		for(INT32 j=0; j<CREATE_CLANMARK_SHAPE_SLOT_MAX; j++)
			m_pCreateClanMarkSelectImage[i][j] = NULL;

	for(INT32 i=0; i<CREATE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pCreateClanMarkBackColorBox[i] = NULL;
		m_pCreateClanMarkShapeColorBox[i] = NULL;
	}

	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CREATE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			m_pTexClanMarkSelect[i][j] = NULL;
		}
	}

	m_pTexClanMarkFinal = NULL;

	m_nClanMarkBackImageIdx = 1;
	m_nClanMarkShapeImageIdx = 1;
	m_nClanMarkBackColorIdx = 1;
	m_nClanMarkShapeColorIdx = 1;

	m_pSubScription						= NULL;
	m_pSubScriptionCaption				= NULL;	// Ŭ�����Կ�û
	m_pSubScriptionOk					= NULL;	// Ŭ�� ���� Ȯ��
	m_pSubScriptionCancel				= NULL;	// Ŭ�� ���� ���

	m_pSubScriptionReceiver				= NULL;	// To. 
	m_pSubScriptionReceiverValue		= NULL;	// ��� Ŭ����
	m_pSubScriptionSenderNick			= NULL;	// �г���
	m_pSubScriptionSenderRank			= NULL;	// �߽��� ���
	m_pSubScriptionSenderNickValue		= NULL;	// �߽��� �г��� 

	m_pSubScriptionSenderWin			= NULL;	// �¼� :
	m_pSubScriptionSenderWinValue		= NULL;	// �߽��� �¼�
	m_pSubScriptionSenderScore			= NULL;	// ����	:
	m_pSubScriptionSenderScoreValue		= NULL;	// �߽��� ����

	m_pSubScriptMessageStatic			= NULL;	// ���� ��û �޽���*( 100���̳�)
	m_pSubScriptMessageValue			= NULL;	// ���� ��û �޽��� �Է�â

	m_nCurClanIndex						= -1;

	m_bWaitingConfirmSubscripting		= FALSE;
	m_bWaitingConfirmCreateClan			= FALSE;
	m_bWaitingIsDuplicateName			= FALSE;
	m_bWaitingIsDuplicateAddress		= FALSE;
	m_bWaitingIsDuplicateMark			= FALSE;
	m_bCheckingName						= FALSE;
	m_bCheckingAddress					= FALSE;

	m_bCheckedName = FALSE;
	m_bCheckedAddress = FALSE;
	m_bCheckedClanMark = FALSE;
	m_bCreateConfirm = FALSE;
	m_bJumpToClanMarkStep = FALSE;
}

CPopupClanCreate::~CPopupClanCreate(void)
{
	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);
	
	_CleanupTexture();
}
	
void CPopupClanCreate::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_CLAN_POPUP_CREATE_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateConfirm();
		break;
	case GCI_CLAN_POPUP_CREATE_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateCancel();
		break;
	case GCI_CLAN_POPUP_INPUTINFO_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputOK();
		break;
	case GCI_CLAN_POPUP_INPUTINFO_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCancel();
		break;
	case GCI_CLAN_POPUP_INPUTINFO_CHECKNICK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCheckNick();
		break;	
	case GCI_CLAN_POPUP_INPUTINFO_ADDRESS_CHECK:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCheckClanAddress();
		break;
	case GCI_CLAN_POPUP_OVERLAPPING_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingOK();
		break;
	case GCI_CLAN_POPUP_OVERLAPPING_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingCancel();
		break;
	case GCI_CLAN_POPUP_OVERLAPPING_CHECKNICK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingCheck();
		break;	
	case GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingAddressOK();
		break;
	case GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingAddressCancel();
		break;
	case GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_CHECK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingAddressCheck();
		break;	
	case GCI_CLAN_POPUP_SUBSCRIPTION_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _SubscriptionOK();
		break;
	case GCI_CLAN_POPUP_SUBSCRIPTION_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _SubScriptionCancel();
		break;
	//	Ŭ�� ��ũ
	case GCI_CLAN_B_POPUP_CLANMARK_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkOK();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkCancel();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_OVERLAPPING_CHECK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkOverlappingCheck();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_GROUP_PREV1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkPrevBackImage();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_GROUP_PREV2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkPrevShapeImage();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_GROUP_NEXT1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkNextBackImage();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_GROUP_NEXT2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkNextShapeImage();
		break;
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR1:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR2:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR3:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR4:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR5:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR6:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR7:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR8:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR9:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkChangeBackColor( pNotify->m_nID - GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR1 + 1);
		break;
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR1:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR2:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR3:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR4:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR5:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR6:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR7:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR8:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR9:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkChangeShapeColor( pNotify->m_nID - GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR1 + 1);
		break;
	}
}

BOOL CPopupClanCreate::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist,  "ClanCreate", m_pClanCreateRoot, -1);

	//////////////////////////////////////////	Ŭ�� ���� �ȳ�
	LinkControlEx(plist, 	"clanCreatePopup", m_pClanCreate, -1);
	{
		LinkControlEx(plist,  "clanCreateCaption", m_pClanCreateCaption, -1);								// Ŭ�� ����
		LinkControlEx(plist,  "clanCreateOk", m_pClanCreateOk, GCI_CLAN_POPUP_CREATE_OK);					// Ŭ������ Ȯ��
		LinkControlEx(plist,  "clanCreateCancel", m_pClanCreateCancel, GCI_CLAN_POPUP_CREATE_CANCEL);		// Ŭ������ ���
		LinkControlEx(plist,  "clanCreateGuideValue", m_pClanCreateGuideValue, -1);							// Ŭ������ ����
	}

	//////////////////////////////////////////	Ŭ�� ���� ���� �Է�
	LinkControlEx(plist, 	"clanInputInfoPopup", m_pClanInputInfo, -1);								// Ŭ������ �Է�
	{
		LinkControlEx(plist, "clanInputInfoCaption", m_pClanInputInfoCaption, -1);						// Ŭ�� �⺻���� �Է�
		LinkControlEx(plist, "clanInputInfoOk", m_pClanInputInfoOk, GCI_CLAN_POPUP_INPUTINFO_OK);		// Ŭ�� �⺻���� �Է� Ȯ��
		LinkControlEx(plist, "clanInputInfoCancel", m_pClanInputInfoCancel, GCI_CLAN_POPUP_INPUTINFO_CANCEL);			// Ŭ�� �⺻���� �Է� ���

		LinkControlEx(plist, "clanInputInfoClanNameStatic", m_pClanInputInfoClanName, -1);				// Ŭ����
		LinkControlEx(plist, "clanInputInfoClanNameEdit", m_pClanInputInfoClanNameEdit, -1);			// Ŭ���� �Է�
		LinkControlEx(plist, "clanInputInfoClanNameCheck", m_pClanInputInfoClanNameCheck, GCI_CLAN_POPUP_INPUTINFO_CHECKNICK);	// Ŭ���� �ߺ� üũ
		LinkControlEx(plist, "clanInputInfoClanNameGuideStatic", m_pClanInputInfoClanNameGuide, -1);	// (�ѱ� 8��, ���� 12��)
		LinkControlEx(plist, "clanInputInfoClanIntroduceStatic", m_pClanInputInfoClanIntroduce, -1);	// Ŭ���Ұ�( �ѱ� 120��, ...)
		LinkControlEx(plist, "clanInpuIntroduceValue", m_pClanInputInfoClanIntroduceValue, -1);			// Ŭ���Ұ��� �Է�
		LinkControlEx(plist, "clanInputInfoClanAddressGuideStatic", m_pClanInputInfoClanAddressGuideStatic, -1);	//	Ŭ�� ����Ʈ �ּ� �Է� �ȳ�
		LinkControlEx(plist, "clanInputInfoClanAddressStatic", m_pClanInputInfoClanAddressStatic, -1);	//	Ŭ�� ����Ʈ �⺻ �ּ�
		LinkControlEx(plist, "clanInputInfoClanAddressEdit", m_pClanInputInfoClanAddressEdit, -1);		//	Ŭ�� ����Ʈ ���� �ּ� �Է�
		LinkControlEx(plist, "clanInputInfoClanAddressCheck", m_pClanInputInfoClanAddressCheck, GCI_CLAN_POPUP_INPUTINFO_ADDRESS_CHECK);	//	Ŭ�� ����Ʈ �ּ� �ߺ� �˻� ��ư
		LinkControlEx(plist,  "clanInputInfoClanAddressBox", m_pClanInputInfoClanAddressBox, -1);		
	}

	//////////////////////////////////////////	Ŭ�� �̸� �ߺ� üũ
	LinkControlEx(plist, 	"clanOverlappingCheckPopup", m_pOverlappingCheck, -1);
	{
		LinkControlEx(plist,  "clanOverlappingCheckCaption", m_pOverlappingCheckCaption, -1);									// Ŭ���� �ߺ� �˻�
		LinkControlEx(plist,  "clanOverlappingCheckOk", m_pOverlappingCheckOk, GCI_CLAN_POPUP_OVERLAPPING_OK);				// Ŭ���� �ߺ��˻� Ȯ��
		LinkControlEx(plist,  "clanOverlappingCheckCancel", m_pOverlappingCheckCancel, GCI_CLAN_POPUP_OVERLAPPING_CANCEL);	// Ŭ���� �ߺ��˻� ���
		LinkControlEx(plist,  "clanOverlappingCheckButton", m_pOverlappingCheckCheck, GCI_CLAN_POPUP_OVERLAPPING_CHECKNICK);	// �ߺ��˻� ��ư

		LinkControlEx(plist,  "clanOverlappingCheckInputStatic", m_pOverlappingCheckInput, -1);									// �ٸ� Ŭ���� �Է�
		LinkControlEx(plist,  "clanOverlappingCheckStatic1", m_pOverlappingCheckInsertedName, -1);								// �ߺ����� �Ǹ�� Ŭ����
		LinkControlEx(plist,  "clanOverlappingCheckStatic2", m_pOverlappingCheckInsertedNameGuide, -1);							// �� �̹� ������̰ų� ����Ҽ� ���� �̸��Դϴ�....
		LinkControlEx(plist,  "clanOverlappingCheckInputEdit", m_pOverlappingCheckInputEdit, -1);								// �ٸ� Ŭ���� �Է�
	}

	//////////////////////////////////////////	URL �ߺ� üũ
	LinkControlEx(plist,  "clanOverlappingAddressPopup", m_pOverlappingAddress, -1);
	{
		LinkControlEx(plist, "clanOverlappingAddressCaption", m_pOverlappingAddressCaption, -1);
		LinkControlEx(plist, "clanOverlappingAddressOk", m_pOverlappingAddressOk, GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_OK);
		LinkControlEx(plist, "clanOverlappingAddressCancel", m_pOverlappingAddressCancel, GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_CANCEL);
		LinkControlEx(plist, "clanOverlappingAddressCheck", m_pOverlappingAddressCheck, GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_CHECK);

		LinkControlEx(plist, "clanOverlappingAddressInputStatic", m_pOverlappingAddressInputStatic, -1);
		LinkControlEx(plist, "clanOverlappingAddressStatic1", m_pOverlappingAddressInsertedName, -1);
		LinkControlEx(plist, "clanOverlappingAddressStatic2", m_pOverlappingAddressInsertedNameGuide, -1);
		LinkControlEx(plist, "clanOverlappingAddressURL", m_pOverlappingAddressURL, -1);
		LinkControlEx(plist, "clanOverlappingAddressInputEdit", m_pOverlappingAddressInputEdit, -1);
	}

	//////////////////////////////////////////	Ŭ�� ��ũ ����
	LinkControlEx(plist,  "clanCreateClanMarkPopup", m_pCreateClanMarkGui, GCI_UNKNOWN);
	{
		LinkControlEx(plist,  "clanCreateClanMarkPopupCaption", m_pCreateClanMarkCaption, GCI_UNKNOWN);

		LinkControlEx(plist,  "clanCreateClanMarkSelectStatic", m_pCreateClanMarkSelectStatic, GCI_UNKNOWN);
		LinkControlEx(plist,  "clanCreateClanMarkBackColorStatic", m_pCreateClanMarkBackColorStatic, GCI_UNKNOWN);
		LinkControlEx(plist,  "clanCreateClanMarkShapeColorStatic", m_pCreateClanMarkShapeColorStatic, GCI_UNKNOWN);

		LinkControlEx(plist,  "clanCreateClanMarkOk", m_pCreateClanMarkButtonOk, GCI_CLAN_B_POPUP_CLANMARK_OK);
		LinkControlEx(plist,  "clanCreateClanMarkCancel", m_pCreateClanMarkButtonCancel, GCI_CLAN_B_POPUP_CLANMARK_CANCEL);
		LinkControlEx(plist,  "clanCreateClanMarkOverlappingCheck", m_pCreateClanMarkButtonOverlappingCheck, GCI_CLAN_B_POPUP_CLANMARK_OVERLAPPING_CHECK);

		LinkControlEx(plist,  "clanCreateClanMarkConfirmImage", m_pCreateClanMarkConfirmImage, GCI_UNKNOWN);

		char temp[256] = "";
		for(INT32 i=0; i<2; i++)
		{
			sprintf_s( temp, "clanCreateClanMarkPrev%d", i+1);
			LinkControlEx(plist,  temp, m_pCreateClanMarkPrev[i], GCI_CLAN_B_POPUP_CLANMARK_GROUP_PREV1 + i);

			sprintf_s( temp, "clanCreateClanMarkNext%d", i+1);
			LinkControlEx(plist,  temp, m_pCreateClanMarkNext[i], GCI_CLAN_B_POPUP_CLANMARK_GROUP_NEXT1 + i);

			for(INT32 j=0; j<5; j++)
			{
				sprintf_s( temp, "clanCreateClanMarkSelectImage%d%d", i+1, j+1);
				LinkControlEx(plist,  temp, m_pCreateClanMarkSelectImage[i][j], GCI_UNKNOWN);
			}
		}
	
		LinkControlEx(plist,  "clanCreateClanMarkBackColorStatic", m_pCreateClanMarkBackColorStatic, GCI_UNKNOWN);
		LinkControlEx(plist,  "clanCreateClanMarkBackColorSelected", m_pCreateClanMarkBackColorSelected, GCI_UNKNOWN);
		for(INT32 i=0; i<10; i++)
		{
			sprintf_s(temp, "clanCreateClanMarkBackColorBox%d", i+1);
			LinkControlEx(plist,  temp, m_pCreateClanMarkBackColorBox[i], GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR1 + i);
		}

		LinkControlEx(plist,  "clanCreateClanMarkShapeColorStatic", m_pCreateClanMarkShapeColorStatic, GCI_UNKNOWN);
		LinkControlEx(plist,  "clanCreateClanMarkShapeColorSelected", m_pCreateClanMarkShapeColorSelected, GCI_UNKNOWN);
		for(INT32 i=0; i<10; i++)
		{
			sprintf_s(temp, "clanCreateClanMarkShapeColorBox%d", i+1);
			LinkControlEx(plist,  temp, m_pCreateClanMarkShapeColorBox[i], GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR1 + i);
		}
	}

	////////////////////////////////////////// Ŭ�� ���� 
	LinkControlEx(plist, 	"clanSubscriptionPopup", m_pSubScription, -1);
	{
		LinkControlEx(plist,  "clanSubscriptionPopupCaption",m_pSubScriptionCaption, -1);									// Ŭ�����Կ�û
		LinkControlEx(plist, "clanSubscriptionPopupOk", m_pSubScriptionOk, GCI_CLAN_POPUP_SUBSCRIPTION_OK);				// Ŭ�� ���� Ȯ��
		LinkControlEx(plist, "clanSubscriptionPopupCancel", m_pSubScriptionCancel, GCI_CLAN_POPUP_SUBSCRIPTION_CANCEL);	// Ŭ�� ���� ���

		LinkControlEx(plist,  "clanSubscriptionPopupReceiverStatic",m_pSubScriptionReceiver, -1);			// To. 
		LinkControlEx(plist,  "clanSubscriptionPopupReceiverValue",m_pSubScriptionReceiverValue, -1);		// ��� Ŭ����
		LinkControlEx(plist,  "clanSubscriptionPopupNickStatic",m_pSubScriptionSenderNick, -1);				// �г���
		LinkControlEx(plist,  "clanSubscriptionPopupRankImage", m_pSubScriptionSenderRank, -1);			// �߽��� ���
		LinkControlEx(plist,  "clanSubscriptionPopupNickValue",m_pSubScriptionSenderNickValue, -1);			// �߽��� �г��� 

		LinkControlEx(plist,  "clanSubscriptionPopupWinStatic",m_pSubScriptionSenderWin, -1);				// �¼� :
		LinkControlEx(plist,  "clanSubscriptionPopupWinValue",m_pSubScriptionSenderWinValue, -1);			// �߽��� �¼�
		LinkControlEx(plist,  "clanSubscriptionPopupScoreStatic",m_pSubScriptionSenderScore, -1);			// ����	:
		LinkControlEx(plist,  "clanSubscriptionPopupScoreValue",m_pSubScriptionSenderScoreValue, -1);		// �߽��� ����

		LinkControlEx(plist,  "clanSubscriptionPopupMessageStatic",m_pSubScriptMessageStatic, -1);			// ���� ��û �޽���*( 100���̳�)
		LinkControlEx(plist,  "clanSubscriptionPopupMemoEdit", m_pSubScriptMessageValue, -1);				// ���� ��û �޽��� �Է�â
	}

	return FALSE;
}	

void CPopupClanCreate::OnInitControl(void)
{
	m_pClanCreate->SetEnable( FALSE);
	{
		INIT_WIN_CAPTION( m_pClanCreateCaption, GAME_STRING("STBL_IDX_CLAN_CREATING"));			// Ŭ�� ����
		INIT_CAPTION_BUTTON( m_pClanCreateOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));			// Ȯ��
		INIT_CAPTION_BUTTON( m_pClanCreateCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));	// ���

		m_pClanCreateGuideValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_512, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
		m_pClanCreateGuideValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanCreateGuideValue->SetTextSpace( 0, 5);
		m_pClanCreateGuideValue->SetTextColor(GetColor(GCT_DEFAULT));
	}

	m_pClanInputInfo->SetEnable( FALSE);		// Ŭ������ �Է�
	{
		INIT_WIN_CAPTION( m_pClanInputInfoCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_INPUT_INFO"));	// Ŭ�� �⺻���� �Է�
		INIT_CAPTION_BUTTON( m_pClanInputInfoOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));			// Ȯ��
		INIT_CAPTION_BUTTON( m_pClanInputInfoCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));	// ���
		INIT_STATIC_CAPTION( m_pClanInputInfoClanName, GAME_STRING("STBL_IDX_CLAN_HEADER_NAME"));	// Ŭ����

		//Ŭ���� �Է¿� â�Դϴ�.
		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
		ime._StyleIME = 0;
		m_pClanInputInfoClanNameEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pClanInputInfoClanNameEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanNameEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pClanInputInfoClanNameEdit->SetText("");
		m_pClanInputInfoClanNameEdit->SetIMEMode( IME_MODE_NATIVE);

		INIT_CAPTION_BUTTON( m_pClanInputInfoClanNameCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));			// �ߺ��˻�

		INIT_STATIC_CAPTION( m_pClanInputInfoClanNameGuide, GAME_STRING("STBL_IDX_CLAN_INPUT_NAME_CONDITION"));		//	"(�ѱ� 8��, ���� 16��)"
		INIT_STATIC_CAPTION( m_pClanInputInfoClanIntroduce, GAME_STRING("STBL_IDX_CLAN_INPUT_GUIDE_CONDITION"));		//	"Ŭ���Ұ� (�ѱ� 120�� �̳�)"

		m_pClanInputInfoClanAddressGuideStatic->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanAddressGuideStatic->SetTextColor(GetColor(GCT_DEFAULT));

		//	"Ŭ�� ����Ʈ �ּ�(URL)�� �Է��� �ֽʽÿ�.\n20�� �̳��� ������ ���ڸ� ����� �� ������ ���� ���ڴ� �Է��� �Ұ��� �մϴ�.\n�Է��� ������ �ּҴ� ���� ������ �Ұ��� �մϴ�."
		m_pClanInputInfoClanAddressGuideStatic->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_INPUT_AZIT"));

		I3COLOR color;
		i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
		m_pClanInputInfoClanAddressStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanAddressStatic->SetTextColor(&color);
		m_pClanInputInfoClanAddressStatic->SetText((char *)g_pConfigEx->GetClan().AzitAddressBase);

		//	Ŭ�� ����Ʈ �ּҸ� �Է¿� â�Դϴ�.
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NET_CLAN_AZIT_URL_SIZE - 1;
		ime._StyleIME = IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
		m_pClanInputInfoClanAddressEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pClanInputInfoClanAddressEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanAddressEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pClanInputInfoClanAddressEdit->SetText("");
		m_pClanInputInfoClanAddressEdit->SetAlphaNumericOnly( true);

		INIT_CAPTION_BUTTON(m_pClanInputInfoClanAddressCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));	//	"�ߺ��˻�"

#ifdef CLAN_ADDRESS_ENABLE
		m_pClanInputInfoClanAddressGuideStatic->SetEnable( TRUE);
		m_pClanInputInfoClanAddressStatic->SetEnable( TRUE);
		m_pClanInputInfoClanAddressEdit->SetEnable( TRUE);
		m_pClanInputInfoClanAddressCheck->SetEnable( TRUE);
		m_pClanInputInfoClanAddressBox->SetEnable( TRUE);
#else
		m_pClanInputInfoClanAddressGuideStatic->SetEnable( FALSE);
		m_pClanInputInfoClanAddressStatic->SetEnable( FALSE);
		m_pClanInputInfoClanAddressEdit->SetEnable( FALSE);
		m_pClanInputInfoClanAddressCheck->SetEnable( FALSE);
		m_pClanInputInfoClanAddressBox->SetEnable( FALSE);
#endif

		//Ŭ�� �Ұ��� �Է¿� â�Դϴ�.
		CREATE_IME_ELEMENT ime2;
		ime2._hWnd = g_hWnd;
		ime2._hInst = g_hInstance;
		ime2._nTextLimitCount = 240;//NET_CLAN_INTRO_SIZE - 1;
		ime2._StyleIME = IME_STYLE_MULTILINE;
		m_pClanInputInfoClanIntroduceValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_512, GAME_FONT_DEFAULT_SIZE, TRUE, &ime2);
		m_pClanInputInfoClanIntroduceValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanIntroduceValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_pClanInputInfoClanIntroduceValue->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ADD_HERE"));		//	"���⿡ �Է��ϼ���"
		m_pClanInputInfoClanIntroduceValue->SetTextAutoWrap( TRUE);
		m_pClanInputInfoClanIntroduceValue->SetIMEMode( IME_MODE_NATIVE);
	}

	m_pOverlappingCheck->SetEnable( FALSE);
	{
		INIT_WIN_CAPTION( m_pOverlappingCheckCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_CHECKING_NAME"));	// "Ŭ���� �ߺ� �˻�"
		INIT_CAPTION_BUTTON( m_pOverlappingCheckOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));					// Ȯ��
		INIT_CAPTION_BUTTON( m_pOverlappingCheckCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));			// ���
		INIT_CAPTION_BUTTON( m_pOverlappingCheckCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));		// �ߺ��˻�

		INIT_VALUE_CAPTION( m_pOverlappingCheckInput, GAME_STRING("STBL_IDX_CLAN_CAPTION_SEARCHING_OTHER_CLAN"));		// "�ٸ� �̸� �˻��ϱ�"

		// �ߺ����� �Ǹ�� Ŭ����
		m_pOverlappingCheckInsertedName->SetTextColor( GetColor( GCT_VALUE));
		m_pOverlappingCheckInsertedName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingCheckInsertedName->SetTextColor(GetColor(GCT_VALUE));
		m_pOverlappingCheckInsertedName->SetText("");

		//
		m_pOverlappingCheckInsertedNameGuide->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
		m_pOverlappingCheckInsertedNameGuide->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingCheckInsertedNameGuide->SetTextColor(GetColor(GCT_DEFAULT));
		m_pOverlappingCheckInsertedNameGuide->SetTextSpace( 0, 5);
		//	"��/�� �̹� ������̰ų� ����Ҽ� ���� �̸��Դϴ�. \n �ٸ� �̸��� ������ �ּ���."
		m_pOverlappingCheckInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));

		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
		ime._StyleIME = 0;
		m_pOverlappingCheckInputEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pOverlappingCheckInputEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingCheckInputEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pOverlappingCheckInputEdit->SetText("");
		m_pOverlappingCheckInputEdit->SetIMEMode( IME_MODE_NATIVE);
	}

	m_pOverlappingAddress->SetEnable( FALSE);
	{
		INIT_WIN_CAPTION( m_pOverlappingAddressCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_CHECK_AZIT"));	// "����Ʈ �ּ� �ߺ� �˻�"
		INIT_CAPTION_BUTTON( m_pOverlappingAddressOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));				// Ȯ��
		INIT_CAPTION_BUTTON( m_pOverlappingAddressCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));		// ���
		INIT_CAPTION_BUTTON( m_pOverlappingAddressCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));	// �ߺ��˻� ��ư

		INIT_STATIC_CAPTION( m_pOverlappingAddressInputStatic, GAME_STRING("STBL_IDX_CLAN_SEARCHING_OTHER_AZIT"));	// "�ٸ� ����Ʈ �ּ� �˻��ϱ�"

		// �ߺ����� �Ǹ�� ����Ʈ �ּ�
		m_pOverlappingAddressInsertedName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingAddressInsertedName->SetTextColor(GetColor(GCT_VALUE));
		m_pOverlappingAddressInsertedName->SetText("");

		m_pOverlappingAddressInsertedNameGuide->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
		m_pOverlappingAddressInsertedNameGuide->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingAddressInsertedNameGuide->SetTextColor(GetColor(GCT_DEFAULT));
		m_pOverlappingAddressInsertedNameGuide->SetTextSpace( 0, 5);
		//	"��/�� �̹� ������̰ų� ����Ҽ� ���� �ּ��Դϴ�.\n�ٸ� �ּҸ� ������ �ּ���."
		m_pOverlappingAddressInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_AZIT"));
		
		I3COLOR color;
		i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
		m_pOverlappingAddressURL->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingAddressURL->SetTextColor(&color);
		m_pOverlappingAddressURL->SetText((char *)g_pConfigEx->GetClan().AzitAddressBase);

		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NET_CLAN_AZIT_URL_SIZE - 1;
		ime._StyleIME = IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
		m_pOverlappingAddressInputEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pOverlappingAddressInputEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingAddressInputEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pOverlappingAddressInputEdit->SetText("");
		m_pOverlappingAddressInputEdit->SetAlphaNumericOnly( true);
	}

	m_pCreateClanMarkGui->SetEnable(FALSE);
	{
		INIT_WIN_CAPTION( m_pCreateClanMarkCaption, GAME_STRING("STBL_IDX_CLAN_CREATE_CLAN_MARK"));		//	"Ŭ�� ��ũ ���"

		INIT_WIN_CAPTION(m_pCreateClanMarkSelectStatic, "SELECT");

		INIT_CAPTION_BUTTON(m_pCreateClanMarkButtonOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));	//	"Ȯ��"
		INIT_CAPTION_BUTTON(m_pCreateClanMarkButtonCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));	//	"���"
		INIT_CAPTION_BUTTON(m_pCreateClanMarkButtonOverlappingCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));	//	"�ߺ��˻�"

		INIT_STATIC_CAPTION(m_pCreateClanMarkBackColorStatic, GAME_STRING("STBL_IDX_CLAN_BACK_COLOR"));		//	"���� ����"
		INIT_STATIC_CAPTION(m_pCreateClanMarkShapeColorStatic, GAME_STRING("STBL_IDX_CLAN_SHAPE_COLOR"));	//	"��� ����"

		for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
		{
			INIT_BUTTON_EX( m_pCreateClanMarkPrev[i]);
			INIT_BUTTON_EX( m_pCreateClanMarkNext[i]);
		}

		for(INT32 i=0; i<CREATE_CLANMARK_COLOR_SLOT_MAX; i++)
		{
			m_pCreateClanMarkBackColorBox[i]->setInputDisable(FALSE);
			m_pCreateClanMarkBackColorBox[i]->SetAllShapeEnable(FALSE);
			m_pCreateClanMarkBackColorBox[i]->SetShapeEnable(i, TRUE);

			m_pCreateClanMarkShapeColorBox[i]->setInputDisable(FALSE);
			m_pCreateClanMarkShapeColorBox[i]->SetAllShapeEnable(FALSE);
			m_pCreateClanMarkShapeColorBox[i]->SetShapeEnable(i, TRUE);
		}
	}

	m_pSubScription->SetEnable( FALSE);
	{
		char temp[256] = "";

		INIT_WIN_CAPTION( m_pSubScriptionCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIBER_NOTE"));	// "Ŭ�� ���� ����"
		INIT_CAPTION_BUTTON(m_pSubScriptionOk, GAME_STRING("STBL_IDX_CLAN_SEND"));						// "������"
		INIT_CAPTION_BUTTON(m_pSubScriptionCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));			// "���"

		INIT_STATIC_CAPTION( m_pSubScriptionReceiver, "To.");			// To."
		INIT_VALUE_CAPTION( m_pSubScriptionReceiverValue, "");			// ��� Ŭ����

		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_MEMBER_NAME"));	// "�г��� :"
		INIT_STATIC_CAPTION( m_pSubScriptionSenderNick, temp);	
		m_pSubScriptionSenderRank->SetEnable( FALSE);					// �߽��� ���
		INIT_VALUE_CAPTION( m_pSubScriptionSenderNickValue, "");		// �߽��� �г��� 

		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_WIN_SCORE"));	// "�¼� :"
		INIT_STATIC_CAPTION( m_pSubScriptionSenderWin, temp);			
		INIT_VALUE_CAPTION( m_pSubScriptionSenderWinValue, "");			// �߽��� �¼�

		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_USER_RECORD"));			// "���� :"
		INIT_STATIC_CAPTION( m_pSubScriptionSenderScore, temp);	

		INIT_VALUE_CAPTION( m_pSubScriptionSenderScoreValue, "");		// �߽��� ����

		INIT_STATIC_CAPTION( m_pSubScriptMessageStatic, GAME_STRING("STBL_IDX_CLAN_SUBSCRIBER_NOTE_CONDITION"));		//	"���� ��û�޽���(�ѱ�100�� �̳�)"

		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = 200;//NET_CLAN_REQUEST_SIZE - 1;
		ime._StyleIME = IME_STYLE_MULTILINE;
		m_pSubScriptMessageValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_512, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pSubScriptMessageValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pSubScriptMessageValue->SetTextColor(GetColor(GCT_VALUE));
		m_pSubScriptMessageValue->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ADD_HERE"));	//	"���⿡ �Է��ϼ���"
		m_pSubScriptMessageValue->SetTextSpace( 0, 5);
		m_pSubScriptMessageValue->SetTextAutoWrap( TRUE);
		m_pSubScriptMessageValue->SetIMEMode( IME_MODE_NATIVE);
	}

	CPopupBase::OnInitControl();
}

BOOL CPopupClanCreate::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			if( InputKeyboardEnter())
			{
				if (m_pClanCreate->isEnable())
				{
					_CreateConfirm();
				}
				else if( m_pClanInputInfo->isEnable())	
				{
					if( !m_pClanInputInfoClanIntroduceValue->getIsFocus())
						_InputOK();
				}
				else if( m_pOverlappingCheck->isEnable())
				{
					_OverLappingOK();
				}
				else if( m_pOverlappingAddress->isEnable())
				{
					_OverLappingAddressOK();
				}
				else if( m_pSubScription->isEnable())
				{
					if( !m_pSubScriptMessageValue->getIsFocus())
						_SubscriptionOK();
				}
			}
		}
	}

	return CPopupBase::OnEvent(event, pObj, param2, code);
}

void CPopupClanCreate::OnGameEvent( INT32 event, INT32 arg)
{
	//	�̺�Ʈ ����� �޾� ó���Ѵ�.
	switch( event )
	{
	case EVENT_CLAN_DUPLICATE_NAME:				//	Ŭ�� �̸� �ߺ� üũ
		{
			if( m_bWaitingIsDuplicateName)
			{
				_OnEventDuplicateName( arg);
			}
		}
		break;
	case EVENT_CLAN_DUPLICATE_ADDRESS:			//	Ŭ�� �ּ� �ߺ� üũ
		{
			if( m_bWaitingIsDuplicateAddress)
			{
				_OnEventDuplicateAddress( arg);
			}
		}
		break;
	case EVENT_CLAN_DUPLICATE_MARK:				//	Ŭ�� ��ũ �ߺ� üũ
		{
			if(m_bWaitingIsDuplicateMark)
			{
				if (EV_SUCCESSED(arg))
				{
					m_bCheckedClanMark = TRUE;

					if( !m_bCreateConfirm)
					{
						ACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);

						STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_POSSIBLE_MARK"));	//	"��� ������ Ŭ�� ��ũ�Դϴ�."
					}
				}
				else
				{
					m_bCheckedClanMark = FALSE;

					if( !m_bCreateConfirm)
					{
						DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);

						//	"����� �� ���� Ŭ�� ��ũ�Դϴ�. \n �ٸ� Ŭ�� ��ũ�� �����Ͻðڽ��ϱ�?"
						//	"Ŭ�� ��ũ ���"
						CAPTION_POPUP_Q(GAME_STRING("STBL_IDX_CLAN_MESSAGE_IMPOSSIBLE_MARK_Q"), 
										GAME_STRING("STBL_IDX_CLAN_CREATE_CLAN_MARK"), MAKE_CALLBACK(cbCreateClanMarkOverlapping), this);
					}
				}

				m_bWaitingIsDuplicateMark = FALSE;
			}
		}
		break;
	case EVENT_CLAN_JOIN_REQUEST:				//	Ŭ�� ���� 
		{
			_OnEventSubScriptionPage( arg);
		}
		break;
	case EVENT_CLAN_CREATE:						//	Ŭ�� ����
		{
			_OnEventCreateClan( arg);
		}
		break;
	//case EVENT_CLAN_CANCEL_REQUEST:
	//	{
	//	}
		break;
	}
	
	if( m_bCreateConfirm && m_bCheckedName && m_bCheckedAddress && m_bCheckedClanMark)
	{
		//	Ŭ������ �̺�Ʈ�� ������.
		_SendCreateClan();
		return;
	}
	//	Ŭ�� �̸�, �ּ� �ߺ� üũ ������ Ȯ�ι�ư�� ������ �ٽ� �ߺ�üũ�� ���������� ��ٷȴٰ� Ŭ�� ��ũ ���� �˾����� �Ѿ���� �Ѵ�.
	else if( m_pCreateClanMarkGui->isEnable() == FALSE && m_bJumpToClanMarkStep && 
			m_bCheckedName && m_bCheckedAddress)
	{
		//	Ŭ�� ��ũ ���� ��������
		_OnPopupCreateClanMark();
		return;
	}
}

void CPopupClanCreate::SetInputDisable(BOOL bDisable)
{
	m_pClanCreateRoot->setInputDisable(bDisable);
}

BOOL CPopupClanCreate::GetInputDisable(void)
{
	return m_pClanCreateRoot->getInputDisable();
}

void CPopupClanCreate::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	//	Ŭ�� �Ұ����� Ŭ���ϴ� ���� ���� ������ ������� �������� �����Ѵ�. komet
	if(m_pClanInputInfo->isEnable())
	{
		if(i3String::Length(m_pClanInputInfoClanIntroduceValue->GetText()) == 0)
		{
			m_bCheckedClanIntro = TRUE;
			m_pClanInputInfoClanIntroduceValue->SetTextColor(GetColor(GCT_VALUE));
			m_pClanInputInfoClanIntroduceValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		}
	}

	//	���� �Ұ����� Ŭ���ϴ� ���� ���� ������ ������� �������� �����Ѵ�. komet
	if(m_pSubScription->isEnable())
	{
		if(i3String::Length(m_pSubScriptMessageValue->GetText()) == 0)
		{
			m_pSubScriptMessageValue->SetTextColor(GetColor(GCT_VALUE));
			m_pSubScriptMessageValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		}
	}
}

void CPopupClanCreate::OnPreGuiNotify(void)
{
	CPopupBase::OnPreGuiNotify();
}

void CPopupClanCreate::OnWheelUpdate(INT32 scrollby)
{
	CPopupBase::OnWheelUpdate( scrollby);
}	

void CPopupClanCreate::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	m_pClanCreateRoot->SetEnable(TRUE);

	// _OnPopupClanCreate()�ȿ��� STRING_POPUP()ȣ�� �� return ��,
	// ��� GUI�� �Է��� �ٽ� Ǯ����ϱ⶧���� ���� �����ϴ�.
	((CStageReady*)m_pParent)->SetMenuBarInputDisable( TRUE);

	if( pArg1)
	{
		m_nCurClanIndex = *(INT32*)pArg1;
		_ResetSubscriptionPage();
		_OnPopupSubScriptionPage();
	}
	else
	{
		_ResetClanCreatePage();
		_OnPopupClanCreate();
	}

	//	"Ŭ���� �����ϱ� ���ؼ� {col:253,167,0,255}%s �̻�{/col}�� ����� �ʿ��ϸ�,\n {col:253,167,0,255}%d{/col} ����Ʈ�� �Ҹ�˴ϴ�.\n \n\n Ŭ���� �����Ͻðڽ��ϱ�? \n"
	UINT8	RequirementRank = 0;
	UINT32	RequirementPoint = 0;
	if ( ! g_pFramework->GetClanContext()->GetClanCreationRequirementInfo(&RequirementRank, &RequirementPoint) )
	{
		// ���� �߻��Ѱ��Դϴ�.
		m_pClanCreateOk->SetEnable(FALSE);
	}

	string str = BOOST_FORMAT2(GAME_STRING("STBL_IDX_CLAN_MESSAGE_FOR_CREATING"), GetCharaRank(RequirementRank), RequirementPoint);

	m_pClanCreateGuideValue->SetText(str.c_str());

	if( m_pStringFilter == NULL)
	{
		m_pStringFilter = CStringFilter::NewObject();
		m_pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if( m_pStringFilter2 == NULL)
	{
		m_pStringFilter2 = CStringFilter::NewObject();
		m_pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

void CPopupClanCreate::_ResetSubscriptionPage()
{
	m_pSubScriptMessageValue->SetIMEText( "");
	m_pSubScriptMessageValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSubScriptMessageValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSubScriptMessageValue->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ADD_HERE"));		//	"���⿡ �Է��ϼ���"
}

void CPopupClanCreate::_ResetClanCreatePage()
{
	m_pClanInputInfoClanIntroduceValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pClanInputInfoClanIntroduceValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanInputInfoClanIntroduceValue->SetIMEText("");
	m_pClanInputInfoClanIntroduceValue->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ADD_HERE"));		//	"���⿡ �Է��ϼ���"

	m_pClanInputInfoClanAddressEdit->SetIMEText("");
	m_pClanInputInfoClanAddressEdit->SetIMEMode(IME_MODE_ALPHANUMERIC);

	m_pClanInputInfoClanNameEdit->SetIMEText("");
	m_pClanInputInfoClanNameEdit->SetIMEMode(IME_MODE_NATIVE);
	m_pClanInputInfoClanNameEdit->SetFocus(TRUE);
	i3GuiRoot::setFocusControl( m_pClanInputInfoClanNameEdit);

	m_bCheckedName = FALSE;
	m_bCheckedAddress = FALSE;
	m_bCheckedClanMark = FALSE;
	m_bCheckedClanIntro = FALSE;

#ifdef CLAN_ADDRESS_ENABLE
	m_pClanInputInfoClanAddressGuideStatic->SetEnable( TRUE);
	m_pClanInputInfoClanAddressStatic->SetEnable( TRUE);
	m_pClanInputInfoClanAddressEdit->SetEnable( TRUE);
	m_pClanInputInfoClanAddressCheck->SetEnable( TRUE);
	m_pClanInputInfoClanAddressBox->SetEnable( TRUE);
#else
	m_pClanInputInfoClanAddressGuideStatic->SetEnable( FALSE);
	m_pClanInputInfoClanAddressStatic->SetEnable( FALSE);
	m_pClanInputInfoClanAddressEdit->SetEnable( FALSE);
	m_pClanInputInfoClanAddressCheck->SetEnable( FALSE);
	m_pClanInputInfoClanAddressBox->SetEnable( FALSE);

	m_bCheckedAddress = TRUE;
#endif
}

void CPopupClanCreate::OnClosePopup(void * pArg1, void * pArg2)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable(FALSE);
	m_pCreateClanMarkGui->SetEnable(FALSE);
	m_pSubScription->SetEnable( FALSE);

	((CStageReady*)m_pParent)->SetMenuBarInputDisable( FALSE);

	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);

	_CleanupTexture();

	m_pClanCreateRoot->SetEnable(FALSE);

	CPopupBase::OnClosePopup( pArg1, pArg2);
}

void CPopupClanCreate::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupClanCreate::_CleanupTexture(void)
{
	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CREATE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			I3_SAFE_RELEASE( m_pTexClanMarkSelect[i][j]);
		}
	}

	I3_SAFE_RELEASE(m_pTexClanMarkFinal);
}

void CPopupClanCreate::_CreateConfirm(void)
{
	UINT32 MyPoint = g_pGameContext->GetMyPoint();

	UINT32 wRank = g_pGameContext->GetRealRank();

	UINT8	RequirementRank = 0;
	UINT32	RequirementPoint = 0;
	if ( ! g_pFramework->GetClanContext()->GetClanCreationRequirementInfo(&RequirementRank, &RequirementPoint) )
	{
		return;
	}

	if( MyPoint > RequirementPoint && wRank >= RequirementRank)
	{
		_OnPopupInputInfo();
		return;
	}

	if( MyPoint < RequirementPoint)
	{ 
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_NO_POINT"));		//	"����Ʈ�� �����մϴ�."
	}
	else if( wRank < RequirementRank)
	{
		string str = BOOST_FORMAT2("{col:253,167,0,255}%s{/col} %s", GetCharaRank( RequirementRank), GAME_STRING("STBL_IDX_CLAN_NEED_HIGH_RANK"));
		STRING_POPUP(str.c_str());
	}
	else
	{
		//	"�˼��մϴ�. Ŭ�� ������ �ʿ��� ��޿�\n�������� ���Ͽ��ų� ����Ʈ�� �����մϴ�.\n������ �ٽ� �õ��� �ּ���."
#if defined(LOCALE_CHINA)
		char szColMsg[512] = "";
		char str[512] = "";
		sprintf( szColMsg, "{col:253,167,0,255}%s{/col}", GetCharaRank( RequirementRank));
		sprintf( str, GAME_STRING("STBL_IDX_CLAN_NEED_HIGH_RANK"), szColMsg);
		STRING_POPUP(str);
#else
		string str = BOOST_FORMAT2("{col:253,167,0,255}%s{/col} %s", GetCharaRank( RequirementRank), GAME_STRING("STBL_IDX_CLAN_NEED_HIGH_RANK"));
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANNOT_CREATING_CLAN"));
#endif
	}

	OnClosePopup();

}

void CPopupClanCreate::_CreateCancel(void)
{
	OnClosePopup();
}

void CPopupClanCreate::_InputOK(void)
{
	char pNick[MAX_STRING_COUNT] = {0,};
	i3String::Copy( pNick, m_pClanInputInfoClanNameEdit->getIMETextBuf(), MAX_STRING_COUNT );

	const char * pAddress = m_pClanInputInfoClanAddressEdit->GetText();

	m_bCheckingName = FALSE;
	m_bCheckingAddress = FALSE;

	if( m_bWaitingIsDuplicateName || m_bWaitingIsDuplicateAddress)
		return;
	
	//	check step 1
	if( i3String::Length(pNick) <= 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_NAME"));		//	"Ŭ������ �����ּ���"
		return;
	}

	//	check step 2
	if( !m_bCheckedName)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_NAME"));		//	"Ŭ���� �ߺ��˻縦 ���ֽʽÿ�."
		return;
	}

#ifdef CLAN_ADDRESS_ENABLE
	//	check step 3
	if( i3String::Length(pAddress) <= 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	// "Ŭ�� ����Ʈ �ּҸ� �����ּ���."
		return;
	}

	//	check step 4
	if( !m_bCheckedAddress)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_AZIT"));	//	"Ŭ�� ����Ʈ �ּ� �ߺ��˻縦 ���ֽʽÿ�."
		return;
	}
	
	//	check step 5
	{
		CStringFilter* pStringFilter = CStringFilter::NewObject();
		pStringFilter->CreateFilter(STR_FILTER_INCLUDE_CHAT_NICK);

		if( pStringFilter->IsFilterSame( pAddress ) == FALSE ) {
			//	"Ŭ�� ����Ʈ �ּҿ� �Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�"
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_AZIT"));
			m_pClanInputInfoClanAddressEdit->SetIMEText("");
			I3_SAFE_RELEASE( pStringFilter );
			return;
		}

		I3_SAFE_RELEASE( pStringFilter );
	}
#endif

	//	check step 6
	{
		char pszString[CPopupClanCreate::MAX_STRING] = {0,};
		i3String::Copy( pszString, m_pClanInputInfoClanIntroduceValue->getIMETextBuf(), CPopupClanCreate::MAX_STRING );
		
		if( m_bCheckedClanIntro == FALSE || i3String::Length( pszString ) == 0 ) 
		{
			//	"Ŭ�� �Ұ��� �ùٸ��� �ʽ��ϴ�.\n�ٽ� Ȯ���� �ֽʽÿ�."
			STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_WRONG_CLAN_GUIDE") );
			return;
		}
//		INT32 idxBlank = i3String::ContainChar(pszString, ' ');

		if( FALSE == m_pStringFilter2->IsFilter( pszString ) ||
			FALSE == m_pStringFilter->IsFilterSame( pszString ) || 
			FALSE == g_pFramework->IsFilter( pszString ) ) 
		{
			//	"Ŭ�� �Ұ��ۿ� �Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�"
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_CLAN_GUIDE"));

			m_pClanInputInfoClanIntroduceValue->SetIMEText("");

			return;
		}
	}

	//	check step 6
	if( _CheckNickNameOverLap( pNick))
	{
		if( _CheckClanAddressValidate( pAddress))
		{
			if( _CheckClanAddressOverLap( pAddress))
			{
				//STRING_POPUP("�̴�� �����Ͻðڽ��ϱ�?", cbQueryCreateClan);

				StepToCreateClan();
			}
		}
		else
		{
			m_pClanInputInfoClanAddressEdit->SetText("");
			m_pClanInputInfoClanAddressEdit->SetIMEText("");
		}
	}
}

BOOL CPopupClanCreate::_CheckNickNameOverLap(const char * pszText)
{
	if( m_bWaitingIsDuplicateName )
		return FALSE;

	if( i3String::Length(pszText) <= 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_NAME"));	//	"Ŭ������ �����ּ���"
		return FALSE;
	}

	{
		bool GoodNickName = true;

		INT32 idxBlank = i3String::ContainChar(pszText, ' ');

		if( -1 != idxBlank )
		{
			_LOG_("NickName Contained Blank");
			GoodNickName = false;
		}
		//else if( FALSE == m_pStringFilter2->IsFilter( pszText ) )
		else if( FALSE == m_pStringFilter2->IsFilterNick( pszText ) )
		{
			_LOG_("NickName Detected IsFilter");
			GoodNickName = false;
		}
		else if( FALSE == m_pStringFilter->IsFilterSame( pszText ) )
		{
			_LOG_("NickName Detected IsFilterSame");
			GoodNickName = false;
		}
		//else if( FALSE == g_pFramework->IsFilter( pszText ) )
		else if( FALSE == g_pFramework->IsFilterNick( pszText ) )
		{
			_LOG_("NickName Detected IsFilter");
			GoodNickName = false;
		}
		else if( FALSE == g_pFramework->CheckValidCharacterByLocale( pszText ) )
		{
			_LOG_("NickName Detected CheckValidCharacterByLocale");
			GoodNickName = false;
		}

		if( !GoodNickName )
		{
			//	�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�"
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));

			m_pClanInputInfoClanNameEdit->SetIMEText("");
			m_pOverlappingCheckInputEdit->SetIMEText("");

			return FALSE;
		}
	}
	m_bWaitingIsDuplicateName = TRUE;

	//	Ŭ�� �ּ� �ߺ� üũ�� ���� ���� �ʵ���
	if(!m_bWaitingIsDuplicateAddress)
 		m_bCheckedName = FALSE; 

	return g_pFramework->GetClanContext()->SendOverlappingClanName(pszText);
}

BOOL CPopupClanCreate::_CheckClanAddressOverLap(const char * pszText)
{
#ifdef CLAN_ADDRESS_ENABLE
 	if( m_bWaitingIsDuplicateAddress) 
		return FALSE;

	if( i3String::Length(pszText) <= 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	//	"Ŭ�� ����Ʈ �ּҸ� �����ּ���."

		return FALSE;
	}
	
	{
		INT32 idxBlank = i3String::ContainChar(pszText, ' ');

		if( -1 != idxBlank || 
			FALSE == m_pStringFilter2->IsFilter( pszText ) || 
			FALSE == m_pStringFilter->IsFilterSame( pszText ) || 
			FALSE == g_pFramework->IsFilter( pszText ) ) 
		{
			//	"�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�."
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));

			m_pClanInputInfoClanAddressEdit->SetIMEText("");
			m_pOverlappingAddressInputEdit->SetIMEText("");

			return FALSE;
		}

		//	check step 5
		{
			CStringFilter* pStringFilter = CStringFilter::NewObject();
			pStringFilter->CreateFilter(STR_FILTER_INCLUDE_CHAT_NICK);

			if( pStringFilter->IsFilterSame( pszText ) == FALSE ) {
				//	"�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�."
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
				m_pClanInputInfoClanAddressEdit->SetIMEText("");
				m_pClanInputInfoClanAddressEdit->SetIMEText("");
				I3_SAFE_RELEASE( pStringFilter );
				return FALSE;
			}

			I3_SAFE_RELEASE( pStringFilter );
		}
	}

	m_bWaitingIsDuplicateAddress = TRUE;

	//	Ŭ���� �ߺ� üũ�� ���� ���� �ʵ���
	if(!m_bWaitingIsDuplicateName)
		m_bCheckedAddress = FALSE;
	
	return g_pFramework->GetClanContext()->SendOverlappingClanAddress(pszText);

#else
	m_bCheckedAddress = TRUE;

	return TRUE;

#endif
}

BOOL CPopupClanCreate::_CheckClanAddressValidate(const char * pszText)
{
#ifdef CLAN_ADDRESS_ENABLE
	const char * ch = pszText;

	if( *ch == 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	//	"Ŭ�� ����Ʈ �ּҸ� �����ּ���."

		m_bCheckedAddress = FALSE;

		return FALSE;
	}

	while(*ch)
	{
		//	Ŭ�� �ּҴ� ���� �ҹ��ڿ� ���ڷθ� �Է� �����ϹǷ� ������ üũ�Ѵ�. (0 ~ 9 && a ~ z)
		if( (*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'z')) 
		{ 
			ch++;

			continue;
		}
		else
		{
			//	"Ŭ�� ����Ʈ URL��\n���� �ҹ��ڿ� ���ڸ� ��� �����մϴ�."
			//	"����Ʈ �ּ� ����"
			CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_ONLY_ENGLISH_OR_NUMERIC"), 
						GAME_STRING("STBL_IDX_CLAN_CAPTION_ERROR_AZIT"));
			
			m_bCheckedAddress = FALSE;

			return FALSE;
		}
	}	

	return TRUE;
#else
	
	m_bCheckedAddress = TRUE;

	return TRUE;
#endif
}

void CPopupClanCreate::_InputCancel(void)
{
	OnClosePopup();
}

void CPopupClanCreate::_InputCheckNick(void)
{
	m_bCheckingName = TRUE;

	char pszBuffer[MAX_STRING_COUNT] = {0,};
	i3String::Copy( pszBuffer, m_pClanInputInfoClanNameEdit->getIMETextBuf(), MAX_STRING_COUNT );
	
	_CheckNickNameOverLap( pszBuffer);
}

void CPopupClanCreate::_InputCheckClanAddress(void)
{
	const char * pszAddress = m_pClanInputInfoClanAddressEdit->GetText();

	m_bCheckingAddress = TRUE;

	if( _CheckClanAddressValidate( pszAddress))
	{
		_CheckClanAddressOverLap( pszAddress);
	}
	else
	{
		m_pClanInputInfoClanAddressEdit->SetText("");
		m_pClanInputInfoClanAddressEdit->SetIMEText("");
	}
}

void CPopupClanCreate::_SendCreateClan(void)
{
	const char * pszClanName = m_pClanInputInfoClanNameEdit->getIMETextBuf();
	const char * pszIntro = m_pClanInputInfoClanIntroduceValue->getIMETextBuf();
	const char * pszURL = m_pClanInputInfoClanAddressEdit->getIMETextBuf();

	//	Ŭ���� ���̴� Ŭ�� ������ �Ұ����Ѵ�.
	if( i3String::Length(pszClanName) == 0 )
	{
		m_bCreateConfirm = FALSE;
		m_bCheckedName = FALSE;

		return;
	}

#ifdef CLAN_ADDRESS_ENABLE
	//	Ŭ���ּ� ���̴� Ŭ�� ������ �Ұ����Ѵ�.
	if( i3String::Length(pszURL) == 0)
	{
		m_bCreateConfirm = FALSE;
		m_bCheckedAddress = FALSE;

		return;
	}
#endif

	if( m_bCreateConfirm && !m_bWaitingConfirmCreateClan &&	m_bCheckedName && m_bCheckedAddress && m_bCheckedClanMark)
	{		
		UINT32 nMarkFlag = 0;

#if defined(USE_CLAN_MARK) && defined(ENABLE_CREATE_CLAN_MARK)
		//	1 base (0 is default)
		I3_BOUNDCHK(m_nClanMarkBackImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
		I3_BOUNDCHK(m_nClanMarkShapeImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
		I3_BOUNDCHK(m_nClanMarkBackColorIdx, CClanMarkManager::GetMarkColorCount()+1);
		I3_BOUNDCHK(m_nClanMarkShapeColorIdx, CClanMarkManager::GetMarkColorCount()+1);

		nMarkFlag = MAKE_CLANMARK_FLAG( m_nClanMarkBackImageIdx, m_nClanMarkShapeImageIdx, m_nClanMarkBackColorIdx, m_nClanMarkShapeColorIdx);
#endif

		//	Ŭ�� ���� �̺�Ʈ�� ������.	
		g_pFramework->GetClanContext()->SendCreateClan(pszClanName, pszIntro, pszURL, nMarkFlag);
		
		m_bWaitingConfirmCreateClan = TRUE;
		m_bCreateConfirm = FALSE;
	}
	else if( m_bWaitingConfirmCreateClan)
	{
		//	"\n ��ø� ��ٷ��ֽʽÿ�.\n �̹� ��û�Ͽ����ϴ�."
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_WAIT"));
	}
}

void CPopupClanCreate::_OverLappingOK(void)
{
	char pszName[MAX_STRING_COUNT] = {0,};
	i3String::Copy( pszName, m_pOverlappingCheckInputEdit->getIMETextBuf(), MAX_STRING_COUNT );

	if( !m_bCheckedName)
	{
		//	"Ŭ���� �ߺ��˻縦 ���ֽʽÿ�."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_NAME"));
		return;
	}

	//if confirmed
	m_bCheckingName = FALSE;

	_CheckNickNameOverLap( pszName);
}

void CPopupClanCreate::_OverLappingCancel(void)
{
	_OnPopupInputInfo();
}

void CPopupClanCreate::_OverLappingCheck(void)
{
	m_bCheckingName = TRUE; 

	char pszName[MAX_STRING_COUNT] = {0,};
	i3String::Copy( pszName, m_pOverlappingCheckInputEdit->getIMETextBuf(), MAX_STRING_COUNT );

	_CheckNickNameOverLap( pszName);
}

void CPopupClanCreate::_OverLappingAddressOK(void)
{
	const char * pszAddress = m_pOverlappingAddressInputEdit->GetText();

	if( i3String::Length(pszAddress) <= 0)
	{
		//	"Ŭ�� ����Ʈ �ּҸ� �����ּ���"
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));
		return;
	}

	if( !m_bCheckedAddress)
	{
		//	"Ŭ�� ����Ʈ �ּ� �ߺ��˻縦 ���ֽʽÿ�."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_AZIT"));
		return;
	}

	//if confirmed
	m_bCheckingAddress = FALSE;

	if( _CheckClanAddressValidate( pszAddress))
	{
		_CheckClanAddressOverLap( pszAddress); 
	}
	else
	{
		m_pOverlappingAddressInputEdit->SetText("");
		m_pOverlappingAddressInputEdit->SetIMEText("");
	}
}

void CPopupClanCreate::_OverLappingAddressCancel(void)
{
	_OnPopupInputInfo();
}

void CPopupClanCreate::_OverLappingAddressCheck(void)
{
	const char * pszAddress = m_pOverlappingAddressInputEdit->GetText();

	m_bCheckingAddress = TRUE;

	if( _CheckClanAddressValidate( pszAddress))
	{
		_CheckClanAddressOverLap( pszAddress); 
	}
	else
	{
		m_pOverlappingAddressInputEdit->SetText("");
		m_pOverlappingAddressInputEdit->SetIMEText("");
	}
}

void CPopupClanCreate::_SubscriptionOK(void)
{
	if( m_bWaitingConfirmSubscripting )
		return;

	{
		const char * pszText = m_pSubScriptMessageValue->GetText();

		if( FALSE == m_pStringFilter2->IsFilter( pszText ) || 
			FALSE == m_pStringFilter->IsFilterSame( pszText ) || 
			FALSE == g_pFramework->IsFilter( pszText ) ) 
		{
			//	"�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�."
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));

			m_pSubScriptMessageValue->SetIMEText( "");

			return;
		}
	}


	//	"������ ��û�Ͻðڽ��ϱ�?"
	//	"Ŭ�� ���� ��û"
	CAPTION_POPUP_Q(GAME_STRING("STBL_IDX_CLAN_MESSAGE_REQUEST_SUBSCRIPTION_Q"), 
					GAME_STRING("STBL_IDX_CLAN_CAPTION_REQUEST_SUBSCRIPTION"), MAKE_CALLBACK(cbQuaryJoinClan), this);
}

void CPopupClanCreate::_SubscriptionSendJoin(void)
{
	g_pFramework->GetClanContext()->SendJoinToClanMember( g_pGameContext->getClanDetailInfo()->_idx,
														  m_pSubScriptMessageValue->getIMETextBuf() );

	m_bWaitingConfirmSubscripting = TRUE;
}

void CPopupClanCreate::_SubScriptionCancel(void)
{
	OnClosePopup();
}

void CPopupClanCreate::_SubscriptionCancelRequest(void)
{
	g_pFramework->GetClanContext()->SendCancelJoinFromClanMember();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupClanCreate(void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
	if(( MyUserInfoBasic._clanidx != 0) && (MyUserInfoBasic._clanstate == 0))
	{
		//	"�̹� Ŭ�������� ��û�� �����Դϴ�.\n���� ��û�� ����Ͻðڽ��ϱ�?"
		//	"���� ��û ���"
		CAPTION_POPUP_Q( GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANCEL_SUBSCRIPTION_Q"), 
						GAME_STRING("STBL_IDX_CLAN_CAPTION_CANCEL_SUBSCRIPTION"), MAKE_CALLBACK(cbCancelRequest), this);

		((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
		OnClosePopup();
		return;
	}

    m_pClanCreate->SetEnable( TRUE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable( FALSE);
	m_pCreateClanMarkGui->SetEnable(FALSE);

	m_bCreateConfirm = FALSE;
	m_bWaitingConfirmCreateClan = FALSE;
	m_bCheckingName = FALSE;
	m_bCheckingAddress = FALSE;
	m_bCheckedName = FALSE;
	m_bCheckedAddress = FALSE;
	m_bCheckedClanMark = FALSE;
	m_bJumpToClanMarkStep = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupInputInfo(void)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( TRUE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable(FALSE);
	m_pCreateClanMarkGui->SetEnable(FALSE);

	m_bCreateConfirm = FALSE;

	m_pClanInputInfoClanNameEdit->SetFocus(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupCheckOverlapping( const char * pszName, BOOL bDuplicate)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( TRUE);
	m_pOverlappingAddress->SetEnable(FALSE);
	m_pCreateClanMarkGui->SetEnable(FALSE);

	m_pOverlappingCheckInsertedName->SetText( pszName);

	if( bDuplicate)
	{
		//	"��/�� �̹� ������̰ų� ����Ҽ� ���� �̸��Դϴ�. \n �ٸ� �̸��� ����� �ֽʽÿ�."
		m_pOverlappingCheckInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));

		m_pOverlappingCheckInputEdit->SetText("");
		m_pOverlappingCheckInputEdit->SetIMEText("");
	}
	else
	{
		m_pOverlappingCheckInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_CAN_USE"));	//	"��/�� ����Ͻ� �� �ֽ��ϴ�."
	}

	m_bCreateConfirm = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupCheckOverlappingAddress( const char * pszName, BOOL bDuplicate)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable(TRUE);
	m_pCreateClanMarkGui->SetEnable(FALSE);

	m_pOverlappingAddressInsertedName->SetText( pszName);

	if( bDuplicate)
	{
		//	"��/�� �̹� ������̰ų� ����Ҽ� ���� �ּ��Դϴ�.\n �ٸ� �ּҸ� ������ �ּ���."
		m_pOverlappingAddressInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_AZIT"));

		m_pOverlappingAddressInputEdit->SetText("");
		m_pOverlappingAddressInputEdit->SetIMEText("");
	}
	else
	{
		//	"��/�� ����Ͻ� �� �ֽ��ϴ�."
		m_pOverlappingAddressInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_CAN_USE"));
	}

	m_bCreateConfirm = FALSE;
}

void CPopupClanCreate::_OnPopupCreateClanMark(void)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable(FALSE);
	m_pCreateClanMarkGui->SetEnable(TRUE);

	//	1 base
	m_nClanMarkBackImageIdx = 1;
	m_nClanMarkShapeImageIdx = 1;
	m_nClanMarkBackColorIdx = 1;
	m_nClanMarkShapeColorIdx = 1;

	m_bCheckedClanMark = FALSE;
	m_bCreateConfirm = FALSE;
	m_bWaitingIsDuplicateMark = FALSE;	

	_CleanupTexture();

	//	���ÿ� Ŭ�� ��ũ ���� �̹����� �̸� ����
	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CREATE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			//	�ؽ�ó ����
			m_pTexClanMarkSelect[i][j] = i3Texture::NewObject(); 
			I3ASSERT( m_pTexClanMarkSelect[i][j] != NULL);

			m_pTexClanMarkSelect[i][j]->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);
		}
	}

	m_pTexClanMarkFinal = i3Texture::NewObject(); 
	I3ASSERT( m_pTexClanMarkFinal != NULL);
	m_pTexClanMarkFinal->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
	ACTIVATE_BUTTON(m_pCreateClanMarkButtonOverlappingCheck);

	_CreateClanMarkChangeBackColor(m_nClanMarkBackColorIdx);
	_CreateClanMarkChangeShapeColor(m_nClanMarkShapeColorIdx);
	
	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		ACTIVATE_BUTTON(m_pCreateClanMarkPrev[i]);
		ACTIVATE_BUTTON(m_pCreateClanMarkNext[i]);

		_CreateClanMarkUserSelectImageUpdate(i);
	}

	for(INT32 i=0; i<CREATE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pCreateClanMarkBackColorBox[i]->setInputDisable(FALSE);
		m_pCreateClanMarkShapeColorBox[i]->setInputDisable(FALSE);
	}
}

void CPopupClanCreate::_CreateClanMarkOK(void)
{	
	if( !m_bWaitingIsDuplicateMark)
	{
		m_bCreateConfirm = TRUE;
		m_bCheckedClanMark = FALSE;

		//	������ Ŭ�� ��ũ�� üũ�ؾ� Ŭ���� ������ �� �ֽ��ϴ�
		_CreateClanMarkOverlappingCheck();
	}
}

void CPopupClanCreate::_CreateClanMarkCancel(void)
{	
	OnClosePopup();
}

void CPopupClanCreate::cbCreateClanMarkOverlapping(void * pThis, INT32 nParam)
{
	if( nParam == MBR_CANCEL)
	{
		CPopupClanCreate * pPopup = (CPopupClanCreate *) pThis;

		pPopup->CreateClanMarkOverlappingConfirm();
	}
}

void CPopupClanCreate::CreateClanMarkOverlappingConfirm(void)
{
	//	"Ŭ�� ��ũ ����� ����Ͻø� \n ���� Ŭ�� ��ũ ����� �������� ������ �ʿ��մϴ�. \n �׷��� ����ϰڽ��ϱ�?"
	//	"Ŭ�� ��ũ ���"
	CAPTION_POPUP_Q(GAME_STRING("STBL_IDX_CLAN_CANCEL_CREATE_CLAN_MARK_Q"), 
					GAME_STRING("STBL_IDX_CLAN_CREATE_CLAN_MARK"), MAKE_CALLBACK(cbCreateClanMarkNoSign), this);
}

void CPopupClanCreate::cbCreateClanMarkNoSign(void * pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		CPopupClanCreate * pPopup = (CPopupClanCreate *) pThis;

		if( pPopup->IsExactTypeOf( CPopupClanCreate::GetClassMeta()))
		{
			pPopup->CreateClanMarkNoSignConfirm();
		}
	}
}

void CPopupClanCreate::CreateClanMarkNoSignConfirm(void)
{
	m_bCreateConfirm = TRUE;
	m_bCheckedClanMark = TRUE;
	
	//	��ũ ������ �����ϰ� �⺻ Ŭ�� ��ũ�� Ŭ�� ������ �մϴ�.
	m_nClanMarkBackImageIdx = 0;
	m_nClanMarkShapeImageIdx = 0;
	m_nClanMarkBackColorIdx = 0;
	m_nClanMarkShapeColorIdx = 0;

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOverlappingCheck);

	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		DEACTIVATE_BUTTON(m_pCreateClanMarkPrev[i]);
		DEACTIVATE_BUTTON(m_pCreateClanMarkNext[i]);
	}
	
	for(INT32 i=0; i<CREATE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pCreateClanMarkBackColorBox[i]->setInputDisable(TRUE);
		m_pCreateClanMarkShapeColorBox[i]->setInputDisable(TRUE);
	}

	//	Ŭ������ �̺�Ʈ�� ������.
	_SendCreateClan();
}

void CPopupClanCreate::_CreateClanMarkOverlappingCheck(void)
{
	if(m_bWaitingIsDuplicateMark == FALSE)
	{
		//	1 base (0 is default)
		I3_BOUNDCHK(m_nClanMarkBackImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
		I3_BOUNDCHK(m_nClanMarkShapeImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
		I3_BOUNDCHK(m_nClanMarkBackColorIdx, CClanMarkManager::GetMarkColorCount()+1);
		I3_BOUNDCHK(m_nClanMarkShapeColorIdx, CClanMarkManager::GetMarkColorCount()+1);

		UINT32 nMarkFlag = MAKE_CLANMARK_FLAG( m_nClanMarkBackImageIdx, m_nClanMarkShapeImageIdx, m_nClanMarkBackColorIdx, m_nClanMarkShapeColorIdx);

		g_pFramework->GetClanContext()->SendOverlappingClanMark(nMarkFlag);

		m_bWaitingIsDuplicateMark = TRUE;
	}
}

void CPopupClanCreate::_CreateClanMarkPrevBackImage(void)
{
	m_nClanMarkBackImageIdx--;

	if( m_nClanMarkBackImageIdx <= 0)							m_nClanMarkBackImageIdx = CClanMarkManager::GetMarkBackImageCount();

	//	���� ��ũ ��� �̹����� ������Ʈ
	_CreateClanMarkUserSelectImageUpdate(0);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
}

void CPopupClanCreate::_CreateClanMarkNextBackImage(void)
{
	m_nClanMarkBackImageIdx++;

	if( m_nClanMarkBackImageIdx > CClanMarkManager::GetMarkBackImageCount())		m_nClanMarkBackImageIdx = 1;

	//	���� ��ũ ��� �̹����� ������Ʈ
	_CreateClanMarkUserSelectImageUpdate(0);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
}

void CPopupClanCreate::_CreateClanMarkPrevShapeImage(void)
{
	m_nClanMarkShapeImageIdx--;

	if( m_nClanMarkShapeImageIdx <= 0)							m_nClanMarkShapeImageIdx = CClanMarkManager::GetMarkShapeImageCount();

	//	���� ��ũ ��� �̹����� ������Ʈ
	_CreateClanMarkUserSelectImageUpdate(1);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
}

void CPopupClanCreate::_CreateClanMarkNextShapeImage(void)
{
	m_nClanMarkShapeImageIdx++;

	if( m_nClanMarkShapeImageIdx > CClanMarkManager::GetMarkShapeImageCount())	m_nClanMarkShapeImageIdx = 1;

	//	���� ��ũ ��� �̹����� ������Ʈ
	_CreateClanMarkUserSelectImageUpdate(1);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
}

void CPopupClanCreate::_CreateClanMarkUserSelectImageUpdate(INT32 nShapeIdx)
{
	INT32 nImageIdx = 1;
	INT32 nColorIdx = 1;
	INT32 nImageMax = 1;

	//	���õ� Ŭ�� ��ũ ���� ���� �̹��� ������Ʈ
	for(INT32 slot=0; slot<CREATE_CLANMARK_SHAPE_SLOT_MAX; slot++)
	{		
		if( nShapeIdx == 0)			//	��� ��ũ
		{
			nImageIdx = m_nClanMarkBackImageIdx;
			nColorIdx = m_nClanMarkBackColorIdx;
			nImageMax = CClanMarkManager::GetMarkBackImageCount();

			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkBackImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}
		else if( nShapeIdx == 1)	//	��� ��ũ	
		{
			nImageIdx = m_nClanMarkShapeImageIdx;
			nColorIdx = m_nClanMarkShapeColorIdx;
			nImageMax = CClanMarkManager::GetMarkShapeImageCount();

			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkShapeImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}

		//	��� 3��°(idx 0) ������ �߽����� �¿� ������ ���� ���� �ǵ��� �ε����� ���Ѵ�.
		switch( slot)
		{
		case 0:		nImageIdx -= 2;		if( nImageIdx <= 0)	nImageIdx += nImageMax;		break;
		case 1:		nImageIdx -= 1;		if( nImageIdx <= 0)	nImageIdx += nImageMax;		break;
		case 2:		nImageIdx += 0;		break;
		case 3:		nImageIdx += 1;		if( nImageIdx > nImageMax)	nImageIdx -= nImageMax;		break;
		case 4:		nImageIdx += 2;		if( nImageIdx > nImageMax)	nImageIdx -= nImageMax;		break;
		default:	continue;
		}

		if( nShapeIdx == 0)			//	��� ��ũ
		{
			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkBackImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}
		else if( nShapeIdx == 1)	//	��� ��ũ	
		{
			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkShapeImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}

		//	�ϳ��� ������ ��ũ ���� �̹����� �����մϴ�.
		_CreateClanMarkUserSelectImage(nShapeIdx, slot, nImageIdx, nColorIdx); 
	}

	//	Ŭ�� ��ũ ���� �̹����� ��������Ƿ� ���� ���� ��ũ �̹����� �Բ� ����
	_CreateClanMarkUserFinalImage();
}

void CPopupClanCreate::_CreateClanMarkUserSelectImage(INT32 nShapeIdx, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx)
{	//	�ϳ��� ������ ��ũ ���� �̹����� �����մϴ�.
	I3_BOUNDCHK( nShapeIdx, CREATE_CLANMARK_COMBINATION_MAX);
	I3_BOUNDCHK( nSlotIdx, CREATE_CLANMARK_SHAPE_SLOT_MAX);

	i3Texture * pTex = m_pTexClanMarkSelect[nShapeIdx][nSlotIdx];
	I3ASSERT(pTex);

	if( nShapeIdx == 0)			//	��� ��ũ
	{
		::MakeClanMarkShapeTex(pTex, CLAN_MARK_MEDIUM, CLAN_MARK_LAYER_FRAME, nImageIdx, nColorIdx);
	}
	else if( nShapeIdx == 1)	//	��� ��ũ
	{
		::MakeClanMarkShapeTex(pTex, CLAN_MARK_MEDIUM, CLAN_MARK_LAYER_CONTENT, nImageIdx, nColorIdx);
	}

	i3GuiControl * pControl = m_pCreateClanMarkSelectImage[nShapeIdx][nSlotIdx];
	i3GuiImage * pImage = pControl->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
	I3ASSERT( pImage != NULL);

	pImage->SetTexture( pTex, 0, 0, CLAN_MARK_MID_PIXEL, CLAN_MARK_MID_PIXEL);
	pControl->SetShapeEnable( GUI_SHAPE_OFFSET, TRUE);
	pControl->UpdateRenderObjects();	//	Sprite�� ���� ������Ʈ �ϱ�����...�̰� ������ GUI���� �ؽ�ó ������ ������Ʈ ���մϴ�. komet
	
}

void CPopupClanCreate::_CreateClanMarkUserFinalImage(void)
{	//	��� ������ �Ϸ�� ���� ��ũ �̹��� ������Ʈ
	i3Texture * pTex = m_pTexClanMarkFinal;
	I3ASSERT(pTex);

	::MakeClanMarkTex(pTex, CLAN_MARK_MEDIUM, m_nClanMarkBackImageIdx, m_nClanMarkShapeImageIdx, m_nClanMarkBackColorIdx,m_nClanMarkShapeColorIdx);

	i3GuiControl * pControl = m_pCreateClanMarkConfirmImage;
	i3GuiImage * pImage = pControl->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
	I3ASSERT( pImage != NULL);

	pImage->SetTexture( pTex, 0, 0, CLAN_MARK_MID_PIXEL, CLAN_MARK_MID_PIXEL);
	pControl->SetShapeEnable( GUI_SHAPE_OFFSET, TRUE);
	pControl->UpdateRenderObjects();	//	Sprite�� ���� ������Ʈ �ϱ�����...�̰� ������ GUI���� �ؽ�ó ������ ������Ʈ ���մϴ�. komet
	
}

void CPopupClanCreate::_CreateClanMarkChangeBackColor(INT32 nColorIdx)
{	//	��� ��ũ ���� ����	(1 base)
	m_nClanMarkBackColorIdx = nColorIdx;

	I3ASSERT( m_nClanMarkBackColorIdx > 0);
	I3_BOUNDCHK( m_nClanMarkBackColorIdx,CClanMarkManager::GetMarkColorCount()+1);

	INT32 idxArray = nColorIdx-1;

	INT32 selectedWidth = m_pCreateClanMarkBackColorSelected->getWidth();
	INT32 slotWidth = m_pCreateClanMarkBackColorBox[idxArray]->getWidth();
	INT32 offsetWidth = (selectedWidth - slotWidth) / 3;
	REAL32 slotPos = m_pCreateClanMarkBackColorBox[idxArray]->getPositionX();

	m_pCreateClanMarkBackColorSelected->setPositionX( slotPos - offsetWidth); 

	PlaySound(GUI_SND_ITEM_SELECTING);

	_CreateClanMarkUserSelectImageUpdate(0);
}

void CPopupClanCreate::_CreateClanMarkChangeShapeColor(INT32 nColorIdx)
{	//	��� ��ũ ���� ���� (1 base)
	m_nClanMarkShapeColorIdx = nColorIdx;

	I3ASSERT( m_nClanMarkShapeColorIdx > 0);
	I3_BOUNDCHK( m_nClanMarkShapeColorIdx,CClanMarkManager::GetMarkColorCount()+1);

	INT32 idxArray = nColorIdx-1;
	
	INT32 selectedWidth = m_pCreateClanMarkShapeColorSelected->getWidth();
	INT32 slotWidth = m_pCreateClanMarkShapeColorBox[idxArray]->getWidth();
	INT32 offsetWidth = (selectedWidth - slotWidth) / 3;
	REAL32 slotPos = m_pCreateClanMarkShapeColorBox[idxArray]->getPositionX();

	m_pCreateClanMarkShapeColorSelected->setPositionX( slotPos - offsetWidth); 

	PlaySound(GUI_SND_ITEM_SELECTING);

	_CreateClanMarkUserSelectImageUpdate(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupSubScriptionPage(void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
	if(( MyUserInfoBasic._clanidx != 0) && (MyUserInfoBasic._clanstate == 0))
	{
		//	"�̹� Ŭ�������� ��û�� �����Դϴ�.\n���� ��û�� ����Ͻðڽ��ϱ�?"
		//	"���� ��û ���"
		CAPTION_POPUP_Q( GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANCEL_SUBSCRIPTION_Q"), 
						GAME_STRING("STBL_IDX_CLAN_CAPTION_CANCEL_SUBSCRIPTION"), MAKE_CALLBACK(cbCancelRequest), this);
		OnClosePopup();
		return;
	}

	char strTotalRecord[MAX_STRING_COUNT];
	char strTotalKillDeath[MAX_STRING_COUNT];

	const USER_INFO_RECORD * pRecordInfo	= g_pGameContext->GetMyUserInfoRecord();
	CLAN_DETAIL_INFO * pDetailedClanInfo	= g_pGameContext->getClanDetailInfo();

	//	"%d�� %d�� %d��"
	#ifdef USE_MODE_RECORD
	sprintf_s(strTotalRecord, GAME_STRING("STBL_IDX_CLAN_DECLARE_SCORE"), pRecordInfo->_season_record._match, pRecordInfo->_season_record._win, pRecordInfo->_season_record._lose);
	sprintf_s(strTotalKillDeath, "%d / %d", pRecordInfo->_season_record._killcount, pRecordInfo->_season_record._death);
	#else
	sprintf_s(strTotalRecord, GAME_STRING("STBL_IDX_CLAN_DECLARE_SCORE"), pRecordInfo->_smatch, pRecordInfo->_swin, pRecordInfo->_slose);
	sprintf_s(strTotalKillDeath, "%d / %d", pRecordInfo->_skillcount, pRecordInfo->_sdeath);
	#endif
	
	m_pSubScriptionSenderWinValue->SetText(strTotalRecord);
	m_pSubScriptionSenderScoreValue->SetText(strTotalKillDeath);

	{
		USER_INFO_BASIC		MyUserInfoBasic;
		g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

		GameGUI::SetRankStatic(m_pSubScriptionSenderRank);
		m_pSubScriptionSenderRank->SetShapeUVIndex(MyUserInfoBasic._rank);
		m_pSubScriptionSenderRank->SetEnable(TRUE);

		SetNameUsingColorTable( m_pSubScriptionReceiverValue, pDetailedClanInfo->_name, pDetailedClanInfo->_color );

		// �߽��� �г��� 
		SetNameUsingColorTable(m_pSubScriptionSenderNickValue, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);
	}

	m_pSubScription->SetEnable( TRUE);
}

void CPopupClanCreate::_OnEventSubScriptionPage( INT32 Arg)
{
	if( EV_SUCCESSED( Arg ) )
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_SUBSCRIPTION"));	//	"���Խ�û�� �Ϸ�Ǿ����ϴ�."
		OnClosePopup();
	}
	else
	{
		switch( Arg ) {
			case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
				{
					//	"\n���� ��û �����Ͽ����ϴ�.\nŬ���� ã���� �����ϴ�."
					//	Ŭ�� ���� ���� ����
					CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_FINDING"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_MAXREQUEST:
				{
					//	"\nŬ�� ���� ó���� �����߽��ϴ�.\n���̻� ���� �� �� �����ϴ�."
					//	Ŭ�� ���� ���� ����
					CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_OVER_MEMBER"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;			
			case EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX:
				{
					//	"\n���� ��û �����Ͽ����ϴ�.\n�̹� �ٸ� Ŭ���� ���ԵǾ� �ְų� ���� ��û�� �����Դϴ�."
					//	Ŭ�� ���� ���� ����	
					CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ALREADY_SUBSCRIPTION"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_REQUEST_FAIL:
				{
					//	"\n���� ��û �����Ͽ����ϴ�"
					//	Ŭ�� ���� ���� ����
					CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_FAIL"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL:
				{
					//	�ش� Ŭ���� ������ �� �ִ� ���ɰ� ���� �ʾ� Ŭ�� ���� ��û�� ��� �Ǿ����ϴ�."
					//	Ŭ�� ���� ���� ����
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION_AGE"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL:
				{
					//	"�ش� Ŭ���� ������ �� �ִ� ��ް� ���� �ʾ� Ŭ�� ���� ��û�� ��� �Ǿ����ϴ�."
					//	Ŭ�� ���� ���� ����
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION_RANK"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			default:
				{
					char szTemp[MAX_PATH];
					//	"\n���� ��û�� �����Ͽ����ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�."
					sprintf_s( szTemp, "%s\n [Errcode : 0x%08x]", 
										GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION"), Arg );

					//	Ŭ�� ���� ���� ����
					CAPTION_POPUP( szTemp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
		}
	}

	m_bWaitingConfirmSubscripting = FALSE;
}

void CPopupClanCreate::_OnEventCreateClan( INT32 Arg)
{
	if( m_bWaitingConfirmCreateClan)
	{
		if( EV_SUCCESSED( Arg ) )
		{
			//	"\n�����մϴ�.\nŬ�� ������ �Ϸ�Ǿ����ϴ�."
			//	"Ŭ�� ���� ����"
			CAPTION_POPUP_C( GAME_STRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_CREATING"), 
							 GAME_STRING("STBL_IDX_CLAN_CAPTION_COMPLETE_CREATING"), MAKE_CALLBACK(cbSuccessCreateClan), this);

#if defined(USE_CLAN_MARK) && defined(ENABLE_CREATE_CLAN_MARK)
			//	��� ���� Ŭ�� ��ũ�� ����
			((CStageReady*)m_pParent)->GetReadyBg()->CharaChangeClanMarkInReadyBg();
#endif
		}
		else
		{
			switch( Arg )
			{
			case EVENT_ERROR_CLAN_MAXCLAN:
				{
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�.\n ���̻� Ŭ���� ����� �����ϴ�."
					//	"Ŭ�� ���� ����"
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_OVERFLOW"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX:
				{
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�.\n �̹� �ٸ� Ŭ���� ���ԵǾ� �ְų� ���� ��û�� �����Դϴ�."
					//	"Ŭ�� ���� ����"
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_ALREADY"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_SEARCH_CLANIDX:
				{
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�.\n �̹� �ߺ��Ǵ� Ŭ���� �Ǵ� ����Ʈ �ּҰ� �����մϴ�."
					//	"Ŭ�� ���� ����"
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_OVERLAPPING"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_CREATE_FAIL:
				{
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�."
					//	"Ŭ�� ���� ����"
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			default:
				{
					char szTemp[MAX_PATH];
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�."
					sprintf_s( szTemp, "%s\n [Errcode : 0x%08x]", GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_ADMIN"), Arg);

					//	"Ŭ�� ���� ����"
					CAPTION_POPUP( szTemp, GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			}
		}

		OnClosePopup();

		m_bWaitingConfirmCreateClan = FALSE;
	}
}

void CPopupClanCreate::_OnEventDuplicateName( INT32 Arg)
{
	char szTemp[MAX_PATH] = ""; 
	
	if( m_pClanInputInfo->isEnable()) 
	{
		const char * pszName = m_pClanInputInfoClanNameEdit->GetText();

		if( Arg == 0)		//	Ŭ������ �ߺ����� �ʴ´�
		{	
			m_bCheckedName = TRUE;

			if( m_bCheckingName)
			{
				//	"��/�� ����Ͻ� �� �ֽ��ϴ�."
				sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s", pszName, GAME_STRING("STBL_IDX_CLAN_CAN_USE"));
				STRING_POPUP( szTemp);
			}
		}
		else		//	Ŭ������ �ߺ��ȴ�.
		{
			m_bCheckedName = FALSE;

			_OnPopupCheckOverlapping(pszName, TRUE);
		}		
	}
	else if( m_pOverlappingCheck->isEnable())
	{
		const char * pszName = m_pOverlappingCheckInputEdit->GetText();

		if( Arg == 0)
		{
			m_bCheckedName = TRUE;

			if( m_bCheckingName)
			{
				_OnPopupCheckOverlapping(pszName, FALSE);
			}
			else
			{
				m_pClanInputInfoClanNameEdit->SetIMEText( m_pOverlappingCheckInputEdit->getIMETextBuf());

				_OnPopupInputInfo();
			}
		}
		else
		{
			m_bCheckedName = FALSE;

			//	"��/�� �̹� ������̰ų� ����Ҽ� ���� �̸��Դϴ�.\n �ٸ� �̸��� ������ �ּ���."
			sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s" , 
							m_pOverlappingCheckInputEdit->GetText(), GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));

			STRING_POPUP( szTemp);

			_OnPopupCheckOverlapping(m_pOverlappingCheckInputEdit->GetText(), TRUE);
		}
	}

	m_bWaitingIsDuplicateName = FALSE;
}

void CPopupClanCreate::_OnEventDuplicateAddress( INT32 Arg)
{
	char szTemp[MAX_PATH]; 
	
	if( m_pClanInputInfo->isEnable()) 
	{
		const char * pszAddress = m_pClanInputInfoClanAddressEdit->GetText(); 

		if( Arg == 0)
		{
			m_bCheckedAddress = TRUE;

			if( m_bCheckingAddress)
			{
				//	"��/�� ����Ͻ� �� �ֽ��ϴ�."
				sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s" , 
								pszAddress, GAME_STRING("STBL_IDX_CLAN_CAN_USE"));

				STRING_POPUP( szTemp);
			}			
		}
		else
		{			
			m_bCheckedAddress = FALSE;

			_OnPopupCheckOverlappingAddress(pszAddress, TRUE);
		}
	}
	else if( m_pOverlappingAddress->isEnable())
	{
		const char * pszAddress = m_pOverlappingAddressInputEdit->GetText();

		if( Arg == 0)
		{
			m_bCheckedAddress = TRUE;

			if( m_bCheckingAddress)
			{
				_OnPopupCheckOverlappingAddress(pszAddress, FALSE); 
			}
			else
			{
				m_pClanInputInfoClanAddressEdit->SetIMEText( pszAddress);
				
				_OnPopupInputInfo();
			}
		}
		else
		{
			m_bCheckedAddress = FALSE;

			//	"��/�� �̹� ������̰ų� ����Ҽ� ���� �ּ��Դϴ�.\n �ٸ� �ּҸ� ������ �ּ���."
			sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s", 
							pszAddress, GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_AZIT"));

			STRING_POPUP( szTemp);

			_OnPopupCheckOverlappingAddress(pszAddress, TRUE);
		}
	}

	m_bWaitingIsDuplicateAddress = FALSE;
}

void CPopupClanCreate::StepToCreateClan(void)
{
#if defined(USE_CLAN_MARK) && defined(ENABLE_CREATE_CLAN_MARK)
	//	Ŭ�� ��ũ ���� ��������
	m_bJumpToClanMarkStep = TRUE;
#else
	//	�ٷ� Ŭ�� ����
	m_bCheckedClanMark = TRUE;
	m_bCreateConfirm = TRUE;

#if !defined( CLAN_ADDRESS_ENABLE)
	m_bCheckedAddress = TRUE;
#endif

#endif
}

void CPopupClanCreate::cbSuccessCreateClan(void* pThis, INT32 nParam)
{
	CPopupClanCreate * pThisPopup = (CPopupClanCreate*) pThis;
	I3ASSERT(pThisPopup != NULL);

	((CStageReady*)pThisPopup->getParent())->TogglePopup(POPUP_NULL);
	((CStageReady*)pThisPopup->getParent())->ChangeStep(STEP_CLAN_MEMBER);	
}

void CPopupClanCreate::cbQueryCreateClan(void* pThis, INT32 nParam)
{
	if( nParam == MB_OK)
	{
		((CPopupClanCreate*) pThis)->StepToCreateClan();
	}
}

void CPopupClanCreate::cbQuaryJoinClan(void* pThis, INT32 nParam)
{

	CPopupClanCreate * pPopup = (CPopupClanCreate*) pThis;

	if( nParam == MB_OK)
	{
		pPopup->_SubscriptionSendJoin();
	}
}

void CPopupClanCreate::cbCancelRequest(void* pThis, INT32 nParam)
{
	CPopupClanCreate * pPopup = (CPopupClanCreate*) pThis;

	if( nParam == MB_OK)
	{
		pPopup->_SubscriptionCancelRequest();
	}
	else
	{
		pPopup->OnClosePopup();
	}
}