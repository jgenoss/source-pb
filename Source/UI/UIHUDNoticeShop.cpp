#include "pch.h"
#include "UIHUDNoticeShop.h"

#include "UIUtil.h"
#include "UIBattleFrame.h"
#include "UIBattlePopupBase.h"
#include "IGShopContext.h"

#include "BattleHud.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
namespace
{
	UIHUDNoticeShop* g_this = 0;
}

UIHUDNoticeShop* UIHUDNoticeShop::instance()
{
	return g_this;
}

LuaFuncReg UIHUDNoticeShop_Glue[] = {
	{nullptr,						nullptr}
};


I3_CLASS_INSTANCE( UIHUDNoticeShop);//, UIHUDBase);

UIHUDNoticeShop::UIHUDNoticeShop() : m_observer_mode(0)
{
	g_this = this;
	GetSlide()->SetTargetTime(false, 1.f / ( UI_SLIDEOUT_SLOP * 2.f) );
	GetSlide()->SetTargetTime(true, 1.f / ( UI_SLIDEIN_SLOP * 2.f) );
	m_visible = true;
}

UIHUDNoticeShop::~UIHUDNoticeShop()
{
}

void	UIHUDNoticeShop::OnCreate( i3GameNode * pParent)
{
	UIHUDBase::OnCreate(pParent);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == false)
	{	// Ʃ�丮�󿡼��� �ε����� �ʵ���
		AddScene("Scene/HUD/PBRe_HUDShopIcon.i3UIs", UIHUDNoticeShop_Glue, true, false);		// scn�� �ٷ� ����..
		
		LuaState* L = UILua::CallLuaFunction(GetScene(0), "GetValidTime");
		INT32 sec = UILua::ReturnLuaInteger(L, 0);
		UIBattleFrame* frame = static_cast<UIBattleFrame*>(pParent);
		frame->SetGameShopValidTime(sec);
	}
	
}

bool	UIHUDNoticeShop::OnEntranceStart( void * pArg1, void * pArg2)
{
	if (UIHUDBase::OnEntranceStart(pArg1, pArg2) == false ) return false;
	
	
	i3::rc_wstring wstrKey =	GameUI::GetKeymapString( GAME_KEY_MAP_INGAME_SHOP);
	
	i3::stack_wstring wstr;

	i3::sprintf(wstr, L"%s (%s)", GAME_RCSTRING("STR_INGAME_SHOP"), wstrKey);

	i3UIScene* scn = GetScene(0);
	LuaState* L = UILua::CallLuaFunction(scn, "SetName");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstr);
		UILua::EndLuaFunction(L, 1);
	}
	
	GetSlide()->AddFrameWnds(GetScene(0), UISlide::R_TO_L);
	GetSlide()->Start(true);

	return true;
}

void	UIHUDNoticeShop::OnExitEnd()
{
	UIHUDBase::OnExitEnd();
	UIHUDNoticeShop::RestorePos();
}

void	UIHUDNoticeShop::ModifyPos()
{
	i3UIScene* scn = GetScene(0);
	LuaState* L = UILua::CallLuaFunction(scn, "ModifyPos");
	if( L != nullptr)
	{
		UILua::EndLuaFunction(L, 0);
	}
}

void	UIHUDNoticeShop::RestorePos()
{
	i3UIScene* scn = GetScene(0);
	LuaState* L = UILua::CallLuaFunction(scn, "RestorePos");
	if( L != nullptr)
	{
		UILua::EndLuaFunction(L, 0);
	}
}

// Ȱ��ȭ�� ���¿����� �߻��Է� ����..
bool UIHUDNoticeShop::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if( UIHUDBase::OnKeyInput( pInputMng) )
		return true;
		
	// 't'Ű�ʹ� �޸� ���⼭ ó��..���ⱳü�� ��� ä��â����, ��ġ��ġ/���������� �����µ�, ä��â������ �ڵ����� ����..
	// ���⼭ ��ġ��ġ,���������� �ϴ� ����..
	UINT64 stroke = g_pFramework->getKeyStroke();
	if (stroke & GAME_KEY_INGAME_SHOP)
	{	
		UINT64 press  = g_pFramework->getKeyPress();
		if (press & GAME_KEY_CHANGEWEAPON )		// ���ⱳü Űó���� StageBattle�� ����..���߿� �ٸ� ������ ó���ϴ°� �������̴�.
			return false;				

		if( UIBattleFrame::i() != nullptr &&
			UIBattleFrame::i()->IsOpenPopup(UBP_INGAME_SHOP) == false)
		{
			if( !m_observer_mode )
			{
				IGShopContext* cont = g_pFramework->GetIGShopContext();
				cont->SetEventOnce_Open();									// ��û�ϴµ��� HUD�� ������ ����..�ϴ� uibattleFrame���� ��Ŷó����..
				return true;
			}
		}
	}
 
	return false;
}

