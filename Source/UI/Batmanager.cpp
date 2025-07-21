#include "pch.h"
#include "Batmanager.h"

#include "UIUtil.h"
#include "VideoUtil.h" 
#include "UIMath.h"
#include "UnitTest/GameUnitTest.h"
#include "GlobalInfoDef.h"

bool BaTMaN::use_batman = false;
bool BaTMaN::disable_ai_attack = false;
bool BaTMaN::gm_observer_mode = false;
bool BaTMaN::enable_input_away_checker = true;
bool BaTMaN::enable_native_language = true;
bool BaTMaN::enable_jump_booster = false;

bool BaTMaN::enable_debug_item_name = false;
bool BaTMaN::enable_move_inven_wnd = true;

REAL32 BaTMaN::magnification_respawn = 1.f;
INT32 BaTMaN::jump_booster_factor = 1;

namespace
{
	//BaTMaN::init_batman() �� �Լ��� ȣ���ϱ� ���� ���� ����ü �̴�.
	struct init
	{
		init()
		{
			BaTMaN::init_batman();
		};
	} ___ginit;

	bool is_file_exist = false;
}

void BaTMaN::init_batman()
{
#if 0 
	#ifdef DEV_BUILD
		is_file_exist = true;
		use_batman = true;
		return;
	#endif
#endif
	is_file_exist = FileUtil::is_exist_file("PointBlank.dbg");
	use_batman = FileUtil::get_file_size("PointBlank.dbg") ? 1 : 0;

#ifdef NC_BUILD
	//is_file_exist = false;		//������ ���� �ÿ� �� �κ� �ּ� ���� �ϸ� �ȴ�.
	//use_batman = false;
#endif
}

void BaTMaN::BatMan_Update(REAL32 rDeltaSeconds)
{
	bool batman = GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_RCTRL);

	//Batmat ��� ����
	if( batman )
	{
		if(  GameUI::IsKeyboardStroked(I3I_KEY_B) )
		{
			use_batman = !use_batman;

			if( use_batman )
				GameUI::PrintSystemChat(L"BatMan Ȱ��ȭ");
			else
				GameUI::PrintSystemChat(L"BatMan ��Ȱ��ȭ");

			g_pFramework->PrintUserInfo();
		}
	}

	batman = GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_LALT);

	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_RALT) )
		{
			enable_native_language = !enable_native_language;

			if( enable_native_language )
			{
				if( CGameStringMap::Get()->CreateForPointBlank() == true )
					GameUI::PrintSystemChat(L"Setting Original Language");
				else
					GameUI::PrintSystemChat(L"Setting Original Language Fail...");
			}
			else
			{
				if( CGameStringMap::Get()->LoadingEnglishLanguage() == true )
					GameUI::PrintSystemChat(L"Setting English Language");
				else
					GameUI::PrintSystemChat(L"Setting English Language Fail...");
			}
		}
	}

	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_SPACE) )
		{
			g_pFramework->GotoNextStage("Exit");
		}
	}
}

