#include "pch.h"

#ifdef	DOMI_FLOW

#include "Domination_WaveMission.h"

#include "GameMode_Domination.h"
#include "../../UI/HUD/UIHUD_Domination_Manager.h"

#include "../ModeScriptManager.h"
#include "../DomiScript.h"

#define DEFAULT_GENERATOR 0
#define ACTIVATE_GENERATOR 1

//----------------------------
//Wave Mission Base
//----------------------------
CWaveMissionBase::CWaveMissionBase( gmode_domination* pmode)
{
	m_pGameMode_Domination = pmode;
}

CWaveMissionBase::~CWaveMissionBase(void)
{
}

/*virtual*/ void CWaveMissionBase::ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer)
{
	switch( iTimerID)
	{
	case 5: //����..Wave Mission����(None, Bomb, Recover....)
#ifdef DOMI_HUD
		if( bSetTimer) UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_MESSAGE, HUD_DMN_EVT_MESSAGE_COUNT_ANNIHILATION, 5); //...
		else UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_MESSAGE, HUD_DMN_EVT_MESSAGE_DISABLE, UIHUDDominationMessage::DMN_MSG_UI_COUNT); //...
#endif
		break;
	}
}

/*virtual*/ i3GameObj* CWaveMissionBase::GetGuildeObject()
{
	CGameCharaBase* pAi;
	VEC3D vDir;
	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());


	i3GameObj* pGuide = NULL;
	UINT32 iMinimumLength = 0xFFFFFFFF;

	for( UINT8 iCnt = 0; iCnt < DINO_COUNT; ++iCnt)
	{
		pAi = g_pCharaManager->getCharaByNetIdx( iCnt, CHARA_INFO_AI);
		if( pAi != NULL && (pAi->isCharaStateMask( CHARA_STATE_DEATH) == false))
		{
			i3Vector::Sub( &vDir, pAi->GetPos(), myPlayer->GetPos());
			if( (UINT32)i3Vector::LengthSq( &vDir) < iMinimumLength)
			{
				iMinimumLength = (UINT32)i3Vector::LengthSq( &vDir);
				pGuide = static_cast<i3GameObj*>(pAi);
			}
		}
	}

	return pGuide;
}

//----------------------------
//Wave Mission None
//----------------------------
CWaveMission_None::CWaveMission_None( gmode_domination* pmode) : CWaveMissionBase(pmode)
{
}

CWaveMission_None::~CWaveMission_None(void)
{
}

//----------------------------
//Wave Mission TimeLimit
//----------------------------
CWaveMission_TimeLimit::CWaveMission_TimeLimit( gmode_domination* pmode) : CWaveMissionBase(pmode),
m_rLimitTime(0), m_rWarningTime(0.f),
m_bColor(false)
{
}

CWaveMission_TimeLimit::~CWaveMission_TimeLimit(void)
{
}

void CWaveMission_TimeLimit::ProcessModeData( INT32 nIndex, INT32 nData)
{
	switch( nIndex)
	{
	case BASE_EVENT_110:
		m_bColor = false;								//ó���� ���� ����.
		m_rLimitTime = static_cast<REAL32>(nData);		//���� �ð� ����.
		break;
	}
}

