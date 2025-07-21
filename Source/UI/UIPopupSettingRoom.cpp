#include "pch.h"
#include "UIPopupSettingRoom.h"

#include "UIPopupSelectMode.h"
#include "UIPopupSelectStage.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "../CommunityContext.h"
#include "../TextSet.h"

enum MAP_MODE
{
	MM_NONE = 0,
	// ��ü��
	MM_DEATHMATCH,
	MM_DEATHMATCH_SNIPING,
	MM_DEATHMATCH_SHORTGUN,
	MM_DEATHMATCH_MELEE,
	MM_DEATHMATCH_4VS4,
	MM_DEATHMATCH_CHALLENGE,
	MM_HEADHUNTER,
	MM_HEADKILLER,
	MM_SUDDENDEATH,
	MM_RPG7,
	MM_DEATHMATCH_PISTOL,

	// Ư����
	MM_ANNIHILATION,
	MM_BOMBMISSION,
	MM_CTF,
	MM_DEFENCE,
	MM_DESTROY,
	MM_SABOTAGE,
	MM_VIP,

	// ������
	MM_DINOSAUR_CC,
	MM_DINOSAUR_DOMINATION,
};


static UIPopupSettingRoom * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPopupSettingRoom*) pFrame->GetPopup( UPW_SETTINGROOM);
}

extern "C"
{
	int UIPopupSettingRoom_ChangeObjectCondition( LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg( L, 1);
		GetThis()->ChangeObjectCondition(cur);
		return 0;
	}

	int UIPopupSettingRoom_ChangeTimeLimit( LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg( L, 1);
		GetThis()->ChangeTimeLimit( cur);
		return 0;
	}

	int UIPopupSettingRoom_ClickOK( LuaState * L)
	{
		GetThis()->SettingRoomOK();
		return 0;
	}

	int UIPopupSettingRoom_ClickCancel( LuaState * L)
	{
		GetThis()->SettingRoomCancel();
		return 0;
	}

	int UIPopupSettingRoom_ClickSelectMode( LuaState * L)
	{
		GetThis()->ClickSelectMode();
		return 0;
	}

	int UIPopupSettingRoom_ClickSelectStage( LuaState * L)
	{
		GetThis()->ClickSelectStage();
		return 0;
	}

	int UIPopupSettingRoom_ChangeAICount ( LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg( L, 1);
		GetThis()->ChangeALCount(cur);
		return 0;
	}

	int UIPopupSettingRoom_ChangeAILevel( LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg( L, 1);
		GetThis()->ChangeAILevel( cur);
		return 0;
	}
}

LuaFuncReg UIPoupSettingRoom_Glue[] = {
	{"ChangeObjectCondition",		UIPopupSettingRoom_ChangeObjectCondition},
	{"ChangeTimeLimit",				UIPopupSettingRoom_ChangeTimeLimit},
	{"pbClickCreate",				UIPopupSettingRoom_ClickOK},
	{"pbClickCreateCancel",			UIPopupSettingRoom_ClickCancel},
	{"ClickSelectMode",				UIPopupSettingRoom_ClickSelectMode},
	{"ClickSelectStage",			UIPopupSettingRoom_ClickSelectStage},
	{"ChangeAICount",				UIPopupSettingRoom_ChangeAICount},
	{"ChangeAILevel",				UIPopupSettingRoom_ChangeAILevel},
	{NULL,							NULL}
};

I3_CLASS_INSTANCE( UIPopupSettingRoom, UIPopupBase);

UIPopupSettingRoom::UIPopupSettingRoom()
{
	m_CreateGameMode = STAGE_TYPE_NA;

	m_nStageID = STAGE_ID_NA;

	m_SubType = 0;
	m_LockWeapon = 0;
	m_LockInterEnter = 0;
	m_LockObserver = 0;
	m_LockTeamBalance = 0;

	m_nAIEnemyCnt = 0;
	m_nAILevel = 0;

	// Ư���� ���� �ڵ�
	g_pGameContext->ClearPreStageOption();

	m_bRandomMap = false;
}

UIPopupSettingRoom::~UIPopupSettingRoom()
{	
}

void UIPopupSettingRoom::_SetDefaultInfo( ROOM_INFO_BASIC * pInfo)
{
	char strPassword[NET_ROOM_PW + 1] = {0,};
	
	// ��й�ȣ �� ���� - NULL���� ���ڿ��� �ƴϴ�! 4byte ���̳ʸ�!
	i3String::NCopy(strPassword, g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);

	LuaState * L = _CallLuaFunction( "SetDefaultInfo");
	i3Lua::PushA2UTF8String( L, pInfo->_Title);
	i3Lua::PushA2UTF8String( L, "");
	i3Lua::PushInteger(L , -1);
	i3Lua::PushA2UTF8String( L, strPassword);
	_EndLuaFunction( L, 4);
}

void UIPopupSettingRoom::_SetOptionWindow()
{
	LuaState * L ;

	if(m_StageOption == STAGE_OPTION_AI)
	{
		L = _CallLuaFunction( "SetAIModeWindow");
		_EndLuaFunction(L, 0);
	}
	else if( getStageType( getStageID()) == STAGE_TYPE_DOMINATION)	///[���� �̼�]�ɼ� â ����.
	{
		L = _CallLuaFunction( "SetDominationModeWindow");
		_EndLuaFunction(L, 0);
	}
	else
	{
		L = _CallLuaFunction( "SetDefaultWindow");
		i3Lua::PushBoolean(L , g_pGameContext->IsEscapeRoom() == false ? 1 : 0);
		_EndLuaFunction( L, 1);
	}
}