void UIHUDNoticeShop::SetVisible(bool visible)
{
	if( LocaleValue::Enable("FieldShop") == false ) return;
	GetScene(0)->EnableScene( visible );
	m_visible = visible;
}

void UIHUDNoticeShop::SetObserverMode(bool mode)
{
	if( LocaleValue::Enable("FieldShop") == false ) return;

	if( mode == true && m_observer_mode == mode ) return;
	m_observer_mode = mode;

	if( GetScene(0) != nullptr)
		GetScene(0)->EnableScene( !mode );
}
 
 
/////////////////////////////////////////////////////////

namespace
{



igshop_ctrl::get_handler* g_get_handler_array[STAGE_MODE_MAX] = { 0, };


struct handler_none : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner){}
	virtual void SetDeathDelay(igshop_ctrl* owner){}

	virtual void SetRespawn(igshop_ctrl* owner) {}
	virtual void SetRoundEnd(igshop_ctrl* owner) {}			// ���� ����� ���� �ʴ´�..
	virtual void SetGameEnd(igshop_ctrl* owner) {}
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl*, REAL32) {}	
	virtual void SetIntermissionStart(igshop_ctrl* owner){}
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}

} g_hNone;


struct handler_idle : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner);
	virtual void SetDeathDelay(igshop_ctrl* owner);

	virtual void SetRespawn(igshop_ctrl* owner);
	virtual void SetRoundEnd(igshop_ctrl* owner) {}			// ���� ����� ���� �ʴ´�..
	virtual void SetGameEnd(igshop_ctrl* owner) {}
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) { }
	virtual void OnUpdate(igshop_ctrl*, REAL32) {}
	virtual void SetIntermissionStart(igshop_ctrl* owner){}
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}

} g_hIdle;

struct handler_death :  igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner) {}			// �̹� ���� ���·� ó���� ����..
	virtual void SetDeathDelay(igshop_ctrl* owner) {}		// ���� ���¿��� �����̴� ����..
	virtual void SetRespawn(igshop_ctrl* owner);
	virtual void SetRoundEnd(igshop_ctrl* owner);
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) { }
	virtual void OnUpdate(igshop_ctrl* owner, REAL32) {}
	virtual void SetIntermissionStart(igshop_ctrl* owner){}	// ���� ���´� ���Ծ���.
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}	// ���� ���´� ���Ծ���.

} g_hDeath;


struct handler_death_delay : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner) {}			// ��ü���� ������ �� �浹�� ���� �ȵ�..
	virtual void SetDeathDelay(igshop_ctrl* owner) {}		// �ߺ�������� ��������..
	virtual void SetRespawn(igshop_ctrl* owner);			
	virtual void SetRoundEnd(igshop_ctrl* owner);
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl* owner, REAL32);
	virtual void SetIntermissionStart(igshop_ctrl* owner){}	
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}	

} g_hDeathDelay;

// ���и��� ���������°� ������ ����, �������� ��� ���̵� ���·� ó���ȴ�..
struct handler_respawn : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner);
	virtual void SetDeathDelay(igshop_ctrl* owner);
	virtual void SetRespawn(igshop_ctrl* owner) { }
	virtual void SetRoundEnd(igshop_ctrl* owner);
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner);
	virtual void OnUpdate(igshop_ctrl* owner, REAL32 delta);
	virtual void SetIntermissionStart(igshop_ctrl* owner){}
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}
	
} g_hRespawn;