void CWaveMission_TimeLimit::ProcessUpdate( REAL32 rDeltaTime)
{
	bool bFlush = false;
	char szTime[WAVE_MISSION_LIMIT_TEXT_BUFFER] = {0,};
	INT32 iTime = static_cast<INT32>(m_rLimitTime);

	m_rLimitTime -= rDeltaTime;
	if( m_rLimitTime < 0.f) m_rLimitTime = 0.f;

	if( iTime > static_cast<INT32>(m_rLimitTime))		
		bFlush = true;
	
	INT32 iMinute = static_cast<INT32>(m_rLimitTime) / 60;
	INT32 iSec = static_cast<INT32>(m_rLimitTime) % 60;
	

	if( m_rLimitTime <= WAVE_MISSION_TIME_LIMIT_WARNING_RRMAIN_TIME)
		m_rWarningTime += rDeltaTime;
 
	if( m_rWarningTime > WAVE_MISSION_TIME_LIMIT_WARNING_COLOR_TIME)
	{	
		m_bColor = !m_bColor;
		m_rWarningTime = 0.f;
		
		bFlush = true;
	}


	if( bFlush)
	{
		sprintf_s( szTime, WAVE_MISSION_LIMIT_TEXT_BUFFER, "%02d:%02d", iMinute, iSec);
	
		if( m_bColor)
		{
			char szWarning[WAVE_MISSION_LIMIT_TEXT_BUFFER] = "{col:255, 0, 0, 255}%s{/col}";

			char szTemp[WAVE_MISSION_LIMIT_TEXT_BUFFER] = {0,};
			sprintf_s( szTemp, WAVE_MISSION_LIMIT_TEXT_BUFFER, szWarning, szTime);

			strcpy_s( szTime, WAVE_MISSION_LIMIT_TEXT_BUFFER, szTemp);
		}

#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_TIME, reinterpret_cast< INT32 >(szTime));
#endif
	}
}

//----------------------------
//Wave Mission Eliminate
//----------------------------
CWaveMission_Eliminate::CWaveMission_Eliminate( gmode_domination* pmode) : CWaveMissionBase(pmode),
m_iKillCount(0)
{
}

CWaveMission_Eliminate::~CWaveMission_Eliminate(void)
{
}

void CWaveMission_Eliminate::ProcessModeData( INT32 nIndex, INT32 nData)
{
	if( nData > 999) nData = 999;

	switch( nIndex)
	{
	case BASE_EVENT_110:
		m_iKillCount = 0;		//�ʱ�ȭ.
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData);
#endif
		break;

	case BASE_EVENT_111:
		m_iKillCount = nData;	//�� kill Count �Դϴ�.
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, m_iKillCount);
#endif
		break;
	}
}

//----------------------------
//Wave Mission Recovery
//----------------------------
CWaveMission_Recovery::CWaveMission_Recovery( gmode_domination* pmode) : CWaveMissionBase(pmode),
m_pDestinationObj(NULL)
{
}

CWaveMission_Recovery::~CWaveMission_Recovery(void)
{
}

void CWaveMission_Recovery::ProcessModeData( INT32 nIndex, INT32 nData)
{
	switch( nIndex)
	{
	case BASE_EVENT_110:
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData );
#endif
		break;

	case BASE_EVENT_111:
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, nData );
#endif
		break;

	case BASE_EVENT_114:	//��� ������Ʈ ȸ�� �� ������ ǥ��.
		{
			i3::vector<i3RespawnObj*>& respawnObjectList = m_pGameMode_Domination->GetRespawnObjectList();

			INT32 size = static_cast<INT32>(respawnObjectList.size());
			I3_BOUNDCHK( nData, size);
			if( 0 <= nData && nData < size)
			{
#ifdef DOMI_HUD
				i3RespawnObj* pRespawnObj = respawnObjectList[nData];
				UIHUDDominationIndicator* pHudIndicate = static_cast<UIHUDDominationIndicator*>(UIHUDDominationMgr::i()->GetHUD( HUD_DMN_INDICATOR));

				if( pHudIndicate->InsertIndication( static_cast<i3GameObj*>(pRespawnObj), WAVE_MISSION_RECOVERY2, UI_Util::CIndicateObject::RESPAWN_OBJECT) == false)
					I3TRACE("  -- Can not Insert Indicate Respawn object(%s)\n", pRespawnObj->GetName());
#endif
			}
		}
		break;
	}
}

