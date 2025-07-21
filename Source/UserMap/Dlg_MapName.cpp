#include "PCH.h"
#include "Dlg_MapName.h"

I3_CLASS_INSTANCE( CDlgMapName, i3UIModalDriver);


BOOL CDlgMapName::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pInfo = (I3UI_CONTROL_NOTIFY *) param2;
		i3UIControl * pCtrl = pInfo->m_pCtrl;

		switch( pInfo->m_nEvent)
		{
			case I3UI_EVT_CLICKED :
				if( (i3String::Compare( pCtrl->GetName(), "Yes") == 0) || (i3String::Compare( pCtrl->GetName(), "OK") == 0))
				{
					// �̸��� �ԷµǾ��°��� Ȯ���Ѵ�.
					// ���� ����� �Էµ��� �ʾҴٸ� Dialog�� ���� �ʴ´�.
					i3UIEditBox * pEditBox = (i3UIEditBox *) pInfo->m_pFrameWnd->FindCtrlByName( "Name");

					if( (pEditBox->getText() == NULL) || (pEditBox->getText()[0] == 0))
					{
						// �̸��� �Էµ��� �ʾҴ�.
						i3UI::MsgBox( I3UI_MSGBOX_OK, NULL, "���� �̸��� �־�߸� ������ �� �ֽ��ϴ�.");
						return TRUE;
					}
				}

				break;
		}
	}

	return i3UIModalDriver::OnEvent( event, pObj, param2, code);
}