struct handler_idle_domi : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner);				// �׾����� �Ѵ°� ����..
	virtual void SetDeathDelay(igshop_ctrl* owner);			// �׾����� �Ѵ°� ����..

	virtual void SetRespawn(igshop_ctrl* owner)	{}			// �������� ���������Ƿ� ���� ���..
	virtual void SetRoundEnd(igshop_ctrl* owner) {}			// ���� ����� ���� �ʴ´�..
	virtual void SetGameEnd(igshop_ctrl* owner) {}
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl*, REAL32) {}
	virtual void SetIntermissionStart(igshop_ctrl* owner);
	virtual void SetIntermissionEnd(igshop_ctrl* owner);	

} g_hIdle_domi;

struct handler_death_domi :  igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner) {}			// �̹� ���� ���·� ó���� ����..
	virtual void SetDeathDelay(igshop_ctrl* owner) {}		// ���� ���¿��� �����̴� ����..
	virtual void SetRespawn(igshop_ctrl* owner);
	virtual void SetRoundEnd(igshop_ctrl* owner){}			// ���尡 ����Ǿ ���� ������..����������ũ ����.
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl* owner, REAL32) {}
	virtual void SetIntermissionStart(igshop_ctrl* owner){}	// ���� ���´� ���Ծ���.
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}	// ���� ���´� ���Ծ���.

} g_hDeath_domi;

struct handler_death_delay_domi : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner) {}			// ��ü���� ������ �� �浹�� ���� �ȵ�..
	virtual void SetDeathDelay(igshop_ctrl* owner) {}		// �ߺ�������� ��������..
	virtual void SetRespawn(igshop_ctrl* owner);			
	virtual void SetRoundEnd(igshop_ctrl* owner);
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl* owner, REAL32);
	virtual void SetIntermissionStart(igshop_ctrl* owner){}	// ���� ���´� ���Ծ���.
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}	// ���� ���´� ���Ծ���.	

} g_hDeathDelay_domi;




void handler_idle::SetDeath(igshop_ctrl* owner) 
{  
	// ������ Ų��..
	owner->EnableNoticeShop(true);		
	owner->SetHandler_Death(); 
}

void handler_idle::SetDeathDelay(igshop_ctrl* owner)
{
	// ���� �ʰ� �ڵ鷯�� �ٲ۴�...  ������Ÿ�Ӱ��� �ݵ�� �����ؾ���..
	owner->ResetDeathDelayTime();
	owner->SetHandler_Death_Delay(); 
}

void handler_respawn::SetDeath(igshop_ctrl* owner)
{
	// �� ��쿡�� �����ִ��� �����ִ��� �� �𸣴µ�, �ѵд�...�ٸ� Ÿ�̸Ӹ� �����Ű�°� ���ڴ�..
	owner->StopShopTimer();
	owner->EnableNoticeShop(true);
	owner->SetHandler_Death(); 
	//
}

void handler_respawn::SetDeathDelay(igshop_ctrl* owner)
{
	// �ϴ� Ÿ�̸Ӵ� ����.. �ڵ鷯�� death�� �����..
	owner->StopShopTimer();
	owner->SetHandler_Death(); 
}

// �Ƹ��� ���� ���� �������� ���ӽ��� �������� ����..
void handler_idle::SetRespawn(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(true);		// ������,��ŸƮ�� HUD�� Ų��...Ų�� �״�� ����..�̰� ǥ���ϻ�...	
	owner->SetHandler_Respawn(); 
	owner->StartShopTimer();
}

void handler_death::SetRespawn(igshop_ctrl* owner) 
{  
	// �̹� �������� ������ ���̸�, �ڵ鷯�� �ٲٰ�, Ÿ�̸Ӹ� �Ҵ�..
	owner->SetHandler_Respawn(); 
	owner->StartShopTimer();
	//
}

void  handler_death_delay::SetRespawn(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(true);		// ������ �߿� �������� �����Ƿ� Ų��..
	owner->SetHandler_Respawn(); 
	owner->StartShopTimer();
}

void handler_death_delay::SetRoundEnd(igshop_ctrl* owner)
{
	owner->SetHandler_Idle(); // hud�� �������� ������, ���¸� �ٲ۴�...
}

void handler_respawn::SetRoundEnd(igshop_ctrl* owner)
{
	owner->StopShopTimer();					// Ÿ�̸� ���д�...
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle(); 
}

void handler_death::SetRoundEnd(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle(); 
}


void handler_death::SetGameEnd(igshop_ctrl* owner)			// �⺻�����δ� ����ǥ�� ������ ����..�޶��� ��츦 ����� �и�..
{
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle();
}

