#include "pch.h"
#include "UIHudMissionBoard.h"
#include "UIHudManager.h"

#include "UIHudUtil.h"
#include "MedalManager.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

#include "MissionCard/MCard.h"
#include "MissionCard/MCardQuest.h"
#include "MissionCard/MCardQuestInfo.h"
#include "MissionCard/MCardUtil.h"

#include "UIUtil.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"


void UIHudMissionBoard::Mission::set(i3UIScene * pScene, const char* c, const char* t)
{
	i3_prof_func();

	check = static_cast<i3UIButtonImageSet*>(pScene->FindChildByName( c ));
	text = FindControl<i3UIStaticText*>(pScene, t);

	text->setTextAutoLineFeed(true);
	text->setTextLineFeedSplitWord(true);

//	const VEC2D* pOrigSize = text->getSize();
//	origSize = *pOrigSize;
}

void UIHudMissionBoard::Mission::set_check(INT32 shapeIdx)
{
	i3_prof_func();
	
	if (check)
	{
		check->SetShapeIdx(shapeIdx);
	}

	if( text )
	{
		COLOR col;

		if (shapeIdx == 2)
			i3Color::Set(&col, (UINT8)0, 255, 0, 255);		// QST_COMPLETED : ���
		else
			i3Color::Set(&col, (UINT8)214, 184, 144, 255);	// �⺻ ��Ʈ ��

		text->SetTextColor(&col);
	}
}

// ���ؽ�Ʈ�ʿ��� �ؽ�Ʈ ���̰� �ʹ� ���, ���η� �÷������� �ڵ带 �����ؾ��� ���̴�..

void UIHudMissionBoard::Mission::set_text(const i3::rc_wstring& t)
{
	if (text)
	{
		i3_prof_func();

		if (strDescForSkip == t)			// ���ٸ� ó�� ����..
			return;

		REAL32 origWidth = text->getWidth();
		text->setSize(origWidth, (REAL32)i3UI::getViewer()->GetViewHeight());			// �ϴ� ������ ũ�� ��ƺ���..�״�����..
		text->SetText(t);
		INT32 extHeight = text->getUIText()->getExtentHeight() + text->getTextSpaceY() * 2 + 2;
		text->setSize(origWidth, (REAL32)extHeight);
		text->SetText(t);

		strDescForSkip = t;
	}
}

void UIHudMissionBoard::Mission::enable_ctrl(bool bEnable)
{
	if(text)
		text->EnableCtrl(bEnable);

	if (check)
		check->EnableCtrl(bEnable);
}

REAL32 UIHudMissionBoard::Mission::getHeight()
{
	if(text != nullptr)
		return text->getHeight();

	return 0.0f;
}

UIHudMissionBoard::UIHudMissionBoard(UIHudManager* p) : iHudBase(p), m_NotifyMedalCount(0), m_NotifyHeight(0.0)
{
}

UIHudMissionBoard::~UIHudMissionBoard()
{
}

/*********************************/
void UIHudMissionBoard::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "MissionNotice");
	m_NotifyHeight = m_FrameList.getFrameWnd(0)->getHeight();

	m_Mission[0].set(pScene, "mn_Chk0", "mn_ST0");
	m_Mission[1].set(pScene, "mn_Chk1", "mn_ST1");
	m_Mission[2].set(pScene, "mn_Chk2", "mn_ST2");
	m_Mission[3].set(pScene, "mn_Chk3", "mn_ST3");

	REAL32 textheight=0.0;
	for(INT32 i = 0; i < MAX_NOTIFY_MADAL_COUNT; i++)
	{
		textheight+=m_Mission[i].getHeight();
	}
	m_NotifyHeight-=textheight;

	i3UIStaticText* pText = FindControl<i3UIStaticText*>(pScene, "mn_ST_Name");
	pText->SetText( GAME_RCSTRING("STR_TBL_PROFILE_CHALLENGE_CARD_CONTENTS") );

}

namespace
{

#define MARGIN_PER_QUEST
#undef MARGIN_PER_QUEST

	static const INT32	MARGIN_PER_QUEST = 5;
}