void UIPopupSettingRoom::_SetStageInfo( void)
{
	LuaState * L;

	STAGE_ID nStageID = getStageID();

	if( nStageID == 0)return; // ���̵��� ���ü� �־� return

	STAGE_TYPE stageType = getStageType( nStageID);

	// �� �̹���
	INT32 imageIdx = g_pFramework->GetMapShapeIdx( (STAGE_ID) nStageID);

	bool bRandomMap = getRandomMap();

	if(bRandomMap)
	{
		imageIdx = g_pFramework->GetRandomMapIdx();
	}

	// �� �̸�, ���
	std::string strMapName;
	std::string strModeName;
	BOOL bAllowChangeMode = FALSE;
	
	GameUI::GetStageMapName( bRandomMap, (STAGE_ID)nStageID, strMapName);
	GameUI::GetStageModeName( strModeName, stageType, m_StageOption);
	bAllowChangeMode = GameUI::IsAllowChangeMode( stageType);

	//  X-Mas Mark
	bool bXMasEnable = false;

	if(TESTBIT(getNewMarkStageTable( getStageOrd(nStageID)) , GOODS_MARK_XMAS))
		bXMasEnable = true;

	L = _CallLuaFunction( "SetMapInfo");
	if( L != NULL)
	{
		i3Lua::PushA2UTF8String( L, strModeName.c_str());
		i3Lua::PushA2UTF8String( L, strMapName.c_str());
		i3Lua::PushInteger( L, imageIdx);
		i3Lua::PushBoolean( L , bXMasEnable ? TRUE : FALSE );
		i3Lua::PushColor( L, GameUI::GetStageModeColor( getStageType( nStageID), m_StageOption ));
		_EndLuaFunction( L, 8);
	}

	//SelectStageOK �Լ����� ȣ��Ǵ� ���� Stage�� ����� ����̱⿡ üũ �� �� ���·� ����Ǿ� �Ѵ�.
	SetLockSniper(FALSE, FALSE);
	SetLockRPG7(FALSE);
}

void UIPopupSettingRoom::_SetCondition( void)
{
	LuaState * L;
	STAGE_ID nStageID = getStageID();
	STAGE_TYPE stageType = getStageType( nStageID);

	// ����/�� �ε��� ����
	INT32 i;
	INT32 idxMin, idxMax, curCondition, curTime;
	bool	bIsRound = false;
	bool	bIsObserver = false;
	char szCondition[ MAX_STRING_COUNT] = {0,};
	char szTimes[ MAX_STRING_COUNT] = {0,};

	UINT32 gameType	= m_SubType & 0x0F;
	UINT32 gameTime	= m_SubType & 0xF0;

	m_GameTypeList.Clear();
	
	{	// ���� �Ǵ� ųī��Ʈ ����
		switch( stageType )
		{
		case STAGE_TYPE_ANNIHILATION	: 
		case STAGE_TYPE_SUDDEN_DEATH	:
		case STAGE_TYPE_HEAD_KILLER	:
		case STAGE_TYPE_BOMB		:
		case STAGE_TYPE_DESTROY		:
		case STAGE_TYPE_DEFENCE		:
			idxMin = BATTLE_ROUND_TYPE_3;
			idxMax = BATTLE_ROUND_TYPE_9;		// ������

			for( i = idxMin; i <= idxMax; i++)
			{
				_AddGameType((void*)i);;
				i3String::Concat( szCondition, TextSet::Round( i));
				i3String::Concat( szCondition, ";");
			}
			bIsRound = true;

			if( stageType == STAGE_TYPE_ANNIHILATION ||
				stageType == STAGE_TYPE_BOMB )
			{
				bIsObserver = true;	// ��������� On
			}
			break;

		case STAGE_TYPE_ESCAPE		:
			i3String::Format(szCondition, MAX_STRING_COUNT, "%s;", GAME_STRING("STBL_IDX_MODE_TYPE1") );
			bIsRound = true;
			//Round ���� ����� �ƴϰ� ���� ���� �̹Ƿ�
			//���� ComboBox �ʱ� ���� ���� 0���� ������ �Ѵ�.
			idxMax = idxMin = gameType;
			break;

		default					:
			idxMin = BATTLE_KILL_TYPE_60;
			idxMax = BATTLE_KILL_TYPE_160;		// Kill��

			for( i = idxMin; i <= idxMax; i++)
			{
				_AddGameType((void*)i);
				i3String::Concat( szCondition, TextSet::KillCount( i));
				i3String::Concat( szCondition, ";");
			}
			break;
		}

		curCondition = gameType - idxMin;
	}

	m_GameTimeList.Clear();

	{	// ���� �ð� ����
		switch( stageType)
		{
		case STAGE_TYPE_BOMB		:
		case STAGE_TYPE_DESTROY		:
		case STAGE_TYPE_ANNIHILATION		:
		case STAGE_TYPE_SUDDEN_DEATH :
		case STAGE_TYPE_HEAD_KILLER :
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_7);
			break;
		case STAGE_TYPE_DEFENCE		:
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			break;

		case STAGE_TYPE_ESCAPE		:
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_5);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_5);
			break;
		case STAGE_TYPE_HEAD_HUNTER	:
		case STAGE_TYPE_DEATHMATCH	:
		default						:
			// AI�� = 5~10�� , ��Ÿ = 5~30�� ����
			if(isTrainingMode())
			{
				idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
				idxMax = HINIBBLE(BATTLE_TIME_TYPE_30);
			}
			else
			{
				idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
				idxMax = HINIBBLE(BATTLE_TIME_TYPE_30);
			}
			break;
		}

		for( i = idxMin; i <= idxMax; i++)
		{
			_AddGameTime((void*)i);
			i3String::Concat( szTimes, TextSet::GameTime( MAKEBYTE(i, 0)));
			i3String::Concat( szTimes, ";");
		}

		curTime = HINIBBLE( gameTime) - idxMin;
	}

	if(isTrainingMode())
	{
		_SetAIModeCondition(szTimes , curTime);
	}
	else if( getStageType(getStageID()) == STAGE_TYPE_DOMINATION)	///[���� �̼�] ���� ����.
	{
		_SetDominationModeCondition();
	}
	else
	{
		L = _CallLuaFunction( "SetConditions");
		if( L != NULL)
		{
			i3Lua::PushBoolean( L, bIsRound?TRUE:FALSE);
			i3Lua::PushBoolean( L, bIsObserver?TRUE:FALSE);
			i3Lua::PushA2UTF8String( L, szCondition);
			i3Lua::PushInteger( L, curCondition);
			i3Lua::PushA2UTF8String( L, szTimes);
			i3Lua::PushInteger( L, curTime);
			i3Lua::PushInteger( L, m_LockInterEnter);
			i3Lua::PushInteger( L, m_LockTeamBalance);
			_EndLuaFunction( L, 8);
		}
	}
}