/*virtual*/ i3GameObj* CWaveMission_Recovery::GetGuildeObject()
{
	//UIHUDDominationScoreBoard* pScore = (UIHUDDominationScoreBoard*)UIHUDDominationMgr::i()->GetHUD( HUD_DMN_SCOREBOARD);
	//if( pScore->GetMissionNoticeState() != UIHUDDominationScoreBoard::NOTICE_END) return NULL;

	//if( m_pDestinationObj != NULL) return static_cast<i3GameObj*>(m_pDestinationObj);

	//UIHUDDominationIndicator* pIndicate = (UIHUDDominationIndicator*)UIHUDDominationMgr::i()->GetHUD( HUD_DMN_INDICATOR);
	//std::vector<UI_Util::CIndicateObject*>& vec = pIndicate->m_vecIndications;

	//for( UINT16 iCnt = 0; iCnt < vec.size(); ++iCnt)
	//{
	//	UI_Util::CIndicateObject* pIndicate = (UI_Util::CIndicateObject*)(vec[iCnt]);
	//	if( pIndicate->GetObjectType() == UI_Util::CIndicateObject::INSTALL_OBJECT)
	//	{
	//		WeaponBase* pObject = static_cast<WeaponBase*>(pIndicate->GetObject());
	//		INT32 nNumber = pObject->getWeaponInfo()->GetNumber();

	//		if( nNumber == GetItemIndex("MISSION_OBJECT") /*Sample*/ || 
	//			nNumber == GetItemIndex("MISSION_OBJECT2")/*Oilcase*/)
	//		{
	//			return pIndicate->GetObject();
	//		}
	//	}
	//}

	return NULL;
}

//----------------------------
//Wave Mission Recovery2
//----------------------------
CWaveMission_Recovery2::CWaveMission_Recovery2( gmode_domination* pmode) : CWaveMissionBase(pmode)	
{
}

CWaveMission_Recovery2::~CWaveMission_Recovery2(void)
{
}

void CWaveMission_Recovery2::ProcessModeData( INT32 nIndex, INT32 nData)
{

}

//----------------------------
//Wave Mission Defence
//----------------------------

CWaveMission_Defence::CWaveMission_Defence( gmode_domination* pmode) : CWaveMissionBase(pmode),
m_pDefenceObj(NULL), 
m_bPressedKey(false), m_bObjectActivated(false), m_bEnableCrossHitOnObjecct(false),
m_rPressedKeyTime(0.0f),
m_iDefenceCount(0)
{
}

CWaveMission_Defence::~CWaveMission_Defence(void)
{
}

void CWaveMission_Defence::_ResetKeyState()
{	
	if( m_bPressedKey)
	{
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());
		myPlayer->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE);
	}

	m_bPressedKey = false;
	m_rPressedKeyTime = 0.f;
}

void CWaveMission_Defence::_Activate()
{
	m_bObjectActivated = true;				//��� ����.

	_ResetKeyState();

	m_bEnableCrossHitOnObjecct = false;
	g_pGameContext->getMissionManager()->setEnableCrossHit( FALSE);	//HUD�� ����, Input�� ���޵��� ��.

#ifdef DOMI_HUD
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_SCOREBOARD_OBJECT_HP_ENABLE, m_pDefenceObj->getNetIndex());
#endif

	//Not Used Object ����.
	std::map<INT32, INT32>::iterator itr;
	itr = m_mapObjectIdx.find( m_pDefenceObj->getNetIndex());
	if( itr != m_mapObjectIdx.end())
	{
		i3Object* pNotUesd = m_pGameMode_Domination->GetWaveMissionObject( itr->second);
		pNotUesd->SetEnable( FALSE);
		pNotUesd->SetDynamicState(I3GAME_DYNAMIC_STATE_NONE);

		m_pDefenceObj->SetEnable( TRUE);
		m_pDefenceObj->SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);
	}
	else
	{
		I3NOTICE("  -- Can not find Not Uesd Defence Object(%d)\n", itr->second);
	}

	I3TRACE(" -- Defence Mission -> Start Defence  \n");
}

void CWaveMission_Defence::_Deactivate()
{
	m_bObjectActivated = false;				//��� ��.
	
	I3TRACE(" -- Defence Mission -> End Defence  \n");
}