void	UIHudMissionBoard::AdjustTextPos(INT32 missionCount)
{
	if (missionCount == 0)
		return;

	VEC3D relCheckPos;	
	relCheckPos.x = relCheckPos.y = 0.f;
	if (m_Mission[0].check)
	{
		relCheckPos = *m_Mission[0].check->getPos();
		relCheckPos -= *m_Mission[0].text->getPos();
	}

	for (INT32 i = 1; i < missionCount ; ++i )		// 1���� ����ȴ�..
	{
		const VEC3D* prevPos = m_Mission[i-1].text->getPos();
		REAL32 prevHeight = m_Mission[i-1].getHeight();

		if ( m_Mission[i].text == nullptr || m_Mission[i].check == nullptr)
			continue;

		m_Mission[i].text->setPos( prevPos->x, prevPos->y + prevHeight + (REAL32)MARGIN_PER_QUEST);

		if (m_Mission[i].check)
		{
			const VEC3D* curPos = m_Mission[i].text->getPos();
			m_Mission[i].check->setPos( curPos->x + relCheckPos.x, curPos->y + relCheckPos.y);
		}
	}
		
}

void	UIHudMissionBoard::CloseMissionBoard()
{
	iHudBase::SetVisible(false);
}

void	UIHudMissionBoard::UpdateForMissionCard()
{
	bool isTutorialMode = hu_ut::is_tutorial_mode();

	if (isTutorialMode || g_pEnvSet->m_nUIVersion == UFV_1_5)			// 0 --> 1.5�� ��� �ݹ��� �͵� ó������ ����..
		return;
	
	// �̼�ī�常 ó���� ��..
	const MCard* pCard = MCardUtil::GetSelectedMCard();
	if (pCard == nullptr)
		return;
	
	REAL32 textheight = 0.0f;
	INT32 count = 0;

	for (INT32 i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; ++i)
	{
		const MCardQuest* pQuest = pCard->GetMCardQuest(i);

		if (!pQuest->IsActive() )
			continue;
		
		INT32 tgtCount = pQuest->GetMCardQuestInfo()->m_TargetCompleteCount;
		INT32 curCount = pQuest->GetCurrCompleteCount_OnBattle();
		INT32 criterionCount = pQuest->GetCurrCompleteCount_OffBattle();

		if (tgtCount <= criterionCount)
			continue;
		
		INT32 shapeIdx = 0;

		if (curCount > 0)
		{
			shapeIdx = (curCount < tgtCount) ? 1 : 2;
		}

		i3::rc_wstring wstrDesc = pQuest->GetCurrHUDDesc();

		m_Mission[count].enable_ctrl(true);

		m_Mission[count].set_check( shapeIdx );

		m_Mission[count].set_text( wstrDesc);
					
		// Text Size�� ����
		textheight+= m_Mission[count].getHeight() + (REAL32) MARGIN_PER_QUEST ;
		++count;
	}

	// count���� �ι�° ��Ʈ�Ѻ��� üũ�ڽ� ��ġ�� �ؽ�Ʈ�� �̵��ؾ���...
	UIHudMissionBoard::AdjustTextPos(count);
		
	for (INT32 i = count ; i <MAX_QUEST_PER_CARD_NEW ; ++i)
	{
		m_Mission[i].enable_ctrl(false);		
	}

	if ( m_FrameList.getFrameWnd(0) != nullptr)
		m_FrameList.getFrameWnd(0)->setSize(m_FrameList.getFrameWnd(0)->getWidth(), m_NotifyHeight + textheight);
}


