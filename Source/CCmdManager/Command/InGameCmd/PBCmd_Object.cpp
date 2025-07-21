#include "pch.h"
#include "PBCmd_Object.h"
#include "../TBoundChk.h"
#include "../../../Cheatkey_LimitPred.h"
#include "../../../GameMode.h"
#include "../../../ObjExplosion.h"

#include "MyRoomInfoContext.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Cheatkey
{
	//--------------------------------------------------
	// ������Ʈ ������
	// ex) "/spawnobj"
	//--------------------------------------------------
	bool RespawnObj::_Execute()
	{
		// ���� ���� �ʿ�.
		g_pEffectManager->Reset();
		g_pTracerBulletManager->ResetAll();
		gmode::i()->ResetObjectStatus();
		obj_explosion::i()->Process_ResetObjectExploded(g_pFramework->GetStageBattle());

		return true;
	}

	ICommand* RespawnObj::Creator()
	{
		return new RespawnObj(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// ������Ʈ�� ������ ����
	// ex) "/obdmg �ε���, ������ ��"
	//  * �ε��� : 0(������ ������Ʈ), 1(����� ������Ʈ), 2(��� �� ���� ������Ʈ)
	//	* ������ �� ���� �� ����Ʈ 100
	//--------------------------------------------------
	bool SetObjDamage::_ParseParams(const i3::vector<i3::string>& params)
	{
		if( params.size() == 0)
			return false;

		m_idx = ::atoi(params[0].c_str());

		if (params.size() == 1)
			m_damage = 100;
		else
		{
			m_damage = ::atoi(params[1].c_str());
			if (m_damage <= 0) m_damage = 100;
		}

		if (!TBoundChk<INT32>(m_idx, TEAMRED_OBJ, OBJ_MAX_COUNT-1)) return false;
		if (!TBoundChk<INT32>(m_damage, 0, std::numeric_limits<INT32>::max())) return false;

		return true;
	}

	bool SetObjDamage::_Execute()
	{
		if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST)
		{
			if(m_idx == HELICOPTER)
			{
				GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_DAMAGE_TO_OBJECT, &m_idx, &m_damage);
				return true;
			}
		}
		
		if (m_idx == TEAMRED_OBJ || m_idx == TEAMBLUE_OBJ)
		{
			GameEventSender::i()->SetEvent(EVENT_QA_COMMAND_DAMAGE_TO_OBJECT, &m_idx, &m_damage);
			return true;
		}

		return false;
	}

	ICommand* SetObjDamage::Creator()
	{
		return new SetObjDamage(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}
}