void CWaveMission_Defence::ProcessModeData( INT32 nIndex, INT32 nData)
{
	switch( nIndex)
	{
	case BASE_EVENT_110:	//��� �� ������Ʈ ����(���̵��� ���� ����ϴ� ������Ʈ ���� �ٸ�)
		{
			UINT8 offset = 5;
#ifdef DOMI_HUD
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData);
#endif

			// ������Ʈ �ε��� ����.
			m_mapObjectIdx.clear();
			IModeScript*	pScript	= tinst<ModeScriptMgr>()->GetScript();
			for( INT8 iCnt = 0; iCnt < nData; ++iCnt)
			{
				INT32 nUsedObjectIdx = pScript->get_modedata(nIndex + offset + iCnt);
				INT32 nNotUsedObjectIdx = pScript->get_modedata(nIndex + offset + DEFENCE_OBJECT_MAX +iCnt);

				std::map<INT32, INT32>::value_type item( nUsedObjectIdx, nNotUsedObjectIdx);
				m_mapObjectIdx.insert( item);
			}
		}
		break;
	case BASE_EVENT_111:	//Defence Object Total Count
		{
			UINT8 offset = 4;
			m_pGameMode_Domination->RegisterWaveMissionObjects( nIndex + offset, nData);									//�� ����.
			m_pGameMode_Domination->RegisterWaveMissionObjects( nIndex + offset + DEFENCE_OBJECT_MAX, nData, false);		//����.


			m_iDefenceCount = 0;
			m_pDefenceObj = NULL;			
		}
		break;
	case BASE_EVENT_112:			// ��� �� ������Ʈ �ε���, ���⼭�� UI�� ǥ�� ��.
		{
#ifdef DOMI_HUD
			if( m_pDefenceObj != NULL)	// ���Ӱ� ������Ʈ �ϱ� �� ������ �ִ� Indication UI�� ����.
				UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_DELETE_STAGE_OBJECT, m_pDefenceObj->getNetIndex());
#endif

			m_pGameMode_Domination->SetCurrentMissionObjectIndex( nData);		//�ѹ��� �ϳ��� ������Ʈ�� 
			m_pDefenceObj = m_pGameMode_Domination->GetCurrentMissionObject();

			I3ASSERT( m_pDefenceObj != NULL);
			if( m_pDefenceObj != NULL)
			{
				I3TRACE("  -- Recv Defence Object Net_Index(%d) Name(%s)\n", m_pDefenceObj->getNetIndex(), m_pDefenceObj->GetName());
			}
			else
			{
				I3TRACE("  -- It is invalid Defence object Idx(%d)\n", nData);
				return;
			}
#ifdef DOMI_HUD
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_SET_STAGE_OBJECT, WAVE_MISSION_DEFENCE);
#endif
		}
		break;

	case BASE_EVENT_113:		// ��� �� ������Ʈ ����	
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, nData);
#endif
		break;

	case BASE_EVENT_114:		//�������� ������Ʈ Ȱ��ȭ ��ȣ�� ����.
		if( nData)
		{
			_Activate();
		}
		else
		{
			_Deactivate();
		}
		break;
	}
}

/*virtual*/	void CWaveMission_Defence::ResetMissionObject()
{
	//��� ��� �̼� ������Ʈ ����(����).
	std::vector<i3Object*>& pMissionObjects = m_pGameMode_Domination->GetWaveMissionObjects();
	
	std::vector<i3Object*>::iterator itr;
	std::map<INT32, INT32>::iterator itr_idx;

	itr = pMissionObjects.begin();
	while( itr != pMissionObjects.end())
	{
		bool bUse = false;
		i3Object* pObject = *itr;

		for( itr_idx = m_mapObjectIdx.begin(); itr_idx != m_mapObjectIdx.end(); ++itr_idx)
		{
			if( itr_idx->second == pObject->getNetIndex()) { bUse = true; break;}
		}
		
		if( !bUse)
		{
			pObject->SetEnable(FALSE);
			pObject->SetDynamicState(I3GAME_DYNAMIC_STATE_NONE);
		}
		++itr;
	}
}