void	UIHudMissionBoard::Update( REAL32 rDeltaSeconds)
{
	bool isTutorialMode = hu_ut::is_tutorial_mode();

	if (isTutorialMode == false && g_pEnvSet->m_nUIVersion != UFV_1_5 )			// 1 --> 1.1 , 2 --> 1.0 �� ��� ������Ʈ ���� ����..
		return;

	i3::wstring text;
	QUEST_STATE qs = QST_NONE;
	REAL32 textheight = 0.0;
	INT32 count = 0;
	for(UINT32 i =0; i<MAX_NOTIFY_MADAL_COUNT; ++i)
	{
		if(isTutorialMode == true)
			qs =  hu::get_tutorial_info(i, text);
		else
			qs =  hu::get_mission_info(i, text);

		if( text.empty() == false)
		{
			INT32 shapeIdx = 0;

			switch(qs)
			{
			case QST_DOING:		shapeIdx = 1;	break;
			case QST_COMPLETED:	shapeIdx = 2;	break;
			default:			break;
			}
			
			m_Mission[count].set_check( shapeIdx );

			m_Mission[count].set_text( text);

			// Text Size�� ����
			textheight+=m_Mission[count++].getHeight() + (REAL32) MARGIN_PER_QUEST;
		}
		
	}

	// count���� �ι�° ��Ʈ�Ѻ��� üũ�ڽ� ��ġ�� �ؽ�Ʈ�� �̵��ؾ���...
	UIHudMissionBoard::AdjustTextPos(count);

	m_FrameList.getFrameWnd(0)->setSize(m_FrameList.getFrameWnd(0)->getWidth(), m_NotifyHeight+textheight);
}

/*********************************/
void UIHudMissionBoard::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	iHudBase::SetVisible(false);
}

void UIHudMissionBoard::event_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	if( hu_ut::is_tutorial_mode() == true)
	{
		iHudBase::SetVisible(true); //Ʃ�丮�� ���� Ȱ��ȭ �ؾ� �ȴ�.
	}
	else if( ( hu_ut::is_stage_mask_training() || hu_ut::is_stage_mask_madness() || hu_ut::is_stage_mask_diehard() ) == true)
	{
		iHudBase::SetVisible(false); 
	}
	else
	{
		if ( g_pEnvSet->m_nUIVersion == UFV_1_5 )
		{
			bool is_enable_mission = hu::is_enable_mission();
			iHudBase::SetEnable(is_enable_mission);
		}
		else
		{
			MCard* pCard = MCardUtil::GetSelectedMCard();
			iHudBase::SetEnable(pCard != nullptr);		// �ӹ�ī�� ����ÿ��� ���ֵ��� ����..
		}
	}
}

void UIHudMissionBoard::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	if(hu_ut::is_domination_mode() == true)
		iHudBase::SetEnable(false);
	else if( ( hu_ut::is_stage_mask_training() || hu_ut::is_stage_mask_madness() || hu_ut::is_stage_mask_diehard() ) == true)
		iHudBase::SetEnable(false);
	else
		iHudBase::SetEnable(true);
}

void UIHudMissionBoard::event_death_battle( INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	const RecvData_EVENT_DEATH_BATTLE& data = i3::value_cast<RecvData_EVENT_DEATH_BATTLE>(UserData);

	const i3::vector<DEATH_INFO_CHARA>& vec_death_info = data.DeathCharaInfo;
	const size_t death_count = vec_death_info.size();

	bool check_visible = true;

	for (UINT32 idx = 0; idx < death_count; idx++)
	{
		const DEATH_INFO_CHARA * pInfo = &vec_death_info[idx];
		INT32 death_idx = GET_DEATHINFO_IDX_TYPE(pInfo->_DeathInfo);

		if (death_idx == BattleSlotContext::i()->GetMySlotIdxEx())
		{
			// �ڽ��� �׾��� ����, Missin Board â�� ���ϴ�.
			check_visible = false;
			break;
		}
	}

	INT32 myidx = BattleSlotContext::i()->GetMySlotIdxEx();
	CGameCharaBase * chara = g_pCharaManager->getCharaByNetIdx(myidx);
	if (chara && chara->isCharaStateMask(CHARA_STATE_DEATH))
	{
		check_visible = false;
	}

	// Stage ������ ����, ����, Ż��, Ż��(�ܿ�) �� ��쿡�� Mission Board�� ����.
	// ���ߵǴ� Stage ��尡 ���� ��� �Ʒ��� �߰� �ϸ� �˴ϴ�.
	switch (MyRoomInfoContext::i()->getStage()->GetStageMode())
	{
	case STAGE_MODE_ANNIHILATION:
	case STAGE_MODE_BOMB:
	case STAGE_MODE_CONVOY:
	case STAGE_MODE_RUN_AWAY:
		iHudBase::SetVisible(check_visible);
		break;
	}
}
