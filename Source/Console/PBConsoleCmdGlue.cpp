
#include "pch.h"

#include "PBConsoleCommon.h"
#include "CheatKey.h"


// �ӽ� �ܼ�Ŀ�ǵ����Դϴ�. Cheatkey�� �����ϱ����� �ӽ������� �ָܼ���ۼ��� ����մϴ�.
static bool ProcessTemporaryConsoleCommand(i3UICSCmdDispCtx* ctx, bool& rst);


static bool PBConsoleCmdDispatch(i3UICSCmdDispCtx* ctx)
{
	bool rstParse, rstExec = false;
	i3::string keyword;	
	i3::vector<i3::string> params;

	char szLabel[1024] = "";
	i3String::WideCharToMultiByte( GetCodePage(), 0, ctx->GetCmdText(), -1, szLabel, _countof(szLabel), nullptr, nullptr);

	i3::string mbcsStr(szLabel);

	bool temporaryResult = false;
	if(ProcessTemporaryConsoleCommand(ctx, temporaryResult))
		return temporaryResult;

	rstParse = Cheatkey::ParseCommand(mbcsStr.c_str(), keyword, params);
	
	if(!rstParse)
	{
		ctx->SetResult(EI3UICSRST_FAIL_EXEC_PARSE_ERR);
		return false;
	}

	i3::shared_ptr<Cheatkey::ICommand> cmd = Cheatkey::CreateCommand(keyword.c_str());

	if( cmd != nullptr)
		rstExec = (*cmd)(params);

	if(!rstExec)
	{
		if( PBConsoleManager::getInstance()->ExecuteCommand( keyword, params, ctx) == false)
		{
			ctx->SetResult(EI3UICSRST_FAIL_EXEC_INVALID_CALL);
			return false;
		}
	}

	ctx->SetResult(EI3UICSRST_OK);	
	return true;
}


// PBConsoleCommon.h���Ͽ���  #define __USE_PBCONSOLE__ �� �����ϸ� 
// �����ϳ��� �Լ��ڵ���� �����Ŀ� �������Ͽ��� ��� �������.
#ifdef __USE_PBCONSOLE__

void PBConsoleCmdGlue_Init() 
{
	i3UIConsoleClientContext::SetPreDispatcher(PBConsoleCmdDispatch);
}

void PBConsoleCmdGlue_Destroy() 
{
	i3UIConsoleClientContext::SetPreDispatcher(nullptr);
}

#endif




//
// �ӽ��ָܼ�ɱ���, CheatKey�� �����Ǹ� �����ٶ�
//

#define PBCS_TMPCMD(x) (!strnicmp(szLabel, x, strlen(x)))

static bool ProcessTemporaryConsoleCommand(i3UICSCmdDispCtx* ctx, bool& rst)
{
	char szLabel[1024] = "";
	i3String::WideCharToMultiByte(GetCodePage(), 0, ctx->GetCmdText(), -1, szLabel, _countof(szLabel), nullptr, nullptr);
	i3::string mbcsStr(szLabel);

	if(PBCS_TMPCMD("/PhyConnect"))
	{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		i3PhysixContext * pCtx = g_pFramework->getPhysixContext();

		if(pCtx && pCtx->getSDK())
			pCtx->getSDK()->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
#endif
		return true;
	}

	return false; // CheatKey�ܼ�Ŀ�ǵ� ����
}
