#include "pch.h"
#include "HackShield.h"
#include "StageBattle.h"
#include "GlobalVariables.h"

#if defined (_HACK_SHIELD_)

// HSHIELD_CALL_UPDATER : V3 �ٽ����� ����Ʈ ������Ʈ ��� (�ٽ��� ���� ��� ������Ʈ��. ������Ʈ ������ ����Ǿ�� ��.)



// ���߽� ��� (dll�� �ٸ�)
#if defined(I3_DEBUG)

#define		HSHIELD_PATH			"\\HShield_Dev"

//	>>> ������ ����! 2009.04.30
// EhSvc.dll�� HShield.dat ������ ������� ���� �������� ���� �����ϸ� ����Ʈ ������Ʈ�� �̿��Ͽ�
// �ڵ� ������Ʈ�� �ϸ� ������Ʈ ������ ���� �������� �ٿ�ްԵǾ� �ʱ�ȭ(_AhnHS_Initialize)�� 
// ����� �� �����Ǿ� �������� �����ϰ� �ǹǷ� �����ÿ��� ����Ʈ ������Ʈ�� ������� ����.
//#define		HSHIELD_CALL_UPDATER

#else

#define		HSHIELD_PATH			"\\HShield"
#define		HSHIELD_CALL_UPDATER

#endif

static TCHAR	g_szHShieldPath[MAX_PATH] = {0,};		//	�ٽ��� ������ �ִ� ���� ���

void CbExitGame(void* pThis,INT32 nParam)
{
	// ��������..
	g_pFramework->GotoNextStage( "Exit");
}

