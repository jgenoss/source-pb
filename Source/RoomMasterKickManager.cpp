#include "pch.h"
#include "RoomMasterKickManager.h"

#include "BattleSlotContext.h"
#include "UserInfoContext.h"

#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"
#include "UI/UIPhaseBase.h"
#include "UI/UIPhaseSubBase.h"
#include "UI/UIMessageBox.h"
#include "UI/UITopMenu_V15.h"

I3_CLASS_INSTANCE(CRoomMasterKickManager);//, i3GameNode);

namespace
{
	void CbOkButtonClick(void* pThis,INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			((CRoomMasterKickManager*)pThis)->CloseMessageBox();
		}
	}
}

CRoomMasterKickManager::CRoomMasterKickManager() : 
	awaychecker(nullptr), count_down_sound(nullptr), messagebox(nullptr),
	other_user_count(0), roommaster_countdown_time(0),
	roommaster_countdown(false), all_ready(false), gamestart(false), roomKick(false)
{
	
}

CRoomMasterKickManager::~CRoomMasterKickManager()
{
	I3_SAFE_RELEASE(awaychecker);
	I3_SAFE_RELEASE(count_down_sound);
}


void CRoomMasterKickManager::RemoveAwayChecker()
{
	if(awaychecker != nullptr)
	{
		this->RemoveChild(awaychecker);
		I3_SAFE_RELEASE(awaychecker);
	}

	if(count_down_sound != nullptr)
	{
		I3_SAFE_RELEASE(count_down_sound);
	}

	if (roomKick == true)
	{
		/*��ð� �������� ����\n�κ�� �̵��Ǿ����ϴ�.*/
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY_COMPLETE"));
		roomKick = false;
	}
}

void CRoomMasterKickManager::AddAwayChekcer( )
{
	if( awaychecker == nullptr)
	{
		awaychecker = CAwayChecker::new_object();
		awaychecker->SetReceiver(this);
		awaychecker->Reset();

		I3_GAMENODE_ADDCHILD(this, awaychecker);

	}

	if( count_down_sound == nullptr)
	{
		const char* soundFileName =
		{
			"UI_Count_A.wav"
		};

		i3GameResSound* pResSound = (i3GameResSound*) g_pFramework->QuaryResource( soundFileName);
		I3ASSERT(pResSound != nullptr);
		I3_REF_CHANGE( count_down_sound, pResSound->getSound());
	}
}

void CRoomMasterKickManager::OnUpdate( REAL32 rDeltaSeconds )
{
	if( awaychecker == nullptr) return;

	if( UserInfoContext::i()->CheckAuthLevel() ) return;

	// ������ ��� üũ
	if(_IsHost())
	{
		other_user_count = 0;
		
		// ���� ���� �˻�
		if(_CheckRoomMasterKick() == false) return;

		if( 0 < other_user_count )
		{
			if ( roommaster_countdown == false)
			{
				// ī��Ʈ �ٿ� ����
				all_ready = true;
			}
		}
		else
		{
			ResetAwayChecker(); 
		}
	}
	// ������ �ƴ� ���
	else
	{
		ResetAwayChecker();
	}

	// Update
	_ProcessAway();

	// AwayChecker Update
	i3GameNode::OnUpdate( rDeltaSeconds );
}

const CAwayChecker* CRoomMasterKickManager::GetAwayChecker( )
{
	if(awaychecker != nullptr)
		return awaychecker;

	return nullptr;
}

void CRoomMasterKickManager::ResetAwayChecker()
{
	if (awaychecker != nullptr)
		awaychecker->Reset();

	if (messagebox != nullptr)
	{	
		GameUI::CloseMsgBox(messagebox);
		messagebox = nullptr;
	}
	roommaster_countdown = false;
	all_ready = false;
	roomKick = false;
	roommaster_countdown_time = 0;
	other_user_count = 0;
}

void CRoomMasterKickManager::ReStartAwayChecker( )
{
	if(awaychecker != nullptr)
		awaychecker->Restart();
}

void CRoomMasterKickManager::MoveSubPhase()
{
	UIMainFrame* pUImainFrame = g_pFramework->GetUIMainframe();
	if (messagebox == nullptr 
		&& roommaster_countdown
		&& roommaster_countdown_time > 0
		&& (!pUImainFrame->IsSubPhaseNone() || pUImainFrame->GetCurrentPopup() != nullptr))
	{
		i3::rc_wstring wstr_countdownText;

		/*{col:255,120,0,255}���� ���� %d�� ��{/col}*/
		i3::sprintf(wstr_countdownText,
			GAME_RCSTRING("STR_POPUP_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_ALARM"), roommaster_countdown_time);

		messagebox = GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_countdownText, nullptr, MAKE_CALLBACK(CbOkButtonClick), this);
	}
}

