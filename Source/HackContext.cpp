#include "pch.h"
#include "HackContext.h"

#include "BattleHud.h"

#include "StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

#include "GameCharaInputContext.h"
#include "GameCharaWeaponContext.h"


void HackContext::InitHackFlag()
{
	m_EnableHackCaution = LocaleValue::Enable("FireWall/EnableHackCaution") ? true : false ;
	m_EnableHackFrezen = LocaleValue::Enable("FireWall/EnableHackFrezen") ? true : false;
}

void HackContext::resetHackUserSlot()
{
	memset( m_HackUserSlot, 0 , sizeof(bool) * SLOT_MAX_COUNT );
	memset( m_fPenaltyTime, 0 , sizeof(REAL32) * SLOT_MAX_COUNT );
	memset( m_eHackType, 0 , sizeof(HACK_CHECK_TYPE) * SLOT_MAX_COUNT );
	memset( m_nHackNumber, 0 , sizeof(INT32) * SLOT_MAX_COUNT );
}

void HackContext::setHackUserSlot(INT32 slot,HACK_CHECK_TYPE Type, INT32 Number)
{
	bool bPreviousState = m_HackUserSlot[slot];

	m_HackUserSlot[slot] = true;	
	m_fPenaltyTime[slot] = PENALTY_TIME;
	m_eHackType[slot]	 = Type;
	m_nHackNumber[slot]  = Number;

	if( BattleSlotContext::i()->getMySlotIdx() == slot)
	{
		if( !bPreviousState)	// revision 36452 //ó�� �ѹ��� �޽����� ���� �ݴϴ�. (hansoft.1479)
			CreatePenaltyMsg();

		CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx( slot);
		pChara->MoveStop();

		pChara->Cmd_Idle_Lower();
		pChara->Cmd_Idle_Upper();

		if( pChara->IsDino() == false)
		{
			WeaponBase * pCurWeapon = pChara->GetCurrentCharaWeapon();
			if( pCurWeapon == nullptr) return;
			WeaponBase * pFireWeapon = pCurWeapon->getUsedWeapon();
			pFireWeapon->FireEnd();
		}
		pChara->getInputContext()->setKeyPressInput( 0);

	}
}

void HackContext::CreatePenaltyMsg()
{
	i3::stack_wstring wstr_msg;
	//sprintf_s( msg, 1024, "{col:255,0,0,255}Apply the Penalty{/col}");
	i3::sprintf( wstr_msg, L"{col:255,0,0,255}%s{/col}", GAME_RCSTRING("STR_HACKER_MSG"));

	g_pFramework->getChatBox()->PutSystemChatting( wstr_msg );
}

void HackContext::penaltyUpdate( REAL32 fElapsedTime)
{
	for( INT32 idx = 0; idx < SLOT_MAX_COUNT; ++idx)
	{
		if( m_HackUserSlot[idx])
		{
			m_fPenaltyTime[idx] -= fElapsedTime;

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)  
			static REAL32 temp = 0.f;

			temp += fElapsedTime;
			if( temp > 2.f)
			{
				i3::stack_wstring wstrTemp;
				i3::sprintf( wstrTemp, L"Slot : %d, Penalty Time : %.3f  \n", idx, m_fPenaltyTime[idx]);
				if (CHud::i())
					CHud::i()->CreateInGameMessage(wstrTemp);

				temp = 0.f;
			}
#endif
			if( m_fPenaltyTime[idx] <= 0)
			{
				resetHackUserSlot( idx);
			}
		}
	}
}


void HackContext::HackingDetected(REAL32 rMoveSpeed, UINT32 nType)
{
	//��ŷ üũ�� ���� �ʴ� ������ �ʿ��ѵ��Ͽ� Ŭ���̾�Ʈ������ ��� üũ�� ������ �ֿ��� üũ����� �۵����� �ʵ��� �մϴ�.

	/*	HackDetect ����� Ȱ��ȭ�Ϸ��� �ش� ������ �����
		ClientTCPSocket_Parser_Battle.cpp �� setHackUserSlot �κ��� �ּ��� ����� �ȴ�.	*/
#if 1
	return;	
#else
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		return;
	}
	UINT32 nRemainBattleTime = (UINT32)UserContext::i()->fRemainingBattleTime;
	GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rMoveSpeed, &nType);
#endif
}

void HackContext::GoodbyeCheatUser(CHEAT_CONTEXT CheatContext)
{
	GameEventSender::i()->SetEvent( EVENT_KICK_CHEAT_USER, &CheatContext );
}