//	�ٽ��� �ݹ� ó��
int __stdcall _CallbackProc(long lCode, long lParamSize, void* pParam)
{
	char szFullMsg[1024] = "";
	char szMsg[1024] = "";

	switch(lCode)
	{
		case AHNHS_ACTAPC_STATUS_HACKSHIELD_RUNNING:
		{
			//	�ٽ��� ���� �۵���..
		}
		break;
		//Engine Callback
		case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			//	"���� ��ġ���� ��ŷ���� �߰ߵǾ� ���α׷��� ������׽��ϴ�.\n(%s)\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_GAME_HACK), (char*)pParam);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			
			break;
		}
		//Speed ����
		case AHNHS_ACTAPC_DETECT_SPEEDHACK:
		{
			if( g_pFramework->GetCurrentStage()->IsExactTypeOf( CStageBattle::GetClassMeta()) )
			{
				CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
				if( pStage->getHud() != NULL) pStage->getHud()->SetActivateHackWarnning();
			}

			//	"���ǵ������� �ǽɵǴ� ������ �����Ǿ����ϴ�. (Code = 0x%X)\r\n".
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_SPEEDHACK), lCode);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);*/

			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}
		//����� ���� 
		case AHNHS_ACTAPC_DETECT_KDTRACE:	
		case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
		{
			//	"���α׷��� ���Ͽ� ����� �õ��� �߻��Ͽ����ϴ�. (Code = 0x%X)\n���α׷��� �����մϴ�.\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_KDTRACE), lCode);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}
		// ���� ��ũ��
		case AHNHS_ACTAPC_DETECT_AUTOMACRO:
		{
			//	"��ũ�� ������� �ǽɵǴ� ������ �����Ǿ����ϴ�. (Code = 0x%X)\n���α׷��� �����մϴ�.\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_AUTOMACRO), lCode);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}

		// �ڵ� ��ġ ����
		case AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS:
		{
			//	"���� ��ġ���� �޸� ������ �����Ǿ� ���α׷��� ������׽��ϴ�.\n(%s)\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_MEM_ACCESS), (char*)pParam);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}

		//�׿� ��ŷ ���� ��� �̻� 
		case AHNHS_ACTAPC_DETECT_AUTOMOUSE:
		case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
		case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
		case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		case AHNHS_ACTAPC_DETECT_LMP_FAILED:
		case AHNHS_ACTAPC_DETECT_ENGINEFAILED:
		case AHNHS_ACTAPC_DETECT_CODEMISMATCH:
		case AHNHS_ACTAPC_DETECT_PROTECTSCREENFAILED:
		case AHNHS_ACTAPC_DETECT_ABNORMAL_HACKSHIELD_STATUS:
		case AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP:
		case AHNHS_ACTAPC_DETECT_ANTIFREESERVER:
		{
			//	��ŷ ��� ��ɿ� �̻��� �߻��Ͽ����ϴ�. (Code = 0x%X)\n���α׷��� �����մϴ�.\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_OTHER), lCode);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}
	}

	return 1;
}

static char g_szLastMsg[1024] = UNKNOWN_TEXT;

static const char * _getErrMsg( INT32 code)
{	
	const char * lpszMsg = UNKNOWN_TEXT;

	switch( code)
	{
	case 0:
		return NULL;
		break;
	case HS_ERR_INVALID_PARAM:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_INVALID_PARAM);	// "�Ķ���Ͱ� �߸��Ǿ����ϴ�.";
		break;
	case HS_ERR_COMPATIBILITY_MODE_RUNNING:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_COMPATIBILITY_MODE_RUNNING);	//	"ȣȯ�� ���� ���α׷��� �������Դϴ�.\n���α׷��� �����մϴ�.";
		break;	
	case HS_ERR_INVALID_LICENSE:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_INVALID_LICENSE);	//"���̼��� Ű�� �߸� �ԷµǾ����ϴ�.";
		break;	
	case HS_ERR_INVALID_FILES:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_INVALID_FILES);	//"�߸��� ���� ��ġ�Ǿ����ϴ�.\n���α׷��� �缳ġ�Ͻñ� �ٶ��ϴ�.";
		break;	
	case HS_ERR_INIT_DRV_FAILED:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_INIT_DRV_FAILED);	//"����̹� ������ �����Ͽ����ϴ�.";
		break;	
	case HS_ERR_DEBUGGER_DETECT:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_DEBUGGER);	//"��ǻ�Ϳ��� ����� ������ �����Ǿ����ϴ�.\n������� ������ ������Ų �ڿ� �ٽ� ��������ֽñ�ٶ��ϴ�.";
		break;	
	case HS_ERR_NOT_INITIALIZED:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_NOT_INITIALIZED);	//"�ʱ�ȭ ���� �ʾҽ��ϴ�.";
		break;	
	case HS_ERR_SERVICE_STILL_RUNNING:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_SERVICE_STILL_RUNNING);	//"�̹� �������Դϴ�.";
		break;	
	case HS_ERR_SERVICE_NOT_RUNNING:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_SERVICE_NOT_RUNNING);	//"���۵��� �ʾҽ��ϴ�.";
		break;
	case HSERROR_ENVFILE_NOTREAD:
	case HSERROR_ENVFILE_NOTWRITE:
	case HSERROR_NETWORK_CONNECT_FAIL:
	case HSERROR_HSUPDATE_TIMEOUT:
	case HSERROR_MISMATCH_ENVFILE:
	case HSERROR_HOSTFILE_MODIFICATION:
	case HSERROR_LIB_NOTEDIT_REG:
	case HSERROR_NOTFINDFILE:
	case HSERROR_PROTECT_LISTLOAD_FAIL:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_UPDATE_FAILED);	//"������Ʈ�� �����߽��ϴ�.";
		break;	
	case HS_ERR_DRV_FILE_CREATE_FAILED:
	case HS_ERR_REG_DRV_FILE_FAILED:
	case HS_ERR_START_DRV_FAILED:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM);	//"��ŷ ���� ��ɿ� ������ �߻��Ͽ����ϴ�.";
		break;
	case HS_ERR_NEED_UPDATE:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_NEED_MODULE);	//"����� ������Ʈ�� �ʿ��մϴ�.";
		break;
	case HS_ERR_NEED_ADMIN_RIGHTS:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_ADMIN);		//"ADMIN ������ �ʿ��մϴ�.";
		break;
	}

	sprintf( g_szLastMsg, "[HackShield:0x%X]\n %s \n", code, lpszMsg);

	return g_szLastMsg;
}