void handler_respawn::SetGameEnd(igshop_ctrl* owner)		// �⺻�����δ� ����ǥ�� ������ ����..�޶��� ��츦 ����� �и�..
{
	owner->StopShopTimer();
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle();
}

void handler_death_delay::SetGameEnd(igshop_ctrl* owner)
{
	owner->SetHandler_Idle();	
}



void handler_idle_domi::SetDeath(igshop_ctrl* owner)		// �׾����� �Ѵ°� ����..
{
	// ������ Ų��..
	owner->EnableNoticeShop(true);		
	owner->SetHandler_Death(); 
}

void handler_idle_domi::SetDeathDelay(igshop_ctrl* owner)	// �׾����� �Ѵ°� ����..
{
	// ���� �ʰ� �ڵ鷯�� �ٲ۴�...  ������Ÿ�Ӱ��� �ݵ�� �����ؾ���..
	owner->ResetDeathDelayTime();
	owner->SetHandler_Death_Delay(); 
}

void handler_death_domi::SetRespawn(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(false);			// ���̿����� �������� ������ ����..
	owner->SetHandler_Idle();				// ������ ���°� ���̵� �����̴�..
}

void  handler_death_delay_domi::SetRespawn(igshop_ctrl* owner)		// ���� ���� �ʾ����Ƿ� ������ ����..
{
	owner->SetHandler_Idle();
}

void handler_death_delay_domi::SetRoundEnd(igshop_ctrl* owner)			// ���� �ʾҰ�, �̰� ������ �ٲٰ� �Ѿ���.
{
	owner->EnableNoticeShop(true);
	owner->SetHandler_Death(); // ���� ���� Ȯ���� ��..
}

void  handler_death_domi::SetGameEnd(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(false);							// ��������ÿ��� �������� ��..
	owner->SetHandler_Idle();
}

void handler_death_delay_domi::SetGameEnd(igshop_ctrl* owner)
{
	owner->SetHandler_Idle();						
}


bool	check_respawn_area( i3RespawnObj* res_obj , VEC3D* my_pos)
{
	VEC3D res_pos;

	i3Vector::Copy(&res_pos, res_obj->GetPos());
ISTEST_S
	{
		res_pos.y += 0.05f;
	}
	else
ISTEST_E
	{
		res_pos.y += 0.2f;
	}

	res_pos.y -= my_pos->y;
	REAL32 dist_y = ::fabs(res_pos.y);

	if ( dist_y > INGAME_SHOP_VALID_DISTANCE_Y ) 
		return false;

	res_pos.x -= my_pos->x;
	res_pos.z -= my_pos->z;
	REAL32 dist_sq_xz = res_pos.x * res_pos.x + res_pos.z * res_pos.z;

	// ������ ���� �˻�..(����ϰ� �Ǹ�...����...)
	return (dist_sq_xz <= INGAME_SHOP_VALID_DISTANCE_XZ * INGAME_SHOP_VALID_DISTANCE_XZ );
}


void handler_respawn::OnUpdate(igshop_ctrl* owner, REAL32 delta)
{
// �������� 20�ʱ����� �Ÿ��˻縦 �ؾ���...	// �� ��쿣 �Ѱ� ���� �͸� ó���Ѵ�..
	CGameCharaBase* my_chara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	
	if ( my_chara == nullptr )	// �̷� �һ�簡 �����...idle�� �ٲٰ� ��������..
	{
		owner->EnableNoticeShop(false);
		owner->StopShopTimer();
		owner->SetHandler_Idle(); 
		return;
	}
	
	VEC3D* my_pos = my_chara->GetPos();
	const GLOBAL_CHARA_INFO* ch_info = my_chara->getCharaInfo();
	INT32 nTeam = ch_info->GetTeam();
 
	CGameRespawn*	game_respawn = g_pCharaManager->getRespawnPointInfo();

	const INT32 num_res = game_respawn->GetRespawnCount((CHARACTER_TEAM_TYPE)nTeam);

	INT32 tc_idx =	owner->GetTCRespawnIdx();

	bool bCollis = false;

	if (tc_idx > -1 && tc_idx < num_res )
	{
		i3RespawnObj* res_obj =	game_respawn->GetRespawnObject(nTeam, tc_idx);
		bCollis = check_respawn_area( res_obj, my_pos );
	}
	
	if (bCollis == false)
	{
		for (INT32 i = 0 ; i < num_res ; ++i )
		{
		// ..��ȿ���� üũ..	
			if ( i == tc_idx ) continue;

			i3RespawnObj* res_obj =	game_respawn->GetRespawnObject(nTeam, i);
			bCollis = check_respawn_area( res_obj, my_pos) ;
			if (bCollis == true)
			{
				owner->SetTCRespawnIdx(i);	break;
			}
		}
	}
	
	owner->EnableNoticeShop(bCollis);
}