void BaTMaN::BatMan_Ingame_Update(REAL32 rDeltaSeconds)
{
	if( is_file_exist == false ) return;
	if( use_batman == false ) return;

	BatMan_Update(rDeltaSeconds);

	bool batman = GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_LALT);

	//�Ϲ� AI �ɸ��� ������Ʈ ���մϴ�.
	if( batman )
	{
		if(  GameUI::IsKeyboardStroked(I3I_KEY_A) )
		{
			disable_ai_attack = !disable_ai_attack;
			if( disable_ai_attack )
				GameUI::PrintSystemChat(L"AI ĳ���� �ٺ�");
			else
				GameUI::PrintSystemChat(L"AI ĳ���� õ��");
		}
	}

	//������ �ӵ�
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_R) )
		{
			if( magnification_respawn == 1.f)
			{
				magnification_respawn = 1000.f;
				GameUI::PrintSystemChat(L"������ �ִ� �ӵ�");
			}
			else
			{
				magnification_respawn = 1.f;
				GameUI::PrintSystemChat(L"������ ���� �ӵ�");
			}
		}
	}

	//�� �Է� ���� ����
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_I) )
		{
			enable_input_away_checker = !enable_input_away_checker;
			if( enable_input_away_checker )
				GameUI::PrintSystemChat(L"�� �Է� ���� ���� Ȱ��ȭ");
			else
				GameUI::PrintSystemChat(L"�� �Է� ���� ���� ��Ȱ��ȭ");
		}
	}

	//�� �Է� ���� ����
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_J) )
		{
			enable_jump_booster = !enable_jump_booster;
			if( enable_jump_booster )
			{
				i3::rc_wstring wstr;
				i3::sprintf(wstr, L"Jump Booster Ȱ��ȭ, Booster Rate : %d", jump_booster_factor);
				GameUI::PrintSystemChat( wstr );
			}
			else
			{
				GameUI::PrintSystemChat(L"Jump Booster ��Ȱ��ȭ");
			}
		}

		if( enable_jump_booster && GameUI::IsKeyboardStroked(I3I_KEY_B) )
		{
			++jump_booster_factor;
			jump_booster_factor = uiMath::roundlimited(jump_booster_factor, 1, 9);
			i3::rc_wstring wstr;
			i3::sprintf(wstr, L"Booster Rate : %d", jump_booster_factor);

			GameUI::PrintSystemChat( wstr );
		}
	}

#if ENABLE_UNIT_TEST
	//hud unit test
	if(batman)
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_Q) )
		{
			CGameUnitTest* hud_unit_test = UnitTest::get_unit_test();
			if(hud_unit_test == 0) { UnitTest::force_create_unit_test(); hud_unit_test = UnitTest::get_unit_test(); } //������ ����

			if( hud_unit_test->m_UnitTestFlag.is(UNIT_TEST_HUD) == true )
				hud_unit_test->m_UnitTestFlag.remove(UNIT_TEST_HUD);
			else
				hud_unit_test->m_UnitTestFlag.add(UNIT_TEST_HUD);

			hud_unit_test->ForceChangeHudTest();
		}
	}
#endif
}

void BaTMaN::BatMan_Outgame_Update(REAL32 rDeltaSeconds)
{
	if( is_file_exist == false ) return;
	if( use_batman == false ) return;

	BatMan_Update(rDeltaSeconds);

	bool batman = GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_LALT);

	//GM ����
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_G) )
		{
			gm_observer_mode = !gm_observer_mode;
			if(gm_observer_mode)
				GameUI::PrintSystemChat(L"GM ������ ���");
			else
				GameUI::PrintSystemChat(L"GM ������ �� ���");
		}
	}

	//debug item name
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_N) )
		{
			enable_debug_item_name = !enable_debug_item_name;
			if(enable_debug_item_name)
				GameUI::PrintSystemChat(L"����� ������ �̸�");
			else
				GameUI::PrintSystemChat(L"���� ������ �̸�");
		}
	}

	//move inven wnd
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_S) )
		{
			enable_move_inven_wnd = !enable_move_inven_wnd;
			if(enable_move_inven_wnd)
				GameUI::PrintSystemChat(L"�κ� ������ ������");
			else
				GameUI::PrintSystemChat(L"�κ� ������ ����");
		}
	}

	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_W) )	//������ â ���� ����
		{
			VideoUtil::VideoContext cxt;

			cxt.m_AA = g_pEnvSet->m_AA;
			cxt.m_AAQ = g_pEnvSet->m_AAQ;
			cxt.m_b16BitsDepth = g_pEnvSet->m_b16BitsDepth;
			cxt.m_bFullScreen = false;
			cxt.m_bTriLinearFilter = g_pEnvSet->m_bTriLinearFilter;
			cxt.m_bVSync = g_pEnvSet->m_bVSync;
			cxt.m_nScreenHeight = g_pEnvSet->m_nScreenHeight;
			cxt.m_nScreenHz = g_pEnvSet->m_nScreenHz;
			cxt.m_nScreenWidth = g_pEnvSet->m_nScreenWidth;
			cxt.m_ScreenDepth = g_pEnvSet->m_ScreenDepth;

			VideoUtil::ApplyVideo(cxt);
		}
	}
}