enum HS_NOTIFY_TYPE
{
	HS_NOTIFY_NONE = 0,
	HS_NOTIFY_MSG_BOX,
	HS_NOTIFY_I3FATAL,
	HS_NOTIFY_I3WARN,
	HS_NOTIFY_I3TRACE,	
};


static BOOL __HS_CHK(INT32 code, HS_NOTIFY_TYPE popup, const char * file, INT32 line)
{
	if( code != 0)
	{		
		const char * pszMsg = _getErrMsg( code);
		char msg[1024] = {};
		i3String::Format(msg, 1024, _T("file : %s / line : %d\n%s"), file, line, pszMsg);

		switch( popup)
		{
		case HS_NOTIFY_MSG_BOX:		MessageBox( NULL, msg, "Error", MB_OK);		break;
		case HS_NOTIFY_I3FATAL:		I3FATAL( msg);								break;
		case HS_NOTIFY_I3WARN:		I3WARN( msg);								break;
		case HS_NOTIFY_I3TRACE:		I3TRACE( msg);								break;
		}

		__log( msg);
		
		return FALSE;
	}

	return TRUE;
}

#define		_HS_CHK(code, popup)		__HS_CHK(code, popup, __FILE__, __LINE__)

//	����Ʈ �������͸� ����� ��� Init�Լ����� ����Ǿ�� �Ѵ�.
//	������Ʈ ������ ����Ǿ� �־�� ��
//
//	>>> ������ ����! 2009.04.30
// EhSvc.dll�� HShield.dat ������ ������� ���� �������� ���� �����ϸ� ����Ʈ ������Ʈ�� �̿��Ͽ�
// �ڵ� ������Ʈ�� �ϸ� ������Ʈ ������ ���� �������� �ٿ�ްԵǾ� �ʱ�ȭ(_AhnHS_Initialize)�� 
// ����� �� �����Ǿ� �������� �����ϰ� �ǹǷ� �����ÿ��� ����Ʈ ������Ʈ�� ������� ����.
BOOL HackShield::RunUpdater(void)
{
	AHNHS_EXT_ERRORINFO	extErrorInfo ={};	// ����͸� ����� ������� ������ 0���� �ʱ�ȭ�ؼ� �ѱ�� �˴ϴ�.

	DWORD dwRet = _AhnHS_HSUpdateEx( g_szHShieldPath,		// �ٽ��� ���� ���
								 1000 * 600,				// ������Ʈ ��ü Ÿ�Ӿƿ�
								 PB_HSHIELD_GAME_CODE,		// ���� �ڵ�
								 AHNHSUPDATE_CHKOPT_HOSTFILE | AHNHSUPDATE_CHKOPT_GAMECODE,
								 extErrorInfo,
								 1000 * 20);				// ���� ���� Ÿ�Ӿƿ�

	/*char temp[256] = "";
	sprintf( temp, "[HackShield] update (0x%X).\r\n", (INT32)dwRet);
	__log( temp);*/

	if( !_HS_CHK( (INT32) dwRet, HS_NOTIFY_MSG_BOX))		return FALSE;

	return TRUE;

}

