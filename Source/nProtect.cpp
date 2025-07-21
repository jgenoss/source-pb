#include "pch.h"

#if defined (_NPROTECT_)

#include "StageBattle.h"
#include "nProtect.h"
#include "GlobalVariables.h"

static CNPGameLib * g_pNPGame = nullptr;

char g_szHackMsg[512];

static const char *	_getErrMsg( UINT32 code, bool bPopupFAQ)
{	
	const char * lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_UNKNOWN_ERROR");/*�� �� ���� ����*/

	// ��6. �ֿ信���ڵ塯�� �����Ͽ� ��Ȳ�� �´� �޽����� ������ݴϴ�.
	switch( code)
	{
		case NPGAMEMON_ERROR_EXIST:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DUPLICATED_GAMEGAURD");/*���Ӱ��尡 ���� �� �Դϴ�. ��� �ĳ� ����� �Ŀ� �ٽ� �����غ��ñ� �ٶ��ϴ�.*/
			break;

		case NPGAMEMON_ERROR_GAME_EXIST:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DUPLICATED_RUN");/*������ �ߺ� ����Ǿ��ų� ���Ӱ��尡 �̹� ���� �� �Դϴ�. ���� ���� �� �ٽ� �����غ��ñ� �ٶ��ϴ�.*/
			break;

		case NPGAMEMON_ERROR_INIT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_INIT_ERROR1");/*���Ӱ��� �ʱ�ȭ �����Դϴ�. ����� �� �ٽ� �����غ��ų� �浹�� �� �ִ� �ٸ� ���α׷����� ������ �� ������ ���ñ� �ٶ��ϴ�.*/
			break;

		case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
		case NPGAMEMON_ERROR_NFOUND_GG:
		case NPGAMEMON_ERROR_AUTH_INI:
 
		case NPGAMEMON_ERROR_NFOUND_INI:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_CHECK_GAMEGAURD");/*���Ӱ��� ������ ���ų� �����Ǿ����ϴ�. ���Ӱ��� �¾� ������ ��ġ�غ��ñ� �ٶ��ϴ�.*/
			break;

		case NPGAMEMON_ERROR_CRYPTOAPI:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DAMAGED_WINDOW_FILE");/*�������� �Ϻ� �ý��� ������ �ջ�Ǿ����ϴ�. ���ͳ� �ͽ��÷η�(IE)�� �ٽ� ��ġ�غ��ñ� �ٶ��ϴ�.*/
			break;

		case NPGAMEMON_ERROR_EXECUTE:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_RUN_FAILED1");/*���Ӱ��� ���࿡ �����߽��ϴ�. ���Ӱ��� �¾� ������ �ٽ� ��ġ�غ��ñ� �ٶ��ϴ�.*/
			break;

		case NPGAMEMON_ERROR_ILLEGAL_PRG:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DETECT_WRONG_PROGRAM");/*�ҹ� ���α׷��� �߰ߵǾ����ϴ�. ���ʿ��� ���α׷��� ������ �� �ٽ� �����غ��ñ� �ٶ��ϴ�.*/
			break;

		case NPGMUP_ERROR_ABORT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_UPDATED_ABORTED");/*���Ӱ��� ������Ʈ�� ����ϼ̽��ϴ�. ������ ��� ���� ���� ��� ���ͳ� �� ���� ��ȭ�� ������ ������ ���ñ� �ٶ��ϴ�.*/
			break;

		case NPGMUP_ERROR_CONNECT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_CONNECT_FAIL2");/*���Ӱ��� ������Ʈ ���� ���ӿ� �����Ͽ����ϴ�. ��� �� �ٽ� �����ϰų�, ��Ʈ�� ���¸� ������ ��, �ٽ� �õ��� �ֽʽÿ�.*/
			break;

			/*
		case HOOK_TIMEOUT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_CHECK_SYSTEM1");//���̷����� �����̿���� ���� �ʱ�ȭ�� �����Ͽ����ϴ�. �ֽ� ����� ������ �� ��ǻ�� ��ü �˻縦 �غ��� ��, �ٽ� �õ��� �ֽʽÿ�.
			break;
			*/

		case NPGAMEMON_ERROR_GAMEGUARD:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_REINSTALL_GAMEGAURD");/*���Ӱ��� �ʱ�ȭ ���� �Ǵ� �������� ���Ӱ��� �����Դϴ�. ���Ӱ��� �¾������� �ٽ� ��ġ�ϰ� ������ ������ ���ñ� �ٶ��ϴ�.*/
			break;

			/*
		case NPGMUP_ERROR_PARAM:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_NO_EXIST_INI_FILE");//ini ������ ���ų� �����Ǿ����ϴ�. ���Ӱ��� �¾� ������ ��ġ�ϸ� �ذ��� �� �ֽ��ϴ�.
			break;
			*/

			/*
		case NPGMUP_ERROR_INIT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_NPGMUP_FAIL");//npgmup.des �ʱ�ȭ �����Դϴ�. ���Ӱ��� ������ ������ �ٽ� ���ӽ����� �غ��ϴ�.
			break;
			*/

		case NPGMUP_ERROR_DOWNCFG:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_CONNECT_FAIL1");/*���Ӱ��� ������Ʈ ���� ���ӿ� �����Ͽ����ϴ�. ��� �� ��õ� �غ��ų�, ���� ��ȭ���� �ִٸ� ������ ������ ���ñ� �ٶ��ϴ�.*/
			break;

		case NPGMUP_ERROR_AUTH:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_UPDATED_FAIL");/*���Ӱ��� ������Ʈ�� �Ϸ����� �� �߽��ϴ�. ���̷��� ����� �Ͻ� �߽� ��Ų �� ��õ� �غ��ðų�, PC ���� ���α׷��� ����Ͻø� ������ ������ ���ñ� �ٶ��ϴ�.*/
			break;

		case NPGAMEMON_ERROR_NPSCAN:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_CHECK_SYSTEM2");/*���̷��� �� ��ŷ�� �˻� ��� �ε��� ���� �߽��ϴ�. �޸� �����̰ų� ���̷����� ���� ������ �� �ֽ��ϴ�.*/
			break;

		default:
			// ������ ���� �޽��� ���
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_RUN_FAILED2");/*���Ӱ��� ���� �� ������ �߻��Ͽ����ϴ�. ���� ���� ���� GameGuard ������ �ִ� *.erl ���ϵ��� Game1@inca.co.kr�� ÷���Ͽ� ���� �����ֽñ� �ٶ��ϴ�.*/
			break;
	}

	 
	// ���� �ڵ带 �Ѱ��ָ� ���� �߻��� �ڵ����� ���Ӱ��� FAQ �������� �����
	if( bPopupFAQ)
	{		
		//g_pNPGame->RunFAQ( code);		
	}

	I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect-err] : CodeNum[ %d ] - [ %s ]", code, lpszMsg );

	return lpszMsg;
}

