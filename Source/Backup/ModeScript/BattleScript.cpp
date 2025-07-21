#include "pch.h"
/*
#include <assert.h>

#include "BattleScript.h"
//------------------------------------------------------------------------------------------
const luaL_Reg CBattleScript::ms_aLuaRegBattle[] = 
{
	LUAREG_PAIR(TEST),
	LUAREG_PAIR(SetModeData),

	LUAREG_END()
};
//------------------------------------------------------------------------------------------
CBattleScript::CBattleScript(i3Stage* pStage) : 
IModeScript(pStage)
{
}
//------------------------------------------------------------------------------------------
CBattleScript::~CBattleScript()
{
	
}
//------------------------------------------------------------------------------------------
void CBattleScript::OnOpen()
{

	// �Լ� ���
	RegFunc(ms_aLuaRegBattle);
	
}


void CBattleScript::Apply(UINT8 ui8ModeType)
{
	switch (ui8ModeType)
	{
	case STAGE_TYPE_DEATHMATCH:
		break;
	case STAGE_TYPE_BOMB:
		break;
	case STAGE_TYPE_DESTROY:
		DestroyModeData();
		break;
	case STAGE_TYPE_ANNIHILATION:
		break;
	case STAGE_TYPE_DEFENCE:
		DefenseModeData();
		break;
	case STAGE_TYPE_SUDDEN_DEATH:
		break;
	case STAGE_TYPE_ESCAPE:
		break;
	case STAGE_TYPE_HEAD_HUNTER:
		break;
	case STAGE_TYPE_HEAD_KILLER:
		break;
	case STAGE_TYPE_TUTORIAL:
		break;
	case STAGE_TYPE_DOMINATION:
		DominationModeData();
		break;
	}
}
//------------------------------------------------------------------------------------------
void CBattleScript::DestroyModeData()
{
	// Lua ���Ͽ� ���ǵǾ� ����. - SOW_������
	//const int DATA_IDX_RED_OBJECT				= 0;
	//const int DATA_IDX_BLUE_OBJECT				= 1;
	const int DATA_IDX_RED_OBJECT_HP			= 2;
	const int DATA_IDX_BLUE_OBJECT_HP			= 3;
	//const int DATA_IDX_ACC_USER_DAMAGE_START	= 4;
	//const int DATA_IDX_ACC_USER_DAMAGE_END		= 19;
	const int DATA_IDX_USER_DAMAGE_START		= 20;
	const int DATA_IDX_USER_DAMAGE_END			= 35;
	//const int DATA_IDX_DESTROY_COUNT_START		= 36;
	//const int DATA_IDX_DESTROY_COUNT_END		= 51;

	if ( m_bSyncData[DATA_IDX_RED_OBJECT_HP] && m_bSyncData[DATA_IDX_BLUE_OBJECT_HP] )
	{
		DESTRUCTION_INFO kInfo;
		kInfo._nRedObjectHP		= (UINT16)m_i32RoomModeData[DATA_IDX_RED_OBJECT_HP];
		kInfo._nBlueObjectHP	= (UINT16)m_i32RoomModeData[DATA_IDX_BLUE_OBJECT_HP];

		for (int i = DATA_IDX_USER_DAMAGE_START ; i <= DATA_IDX_USER_DAMAGE_END ; ++i)
		{
			kInfo._nUserDamage[i-DATA_IDX_USER_DAMAGE_START] = (UINT16)m_i32RoomModeData[i];
		}

		g_pGameContext->getMissionManager()->setDestructionInfo( &kInfo );

		m_bSyncData[DATA_IDX_RED_OBJECT_HP] = false;
		m_bSyncData[DATA_IDX_BLUE_OBJECT_HP] = false;
	}
}
//------------------------------------------------------------------------------------------
void CBattleScript::DefenseModeData()
{
	// Lua ���Ͽ� ���ǵǾ� ����. - SOW_������
	//const int DATA_IDX_TARGET_0_OBJECT			= 0;
	//const int DATA_IDX_TARGET_1_OBJECT			= 1;
	const int DATA_IDX_TARGET_0_OBJECT_HP		= 2;
	const int DATA_IDX_TARGET_1_OBJECT_HP		= 3;
	//const int DATA_IDX_ACC_USER_DAMAGE_START	= 4;
	//const int DATA_IDX_ACC_USER_DAMAGE_END		= 19;
	const int DATA_IDX_USER_DAMAGE_START		= 20;
	const int DATA_IDX_USER_DAMAGE_END			= 35;
	//const int DATA_IDX_DEFENSE_COUNT_START		= 36;
	//const int DATA_IDX_DEFENSE_COUNT_END		= 51;
	//const int DATA_IDX_REMAIN_HP				= 52;

	if ( m_bSyncData[DATA_IDX_TARGET_0_OBJECT_HP] && m_bSyncData[DATA_IDX_TARGET_1_OBJECT_HP] )
	{
		DEFENCE_INFO kInfo;
		// [ ��� �̼� ������Ʈ ] �ڸ�Ʈ ���� ���� 
		kInfo._nObjectHP[MISSION_FIRST_DEFENCE_OBJECT]	= (UINT16)m_i32RoomModeData[DATA_IDX_TARGET_1_OBJECT_HP];
		kInfo._nObjectHP[MISSION_SECOND_DEFENCE_OBJECT]	= (UINT16)m_i32RoomModeData[DATA_IDX_TARGET_0_OBJECT_HP];

		for (int i = DATA_IDX_USER_DAMAGE_START ; i <= DATA_IDX_USER_DAMAGE_END ; ++i)
		{
			// [ ��� �̼� ������Ʈ ] �ڸ�Ʈ ���� ����
			kInfo._nUserDamage[MISSION_FIRST_DEFENCE_OBJECT][i-DATA_IDX_USER_DAMAGE_START]	= (UINT16)m_i32RoomModeData[i];
			kInfo._nUserDamage[MISSION_SECOND_DEFENCE_OBJECT][i-DATA_IDX_USER_DAMAGE_START]	= 0;
			m_bSyncData[i] = false;
		}

		g_pGameContext->getMissionManager()->setDefenceInfo( &kInfo);

		m_bSyncData[DATA_IDX_TARGET_0_OBJECT_HP] = false;
		m_bSyncData[DATA_IDX_TARGET_1_OBJECT_HP] = false;
	}
}

//------------------------------------------------------------------------------------------
void CBattleScript::DominationModeData(void)
{
	// Lua ���Ͽ� ���ǵǾ� ����. - SOW_������
	static const int	DATA_IDX_WAVE_GOAL				= 0;	// ���̺� ��ǥ
	static const int	DATA_IDX_DAY_NUMBER				= 1;	// �������� DAY
	static const int	DATA_IDX_WAVE_NUMBER			= 2;	// �������� WAVE
	static const int	DATA_IDX_USER_SCORE_START		= 3;	// �� ������ ����
	static const int	DATA_IDX_USER_SCORE_END			= 18;	// �� ������ ����
	static const int	DATA_IDX_USER_SP_START			= 19;	// �� ������ ���� SP
	static const int	DATA_IDX_USER_SP_END			= 34;	// �� ������ ���� SP
	static const int	DATA_IDX_RESPAWN_REMAIN_START	= 35;	// �� ������ ���� ���
	static const int	DATA_IDX_RESPAWN_REMAIN_END		= 50;	// �� ������ ���� ���
	static const int	DATA_IDX_REMAIN_DINO_COUNT		= 51;
	static const int	DATA_IDX_USER_SP_IS_ABS			= 52;	// �������� SP ���� ���밪���� üũ�÷��� (���밪�϶� true)


	// Day
	if( m_bSyncData[ DATA_IDX_DAY_NUMBER] )
	{
		g_pGameContext->SetDay( m_i32RoomModeData[ DATA_IDX_DAY_NUMBER] );
		m_bSyncData[ DATA_IDX_DAY_NUMBER]	= false;
	}
	// Wave
	if( m_bSyncData[ DATA_IDX_WAVE_NUMBER] )
	{
		g_pGameContext->SetWave( m_i32RoomModeData[ DATA_IDX_WAVE_NUMBER] );
		m_bSyncData[ DATA_IDX_WAVE_NUMBER]	= false;
	}	
	// Flow Type
	if( m_bSyncData[ DATA_IDX_WAVE_GOAL] )
	{
		g_pGameContext->SetFlow( m_i32RoomModeData[ DATA_IDX_WAVE_GOAL] );
		m_bSyncData[ DATA_IDX_WAVE_GOAL]	= false;
	}

	// SP ����
	if ( m_bSyncData[ DATA_IDX_USER_SP_START] && m_bSyncData[ DATA_IDX_USER_SP_END] )
	{
		INT32		nIndex	= 0;

		for( INT32 i = DATA_IDX_USER_SP_START; i <= DATA_IDX_USER_SP_END; i++)
		{
			g_pGameContext->SetSPCur( nIndex, (REAL32)m_i32RoomModeData[ i] );
			m_bSyncData[ i]	= false;

			nIndex++;
		}
		
		if( m_bSyncData[ DATA_IDX_USER_SP_IS_ABS])
		{
			g_pGameContext->PushEventGame( EVENT_SP_CHECK, m_i32RoomModeData[ DATA_IDX_USER_SP_IS_ABS]);
		}
		else
		{
			g_pGameContext->PushEventGame( EVENT_SP_CHECK, FALSE);
		}
	}
}

*/

//------------------------------------------------------------------------------------------
// Glue Function
//------------------------------------------------------------------------------------------