//
BOOL HackShield::Init()
{
	INT32 nRet = 0;
	TCHAR szFullFilePath[MAX_PATH<<2] = {};
	DWORD dwOption = 0;


	//	�ٽ��带 �ʱ�ȭ�ϴ� EhSvc.dll�� HShield.dat ������ ��� ������ ���ϰ� ���߿� �������� �ٸ��Ƿ� ��ε� �ٸ���.


	::GetCurrentDirectory( MAX_PATH, szFullFilePath);
	i3String::Concat(szFullFilePath, HSHIELD_PATH);

	i3String::Copy(g_szHShieldPath, szFullFilePath);	// HShield-Path ����



	// �ٽ��� ����Ʈ ������Ʈ�� ȣ���Ѵ�. (��, �̸� ������Ʈ ������ ���� �� �����̵Ǿ� �־�� ��)
	//	EhSvc.dll�� �ε�Ǳ� ���� �ٽ��� ��� ������Ʈ�� �Ϸ�Ǿ�� �Ѵ�.
#if defined(HSHIELD_CALL_UPDATER)

	if( ! HackShield::RunUpdater())
		return FALSE;
#else
	__log( "[HackShield] no update.\r\n");
#endif

	// �ٽ��� ������ EhSvc.dll ��ġ�� �����մϴ�.
	i3String::Concat( szFullFilePath, _T("\\EhSvc.dll"));		// EHSVC.DLL =Path ����

	/*
	AHNHS_CHKOPT_ALL �ɼǿ� �Ʒ� �ɼ� ��� ����

	AHNHS_CHKOPT_SPEEDHACK					: ���ǵ� �� ����
	AHNHS_CHKOPT_READWRITEPROCESSMEMORY		: �ٽ��� DLL�� ����ϴ� ���μ��� �޸𸮸� �ܺο��� ���� ����
	AHNHS_CHKOPT_KDTRACER					: Ŀ�� ��� ����� ������ �����Ͽ� ���ӿ� �˷���.
	AHNHS_CHKOPT_OPENPROCESS				: �ٽ��� DLL�� ����ϴ� ���μ����� ���� ������ ��� OpenProcess API �Լ� ȣ���� ����.
	AHNHS_CHKOPT_AUTOMOUSE					: ���� ���콺 ������ ���α׷��� ���� ���� ���� �ʵ��� ����
	AHNHS_CHKOPT_MESSAGEHOOK				: �޽��� ��ŷ ����
	AHNHS_CHKOPT_PROCESSSCAN				: �ֱ������� ���μ��� ��� �˻��Ͽ� ��ŷ �� ���� ���� �˻�
	AHNHS_CHKOPT_HSMODULE_CHANGE			: 
	AHNHS_USE_LOG_FILE						: �ٽ���� ���õ� �α׸� ��� EhSvc.dll�� ��ġ�� hshield.log�� ���� ����. ��ȣȭ�Ǿ� �־� �ٸ��� Ȯ�� ����.
	*/

	// Initialize �Լ� ȣ�� �ɼ� �÷��� ����
	dwOption =	AHNHS_CHKOPT_ALL;

	//	���� ��ü���� �޽��� ��ŷ�� ����ϱ� ������ �޽��� ��ŷ�� ������ ������ �÷����� �� ����.
	dwOption &= ~AHNHS_CHKOPT_MESSAGEHOOK;

#if defined (I3_DEBUG)	
	//	�ڵ� �����ÿ��� ���� ���� ������� ������.
	dwOption &= ~AHNHS_CHKOPT_READWRITEPROCESSMEMORY;
	dwOption &= ~AHNHS_CHKOPT_KDTRACER;
	dwOption &= ~AHNHS_CHKOPT_OPENPROCESS;
#else	
	dwOption |= AHNHS_DISPLAY_HACKSHIELD_LOGO;		// �ʱ�ȭ�� �ٽ��� �̹��� �ΰ� ǥ������.
	dwOption |= AHNHS_DISPLAY_HACKSHIELD_TRAYICON;	// Ʈ���̾����ܿ� �ٽ��带 ������.
	dwOption |= AHNHS_CHKOPT_STANDALONE;			// ������ ���� �ڵ带 ����ϴ� ���ӿ� ���Ͽ� �ߺ� ������ �Ұ����ϵ��� �ٽ��忡�� ���� �ڵ带 �����մϴ�	
	dwOption |= AHNHS_CHKOPT_PROTECT_D3DX;			// DirectX ����� VTable�� ��ŷ �ϴ� ��ŷ ���� �����ϴ� ���. Windows NT �̻��� OS������ ����
	dwOption |= AHNHS_CHKOPT_LOCAL_MEMORY_PROTECTION;	//	������ ��ȣ ��� ������ �޸� ������ ��ȣ�ϴ� �ɼ��Դϴ�.
	dwOption |= AHNHS_CHKOPT_SELF_DESTRUCTION;		//	�ٽ��忡�� ��ŷ ������ �����Ͽ� CallBack �Լ� ȣ�� �� ���Ŀ��� ���� ���μ����� ���� ���� ���� ���, ���� �ð� ���� (1�� ��) �ٽ��� ��ü������ ���μ����� �����Ű�� ����Դϴ�.
	dwOption |= AHNHS_CHKOPT_DETECT_VIRTUAL_MACHINE; // ���� �ӽ� ���α׷�����
	//dwOption |= AHNHS_CHKOPT_UPDATED_FILE_CHECK; 
#endif

	//	Initialize �Լ� ȣ���Ͽ� �ٽ��带 �ʱ�ȭ�մϴ�.
	nRet = _AhnHS_Initialize( szFullFilePath, 
								_CallbackProc,		// �ݹ� �Լ�
								PB_HSHIELD_GAME_CODE,	// ���� ���� �ڵ�
								PB_HSHIELD_LICENSE_KEY,	// ���̼��� Ű
								dwOption,
								AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL);

	/*char temp[256] = "";
	sprintf( temp, "[HackShield] initialize (0x%X).\r\n", nRet);
	__log( temp);*/
	
	//	�ʱ�ȭ ���н� ���� ó��
	if( !_HS_CHK( nRet, HS_NOTIFY_MSG_BOX))		return FALSE;
				
	return TRUE;
}