void CWaveMission_Defence::ProcessInput( REAL32 rDeltaSeconds)
{
	if( m_bEnableCrossHitOnObjecct)
	{
		if( g_pFramework->getKeyStroke() & GAME_KEY_ACTION_SPECIAL) m_bPressedKey = true;

		if( (g_pFramework->getKeyPress() & GAME_KEY_ACTION_SPECIAL) && m_bPressedKey)
		{
			CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());
			myPlayer->addEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE);

			m_rPressedKeyTime += rDeltaSeconds;
			if( m_rPressedKeyTime >= DEFENCE_OBJECT_ACTIVATE_DELAY)
			{
				g_pGameContext->PushKeyNetwork( GAME_KEY_MAP_SPECIALACTION);
				g_pGameContext->SetEvent( EVENT_KEY_PROCESS_NET );

				_ResetKeyState();
			}
		}
		else
		{
			_ResetKeyState();
		}
	}
}

void CWaveMission_Defence::ProcessEventKey( INT32 Arg)
{
	GAME_KEY_MAP	Keymap	= g_pGameContext->GetKeyNetwork( Arg);

	switch( Keymap)
	{
	case GAME_KEY_MAP_SPECIALACTION :
		{
		}
		break;
	}
}

void CWaveMission_Defence::ProcessUpdate(REAL32 rDeltaTime)
{
}

bool CWaveMission_Defence::CheckCrossHitMissionObject( CGameCharaBase* pCharacter)
{
	if( m_pDefenceObj == NULL || m_bObjectActivated) return false;

	MATRIX matObject;
	GlobalFunc::GetNowFrameMatrix( &matObject, m_pDefenceObj->getSceneObject());

	VEC3D vDir;

	i3Vector::Sub( &vDir, i3Matrix::GetPos( &matObject), pCharacter->GetPos());
	REAL32 rDist = i3Vector::Length( &vDir);

	//���� 3m ���� ������ �˻�.
	if( (rDist < 3.f && (i3Math::abs( getY( &vDir)) < 0.05f)) == false)
	{
		_ResetKeyState();

		m_bEnableCrossHitOnObjecct = false;
		return false;
	}

	i3Vector::Sub( &vDir, i3Matrix::GetPos( &matObject), i3Matrix::GetPos( g_pCamera->getCamMatrix()));

	REAL32 rLength_FromCamera = i3Vector::Length( &vDir);

	i3CollideeLine cLine;
	cLine.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	cLine.SetDir( &vDir);

	I3_PHYSIX_HIT_RESULT * pResultBomb = g_pFramework->raycastClosest( &cLine, CGRP_MASKGROUP_ATTACK);
	REAL32 rCollisionLen = 1000000.f;
	if( pResultBomb != NULL)
	{	
		rCollisionLen = pResultBomb->m_T;
	}

	REAL32 rRadius_Object = rLength_FromCamera - rCollisionLen;

	if( rDist <= (rRadius_Object + RADIUS /*+ 0.1f*/))
	{
		// ȭ�鿡 ���� ���� ����� ĳ���� ���������� �մϴ�.
		// ī�޶� ���������� �ϸ� ĳ���Ͱ� �����鼭 ��ü�� Ǯ���� �˴ϴ�.
		i3Vector::Sub( &vDir, i3Matrix::GetPos( &matObject), pCharacter->GetPos());
		i3Vector::Normalize( &vDir, &vDir );
		REAL32 rDot = i3Vector::Dot( &vDir, i3Matrix::GetAt( pCharacter->GetMatrix()));
		if( rDot > 0.6f)
		{
			m_bEnableCrossHitOnObjecct = true;
			g_pGameContext->getMissionManager()->setEnableCrossHit( TRUE);
			return true;
		}
	}

	_ResetKeyState();

	m_bEnableCrossHitOnObjecct = false;
	return false;
}

/*virtual*/ i3GameObj* CWaveMission_Defence::GetGuildeObject()
{
	//UIHUDDominationScoreBoard* pScore = (UIHUDDominationScoreBoard*)UIHUDDominationMgr::i()->GetHUD(HUD_DMN_SCOREBOARD);
	//if( pScore->GetMissionNoticeState() != UIHUDDominationScoreBoard::NOTICE_END) return NULL;

	//if( m_pDefenceObj != NULL && m_pDefenceObj->getCurrentHP() > 0)
	//	return static_cast<i3GameObj*>(m_pDefenceObj);

	return NULL;
}