static bool		_NP_CHK( UINT32 code)
{
	if( code != NPGAMEMON_SUCCESS)
	{		
		I3PRINTLOG(I3LOG_FATAL,  _getErrMsg( code, false));
		return false;
	}
	return true;
}

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	// ���⿡�� ���� Message Box ���� ����� �ʰ�, Queueing�� �ߴٰ�
	// �����ϱ� ���� ����ؾ� �Ѵ�.
	// �̰��� Popup��Ű�� �ش� ��ġ�� ������, GameGuard�� ��ġ�� ã�Ƴ���
	// Hack�� ���ɼ��� �����ֱ� ������...
	CHud * pHUD = nullptr;
	if( g_pFramework->IsBattleStage() )
		pHUD = CHud::i();

	switch (dwMsg)
    {
		case NPGAMEMON_CHECK_CSAUTH2 : 
			{
				GG_AUTH_DATA AuthData;
				memcpy(&AuthData, (PVOID)dwArg, sizeof(GG_AUTH_DATA));
				GameEventSender::i()->SetEvent( EVENT_GAME_GUARD_AUTH2, (void*)dwArg);
			}
			return true; 
			break; 
		case NPGAMEMON_CHECK_CSAUTH3 : 
			{
				I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect] : 3" );	//CS �������� ���� ����
				GameEventSender::i()->SetEvent( EVENT_GAME_GUARD_AUTH3, (void*)dwArg );
			}
			return true; 
			break; 
		case NPGAMEMON_COMM_ERROR :
		case NPGAMEMON_COMM_CLOSE :
			return false;

		case NPGAMEMON_INIT_ERROR:
			sprintf_s(g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_INIT_ERROR2"), dwArg);/*���Ӱ��� �ʱ�ȭ ���� : %lu*/
			return false;

		case NPGAMEMON_SPEEDHACK:
			if( pHUD != nullptr) pHUD->SetActivateHackWarnning();
			sprintf_s(g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DETECT_SPEED_HACK"));/*���ǵ����� �����Ǿ����ϴ�.*/			
			return false;

		case NPGAMEMON_GAMEHACK_KILLED:
			if( pHUD != nullptr) pHUD->SetActivateHackWarnning();
			sprintf_s( g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DETECT_GAME_HACK"), g_pNPGame->GetInfo());/*�������� �߰ߵǾ����ϴ�.\r\n%s*/
			return false;

		case NPGAMEMON_GAMEHACK_DETECT:
			if( pHUD != nullptr) pHUD->SetActivateHackWarnning();
			sprintf_s(g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DETECT_GAME_HACK"), g_pNPGame->GetInfo());/*�������� �߰ߵǾ����ϴ�.\r\n%s*/
			return false;

		case NPGAMEMON_GAMEHACK_DOUBT:
			if( pHUD != nullptr) pHUD->SetActivateHackWarnning();
			sprintf_s(g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_CHANGED_GAME_FILE"));/*�����̳� ���Ӱ��尡 �����Ǿ����ϴ�.*/
			return false;
	}

	return TRUE;		// <- Hacking�� �߰ߵ� ��쿡�� ��� Game�� �����ϰڴٴ� �ǹ�.
	// return FALSE;	// <- ���� Hacking�� Detect�Ǿ��� �� ������ �����Ϸ��� �̰ɷ�...
}

void nProtect::SetServerAuth2(GG_AUTH_DATA * pAuthData)
{
	g_pNPGame->Auth2(pAuthData); //����ó���� �������ƾ� �Ѵ�. ��ī���� ���� ����
}

void nProtect::SetServerAuth3(PBYTE pPacket, UINT32 size)
{
	DWORD ret = g_pNPGame->Auth3( pPacket, size, 0); //����ó���� �������ƾ� �Ѵ�. ��ī���� ���� ����
	if( ret == 1 )
	{
		I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect] : 2" );	//CS ������ ���� ��û
	}
	else
	{
		//nProtect �������� ���� �ڵ带 Ȯ��
		I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect-err] : 2 - [ %d ]", ret );	//CS ������ ���� ��û ����
	}
}

bool nProtect::Init( HWND hwndMain)
{
	UINT32 code; 

#ifdef LOCALE_THAILAND
	g_pNPGame = new CNPGameLib("PointBlankTH");
#elif defined(LOCALE_KOREA)
	g_pNPGame = new CNPGameLib("PointBlank");
#else
	I3ASSERT_0;
#endif

	code = g_pNPGame->Init();
	if( _NP_CHK( code) == false)
	{
		I3PRINTLOG(I3LOG_NOTICE, "_NP_CHK false code : %d", code);
		return false;
	}	

	g_pNPGame->SetHwnd( hwndMain);
	return true;
}

bool nProtect::Close(void)
{
/*
*���Ӱ��� ����� �Ʒ� ���� ������ �ٽ� �ѹ� ������ �ֽñ� �ٶ��ϴ�.
1. new�� ����Ͽ� �������� ��ü�� ������ ��� �������� ������ CNPGameLib ��ü�� ���� ���� ó�� �κп��� �ݵ�� delete�� ���ּž� �մϴ�.
*/
	delete g_pNPGame; 
	g_pNPGame = nullptr; 
	return true;
}

bool nProtect::CheckAlive(void)
{
	return g_pNPGame->Check() == NPGAMEMON_SUCCESS;
}

bool nProtect::SendUserID( char * pszID)
{
	UINT32 code = g_pNPGame->Send( pszID);

	return _NP_CHK( code);
}

bool nProtect::Set_D3DDeviceInfo(DWORD dwD3DVer, LPVOID *lppD3DDevicePtr)
{
	return (SetD3DDeviceInfo(dwD3DVer, lppD3DDevicePtr) == true) ? true : false;
}

#endif