void UIPopupSettingRoom::_SetAIModeCondition(char * szTimes, INT32 curTime)
{
	char szLevel[256] = {0};

	for(INT32 idx = 0 ; idx < TRAINING_DIFFICULTY_LEVEL_HIGH ; idx++)
	{
		i3String::Concat( szLevel, TextSet::AIModeLevel(idx + 1));
		i3String::Concat( szLevel, ";");		
	}

	LuaState* L = _CallLuaFunction("SetAIConditions");
	i3Lua::PushA2UTF8String(L , "1;2;3;4;5;6;7;8");
	i3Lua::PushInteger(L , m_nAIEnemyCnt - 1);
	i3Lua::PushA2UTF8String(L , szTimes);
	i3Lua::PushInteger( L, curTime);
	i3Lua::PushA2UTF8String(L , szLevel);
	i3Lua::PushInteger( L, m_nAILevel - 1 );
	_EndLuaFunction( L, 6);
}

void UIPopupSettingRoom::_SetDominationModeCondition()
{
	char szLevel[256] = {0};

	for(INT32 idx = 0 ; idx < TRAINING_DIFFICULTY_LEVEL_HIGH ; idx++)
	{
		i3String::Concat( szLevel, TextSet::AIModeLevel(idx + 1));
		i3String::Concat( szLevel, ";");		
	}

	LuaState* L = _CallLuaFunction("SetDominationConditions");
	i3Lua::PushA2UTF8String(L , szLevel);
	i3Lua::PushInteger( L, m_nAILevel - 1 );
	_EndLuaFunction( L, 2);
}

void UIPopupSettingRoom::_SetWindowName()
{
	LuaState* L = _CallLuaFunction("SetWindowName");

	if(L != NULL)
	{
		// �κ� & ������� ������ �̸��� �������ֱ� ���� �ڵ��Դϴ�.
		i3Lua::PushBoolean(L , 
			g_pFramework->GetUIMainframe()->GetCurrentPhaseType() == UIPHASE_LOBBY ? TRUE : FALSE);
		_EndLuaFunction(L , 1);
	}
}

void UIPopupSettingRoom::_SetLimits( const LimitAtt & limitAtt )
{
	LuaState * L = _CallLuaFunction( "setLimitWeapon");
	i3Lua::PushInteger( L, limitAtt.Primary );			// �ֹ���
	i3Lua::PushInteger( L, limitAtt.Secondary);			// ��������
	i3Lua::PushInteger( L, limitAtt.Melee );	// ���� ����
	i3Lua::PushInteger( L, limitAtt.Throw );				// ��ô ����
	i3Lua::PushInteger( L, limitAtt.RPG7 );				// RPG7
	_EndLuaFunction( L, 5);

	// Ư��������
	INT32 weaponMode = 0;
	
	STAGE_TYPE eStageType = getStageType( m_nStageID);


	// ������, ������, ���̼�, �ı��̼��� �ֹ��� Lock
	if( isSniperMode() || isShotGunMode() || isEscapeMode() ||
		eStageType == STAGE_TYPE_DEFENCE ||
		eStageType == STAGE_TYPE_DESTROY)	weaponMode = 1;	// �ֹ��� Lock
	else if( isKnuckleMode())				weaponMode = 2;	// �ֹ���,�������� off

	L = _CallLuaFunction("SpecialWeaponMode");
	if( L != NULL)
	{
		i3Lua::PushInteger(L, weaponMode);
		_EndLuaFunction(L,1);
	}
}

void UIPopupSettingRoom::_SetObserver( void)
{
	LuaState * L = _CallLuaFunction( "setLimitObserver");
	i3Lua::PushInteger( L, TESTBIT(m_LockObserver, LOCK_OBSERVER_CHASE)?1:0);	// ���� ���
	i3Lua::PushInteger( L, TESTBIT(m_LockObserver, LOCK_OBSERVER_ENEMY)?1:0);	// ����� ����
	i3Lua::PushInteger( L, TESTBIT(m_LockObserver, LOCK_OBSERVER_FREE)?1:0);		// 3��Ī ����
	i3Lua::PushInteger( L, TESTBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP)?1:0);	// HPǥ��
	_EndLuaFunction( L, 4);
}

