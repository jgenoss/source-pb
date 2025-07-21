#if !defined __HACK_SHIELD_H
#define __HACK_SHIELD_H

#if defined (_HACK_SHIELD_)

#include "HShield.h"
#include "HSUpChk.h"

#pragma comment (lib, "version.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "HShield.lib")		//	V3 HackShield ���̺귯��
#pragma comment (lib, "HsUpChk.lib")		//	V3 HackShield ���̺귯��

#define PB_HSHIELD_GAME_CODE		5350							//	���� ���� �ڵ� (�ȷ����� ���޹���, 4�ڸ� ����)
#define PB_HSHIELD_LICENSE_KEY		"557ADA9BF431C2F034AA31A8"		//	���̼��� Ű (�ȷ����� ���޹���, 24�ڸ� ���ڿ�)

namespace HackShield
{
	BOOL RunUpdater(void);						//	����Ʈ �������͸� ����� ��� Init�Լ����� ����Ǿ�� �Ѵ�.
	BOOL Init();								//	�ʱ�ȭ
	BOOL StartService( BOOL bErrorHandle);		//	��ŷ ���� ���� ����
	BOOL StopService( BOOL bErrorHandle);		//	��ŷ ���� ���� ����
	BOOL Release( BOOL bErrorHandle);			//	����

	//	�������� ������ ��ȣȭ�� ���� ��û�� �޽����� ��ȣȭ�ϰ� ���� Ŭ���̾�Ʈ ������ ������ ��ȣȭ�Ͽ� ���� �޽����� ����ϴ�.
	BOOL MakeResponse( AHNHS_TRANS_BUFFER * pRequest, AHNHS_TRANS_BUFFER * pResponse);	

	//	BUG_TRAP ���� �� ��츸...
	//	������ ������ ����Ǿ��� ��쿡�� �ٽ���� �����ƾ�� ����Ǿ�� �ϹǷ� exception�� �� �Լ��� ȣ���ϵ��� �Ѵ�.
	void SetUnhandledException(void);
	void UnhandledExceptionHandler(void);		
}

#endif	// _HACK_SHIELD_
#endif