//----------------------------
//Wave Mission Destroy
//----------------------------
CWaveMission_Destroy::CWaveMission_Destroy( gmode_domination* pmode) : CWaveMissionBase(pmode),
m_pDestroyObj(NULL)
{
}

CWaveMission_Destroy::~CWaveMission_Destroy(void)
{
}

void CWaveMission_Destroy::ProcessModeData( INT32 nIndex, INT32 nData)
{
	switch( nIndex)
	{
	case BASE_EVENT_113:	//�ı� �̼ǿ��� ��� �ϴ� total ������Ʈ ��
		m_pGameMode_Domination->RegisterWaveMissionObjects( nIndex + 1, nData);
		m_pDestroyObj = NULL;
		break;

	case BASE_EVENT_110:
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData);
#endif
		break;

	case BASE_EVENT_111: // �ı����� ������Ʈ �ε���
		{
#ifdef DOMI_HUD
			if( m_pDestroyObj != NULL)
				UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_DELETE_STAGE_OBJECT, m_pDestroyObj->getNetIndex());
#endif

			m_pGameMode_Domination->SetCurrentMissionObjectIndex( nData);		//�ѹ��� �ϳ��� ������Ʈ��
			m_pDestroyObj = m_pGameMode_Domination->GetCurrentMissionObject();

			I3ASSERT( m_pDestroyObj != NULL);
			if( m_pDestroyObj != NULL)
			{
				m_pDestroyObj->SetEnable( TRUE);
				m_pDestroyObj->SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);
				I3TRACE("  -- Recv Destroy Object Net_Index(%d) Name(%s)\n", m_pDestroyObj->getNetIndex(), m_pDestroyObj->GetName());
			}
			else
			{
				I3TRACE("  -- It is invalid Destroy object Idx(%d)\n", nData);
				return;
			}


#ifdef DOMI_HUD
			UIHUDDominationBase* pScoreBoard = UIHUDDominationMgr::i()->GetHUD(HUD_DMN_SCOREBOARD);
			PushScoreBoardEvent* pEvent = new PushScoreBoardEvent( m_pGameMode_Domination);
			pEvent->SetEvent( PushScoreBoardEvent::HUD, HUD_DMN_SCOREBOARD, HUD_DMN_EVT_SCOREBOARD_OBJECT_HP_ENABLE, m_pDestroyObj->getNetIndex());

			bool bSucess = pScoreBoard->PushEvent( pEvent);

			if( bSucess == false)
			{
				delete pEvent;
				UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_SCOREBOARD_OBJECT_HP_ENABLE, m_pDestroyObj->getNetIndex());
			}

			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_SET_STAGE_OBJECT, WAVE_MISSION_DESTROY);
#endif
		}
		break;

	case BASE_EVENT_112:
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, nData);
#endif
		break;
	}
}

/*virtual*/	void CWaveMission_Destroy::ResetMissionObject()
{
	//�ı� �̼� �ı� ������Ʈ ����(����).
	std::vector<i3Object*>& pMissionObjects = m_pGameMode_Domination->GetWaveMissionObjects();
	std::vector<i3Object*>::iterator itr;
	itr = pMissionObjects.begin();
	while( itr != pMissionObjects.end())
	{
		(*itr)->SetEnable(FALSE);
		(*itr)->SetDynamicState(I3GAME_DYNAMIC_STATE_NONE);
		++itr;
	}

	if( m_pDestroyObj != NULL)
	{//Reset �ϱ��� Ÿ�� ������Ʈ �ε����� �޾��� ���(BASE_EVENT_111)�� �ٷ� ����� �Ѵ�.
		m_pDestroyObj->SetEnable( TRUE);
		m_pDestroyObj->SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);
	}
}