bool UIPopupSettingRoom::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;


	m_nAIEnemyCnt = g_pGameContext->getEnemyAiCount();
	m_nAILevel = g_pGameContext->getDifficultyLevel();

	//�������� �ҷ����� �� �ʱ�ȭ
	UpdateRoomInfo();

	// [�ӽ�] ���� â �� ����ȵ��� ��쿡�� ���Ӹ�带 ��Ȱ��ȭ�մϴ�.
	// ��ȹ ���뿡 ���� ���������Դϴ�.
	BOOL bModeSelEnable = TRUE;

	if(isSniperMode() || isHeadHunterMode() || 
		isTrainingMode() || isKnuckleMode() || isEscapeMode() )
	{
		bModeSelEnable = FALSE;
	}

	LuaState* L = _CallLuaFunction("setSpecialModeConfig");
	i3Lua::PushBoolean(L , bModeSelEnable); 
	_EndLuaFunction(L , 1);


	//UpdateRoomInfo ȣ��ÿ� _SetStageInfo �� �Լ� �ȿ��� SetLockRPG7 �� ȣ���Ѵ�.
	//Lobby���� ������ ������ �ٽ� �����Ѵ�.
	SetLockSniper(TESTBIT(m_LockWeapon, LOCK_SECONDARY) ? 1 : 0,
		TESTBIT(m_LockWeapon, LOCK_THROW) ? 1 : 0);
	SetLockRPG7(TESTBIT(m_LockWeapon, LOCK_RPG7) ? 1 : 0);

	_SetWindowName();

	return true;
}

void UIPopupSettingRoom::ChangeMode( UI_MODE_TYPE mode, UI_LIMIT_MODE limit /*= ULM_NONE*/)
{
	m_StageOption = STAGE_OPTION_NONE;

	// �������� ��� Ȱ��ȭ ����� �ɼ��� �߰��Ѵ�. (1ȸ��)
	if(limit == ULM_SHOTGUN)
		g_pGameContext->AddPreStageOption(STAGE_OPTION_SHOTGUN);

	switch( mode)
	{
	case UMT_BOMBMISSION :	m_nStageID = getFirstStageByType( STAGE_TYPE_BOMB);	break;
	case UMT_ANNIHILATION :
		m_nStageID = getFirstStageByType( STAGE_TYPE_ANNIHILATION);
		m_StageOption = STAGE_OPTION_ANNIHILATION;
		break;
	case UMT_DEATHMATCH :	m_nStageID = getFirstStageByType( STAGE_TYPE_DEATHMATCH);	break;
	case UMT_DESTROY :		m_nStageID = getFirstStageByType( STAGE_TYPE_DESTROY);		break;
	case UMT_AI :
		g_pGameContext->AddPreStageOption( STAGE_OPTION_AI);
		
		m_nStageID = getFirstStageByType( STAGE_TYPE_DEATHMATCH);
		m_StageOption = STAGE_OPTION_AI;
		m_nAIEnemyCnt = 8;
		m_nAILevel = TRAINING_DIFFICULTY_LEVEL_LOW;

		g_pGameContext->RemovePreStageOption( STAGE_OPTION_AI);
		break;
	case UMT_ESCAPE :		m_nStageID = getFirstStageByType( STAGE_TYPE_ESCAPE);	break;
	case UMT_DEFENCE :		m_nStageID = getFirstStageByType( STAGE_TYPE_DEFENCE);	break;
	case UMT_SUDDENDEATH :
		m_nStageID = getFirstStageByType( STAGE_TYPE_SUDDEN_DEATH);
		m_StageOption = STAGE_OPTION_SUDDENDEATH;
		break;
	case UMT_HEADHUNTER :
		m_nStageID = getFirstStageByType( STAGE_TYPE_HEAD_HUNTER);
		m_StageOption = STAGE_OPTION_HEADHUNTER;
		break;
	case UMT_CLANMATCH :	break;
	case UMT_TUTORIAL :
		m_StageOption = STAGE_OPTION_TUTORIAL;
		break;

	case UMT_DOMINATION :	
		///[���� �̼�] ����Ʈ ��������.
		///m_nStageID = getFirstStageByType( STAGE_TYPE_DOMINATION);
		 if( LocaleValue::Enable( "EnableDomination"))
			m_nStageID = STAGE_ID_DEADTOWN;

		break;
	default :
		I3FATAL( "Not set mode!!!");
	}

	// ù ��ȿ�� ���������� ã�� �� ������ �ɼ��� �����Ѵ�.
	if(	g_pGameContext->IsPreStageOption(STAGE_OPTION_SHOTGUN))
		g_pGameContext->RemovePreStageOption(STAGE_OPTION_SHOTGUN);

	switch( limit)
	{
	case ULM_ROCKET :
		m_StageOption = STAGE_OPTION_RPG;
		break;
	case ULM_SNIPER :
		m_StageOption = STAGE_OPTION_SNIPER;
		m_nStageID = getFirstSniperStage();
		break;
	case ULM_SHOTGUN :
		m_StageOption = STAGE_OPTION_SHOTGUN;
		break;
	case ULM_KNUCKLE :
		m_StageOption = STAGE_OPTION_KNUCKLE;
		m_nStageID = getFirstKnuckleStage();
		break;
	}

	//STAGE_OPTION_RPG			= 0x0002,

	// ��� ����� ������ �ʱ�ȭ
	if(m_bRandomMap)
	{	
		m_bRandomMap = false;
		g_pGameContext->ClearRandomMapIndexList();
	}
	
	_SetOptionWindow();

	GetMainFrame()->ClosePopup( UPW_SELECTMODE);
}

