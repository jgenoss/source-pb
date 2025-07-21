#include "pch.h"
#include "PBCmd_Warp.h"
#include "../../../Cheatkey_LimitPred.h"
#include "../../../Cheatkey_WarpHelpers.h"
#include "../../../GameEventHandler.h"
#include "../../../BattleHUD.h"

namespace Cheatkey
{
	//------------------------------------------------------------------------------------------
	// ���� : Aĳ����(�Ǵ� �� �̻��� ĳ����)�� B�������� �̵�
	//
	// ex) "/wp slot[0] nick[test01]" : 0�� ĳ���͸� 'test01' ĳ���� ��ġ�� �̵�
	//     "/wp slot[0] pos[x,y,z]"   : 0�� ĳ���͸� (x,y,z) ��ġ�� �̵�
	//     "/wp slot[0] set[bomb-a]"  : 0�� ĳ���͸� ���Ͽ� ����� bomb-a ��ġ���� �̵�
	//     "/wp nick[test02] nick[test01]" : 'test02' ĳ���͸� 'test01' ĳ���� ��ġ�� �̵�
	//     "/wp nick[test02] pos[x,y,z]"   : 'test02' ĳ���͸� (x,y,z) ��ġ�� �̵�
	//     "/wp nick[test02] set[bomb-a]"  : 'test02' ĳ���͸� ���Ͽ� ����� bomb-a ��ġ���� �̵�
	//     "/wp slot[0,1,2] set[bomb-a]"  : 1, 2, 3 ĳ���͸� ���Ͽ� ����� bomb-a ��ġ���� �̵�
	//-------------------------------------------------------------------------------------------
	bool Warp::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() < 2) return false;

		WarpHelpers::Parser_SearchMovementPlayer psmp;
		i3::string param_players(params[0]);
		if (psmp.operator()(param_players, m_players) == false) return false;

		WarpHelpers::Parser_SearchMovementDestination psmd;
		i3::string param_destination(params[1]);
		if (psmd.operator()(param_destination, m_destination) == false) return false;

		if (!i3Vector::isValid(&m_destination)) return false;

		return true;
	}

	bool Warp::_Execute()
	{
		QA_COMMAND_TELEPORT_DATA info;

		for (size_t i=0; i<m_players.size(); i++)
		{
			info._i32SlotIdx = m_players[i];

			info._ar32Pos[0] = m_destination.x;
			info._ar32Pos[1] = m_destination.y;
			info._ar32Pos[2] = m_destination.z;

			GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_TELEPORT_SOMEONE, &info);
		}

		return true;
	}

	ICommand* Warp::Creator()
	{
		return new Warp(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//-----------------------------------------------------------------------------------
	// �̵� ��ġ ���� : ��ǥ �Է� �޾Ƽ�
	//	ex) "/wpsave bomb-a, 1.0, 2.0, 3.0" : bomb-a �̸����� (1.0, 2.0, 3.0) ��ǥ ����
	//		"/wpsave bomb-a" : bomb-a �̸����� ���� ī�޶� ��ġ ����
	//-----------------------------------------------------------------------------------
	bool Warp_SaveLocation::_ParseParams(const i3::vector<i3::string>& params)
	{
		m_locationName = params[0];

		if (params.size() == 1) // ī�޶� ��ġ
		{
			i3Vector::Copy(&m_pos, i3Matrix::GetPos(g_pCamera->getCamMatrix()));
		}
		else if (params.size() == 4) // ��ǥ ��ġ
		{
			m_pos.x = static_cast<REAL32>(::atof(params[1].c_str()));
			m_pos.y = static_cast<REAL32>(::atof(params[2].c_str()));
			m_pos.z = static_cast<REAL32>(::atof(params[3].c_str()));
		}

		if (!i3Vector::isValid(&m_pos)) return false;

		return true;
	}

	bool Warp_SaveLocation::_Execute()
	{
		WarpHelpers::WarpKey k;
		k.stageID = g_pFramework->GetCurrentStageIndex();
		k.locationName = m_locationName.c_str(); i3::to_upper(k.locationName);

		// ����� �Ҹ� �� �ڵ� ����Ǳ� ������ �޸𸮿� ���常 ��Ŵ.
		WarpHelpers::WarpStorageInst().AddLocation(k, m_pos);

		if (CHud::i()->isEnableCheatkeyWarpList())	// ��� ����.
		{
			i3::shared_ptr<ICommand> cmdHide(Warp_HideLocations::Creator());
			(*cmdHide)(i3::vector<i3::string>());

			i3::shared_ptr<ICommand> cmdShow(Warp_ShowLocations::Creator());
			(*cmdShow)(i3::vector<i3::string>());
		}

		return true;	
	}

	ICommand* Warp_SaveLocation::Creator()
	{
		return new Warp_SaveLocation(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// �̵� ��ġ ����
	// ex) "/wpdel bomb-a" : ���� ���� bomb-a ��ǥ ����
	//--------------------------------------------------
	bool Warp_DelLocation::_ParseParams(const i3::vector<i3::string>& params)
	{
		m_locationName = params[0];
		return true;
	}

	bool Warp_DelLocation::_Execute()
	{
		WarpHelpers::WarpKey k;
		k.stageID = g_pFramework->GetCurrentStageIndex();
		k.locationName = m_locationName.c_str();

		// ����� �Ҹ� �� �ڵ� ����Ǳ� ������ �޸𸮿� ���常 ��Ŵ.
		WarpHelpers::WarpStorageInst().DelLocation(k);

		if (CHud::i()->isEnableCheatkeyWarpList())	// ��� ����.
		{
			i3::shared_ptr<ICommand> cmdHide(Warp_HideLocations::Creator());
			(*cmdHide)(i3::vector<i3::string>());

			i3::shared_ptr<ICommand> cmdShow(Warp_ShowLocations::Creator());
			(*cmdShow)(i3::vector<i3::string>());
		}

		return true;
	}

	ICommand* Warp_DelLocation::Creator()
	{
		return new Warp_DelLocation(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// ��� ĳ���� ����
	// ex) "/warpall"
	//--------------------------------------------------
	bool WarpAll::_ParseParams(const i3::vector<i3::string>& params)
	{
		WarpHelpers::Parser_SearchMovementDestination psmd;
		i3::string param_destination(params[0]);
		if (psmd.operator()(param_destination, m_destination) == false) return false;

		if (!i3Vector::isValid(&m_destination)) return false;

		return true;
	}

	bool WarpAll::_Execute()
	{
		QA_COMMAND_TELEPORT_DATA info;

		for (size_t i=0; i<SLOT_MAX_COUNT; i++)
		{
			if (g_pCharaManager->getCharaByNetIdx(i) == nullptr) continue;

			info._i32SlotIdx = i;

			info._ar32Pos[0] = m_destination.x;
			info._ar32Pos[1] = m_destination.y;
			info._ar32Pos[2] = m_destination.z;

			GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_TELEPORT_SOMEONE, &info);
		}

		return true;
	}

	ICommand* WarpAll::Creator()
	{
		return new WarpAll(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// ���� ���� ���� ������ ��ġ �����ֱ�
	// ex) "/wpshow"
	//--------------------------------------------------
	bool Warp_ShowLocations::_Execute()
	{
		i3::vector<WarpHelpers::WarpKey> keys;
		WarpHelpers::WarpStorageInst().FillupAllKeysTo(keys);

		i3::string text("Cheatkey Warp Locations :\n\n");
		char tmp[256] = {0,};

		for (size_t i=0; i<keys.size(); i++)
		{
			VEC3D pos;
			WarpHelpers::WarpStorageInst().GetLocation(keys[i], pos);

			text += "["; text += keys[i].locationName; text += "]  ";

			sprintf_s(tmp, "%.1f   %.1f   %.1f \n", pos.x, pos.y, pos.z);

			text += tmp;

			memset(tmp, 0, 256);
		}

		i3::wstring wtext(text.begin(), text.end());
		CHud::i()->ShowCheatkeyWarpList(wtext);

		return true;
	}

	ICommand* Warp_ShowLocations::Creator()
	{
		return new Warp_ShowLocations(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// ���� ���� ���� ������ ��ġ �׸� �����ֱ�
	// ex) "/wphide"
	//--------------------------------------------------
	bool Warp_HideLocations::_Execute()
	{
		CHud::i()->HideCheatkeyWarpList();

		return true;
	}

	ICommand* Warp_HideLocations::Creator()
	{
		return new Warp_HideLocations(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}
}