void handler_death_delay::OnUpdate(igshop_ctrl* owner, REAL32 rDelta) 
{
	owner->AddDeathDelayTime(rDelta);	
	
	if (owner->GetCurrDeathDelayTime() >= INGAME_SHOP_DEATHDELAY_ROUND )
	{
		owner->EnableNoticeShop(true);
		owner->SetHandler_Death(); 
		// ���⼭ ������������, �����Ҷ��� �����Ұ�..
	}
}

void handler_respawn::OnRespawnTimeDone(igshop_ctrl* owner)
{
// �������ð� ����� idle�� ������... (���� ��쿣 �ƿ� �������� ���� ����)
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle();
}


void handler_death_delay_domi::OnUpdate(igshop_ctrl* owner, REAL32 rDelta)
{
	owner->AddDeathDelayTime(rDelta);
	if (owner->GetCurrDeathDelayTime() >= INGAME_SHOP_DEATHDELAY_ROUND )
	{
		owner->EnableNoticeShop(true);
		owner->SetHandler_Death(); 
		// ���⼭ ������������, �����Ҷ��� �����Ұ�..
	}
}

void handler_idle_domi::SetIntermissionStart(igshop_ctrl* owner)
{
	// ���͹̼� ���۽ÿ� Ų��..

	UIHUDNoticeShop* notice = static_cast<UIHUDNoticeShop*>(UIBattleFrame::i()->GetHUD(UIHUD_NOTICE_SHOP));
	notice->ModifyPos();

	owner->EnableNoticeShop(true);		
	// ���� ���� �ƴϰ�, �������Ȱ͵� �ƴϱ� ������, �״�� ���д�.....
}

void handler_idle_domi::SetIntermissionEnd(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(false);		// ���͹̼��߿� ���������Ͱ����Ƿ� �̰� ������ �׳� ���ش�..
}


struct get_handler_normal : igshop_ctrl::get_handler
{
	virtual igshop_ctrl::handler*		idle() { return &g_hIdle; }
	virtual igshop_ctrl::handler*		death() { return &g_hDeath; }
	virtual igshop_ctrl::handler*		death_delay() { return &g_hDeathDelay; }
	virtual igshop_ctrl::handler*		respawn() { return &g_hRespawn; }
} g_get_handler_normal;


struct get_handler_domi : igshop_ctrl::get_handler
{
	virtual igshop_ctrl::handler*		idle() { return &g_hIdle_domi; }
	virtual igshop_ctrl::handler*		death() { return &g_hDeath_domi; }
	virtual igshop_ctrl::handler*		death_delay() { return &g_hDeathDelay_domi; }
	virtual igshop_ctrl::handler*		respawn() { return &g_hIdle_domi; }	
} g_get_handler_domi;


struct init 
{
	init()
	{
		for (INT32 i = 0 ; i < STAGE_MODE_MAX; ++i)
			g_get_handler_array[i] = &g_get_handler_normal;
	};
}_;

igshop_ctrl::get_handler*	get_handler_mtd() { return g_get_handler_array[MyRoomInfoContext::i()->getStage()->GetStageMode()]; }

}

namespace
{
	bool IsThisGameModeFieldShopEnable()
	{
		return !(	MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_SNIPER_ONLY) ||
				MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_SHOTGUN_ONLY) ||
				MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_KNUCKLE_ONLY) || 
				MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)			// Ʃ�͸��� ���...
				);
	}
}


igshop_ctrl::igshop_ctrl() : m_get_handler(get_handler_mtd()), 
	m_handler(&g_hNone), m_tcRespawnIdx(-1), m_bEnableNoticeShop(false), m_fCurrDeathDelayTime_Round(0.f)
{
	if ( LocaleValue::Enable("FieldShop") &&
		IsThisGameModeFieldShopEnable() )
		m_handler = m_get_handler->idle(); 

	m_sc.SetReverse(true);
	m_sc_cb.SetObj(this);
	m_sc.SetCallback(&m_sc_cb);
	m_sc.SetBaseSec(INGAME_SHOP_VALID_TIME);
}