void UIPopupSettingRoom::ChangeStage( STAGE_ID stage)
{
	GetMainFrame()->ClosePopup( UPW_SELECTSTAGE);

	m_nStageID = stage;
}

void UIPopupSettingRoom::ChangeTimeLimit( INT32 comboIndex)
{
	UINT8 gameTime = _GetGameTime(comboIndex);
	UINT8 gameType = LONIBBLE(m_SubType);

	m_SubType = MAKEBYTE(gameTime, gameType);
}

void UIPopupSettingRoom::ChangeObjectCondition( INT32 comboIndex)
{
	UINT8 gameTime = HINIBBLE(m_SubType);
	UINT8 gameType = LONIBBLE(_GetGameType(comboIndex));

	m_SubType = MAKEBYTE(gameTime, gameType);
}

void UIPopupSettingRoom::UpdateRoomInfo()
{
	//Control State�� ��ư� ����
	ROOM_INFO_BASIC	roomInfo;
	ROOM_INFO_ADD	roomAddInfo;

	m_bRandomMap = g_pGameContext->IsRandomMap();

	g_pGameContext->ClearRandomMapIndexList();

	if(m_bRandomMap)
	{
		if(g_pGameContext->GetRandomMapCount() < 1)
		{
			g_pGameContext->ClearRandomMapList();
			g_pGameContext->ClearRandomMapIndexList();
			ROOM_INFO_BASIC		MyRoomInfo;
			g_pGameContext->getMyRoomInfo(&MyRoomInfo);
			m_bRandomMap = false;
			MyRoomInfo._InfoFlag = SET_ROOM_INFO_RANDOM_MODE( MyRoomInfo._InfoFlag, m_bRandomMap);
			g_pGameContext->setMyRoomInfo(&MyRoomInfo);
		}
		else
		{
			for(INT32 i = 0 ; i < g_pGameContext->GetRandomMapCount() ; i++)
			{
				INT32 MapIndex = g_pGameContext->GetRandomMap(i);	
				g_pGameContext->AddRandomMapIndex(MapIndex);
			}
		}
	}

	g_pGameContext->getMyRoomInfo(&roomInfo);
	g_pGameContext->getMyRoomInfoAdd(&roomAddInfo);

	if( roomInfo._StageID == 0) return;

	m_nStageID = (STAGE_ID) roomInfo._StageID;
	m_LockWeapon = roomInfo._WeaponFlag;
	m_LockInterEnter = roomAddInfo._LockInterEnter;
	m_LockTeamBalance = roomAddInfo._LockTeamBalance;
	m_LockObserver = roomAddInfo._LockObserver;
	m_SubType = roomAddInfo._SubType;
	
	m_StageOption = (STAGE_OPTION_TYPE)g_pGameContext->GetStageOption();

	// �������� ��ü���� ���� �����ϳ�, �������� ��� Ȱ��ȭ ���� �ʴ�
	// ���������� �����ϱ� ����, �������� �ɼ��� ������ �������ش�.
	if(getStageType(m_nStageID) == STAGE_TYPE_ANNIHILATION)
		m_StageOption = STAGE_OPTION_ANNIHILATION;
	
	// �⺻ ����
	_SetDefaultInfo( &roomInfo);

	// Map ����
	_SetStageInfo();

	// ��ǥ
	_SetCondition();

	// ���� ���
	LimitAtt att;

	att.Primary = TESTBIT(m_LockWeapon, LOCK_PRIMARY) ? 1 : 0;
	att.Secondary = TESTBIT(m_LockWeapon, LOCK_SECONDARY) ? 1 : 0;
	att.Melee = TESTBIT(m_LockWeapon, LOCK_MELEE) ? 1 : 0;
	att.Throw = TESTBIT(m_LockWeapon, LOCK_THROW) ? 1 : 0;
	att.RPG7 = TESTBIT(m_LockWeapon, LOCK_RPG7) ? 1 : 0;

	_SetLimits(att);

	// ���� ���
	_SetObserver();

	_SetOptionWindow();
}

/*virtual*/ bool UIPopupSettingRoom::OnExitStart( void)
{
	if( UIPopupBase::OnExitStart() == false)
		return false;

	// ��â�� ������ ������ ����â�鵵 ����!!!
	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTMODE))
		GetMainFrame()->ClosePopup( UPW_SELECTMODE);

	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTSTAGE))
		GetMainFrame()->ClosePopup( UPW_SELECTSTAGE);

	return true;
}

/*virtual*/ bool UIPopupSettingRoom::OnCloseButton( void)
{
	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTMODE))
		return GetMainFrame()->GetPopup( UPW_SELECTMODE)->OnCloseButton();

	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTSTAGE))
		return GetMainFrame()->GetPopup( UPW_SELECTSTAGE)->OnCloseButton();

	return true;
}