BOOL HackShield::StartService(BOOL bErrorHandle)
{
	INT32 nRet = _AhnHS_StartService();

	/*char temp[256] = "";
	sprintf( temp, "[HackShield] start (0x%X).\r\n", nRet);
	__log( temp);*/

	if( bErrorHandle)
	{
		//	�ʱ�ȭ ���н� ���� ó��
		if( !_HS_CHK( nRet, HS_NOTIFY_I3FATAL))		return FALSE;
	}
	else
	{
		if(nRet != 0)	return FALSE;
	}

	return TRUE;	
}

BOOL HackShield::StopService(BOOL bErrorHandle)
{
	INT32 nRet = _AhnHS_StopService();

	/*char temp[256] = "";
	sprintf( temp, "[HackShield] stop (0x%X).\r\n", nRet);
	__log( temp);*/

	if( bErrorHandle)
	{
		//	�ʱ�ȭ ���н� ���� ó��
		if( !_HS_CHK( nRet, HS_NOTIFY_I3FATAL))		return FALSE;
	}
	else
	{
		if(nRet != 0)	return FALSE;
	}

	return TRUE;
}

BOOL HackShield::Release(BOOL bErrorHandle)
{
	INT32 nRet = _AhnHS_Uninitialize();
	
	/*char temp[256] = "";
	sprintf( temp, "[HackShield] release (0x%X).\r\n", nRet);
	__log( temp);*/

	if( bErrorHandle)	
	{
		//	�ʱ�ȭ ���н� ���� ó��
		if( !_HS_CHK( nRet, HS_NOTIFY_I3FATAL))		return FALSE;
	}
	else
	{
		if(nRet != 0)	return FALSE;
	}
				
	return TRUE;
}

static INT32 s_ResponseRet = 0;