void	igshop_ctrl::SetGameShopValidTime(INT32 sec)
{
	m_sc.SetBaseSec(sec);	
}

void	igshop_ctrl::OnUpdate(REAL32 delta)
{
	igshop_ctrl::Process_NoticeShopQueue();

	m_sc.Update();
	m_handler->OnUpdate(this, delta);
}

void	igshop_ctrl::Process_NoticeShopQueue()
{
	if ( m_queue_popup.empty() ) 
		return;

	UIBattleFrame* frame = UIBattleFrame::i();
	UIHUDBase*	hud = frame->GetHUD(UIHUD_NOTICE_SHOP);
	bool		hud_enable = (hud->isEnable() != false);

	// �����̵� ������ �ѹ��� �ϳ���....�ϰ�ó���� �Ҽ� ����...
	bool target_enable = m_queue_popup[0];
	if (target_enable != hud_enable)
	{
		frame->EnableHUD(UIHUD_NOTICE_SHOP, target_enable != false  );
		m_queue_popup.pop_front();
		I3TRACE("-- �����̵� �� HUD enable ����ó�� ����:%d -- \n", target_enable);
	}
	
}

void	igshop_ctrl::SetDeath()
{
	m_handler->SetDeath(this);
}

void	igshop_ctrl::SetDeathDelay()
{
	m_handler->SetDeathDelay(this);
}

void	igshop_ctrl::SetRespawn()
{
	m_handler->SetRespawn(this);
}

void	igshop_ctrl::SetRoundEnd()
{
	m_handler->SetRoundEnd(this);
}

void	igshop_ctrl::SetGameEnd()
{
	// ����ÿ��� ������ ó���� ���� ť�� ���..
	m_queue_popup.clear();
	//
	m_handler->SetGameEnd(this);
}

void	igshop_ctrl::SetIntermissionStart()
{
	m_handler->SetIntermissionStart(this);
}

void	igshop_ctrl::SetIntermissionEnd()
{
	m_handler->SetIntermissionEnd(this);
}

void	igshop_ctrl::StartShopTimer()
{
	m_sc.Start();
}

void	igshop_ctrl::StopShopTimer()
{
	m_sc.Stop();
}

void	igshop_ctrl::OnSec_Start(INT32 result)
{
	
}

void	igshop_ctrl::OnSec_Change(INT32 result)
{

}

void	igshop_ctrl::OnSec_Done(INT32 result)
{
	m_handler->OnRespawnTimeDone(this);
}

void	igshop_ctrl::OnSec_Stop(INT32 result)
{

}

void	igshop_ctrl::EnableNoticeShop(bool bEnable)
{
	if( UIHUDNoticeShop::instance()->m_visible == false ) return;

	// ������ �������� ��� ����.
	CGameCharaBase* localPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	if (localPlayer->IsDino())
		return;

	if (m_bEnableNoticeShop != bEnable)
	{
		m_bEnableNoticeShop = bEnable;
		
		UIBattleFrame* frame = UIBattleFrame::i();	
		if (frame)
		{
			size_t num_queue = m_queue_popup.size();

			if (num_queue == 0)
			{
				if ( frame->GetHUD(UIHUD_NOTICE_SHOP)->isEnable() == bool(bEnable) )
				{
					// �����̵��߿� ���º�ȯ�� ����� �� ó������ �ʱ� ������ �� ��쿡 ���ؼ� ������Ʈ�� ������..
					m_queue_popup.push_back(bEnable);	// ������ ó���� ��Ȳ�� ����Ѵ�..
					I3TRACE("-- �����̵� �� HUD enable ����õ� Ƚ��:%d ����:%d -- \n", num_queue, bool(bEnable));
				}
				else
				{
					frame->EnableHUD(UIHUD_NOTICE_SHOP, bEnable);	
				}
			}
			else		// ť�׵Ȱ��� �ϳ��� �ִٸ�, ��� ť����..
			{
				m_queue_popup.push_back(bEnable);
				I3TRACE("-- �����̵� �� HUD enable ����õ� Ƚ��:%d ����:%d -- \n", num_queue, bool(bEnable));
			}
		}
	}
	
}