/*virtual*/ i3GameObj* CWaveMission_Destroy::GetGuildeObject()
{
	//UIHUDDominationScoreBoard* pScore = (UIHUDDominationScoreBoard*)UIHUDDominationMgr::i()->GetHUD(HUD_DMN_SCOREBOARD);
	//if( pScore->GetMissionNoticeState() != UIHUDDominationScoreBoard::NOTICE_END) return NULL;

	//if( m_pDestroyObj != NULL && m_pDestroyObj->getCurrentHP() > 0) 
	//	return static_cast<i3GameObj*>(m_pDestroyObj);

	return NULL;
}

//----------------------------
//Wave Mission Bomb
//----------------------------
CWaveMission_Bomb::CWaveMission_Bomb( gmode_domination* pmode) : CWaveMissionBase(pmode),
m_pGuideObject(NULL)
{
}

CWaveMission_Bomb::~CWaveMission_Bomb(void)
{
}

void CWaveMission_Bomb::ProcessModeData( INT32 nIndex, INT32 nData)
{
	switch( nIndex)
	{
	case BASE_EVENT_110:
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData );
#endif
		break;

	case BASE_EVENT_111:
#ifdef DOMI_HUD
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, nData );
#endif
		break;
	}
}

/*virtual*/ i3GameObj* CWaveMission_Bomb::GetGuildeObject()
{//���� ��ġ
	CGameCharaBase* pAi;

	for( UINT8 iCnt = 0; iCnt < DINO_COUNT; ++iCnt)
	{
		pAi = g_pCharaManager->getCharaByNetIdx( iCnt, CHARA_INFO_AI);

		if( pAi != NULL && pAi->IsDomiMutantRex() && pAi->isCharaStateMask(CHARA_STATE_DEATH) == false)
		{
			return static_cast<i3GameObj*>(pAi);
		}
	}

	return NULL;
}

//----------------------------
//Wave Mission hold the line
//----------------------------
CWaveMission_Block::CWaveMission_Block( gmode_domination* pmode) : CWaveMissionBase(pmode),
m_iLimit(0), m_nCurrentTriggerAI(-1),
m_pAiRespawnObj(NULL)
{
}

CWaveMission_Block::~CWaveMission_Block(void)
{
}

void CWaveMission_Block::ProcessModeData( INT32 nIndex, INT32 nData)
{
	switch( nIndex)
	{
	case BASE_EVENT_110:
		{
			m_vecTriggerAI.clear();
			m_nCurrentTriggerAI = -1;

			m_iLimit = (UINT16)nData;
			char szRemain[WAVE_MISSION_LIMIT_TEXT_BUFFER] = {0,};	

			sprintf_s( szRemain, WAVE_MISSION_LIMIT_TEXT_BUFFER, "%d", m_iLimit);

#ifdef DOMI_HUD
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_REMAIN, reinterpret_cast< INT32 >(szRemain));
#endif
		}
		break;

	case  BASE_EVENT_111:
		{
#ifdef DOMI_HUD
			UINT16 iRemain = m_iLimit - (UINT16)nData;
			if( iRemain <= 0) iRemain = 0;

			char szRemain[WAVE_MISSION_LIMIT_TEXT_BUFFER] = {0,};
			sprintf_s( szRemain, WAVE_MISSION_LIMIT_TEXT_BUFFER, "%d", iRemain);

			if( iRemain <= 10)
			{
				ZeroMemory( szRemain, sizeof(szRemain));

				REAL32 fRatio = uiMath::interpolate( (REAL32)iRemain, 0.f, 10.f);
				sprintf_s( szRemain, WAVE_MISSION_LIMIT_TEXT_BUFFER, "{col:255, %d, %d, 255}%d{/col}", (UINT8)(255*fRatio), (UINT8)(255*fRatio), iRemain);
			}

			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_REMAIN, reinterpret_cast< INT32 >(szRemain));
#endif
		}
		break;

	case BASE_EVENT_112:	//AI ������ ������Ʈ �ε���
		{
			i3::vector<i3RespawnObj*>& respawnObjectList = m_pGameMode_Domination->GetRespawnObjectList();

			INT32 size = static_cast<INT32>(respawnObjectList.size());
			I3_BOUNDCHK( nData, size);
			if( 0 <= nData && nData < size)
			{
				m_pAiRespawnObj = respawnObjectList[nData];
			}
		}
		break;

	case BASE_EVENT_113:	//Ʈ���� �ǵ帰 AI idx
		{
			m_vecTriggerAI.push_back( nData);
		}
		break;

	case BASE_EVENT_114:	//AI ���� ���� ������ ������Ʈ �ε���
		{
			i3::vector<i3RespawnObj*>& respawnObjectList = m_pGameMode_Domination->GetRespawnObjectList();

			INT32 size = static_cast<INT32>(respawnObjectList.size());
			I3_BOUNDCHK( nData, size);
			if( 0 <= nData && nData < size)
			{
				m_pAiDestObj = respawnObjectList[nData];

#ifdef DOMI_HUD
				//Indicate �߰�.
				UIHUDDominationIndicator* pHudIndicate = static_cast<UIHUDDominationIndicator*>(UIHUDDominationMgr::i()->GetHUD( HUD_DMN_INDICATOR));
				if( pHudIndicate->InsertIndication( static_cast<i3GameObj*>(m_pAiDestObj), WAVE_MISSION_BLOCK, UI_Util::CIndicateObject::RESPAWN_OBJECT) == false)
					I3TRACE("  -- Can not Insert Indicate Respawn object(%s)\n", m_pAiDestObj->GetName());
#endif
			}
		}
		break;

	case BASE_EVENT_115:	//������ AI Idx
		{
			std::vector<INT32>::iterator itr = m_vecTriggerAI.begin();
			while( itr != m_vecTriggerAI.end())
			{
				if( *itr == nData) 
				{
					m_vecTriggerAI.erase( itr);
#ifdef DOMI_HUD
					UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_DELETE_CHARACTER_OBJECT, *itr);
#endif
					break;
				}
			}
		}
		break;
	}
}