bool CRoomMasterKickManager::_CheckRoomMasterKick()
{
	INT32 ready_slot_count = 0;
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{	
		if( BattleSlotContext::i()->getMainSlotIdx() == i)		continue;

		const SLOT_INFO* pSlot = BattleSlotContext::i()->getSlotInfo(i);

		// ���Կ� �������� �ִ�.
		if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
		{
			if( SLOT_STATE_READY != pSlot->_State)
			{	// ���� Ready�� �ƴմϴ�.
				// ī��Ʈ�ٿ��� �����մϴ�.
				ResetAwayChecker();
				return false;
			}

			// �������� ������ ����� ���� ���� ���� count
			if(BattleSlotContext::i()->getMainSlotIdx()%2 == i%2)
			{
				ready_slot_count++; 
			}
			// ���� �� ����
			other_user_count++;

		}			
	}

	// �������� ������ ��ι���� ���� ���̶�� ī��Ʈ�ٿ� �����ؾ���
	if(other_user_count == ready_slot_count)
	{
		ResetAwayChecker();
		return false;
	}

	return true;
}

bool CRoomMasterKickManager::OnEvent( UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code )
{
	switch(event)
	{
		// ������ �̺�Ʈ
	case ACE_AWAY:
		{
			if(gamestart)
			{
				ResetAwayChecker();
				return true;
			}
			
			_EnterLobby();

			return true;
		} break;
		// �˾� �̺�Ʈ
	case ACE_CAUTION:
		{
			UIMainFrame* pUImainFrame = g_pFramework->GetUIMainframe();
			if( messagebox == nullptr && 
				(!pUImainFrame->IsSubPhaseNone() || pUImainFrame->GetCurrentPopup() != nullptr) )
			{
				i3::rc_wstring wstr_countdownText;

				/*{col:255,120,0,255}���� ���� %d�� ��{/col}*/
				i3::sprintf(wstr_countdownText, 
					GAME_RCSTRING("STR_POPUP_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_ALARM"), roommaster_countdown_time);

				messagebox = GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_countdownText, nullptr, MAKE_CALLBACK(CbOkButtonClick), this);
			}
			return true;
		} break;
	default:
		break;
	}

	return false;
}

bool CRoomMasterKickManager::_IsHost()
{
	return BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx();
}

void CRoomMasterKickManager::__CountDownSound()
{
	// ī��Ʈ �ٿ� ���� �˻�
	if(roommaster_countdown)
	{
		// �ʴ��� �ð� üũ
		if( awaychecker->GetCurrentTime() < roommaster_countdown_time)
		{
			roommaster_countdown_time = static_cast<INT32>(awaychecker->GetCurrentTime());

			// currentTime(ĸ�� ������ �ð� + ���� ����ð�)
			if ( 0 <= roommaster_countdown_time)
			{
				i3::rc_wstring wstr_countdownText;

				/*{col:255,120,0,255}���� ���� %d�� ��{/col}*/
				i3::sprintf(wstr_countdownText, 
					GAME_RCSTRING("STR_TBL_GUI_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_ALARM"), roommaster_countdown_time + 1);

				g_pFramework->getChatBox()->PutSystemChatting(wstr_countdownText);

				i3SoundContext* pSoundContext = g_pViewer->GetSoundContext();
				if( pSoundContext != nullptr)
				{
					pSoundContext->Play(count_down_sound);
				}
			}
		}
	}
}

void CRoomMasterKickManager::__CheckAllReady()
{
	if( all_ready )
	{
		// AwayChecker �ʱ�ȭ
		ResetAwayChecker();
		
		UIMainFrame* uiMainframe = g_pFramework->GetUIMainframe();

		uiMainframe->setKickBtNoStartGame( false);
		uiMainframe->CloseAwayMessageBox();

		// �ð� ����
		awaychecker->SetTime(AWAY_INPUT_ROOM_MASTER_GAMESTART_TIME, AWAY_INPUT_ROOM_MASTER_GAMESTART_TIME);

		all_ready = false;
		roommaster_countdown = true;
		
		roommaster_countdown_time = static_cast<INT32>(awaychecker->GetCurrentTime());
	}
}

void CRoomMasterKickManager::__ProcessMessageBox()
{
	if(messagebox != nullptr)
	{
		if( awaychecker->GetCurrentTime() != roommaster_countdown_time)
		{
			i3::rc_wstring wstr_countdownText;

			/*{col:255,120,0,255}���� ���� %d�� ��{/col}*/
			i3::sprintf(wstr_countdownText, 
				GAME_RCSTRING("STR_POPUP_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_ALARM"), roommaster_countdown_time + 1);

			messagebox->ChangeMessage(wstr_countdownText, nullptr);
		}
	}
}

void CRoomMasterKickManager::_ProcessAway()
{
	// Game�� ���� �Ǿ����� Checker�� ����.
	if( gamestart )
	{
		ResetAwayChecker();
	}

	// Check All Ready
	__CheckAllReady();

	// Sound üũ
	__CountDownSound();

	// MessageBox ���� ����
	__ProcessMessageBox();
}

void CRoomMasterKickManager::_EnterLobby()
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	// MessageBox�� �ݴ´�.
	CloseMessageBox();

	// Room Master Kick���� ���� ���ߴٴ� ǥ��
	roomKick = true;

	GameEventSender::i()->SetEvent(EVENT_ENTER_LOBBY);

	// TopMenu ��ư�� ���� UnSelect�� �����.
	if(g_pEnvSet->m_nUIVersion == UFV_1_5)
		pUIFrame->GetTopMenu_V15()->AllMenuBtnShapeToNormal();
}

void CRoomMasterKickManager::CloseMessageBox( )
{
	if(messagebox != nullptr)
	{
		messagebox->Close();
	}

	messagebox = nullptr;
}