void UIPopupSettingRoom::SettingRoomOK( void)
{
	BOOL bNeedFilter = TRUE;
	char szTemp[MAX_STRING_COUNT] = "";
	char *pszTitleName = NULL;
	LuaState * L;

	L = _CallLuaFunction( "getTitleName");
	pszTitleName = (char*) _ReturnLuaString( L, 0);

	// �� �ܾ��Է�
	i3String::Copy( szTemp, pszTitleName, MAX_STRING_COUNT );
	i3String::RemoveAllBlank( szTemp );
	if( i3String::Length( szTemp ) <= 0) { 
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_LOBBY_PLEASE_WRITE_ROOM_NAME"));/*�� ������ �Է��Ͽ� �ֽʽÿ�*/
		return;
	}

	// ����� �̸��� �⺻ �����ϴ� �����̶�� ���͸����� �ʴ´�. �⺻ �������� ���Ϳ� �ɷ� �� ������ �ȵǴ� ������ �ذ���. komet
	for(INT32 i=0; i<4; i++)
	{
		i3String::Format(szTemp, MAX_STRING_COUNT, "STBL_IDX_ROOMNAME_%d", i+1);	// 1 ~ 4

		if( i3String::Compare( GAME_STRING( szTemp), pszTitleName) == 0)
		{
			bNeedFilter = FALSE;	// �⺻ �����ϴ� �������̹Ƿ� ������ �ʿ����.
			break;
		}
	}	

	if( bNeedFilter)
	{
#if defined(LOCALE_CHINA)
		CStringFilter* pStringFilter = CStringFilter::NewObject();
		CStringFilter* pStringFilter2 = CStringFilter::NewObject();
		pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
		pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);

		if( FALSE == g_pFramework->IsFilter( pszTitleName ) || i3String::ContainNoCase(szTemp, "{col:") >= 0 || FALSE == pStringFilter2->IsFilter( pszTitleName )
			|| FALSE == pStringFilter->IsFilterSame( pszTitleName ))

		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_LOBBY_WRONG_WORD"));/*�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�*/
			I3_SAFE_RELEASE( pStringFilter2 );
			I3_SAFE_RELEASE( pStringFilter );
			return;
		}	
		I3_SAFE_RELEASE( pStringFilter2 );
		I3_SAFE_RELEASE( pStringFilter );
#else
		if( FALSE == g_pFramework->IsFilter( pszTitleName ) || i3String::ContainNoCase(szTemp, "{col:") >= 0)
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_LOBBY_WRONG_WORD"));/*�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�*/
			return;
		}	