BOOL HackShield::MakeResponse( AHNHS_TRANS_BUFFER * pRequest, AHNHS_TRANS_BUFFER * pResponse)
{
	if( pRequest && pResponse)
	{
		UINT32 nRet = _AhnHS_MakeResponse( pRequest->byBuffer, pRequest->nLength, pResponse);

		if( nRet != ERROR_SUCCESS)
		{
			switch( nRet)
			{
				// ������ üũ�� TRUE ��ȯ �մϴ�.
			case HS_ERR_ANTICPXCNT_BAD_HSHIELD_MODULE:
				//I3NOTICE( "[HackShield] Detect invalid response (0x%X)", nRet);
			case HS_ERR_ANTICPXCNT_BAD_CLIENT_FILE:
#ifdef I3_DEBUG
			case HS_ERR_ANTICPXCNT_DEBUGGER_DETECTED:		// ����� ��Ȳ ����
#endif
				s_ResponseRet = nRet;
				return TRUE;
			default:
			//case HS_ERR_ANTICPXCNT_INVALID_PARAMETER:		// �Ķ���� ���� �ùٸ��� �ʴ�.
			//case HS_ERR_ANTICPXCNT_INVALID_ADDRESS:			// �߸��� �޸� �ּ�
			//case HS_ERR_ANTICPXCNT_NOT_ENOUGH_MEMORY:		// �޸� ����
			//case HS_ERR_ANTICPXCNT_CRC_TABLE_INIT_FAILED:	// �ʱ�ȭ�� ����
			//case HS_ERR_ANTICPXCNT_BAD_LENGTH:				// �޽��� ����� �߸�
			//case HS_ERR_ANTICPXCNT_INSUFFICIENT_BUFFER:		// ���޵� ������ ����� �߸�
			//case HS_ERR_ANTICPXCNT_NOT_SUPPORTED:			// ���� �������� ���� ����.
			//case HS_ERR_ANTICPXCNT_FILE_NOT_FOUND:			// Ŭ���̾�Ʈ ������ ã�� �� ����.
			//case HS_ERR_ANTICPXCNT_INVALID_MESSAGE_SIZE:	// �Է� ���� �޽����� ũ�Ⱑ �߸�
			//case HS_ERR_ANTICPXCNT_BAD_FORMAT:				// �ùٸ� ������ �ƴ�.			
			//case HS_ERR_ANTICPXCNT_BAD_HSHIELD_MODULE:		// �ٽ��� ��� ��ΰ� �߸��Ǿ��ų� ����� �߸�
			//case HS_ERR_ANTICPXCNT_BAD_CLIENT_FILE:			// Ŭ���̾�Ʈ ����� �߸�
			//case HS_ERR_ANTICPXCNT_BAD_REQUEST:				// �����κ��� ���� ��û �޽����� �߸�
			//case HS_ERR_ANTICPXCNT_HSHIELD_CORE_ENGINE_NOT_WORKING:	// �ٽ��� �ھ� ������ ���� �۵� ����.
			//case HS_ERR_ANTICPXCNT_UNKNOWN:
				{
					if( s_ResponseRet != nRet)
					{
						//I3NOTICE( "[HackShield] Detect invalid response (0x%X)", nRet);

						s_ResponseRet = nRet;
					}
				}
				return FALSE;
			}
				
		}
		else
		{
			s_ResponseRet = nRet;
			return TRUE;
		}
	}

	return FALSE;
}

static LPTOP_LEVEL_EXCEPTION_FILTER s_lpPreviousFilter = NULL;

LONG CALLBACK ProcessUnhandledException(LPEXCEPTION_POINTERS params)
{
	//	�ٽ��� ���� (���� ����ÿ��� ó������� ��.)
	HackShield::StopService(FALSE);
	HackShield::Release(FALSE);

	/*char szMsg[MAX_STRING_COUNT] = "";
	char * pszExceptType = "";
	switch(params->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		pszExceptType="Access violation. You're probably doing something nasty with a null pointer.";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		pszExceptType="Array bounds exceeded. You're reading beyond the end of an array.";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		pszExceptType="Float divide by zero. You're dividing a float or double by zero.";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		pszExceptType="Integer divide by zero. You're dividing an int or long by zero.";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		pszExceptType="Illegal instruction. Either your .EXE was corrupted or somebody's\n"
			"been fooling around with bad assembly code.";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		pszExceptType="Noncontinuable exception. You hit \"ignore\" after a fatal error.";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		pszExceptType="Stack overflow. You either have a problem with recursive functions or you're\n"
			"declaring WAY too many local variables.";
		break;
	default:
		pszExceptType="Unknown exception. Try searching for the code under MSDN.";
		break;
	}

	i3String::Format( szMsg, sizeof( szMsg),
				">>> Unhandled exception: 0x%08X (%s)\r\nAddress: 0x%08X\r\n",
				params->ExceptionRecord->ExceptionCode,
				pszExceptType,
				params->ExceptionRecord->ExceptionAddress);

	__log( szMsg);*/

	//	���� �ڵ鷯 ����
	SetUnhandledExceptionFilter( s_lpPreviousFilter);

#ifdef I3_DEBUG
    return EXCEPTION_CONTINUE_SEARCH;    
#else
    return EXCEPTION_EXECUTE_HANDLER;
#endif
}

void HackShield::SetUnhandledException(void)
{
	//	�ڵ鷯 ����
	s_lpPreviousFilter = SetUnhandledExceptionFilter( ProcessUnhandledException);
}

#endif