/*virtual*/ void CWaveMission_Block::ProcessUpdate( REAL32 rDeltaTime)
{
	std::vector<INT32>::iterator itr = m_vecTriggerAI.begin();

	while( itr != m_vecTriggerAI.end())
	{
		CGameCharaBase* pAi = g_pCharaManager->getCharaByNetIdx( *itr, CHARA_INFO_AI);

		if( pAi == NULL || pAi->isCharaStateMask( CHARA_STATE_DEATH))
		{
#ifdef DOMI_HUD
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_DELETE_CHARACTER_OBJECT, *itr);
#endif
			itr = m_vecTriggerAI.erase( itr);
			continue;
		}
		++itr;
	}

	itr = m_vecTriggerAI.begin();
	if( !m_vecTriggerAI.empty() && m_nCurrentTriggerAI != *itr)
	{
#ifdef DOMI_HUD
		CGameCharaBase* pNewAi = g_pCharaManager->getCharaByNetIdx( *itr, CHARA_INFO_AI);

		UIHUDDominationIndicator* pHudIndicate = static_cast<UIHUDDominationIndicator*>(UIHUDDominationMgr::i()->GetHUD( HUD_DMN_INDICATOR));
		if( pHudIndicate->InsertIndication( static_cast<i3GameObj*>(pNewAi), WAVE_MISSION_ELIMINATE, UI_Util::CIndicateObject::CHARACTER_OBJECT) == false)
			I3TRACE("  -- Can not Insert Indicate Respawn object(%s)\n", m_pAiRespawnObj->GetName());
#endif

		m_nCurrentTriggerAI = *itr;
	}
}

/*virtual*/ i3GameObj* CWaveMission_Block::GetGuildeObject()
{
	//Ʈ���Ÿ� �ǵ帰 AI�� ������ AI�� ���� ����
	if( !m_vecTriggerAI.empty())
	{
		CGameCharaBase* pAi = g_pCharaManager->getCharaByNetIdx( *(m_vecTriggerAI.begin()), CHARA_INFO_AI);
		return static_cast<i3GameObj*>(pAi);
	}

	//Ʈ���Ÿ� �ǵ帰 AI�� ������ ������ ������Ʈ ����
	return static_cast<i3GameObj*>(m_pAiRespawnObj);
}

#endif