#endif

	}

	ROOM_INFO_ALL Info, OldInfo;

	g_pGameContext->getMyRoomInfo(&Info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&Info._Add);

	i3mem::Copy( &OldInfo, &Info, sizeof(Info));

	SetInputDisable( false);

	// ����� ������ ���մϴ�!

	// 1.���̸�
	{
		// ! praptor - IME�� ���� �ֽ��ϴ�. �ʱ�ȭ�� �Ѱ��� �ִ���̺��� ū ���ڿ���
		// �Ѿ� �� �� �ֽ��ϴ�. ���ڿ������ �ݵ�� Ȯ���ؾ��մϴ�.

		// IME�� ���� �ִ���̺��� ū ���ڿ��� ������ ��� �߶󳽴�.
		char strTitle[MAX_STRING_COUNT];
		i3String::Copy( strTitle, pszTitleName, MAX_STRING_COUNT );		

		UINT32 titleLength = i3String::Length(strTitle);

		if (NET_ROOM_NAME_SIZE <= titleLength)
		{
			for(INT32 i = NET_ROOM_NAME_SIZE - 1; i >= 0; i--)
			{
				if (2 == i3String::WhatIsCode(strTitle, i))
				{
					strTitle[i] = '\0';
				}
				else
				{
					strTitle[i] = '\0';
					break;
				}
			}
		}

		i3String::Copy(Info._Basic._Title, strTitle, NET_ROOM_NAME_SIZE );
	}

	// 2.���Ӹ�� / Stage
	{	
		Info._Basic._StageID = m_nStageID;			
	}

	// 3.��й� ����
	{
		L = _CallLuaFunction( "getPassword");
		const char * pszPassword = _ReturnLuaString( L, 0);
		bool bChangePW = false;

		if( pszPassword != NULL)
		{
			INT32 Length = i3String::Length( pszPassword);

			if( Length > 0 && Length <= sizeof(char) * NET_ROOM_PW)
			{
				Info._Basic._InfoFlag |=  ROOM_INFO_FLAG_PRIVATE_MODE;

				// 4. ��й�ȣ
				if( i3String::Compare( g_pGameContext->getMyRoomPW(), pszPassword) != 0)
				{
					//�ϴ� ���� ������ �н����带 ���� Clear �� �缳��
					i3mem::FillZero(g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
					i3mem::Copy( g_pGameContext->getMyRoomPW(), pszPassword, sizeof(char) * Length);
					g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_PASSWORD_N);
					bChangePW = true;
				}
			}
			else if(Length == 0)
			{
				Info._Basic._InfoFlag &= ~(ROOM_INFO_FLAG_PRIVATE_MODE);
				i3mem::FillZero( g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
				g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_PASSWORD_N);
				bChangePW = true;				
			}
		}
	}

	// 4. ���� ��� ���� �޴�
	{			
		//
		//// ���� ����	
		m_LockWeapon = 0;

		UNSETBIT(m_LockWeapon, LOCK_PRIMARY);
		UNSETBIT(m_LockWeapon, LOCK_SECONDARY);
		UNSETBIT(m_LockWeapon, LOCK_RPG7);
		UNSETBIT(m_LockWeapon, LOCK_THROW);

		L = _CallLuaFunction( "isLimitPrimary");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockWeapon, LOCK_PRIMARY);
		else
			UNSETBIT(m_LockWeapon, LOCK_PRIMARY);
		

		L = _CallLuaFunction( "isLimitSecondary");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockWeapon, LOCK_SECONDARY); 
		else
			UNSETBIT(m_LockWeapon, LOCK_SECONDARY);
		

		L = _CallLuaFunction( "isLimitRPG7");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockWeapon, LOCK_RPG7);
		else
			UNSETBIT(m_LockWeapon, LOCK_RPG7);

		L = _CallLuaFunction( "isLimitThrow");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockWeapon, LOCK_THROW); 
		else
			UNSETBIT(m_LockWeapon, LOCK_THROW);

		SETBIT( m_LockWeapon, LOCK_MELEE);

		Info._Basic._WeaponFlag = m_LockWeapon;

		UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);

		//
		// �������
		L = _CallLuaFunction( "isLimitObserver");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockObserver, LOCK_OBSERVER_CHASE);
		else
			UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);

		{
			L = _CallLuaFunction( "isLimitObserver3");
			if( _ReturnLuaBoolean( L, 0))
				SETBIT( m_LockObserver, LOCK_OBSERVER_FREE);
			else
				UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
		}

		{
			L = _CallLuaFunction( "isLimitObserverEnemy");
			if( _ReturnLuaBoolean( L, 0))
				SETBIT( m_LockObserver, LOCK_OBSERVER_ENEMY);
			else
				UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
		}

		{
			L = _CallLuaFunction( "isLimitObserverHP");
			if( _ReturnLuaBoolean( L, 0))
				SETBIT( m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
			else
				UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
		}

		// ��������
		{
			LuaState * L = _CallLuaFunction( "isLimitInter");
			i3Lua::PushBoolean( L, getStageType(getStageID()) == STAGE_TYPE_DOMINATION);
			m_LockInterEnter = _ReturnLuaBoolean( L, 1) ? 1 : 0;
		}

		//// ���뷱��
		{
			LuaState * L = _CallLuaFunction( "getAutoTeamBalance");
			m_LockTeamBalance = static_cast<UINT8>(_ReturnLuaInteger(L, 0));
		}

		Info._Add._SubType = m_SubType;
		Info._Add._LockInterEnter = m_LockInterEnter;
		Info._Add._LockObserver = m_LockObserver;
		Info._Add._LockTeamBalance = m_LockTeamBalance;
	}

	// �� ������ �ȹٲ�� ������ �ּ� 2011.09.21 ���õ
	//Info._Basic._StageMask = STAGE_MASK_TYPE_TRAINING;

	// PreStageOption = �����UI Ư������ ���� ����...	
	// �ǹ̰� ���� �κ��ΰͰ��Ƽ� �ּ�ó���մϴ�. 2011.10.25 ���õ
	//if( g_pGameContext->IsPreStageOption( STAGE_OPTION_SNIPER ) ) 
	//{
	//	Info._Basic._WeaponFlag = SET_ROOM_BASIC_WEAPON_SNIPER( Info._Basic._WeaponFlag );
	//	g_pGameContext->RemovePreStageOption( STAGE_OPTION_SNIPER );
	//}

	//if( g_pGameContext->IsPreStageOption( STAGE_OPTION_SHOTGUN ) ) 
	//{
	//	Info._Basic._WeaponFlag = SET_ROOM_BASIC_WEAPON_SHOTGUN( Info._Basic._WeaponFlag );
	//	g_pGameContext->RemovePreStageOption( STAGE_OPTION_SHOTGUN );
	//}

	//if( g_pGameContext->IsPreStageOption( STAGE_OPTION_HEADHUNTER ) )
	//{
	//	g_pGameContext->RemovePreStageOption( STAGE_OPTION_HEADHUNTER );
	//}

	if( isSniperMode())
		SETBIT(Info._Basic._WeaponFlag ,LOCK_SNIPER); 
	else if( isShotGunMode())
		Info._Basic._WeaponFlag = SET_ROOM_BASIC_WEAPON_SHOTGUN( Info._Basic._WeaponFlag );

	if(isTrainingMode())
	{
		g_pGameContext->setEnemyAiCount(m_nAIEnemyCnt);
		g_pGameContext->setDifficultyLevel(m_nAILevel);
		g_pGameContext->setCurrentDifficultyLevel(m_nAILevel);
	}

	// ������ ����
	{
		Info._Basic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE( Info._Basic._InfoFlag, getRandomMap());

		g_pGameContext->ClearRandomMapList();

		for(INT32 idx = 0 ; idx < g_pGameContext->GetRandomMapIndexCount() ; idx ++)
		{
			INT32 CheckMap = g_pGameContext->GetRandomMapIndex(idx);
			g_pGameContext->AddRandomMap(CheckMap);
		}
	}

	// ���� ����Ȱ��� ������ ������.
	bool bChanged = false;
	// ���� �ٲ������
	if( Info._Basic._WeaponFlag != OldInfo._Basic._WeaponFlag ||
		Info._Basic._StageID != OldInfo._Basic._StageID ||
		Info._Basic._InfoFlag != OldInfo._Basic._InfoFlag ||
		i3String::Compare( Info._Basic._Title, OldInfo._Basic._Title) != 0 )
	{
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOMINFO_N, &Info._Basic );
		I3TRACE( "SettingRoom EVENT_CHANGE_ROOMINFO_N\n");
		bChanged = true;
	}

	

	// �� �̿ܿ� �ٸ� ���� �ٲ������
	if (Info._Add._LockInterEnter != OldInfo._Add._LockInterEnter ||
		Info._Add._LockObserver != OldInfo._Add._LockObserver ||
		Info._Add._LockTeamBalance != OldInfo._Add._LockTeamBalance	||
		Info._Add._SubType != OldInfo._Add._SubType)
	{
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_OPTION_N, &Info._Add);
		I3TRACE( "SettingRoom EVENT_CHANGE_ROOM_OPTION_N\n");
		bChanged = true;
	}

	if( bChanged == false)
	{
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &Info);
		I3TRACE( "SettingRoom EVENT_CHANGE_ROOM_ALLINFO_N\n");
	}
	

	GetMainFrame()->TogglePopup( UPW_SETTINGROOM);
}

void UIPopupSettingRoom::SettingRoomCancel( void)
{
	GetMainFrame()->TogglePopup( UPW_SETTINGROOM);
}


/*virtual*/ void UIPopupSettingRoom::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "UIRe/PointBlankRe_Lobby_CreatRoom.i3UIE", UIPoupSettingRoom_Glue);
}

/*virtual*/ bool UIPopupSettingRoom::OnKey_Enter( void)
{
	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTMODE) == false &&
		GetMainFrame()->IsOpenedPopup( UPW_SELECTSTAGE) == false )
	{
		// ENTERŰ�� focus������ IME control���� ó���ϱ⶧����,
		// focus������ IME control�� ���� ��쿡�� ó���Ѵ�.
		g_pFramework->SetFocus(this);
		SettingRoomOK();
		return true;
	}

	return false;
}

void UIPopupSettingRoom::ClickSelectStage( void)
{
	GetMainFrame()->OpenPopup( UPW_SELECTSTAGE, &m_nStageID, &m_StageOption);
}

void UIPopupSettingRoom::ClickSelectMode( void)
{
	i3UI::setDisableUIInput(true);
	GetMainFrame()->OpenPopup( UPW_SELECTMODE);
}

/*virtual*/ void UIPopupSettingRoom::SelectMode( UI_MODE_TYPE mode, UI_LIMIT_MODE limit /*= ULM_NONE*/)
{
	ChangeMode(mode , limit);

	STAGE_TYPE stageType = getStageType( m_nStageID);

	// Ȯ�ο� 
	m_CurModeSelect = (INT32) stageType - 1;

	// �⺻ StageID�� SubType�� ����.
	GameUI::GetDefaultStageInfo( m_nStageID, m_SubType, stageType);

	ROOM_INFO_BASIC	roomInfo;
	g_pGameContext->getMyRoomInfo(&roomInfo);

	roomInfo._StageID = m_nStageID;

	_SetStageInfo();

	// ��ǥ ī��Ʈ
	_SetCondition();

	// ���� ���
	LimitAtt att;
	_SetLimits(att);

	// ���� ��� (��� ����� ��������� �ʱ�ȭ)
	m_LockObserver = 0;
	_SetObserver();
}


/*virtual*/ void UIPopupSettingRoom::SelectStageOK( STAGE_ID stage)
{
	ChangeStage(stage);
	
	_SetStageInfo();
}

void UIPopupSettingRoom::SetLockSniper(int lockSecondary, int lockThrow)
{
	if( m_StageOption == STAGE_OPTION_SNIPER )
	{
		LuaState* L = _CallLuaFunction( "SetLockSniper");
		if( L != NULL)
		{
			i3Lua::PushBoolean( L, lockSecondary);
			i3Lua::PushBoolean( L, lockThrow);
			_EndLuaFunction( L, 2);
		}
	}
}

void UIPopupSettingRoom::SetLockRPG7(int lockRpg7)
{
	LuaState* L = _CallLuaFunction( "SetLockRPG7");
	if( L != NULL)
	{
		i3Lua::PushBoolean( L, isRPG7Mode() ? FALSE : TRUE);

		i3Lua::PushInteger( L, lockRpg7);
		_EndLuaFunction( L, 2);
	}
}

bool UIPopupSettingRoom::isRPG7Mode()
{
	STAGE_ID stageID = getStageID();	
	STAGE_TYPE stageType = getStageType(stageID);
	STAGE_SLOT_MODE stageMode = getStageSlot(stageID);

	bool bRPG7 = true;

	//RPG7�� STAGE_SLOT_MODE_4VS4(Stage Mode) �̴�.
	if( stageMode != STAGE_SLOT_MODE_4VS4 )
		bRPG7 = false;

	// RPG7�� �ı�/��� �̼ǿ� ����� �� ����.
	if( bRPG7 && ( stageType == STAGE_TYPE_DESTROY || stageType == STAGE_TYPE_DEFENCE) )
		bRPG7 = false;

	//RPG7�� ���� ���õ� Stage Mode�� STAGE_OPTION_NONE �̾�� �Ѵ�.
	if( bRPG7 && m_StageOption != STAGE_OPTION_NONE )
		bRPG7 = false;

	//RPG7�� ������ ������ StageOption�� STAGE_OPTION_RPG �̾�� �Ѵ�.
	if( bRPG7 && s_StageInfoTableList.IsActiveStageOption( stageID, STAGE_OPTION_RPG ) == false )
		bRPG7 = false;

	return bRPG7;
}