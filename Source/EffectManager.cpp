#include "pch.h"
#include "EffectManager.h"
#include "GlobalVariables.h"
#include "ParserHelper.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

#define DECAL_DEPTH			-0.008f
#define DECAL_BLOOD_DEPTH	-0.005f

static void __CancelEffectAtObject( i3Node * pNode)
{
	if( g_pEffectManager != nullptr)
	{
		i3::vector<i3TimeStamp*>& pEffectList = g_pEffectManager->getObjectEffectList();
		for( size_t i = 0; i < pEffectList.size(); i++)
		{
			i3TimeStamp * pEffect = pEffectList[i];
			if( pEffect == pNode)
			{
				i3::vu::erase_index(pEffectList, i);
				break;
			}
		}
	}
}

I3_CLASS_INSTANCE( CEffectManager);

//#define DEBUG_EFFECT_MEM
#define ENABLED_RESTORE_SNDPLAYINFO //SoundNode�� ���Ե� SoundPlayInfo�� ���� Rsc���� �������ذ����� �ٲߴϴ�.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���� ��ź ����Ʈ
const char * s_szTerrainEffect[ I3_TERRAIN_TYPE_COUNT][ WEAPON_EFFECT_TYPE_COUNT] =
{
	//	Knife			Handgun				Assault Rfl.			SMG				SniperRifle.		Shotgun		����		���� �޼�		t-rex				Sting
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" , 	"Hit_Dino_Sting_Cm", 	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// 0,									// Common
	{	"Hit_M9_Co"	,	"Hit_Ar_Co",		"Hit_Ar_Co",		"Hit_Ar_Co",	"Hit_Ar_Co",	"Hit_Sg_Co", 	"Hit_M9_Co", 	"Hit_M9_Co", 	"Hit_M9_Co", 	"Hit_Dino_Sting_Co" ,	"Hit_Dino_Sting_Co",	"Hit_Dino_Sting_Co"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_CONCRETE = 1,				// ��ũ��Ʈ
	{	"Hit_M9_Me"	,	"Hit_Ar_Me",		"Hit_Ar_Me",		"Hit_Ar_Me",	"Hit_Ar_Me",	"Hit_Sg_Me", 	"Hit_M9_Me", 	"Hit_M9_Me", 	"Hit_M9_Me", 	"Hit_Dino_Sting_Me" , 	"Hit_Dino_Sting_Me", 	"Hit_Dino_Sting_Me"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_STEAL,					// ö��
	{	"Hit_M9_So"	,	"Hit_Ar_So",		"Hit_Ar_So",		"Hit_Ar_So",	"Hit_Ar_So",	"Hit_Sg_So", 	"Hit_M9_So", 	"Hit_M9_So", 	"Hit_M9_So", 	"Hit_Dino_Sting_So" ,	"Hit_Dino_Sting_So",	"Hit_Dino_Sting_So"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_GROUND,					// ��
	{	"Hit_M9_Wo"	,	"Hit_Ar_Wo",		"Hit_Ar_Wo",		"Hit_Ar_Wo",	"Hit_Ar_Wo",	"Hit_Sg_Wo", 	"Hit_M9_Wo", 	"Hit_M9_Wo", 	"Hit_M9_Wo", 	"Hit_Dino_Sting_Wo" ,	"Hit_Dino_Sting_Wo",	"Hit_Dino_Sting_Wo"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_WOOD,						// ����
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_SNOW,						// ��
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_WATER_DEEP,				// �� (����)
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_WATER_SHALLOW,			// �� (����)
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_WET_CLAY,					// ����
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_GRASS,					// �ܵ�
	{	"Hit_M9_Mb"	,	"Hit_Ar_Mb",		"Hit_Ar_Mb",		"Hit_Ar_Mb",	"Hit_Ar_Mb",	"Hit_Sg_Mb", 	"Hit_M9_Mb", 	"Hit_M9_Mb", 	"Hit_M9_Mb", 	"Hit_Dino_Sting_Mb" ,	"Hit_Dino_Sting_Mb",	"Hit_Dino_Sting_Mb"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_MARBLE,					// �븮��
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_FALLLEAF					
	{	"Hit_M9_Co"	,	"Hit_Ar_Co",		"Hit_Ar_Co",		"Hit_Ar_Co",	"Hit_Ar_Co",	"Hit_Sg_Co", 	"Hit_M9_Co", 	"Hit_M9_Co", 	"Hit_M9_Co", 	"Hit_Dino_Sting_Co" ,	"Hit_Dino_Sting_Co",	"Hit_Dino_Sting_Co"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_CONCRETE_THIN
	{	"Hit_M9_Me"	,	"Hit_Ar_Me",		"Hit_Ar_Me",		"Hit_Ar_Me",	"Hit_Ar_Me",	"Hit_Sg_Me", 	"Hit_M9_Me", 	"Hit_M9_Me", 	"Hit_M9_Me", 	"Hit_Dino_Sting_Me" ,	"Hit_Dino_Sting_Me",	"Hit_Dino_Sting_Me"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_STEEL_THIN
	{	"Hit_M9_Wo"	,	"Hit_Ar_Wo",		"Hit_Ar_Wo",		"Hit_Ar_Wo",	"Hit_Ar_Wo",	"Hit_Sg_Wo", 	"Hit_M9_Wo", 	"Hit_M9_Wo", 	"Hit_M9_Wo", 	"Hit_Dino_Sting_Wo" ,	"Hit_Dino_Sting_Wo",	"Hit_Dino_Sting_Wo"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_WOOD_THIN
	{	"Hit_M9_Mb"	,	"Hit_Ar_Mb",		"Hit_Ar_Mb",		"Hit_Ar_Mb",	"Hit_Ar_Mb",	"Hit_Sg_Mb", 	"Hit_M9_Mb", 	"Hit_M9_Mb", 	"Hit_M9_Mb", 	"Hit_Dino_Sting_Mb" ,	"Hit_Dino_Sting_Mb",	"Hit_Dino_Sting_Mb"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_MARBLE_THIN
	{	"Hit_M9_Pa"	,	"Hit_Ar_Pa",		"Hit_Ar_Pa",		"Hit_Ar_Pa",	"Hit_Ar_Pa",	"Hit_Sg_Pa", 	"Hit_M9_Pa", 	"Hit_M9_Pa", 	"Hit_M9_Pa", 	"Hit_Dino_Sting_Pa" ,	"Hit_Dino_Sting_Pa",	"Hit_Dino_Sting_Pa"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_PAPER
	{	"Hit_M9_Gl"	,	"Hit_Ar_Gl",		"Hit_Ar_Gl",		"Hit_Ar_Gl",	"Hit_Ar_Gl",	"Hit_Sg_Gl", 	"Hit_M9_Gl", 	"Hit_M9_Gl", 	"Hit_M9_Gl", 	"Hit_Dino_Sting_Gl" ,	"Hit_Dino_Sting_Gl",	"Hit_Dino_Sting_Gl"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_GLASS
	{	"Hit_M9_Pl"	,	"Hit_Ar_Pl",		"Hit_Ar_Pl",		"Hit_Ar_Pl",	"Hit_Ar_Pl",	"Hit_Sg_Pl", 	"Hit_M9_Pl", 	"Hit_M9_Pl", 	"Hit_M9_Pl", 	"Hit_Dino_Sting_Pl" ,	"Hit_Dino_Sting_Pl",	"Hit_Dino_Sting_Pl"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_PLASTIC
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_RUBBER
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_CLOT
	{	"Hit_M9_So"	,	"Hit_Ar_So",		"Hit_Ar_So",		"Hit_Ar_So",	"Hit_Ar_So",	"Hit_Sg_So", 	"Hit_M9_So", 	"Hit_M9_So", 	"Hit_M9_So", 	"Hit_Dino_Sting_So" ,	"Hit_Dino_Sting_So",	"Hit_Dino_Sting_So"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_GROUND_THIN
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			//I3_TERRAIN_SNOW_THIN
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			//I3_TERRAIN_WET_CLAY_THIN
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			//	I3_TERRAIN_GRASS_THIN,
	{	"Hit_M9_Pa"	,	"Hit_Ar_Pa",		"Hit_Ar_Pa",		"Hit_Ar_Pa",	"Hit_Ar_Pa",	"Hit_Sg_Pa", 	"Hit_M9_Pa", 	"Hit_M9_Pa", 	"Hit_M9_Pa", 	"Hit_Dino_Sting_Pa" ,	"Hit_Dino_Sting_Pa",	"Hit_Dino_Sting_Pa"	,	"Hmk_Sg_Water"},			//	I3_TERRAIN_PAPER_THIN,
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			//I3_TERRAIN_FALLLEAF_THIN,
	{	"Hit_M9_Gl"	,	"Hit_Ar_Gl",		"Hit_Ar_Gl",		"Hit_Ar_Gl",	"Hit_Ar_Gl",	"Hit_Sg_Gl", 	"Hit_M9_Gl", 	"Hit_M9_Gl", 	"Hit_M9_Gl", 	"Hit_Dino_Sting_Gl" ,	"Hit_Dino_Sting_Gl",	"Hit_Dino_Sting_Gl"	,	"Hmk_Sg_Water"},			//I3_TERRAIN_GLASS_THIN,
	{	"Hit_M9_Pl"	,	"Hit_Ar_Pl",		"Hit_Ar_Pl",		"Hit_Ar_Pl",	"Hit_Ar_Pl",	"Hit_Sg_Pl", 	"Hit_M9_Pl", 	"Hit_M9_Pl", 	"Hit_M9_Pl", 	"Hit_Dino_Sting_Pl" ,	"Hit_Dino_Sting_Pl",	"Hit_Dino_Sting_Pl"	,	"Hmk_Sg_Water"},			//I3_TERRAIN_PLASTIC_THIN,
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			//I3_TERRAIN_RUBBER_THIN,
	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_Dino_Sting_Cm" ,	"Hit_Dino_Sting_Cm",	"Hit_Dino_Sting_Cm"	,	"Hmk_Sg_Water"},			//I3_TERRAIN_CLOTH_THIN,
	{	"Hit_M9_Me"	,	"Hit_Ar_Me",		"Hit_Ar_Me",		"Hit_Ar_Me",	"Hit_Ar_Me",	"Hit_Sg_Me", 	"Hit_M9_Me", 	"Hit_M9_Me", 	"Hit_M9_Me", 	"Hit_Dino_Sting_Me" ,	"Hit_Dino_Sting_Me",	"Hit_Dino_Sting_Me"	,	"Hmk_Sg_Water"},			// I3_TERRAIN_BARBEDWIRE
					
#if defined (THAILAND_SONGKRAN_EVENT)		//	�±� ������ �Ǹ� �ϴû�����
	{	"ExpHu01_1_Blue"	,	"ExpHu01_1_Blue",		"ExpHu01_1_Blue",		"ExpHu01_1_Blue",	"ExpHu01_1_Blue",	"ExpHu01_1_Blue", 	"ExpHu01_1_Blue", 	"ExpHu01_1_Blue", 	"ExpHu01_1_Blue", "ExpHu01_1_Blue","ExpHu01_1_Blue", "ExpHu01_1_Blue"},			// I3_TERRAIN_BLOOD
//	�߱� ������ ����� ��� ������ ���´�.

#if !defined(_CHINA_GREEN_BLOOD_)
	{	"ExpHu01_1_HitEffect"	,	"ExpHu01_1_HitEffect",		"ExpHu01_1_HitEffect",		"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect", 	"ExpHu01_1_HitEffect", 	"ExpHu01_1_HitEffect", 	"ExpHu01_1_HitEffect", "ExpHu01_1_HitEffect"},			// I3_TERRAIN_BLOOD
#else
	{	"ExpHu01_1"	,	"ExpHu01_1",		"ExpHu01_1",		"ExpHu01_1",	"ExpHu01_1",	"ExpHu01_1", 	"ExpHu01_1", 	"ExpHu01_1", 	"ExpHu01_1", 	"ExpHu01_1",	"ExpHu01_1"},			// I3_TERRAIN_BLOOD
#endif

#else
	{	"ExpHu01_1"	,	"ExpHu01_1",		"ExpHu01_1",		"ExpHu01_1",	"ExpHu01_1",	"ExpHu01_1", 	"ExpHu01_1", 	"ExpHu01_1", 	"ExpHu01_1", 	"ExpHu01_1", 	"ExpHu01_1", 	"ExpHu01_1",	"Hmk_Sg_Water"},			// I3_TERRAIN_BLOOD
#endif

	{	"Hit_M9_Me",	"Hit_Ar_L2_01",		"Hit_Ar_L2_01",		"Hit_Ar_L2_01",	"Hit_Ar_L2_01",	"Hit_Ar_L2_01", "Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me"},			// I3_TERRAIN_TEMP1
	{	"Hit_M9_Me",	"Hit_Ar_L2_02",		"Hit_Ar_L2_02",		"Hit_Ar_L2_02",	"Hit_Ar_L2_02",	"Hit_Ar_L2_02", "Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me",	"Hit_M9_Me"},			// I3_TERRAIN_TEMP2

	{	"Hit_M9_Cm"	,	"Hit_Ar_Cm",		"Hit_Ar_Cm",		"Hit_Ar_Cm",	"Hit_Ar_Cm",	"Hit_Sg_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm", 	"Hit_M9_Cm",	"Hmk_Sg_Water"},			// I3_TERRAIN_CLOTH_NO_DECAL

	{	"Hit_M9_Me"	,	"Hit_Ar_Me",		"Hit_Ar_Me",		"Hit_Ar_Me",	"Hit_Ar_Me",	"Hit_Sg_Me", 	"Hit_M9_Me", 	"Hit_M9_Me", 	"Hit_M9_Me", 	"Hit_Dino_Sting_Me" , 	"Hit_Dino_Sting_Me", 	"Hit_Dino_Sting_Me",	"Hmk_Sg_Water"},			// I3_TERRAIN_STEEL_WIRE,			
	{	"Hit_M9_Wo"	,	"Hit_Ar_Wo",		"Hit_Ar_Wo",		"Hit_Ar_Wo",	"Hit_Ar_Wo",	"Hit_Sg_Wo", 	"Hit_M9_Wo", 	"Hit_M9_Wo", 	"Hit_M9_Wo", 	"Hit_Dino_Sting_Wo" ,	"Hit_Dino_Sting_Wo",	"Hit_Dino_Sting_Wo",	"Hmk_Sg_Water"},			// I3_TERRAIN_WOOD_WIRE,			
	{	"Hit_M9_Pl"	,	"Hit_Ar_Pl",		"Hit_Ar_Pl",		"Hit_Ar_Pl",	"Hit_Ar_Pl",	"Hit_Sg_Pl", 	"Hit_M9_Pl", 	"Hit_M9_Pl", 	"Hit_M9_Pl", 	"Hit_Dino_Sting_Pl" ,	"Hit_Dino_Sting_Pl",	"Hit_Dino_Sting_Pl",	"Hmk_Sg_Water"},			// I3_TERRAIN_PLASTIC_WIRE,	
	{ ""	,	"",		"",		"",	"",	"", 	"", 	"", 	"", 	"" ,	"",	""	,	"" },			// I3_TERRAIN_NONE
};																											  
																											  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ź�� ����Ʈ
const char * s_szBulletHoleEffect[ I3_TERRAIN_TYPE_COUNT][ WEAPON_EFFECT_TYPE_COUNT] =						  
{
	//	Knife			Handgun				Assault Rfl.			SMG			SniperRifle.		Shotgun			����						���� �޼�				Ƽ����						����
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// 0,									// Common
	{	"Hmk_M9_Co",	"Hmk_Ar_Co",		"Hmk_Ar_Co",		"Hmk_Ar_Co",	"Hmk_Ar_Co",	"Hmk_Sg_Co",		"Hmk_Velociraptor_Co",		"Hmk_Velociraptor_Co_1",	"Hmk_T_Rex_Bite_Co",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_CONCRETE = 1,				// ��ũ��Ʈ
	{	"Hmk_M9_Me",	"Hmk_Ar_Me",		"Hmk_Ar_Me",		"Hmk_Ar_Me",	"Hmk_Ar_Me",	"Hmk_Sg_Me",		"Hmk_Velociraptor_Me",		"Hmk_Velociraptor_Me_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_STEAL,					// ö��
	{	"Hmk_M9_So",	"Hmk_Ar_So",		"Hmk_Ar_So",		"Hmk_Ar_So",	"Hmk_Ar_So",	"Hmk_Sg_So",		"Hmk_Velociraptor_So",		"Hmk_Velociraptor_So_1",	"Hmk_T_Rex_Bite_So",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_GROUND,					// ��
	{	"Hmk_M9_Wo",	"Hmk_Ar_Wo",		"Hmk_Ar_Wo",		"Hmk_Ar_Wo",	"Hmk_Ar_Wo",	"Hmk_Sg_Wo",		"Hmk_Velociraptor_Wo",		"Hmk_Velociraptor_Wo_1",	"Hmk_T_Rex_Bite_Wo",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Wo",	"Hmk_Elite_Wo_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_WOOD,						// ����
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm"	,	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_SNOW,						// ��
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm" ,	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_WATER_DEEP,				// �� (����)
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm"	,	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_WATER_SHALLOW,			// �� (����)
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_WET_CLAY,					// ����
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_GRASS,					// �ܵ�
	{	"Hmk_M9_Cm",	"Hmk_Ar_Mb",		"Hmk_Ar_Mb",		"Hmk_Ar_Mb",	"Hmk_Ar_Mb",	"Hmk_Sg_Mb",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm"	,	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_MARBLE,					// �븮��
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_FALLLEAF
	{	"Hmk_M9_Co",	"Hmk_Ar_Co",		"Hmk_Ar_Co",		"Hmk_Ar_Co",	"Hmk_Ar_Co",	"Hmk_Sg_Co",		"Hmk_Velociraptor_Co",		"Hmk_Velociraptor_Co_1",	"Hmk_T_Rex_Bite_Co",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_CONCRETE_THIN
	{	"Hmk_M9_Me",	"Hmk_Ar_Me",		"Hmk_Ar_Me",		"Hmk_Ar_Me",	"Hmk_Ar_Me",	"Hmk_Sg_Me",		"Hmk_Velociraptor_Me",		"Hmk_Velociraptor_Me_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_STEEL_THIN,
	{	"Hmk_M9_Wo",	"Hmk_Ar_Wo",		"Hmk_Ar_Wo",		"Hmk_Ar_Wo",	"Hmk_Ar_Wo",	"Hmk_Sg_Wo",		"Hmk_Velociraptor_Wo",		"Hmk_Velociraptor_Wo_1",	"Hmk_T_Rex_Bite_Wo",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Wo",	"Hmk_Elite_Wo_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_WOOD_THIN,	
	{	"Hmk_M9_Mb",	"Hmk_Ar_Mb",		"Hmk_Ar_Mb",		"Hmk_Ar_Mb",	"Hmk_Ar_Mb",	"Hmk_Sg_Mb",		"Hmk_Velociraptor_Mb",		"Hmk_Velociraptor_Mb_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Mb",	"Hmk_Elite_Mb_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_MARBLE_THIN, 
	{	"Hmk_M9_Pa",	"Hmk_Ar_Pa",		"Hmk_Ar_Pa",		"Hmk_Ar_Pa",	"Hmk_Ar_Pa",	"Hmk_Sg_Pa",		"Hmk_Velociraptor_Pa",		"Hmk_Velociraptor_Pa_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_PAPER
	{	"Hmk_M9_Gl",	"Hmk_Ar_Gl",		"Hmk_Ar_Gl",		"Hmk_Ar_Gl",	"Hmk_Ar_Gl",	"Hmk_Sg_Gl",		"Hmk_Velociraptor_Gl",		"Hmk_Velociraptor_Gl_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Gl",	"Hmk_Elite_Gl_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_GLASS
	{	"Hmk_M9_Pl",	"Hmk_Ar_Pl",		"Hmk_Ar_Pl",		"Hmk_Ar_Pl",	"Hmk_Ar_Pl",	"Hmk_Sg_Pl",		"Hmk_Velociraptor_Pl",		"Hmk_Velociraptor_Pl_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_PLASTIC
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_RUBBER
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_CLOTH
	{	"Hmk_M9_So",	"Hmk_Ar_So",		"Hmk_Ar_So",		"Hmk_Ar_So",	"Hmk_Ar_So",	"Hmk_Sg_So",		"Hmk_Velociraptor_So",		"Hmk_Velociraptor_So_1",	"Hmk_T_Rex_Bite_So",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_GROUND_THIN
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_SNOW_THIN
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_WET_CLAY_THIN
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_GRASS_THIN,
	{	"Hmk_M9_Pa",	"Hmk_Ar_Pa",		"Hmk_Ar_Pa",		"Hmk_Ar_Pa",	"Hmk_Ar_Pa",	"Hmk_Sg_Pa",		"Hmk_Velociraptor_Pa",		"Hmk_Velociraptor_Pa_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_PAPER_THIN,
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_FALLLEAF_THIN,
	{	"Hmk_M9_Gl",	"Hmk_Ar_Gl",		"Hmk_Ar_Gl",		"Hmk_Ar_Gl",	"Hmk_Ar_Gl",	"Hmk_Sg_Gl",		"Hmk_Velociraptor_Gl",		"Hmk_Velociraptor_Gl_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Gl",	"Hmk_Elite_Gl_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_GLASS_THIN,
	{	"Hmk_M9_Pl",	"Hmk_Ar_Pl",		"Hmk_Ar_Pl",		"Hmk_Ar_Pl",	"Hmk_Ar_Pl",	"Hmk_Sg_Pl",		"Hmk_Velociraptor_Pl",		"Hmk_Velociraptor_Pl_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_PLASTIC_THIN,
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_RUBBER_THIN,
	{	"Hmk_M9_Cm",	"Hmk_Ar_Cm",		"Hmk_Ar_Cm",		"Hmk_Ar_Cm",	"Hmk_Ar_Cm",	"Hmk_Sg_Cm",		"Hmk_Velociraptor_Cm",		"Hmk_Velociraptor_Cm_1",	"Hmk_T_Rex_Bite_Cm",	"Hmk_Dino_Sting_Attack_A",	"Hmk_Elite_Me",	"Hmk_Elite_Me_1",		"Hmk_Sg_Water",	},			// I3_TERRAIN_CLOTH_THIN,
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},		// I3_TERRAIN_BARBEDWIRE,
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},								// I3_TERRAIN_BLOOD,
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},								// I3_TERRAIN_TEMP1,
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},								// I3_TERRAIN_TEMP2,
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},								// I3_TERRAIN_CLOTH_NO_DECAL,
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},		// I3_TERRAIN_STEEL_WIRE,				// ź��(x)	
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},		// I3_TERRAIN_WOOD_WIRE,				// ź��(x)
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},		// I3_TERRAIN_PLASTIC_WIRE,				// ź��(x)
	{	"",				"",					"",					"",				"",				"",					"",							"",							"",						"",							"",				"",						"",				},		// I3_TERRAIN_NONE,				// ź��(x)
};

// ��, �ٴ� ���� ����Ʈ		
const char * s_szBloodEffect[BLOOD_TERRAIN_TYPE_COUNT][ BLOOD_EFFECT_TYPE_COUNT ] = 
{
	{ "Bmk_Ar_Wa",		"Bmk_Smg_Wa",	"Bmk_Hg_Wa",	"Bmk_Sg_Wa",	"",				"Bmk_Gr_Wa",	"Bmk_Sr_Wa",		"Bmk_Ar_Wa_Water"		},
	{ "Bmk_Ar_Fl",		"Bmk_Smg_Fl",	"Bmk_Hg_Fl",	"Bmk_Sg_Fl",	"Bmk_M9_Fl",	"Bmk_Gr_Fl",	"Bmk_Sr_Fl",		"Bmk_Ar_Fl_Water"		},
};

const char * s_szBloodEffect_Young[BLOOD_TERRAIN_TYPE_COUNT][ BLOOD_EFFECT_TYPE_COUNT ] = 
{
	{ "Bmk_Ar_Wa_Young",		"Bmk_Smg_Wa_Young",	"Bmk_Hg_Wa_Young",	"Bmk_Sg_Wa_Young",	"",				"Bmk_Gr_Wa_Young",	"Bmk_Sr_Wa_Young"		,		"Bmk_Ar_Wa_Water"},
	{ "Bmk_Ar_Fl_Young",		"Bmk_Smg_Fl_Young",	"Bmk_Hg_Fl_Young",	"Bmk_Sg_Fl_Young",	"Bmk_M9_Fl_Young",	"Bmk_Gr_Fl_Young",	"Bmk_Sr_Fl_Young"	,		"Bmk_Ar_Fl_Water"},
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ȯ�� ����Ʈ
const char * s_szEnvironmentEffect[ ENV_EFFECT_TYPE_COUNT] =
{
	"Chara_Down_Smoke_01"		,
	"Chara_Walk_Smoke_01"		,
	"Weapon_Down_Smoke_01"		,
	"Helmet_Down_Smoke_01"		,
	"Bomb_Light"				,
	"Dust_Uh60"					,
	"Fir_RangeGolem_01"			,
	"Hit_SiegeGolem_01"			,
	"Hit_RangeGolem_Cm_01"		,
	"Smk_RPG7_01"				,
	"Trex_Walk_Smoke_01"		,
	"Exp_C5_01"					,
	"buff"						,
	"GlowLight"					,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	��� �ǰ� ����Ʈ

const char * g_szCharaHitEffectWater[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam			Knife					Handgun					Assult Rfl.				SMG						SniperRifle.				Shotgun				Grenade			
	"ExpHu01_1_Water",			"ExpHu01_1_Water",		"ExpHu01_1_Water",		"ExpHu01_1_Water",		"ExpHu01_1_Water",		"ExpHu01_1_Water",			"ExpHu01_1_Water",  "ExpHu01_1_Water",	"ExpHu01_1_Water",
	"ExpHu01_1_Water",			"ExpHu02_1_Water",		"ExpHu02_1_Water",		"ExpHu02_1_Water",		"ExpHu02_1_Water",		"ExpHu02_1_Water",			"ExpHu02_1_Water",  "ExpHu02_1_Water",	"ExpHu02_1_Water",
};


#if defined (THAILAND_SONGKRAN_EVENT)		//	�±� ������ �Ǹ� �ϴû�����

const char * g_szCharaHitEffect[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam			Knife			Handgun				Assult Rfl.			SMG				SniperRifle.		Shotgun			Grenade			
	"ExpHu01_0_Blue",			"ExpHu01_2_Blue",		"ExpHu01_1_Blue",		"ExpHu01_1_Blue",		"ExpHu01_1_Blue",	"ExpHu01_1_Blue",		"ExpHu01_1_Blue",  "ExpHu01_1_Blue",		
	"ExpHu01_0_Blue",			"ExpHu01_2_Blue",		"ExpHu02_1_Blue",		"ExpHu02_1_Blue",		"ExpHu02_1_Blue",	"ExpHu02_1_Blue",		"ExpHu02_1_Blue",  "ExpHu02_1_Blue",		
};

const char * g_szCharaHitEffect_Young[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam	Knife					Handgun					Assult Rfl.				SMG					SniperRifle.			Shotgun		Grenade		
	"ExpHu01_0_Blue",	"ExpHu01_2_Blue",		"ExpHu01_1_Blue",		"ExpHu01_1_Blue",		"ExpHu01_1_Blue",	"ExpHu01_1_Blue",		"ExpHu01_1_Blue",  "ExpHu01_1_Blue",	
	"ExpHu01_0_Blue",	"ExpHu01_2_Blue",		"ExpHu02_1_Blue",		"ExpHu02_1_Blue",		"ExpHu02_1_Blue",	"ExpHu02_1_Blue",		"ExpHu02_1_Blue",  "ExpHu02_1_Blue",	
};

const char * g_szDinoHitEffect[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam		Knife				Handgun				Assult Rfl.			SMG				SniperRifle.		Shotgun			Grenade			
	"ExpHu01_0_Blue",			"ExpHu01_2_Blue",		"ExpDi01_1",		"ExpDi01_1",		"ExpDi01_1",	"ExpDi01_1",		"ExpDi01_1",  "ExpDi01_1",		
	"ExpHu01_0_Blue",			"ExpHu01_2_Blue",		"ExpDi02_1",		"ExpDi02_1",		"ExpDi02_1",	"ExpDi02_1",		"ExpDi02_1",  "ExpDi02_1",		
};

const char * g_szDinoHitEffect_Young[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam	Knife					Handgun					Assult Rfl.				SMG					SniperRifle.			Shotgun		Grenade		
	"ExpHu01_0_Blue",	"ExpHu01_2_Blue",		"ExpDi01_1_Young",		"ExpDi01_1_Young",		"ExpDi01_1_Young",	"ExpDi01_1_Young",		"ExpDi01_1_Young",  "ExpDi01_1_Young",	
	"ExpHu01_0_Blue",	"ExpHu01_2_Blue",		"ExpDi02_1_Young",		"ExpDi02_1_Young",		"ExpDi02_1_Young",	"ExpDi02_1_Young",		"ExpDi02_1_Young",  "ExpDi02_1_Young",	
};

#elif defined(LOCALE_VIETNAM)

const char * g_szCharaHitEffect[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam			Knife					Handgun					Assult Rfl.				SMG						SniperRifle.				Shotgun				Grenade			
	"ExpHu01_1_HitEffect",		"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",  "ExpHu01_1_HitEffect",	"HMK_Dino_Elite_Attack_B",
	"ExpHu01_1_HitEffect",		"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",  "ExpHu02_1_HitEffect",	"HMK_Dino_Elite_Attack_B",
};

const char * g_szCharaHitEffect_Young[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam			Knife					Handgun					Assult Rfl.				SMG						SniperRifle.				Shotgun				Grenade			
	"ExpHu01_0_Young",			"ExpHu01_2_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",		"HMK_Dino_Elite_Attack_B",
	"ExpHu01_0_Young",			"ExpHu01_2_Young",		"ExpHu02_1_Young",		"ExpHu02_1_Young",		"ExpHu02_1_Young",		"ExpHu02_1_Young",		"ExpHu02_1_Young",		"ExpHu02_1_Young",		"HMK_Dino_Elite_Attack_B",
};

const char * g_szDinoHitEffect[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam			Knife					Handgun					Assult Rfl.				SMG						SniperRifle.				Shotgun				Grenade			
	"ExpHu01_1_HitEffect",		"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",  "ExpHu01_1_HitEffect",	"HMK_Dino_Elite_Attack_B",
	"ExpHu01_1_HitEffect",		"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",  "ExpHu02_1_HitEffect",	"HMK_Dino_Elite_Attack_B",
};

const char * g_szDinoHitEffect_Young[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam			Knife					Handgun					Assult Rfl.				SMG						SniperRifle.				Shotgun				Grenade			
	"ExpHu01_1_HitEffect",		"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",	"ExpHu01_1_HitEffect",  "ExpHu01_1_HitEffect",	"HMK_Dino_Elite_Attack_B",
	"ExpHu01_1_HitEffect",		"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",	"ExpHu02_1_HitEffect",  "ExpHu02_1_HitEffect",	"HMK_Dino_Elite_Attack_B",
};

//	��Ű���� ���Ÿ������� ����Ʈ �ǰ� ���� ���̵���
#elif defined( LOCALE_TURKEY)

const char * g_szCharaHitEffect[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam			Knife			Handgun				Assult Rfl.			SMG				SniperRifle.		Shotgun			Grenade		Buff	
	"ExpHu01_0",			"ExpHu01_2",		"ExpHu01_1",		"ExpHu01_1",		"ExpHu01_1",	"ExpHu01_1",		"ExpHu01_1",  "ExpHu01_1",	"HMK_Dino_Elite_Attack_B",	
	"ExpHu01_0",			"ExpHu01_2",		"ExpHu01_1",		"ExpHu01_1",		"ExpHu01_1",	"ExpHu01_1",		"ExpHu01_1",  "ExpHu01_1",	"HMK_Dino_Elite_Attack_B",	
};

const char * g_szCharaHitEffect_Young[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam	Knife					Handgun					Assult Rfl.				SMG					SniperRifle.			Shotgun					Grenade			Buff
	"ExpHu01_0_Young",	"ExpHu01_2_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",	"ExpHu01_1_Young",		"ExpHu01_1_Young",  "ExpHu01_1_Young",	"HMK_Dino_Elite_Attack_B",
	"ExpHu01_0_Young",	"ExpHu01_2_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",	"ExpHu01_1_Young",		"ExpHu01_1_Young",  "ExpHu01_1_Young",	"HMK_Dino_Elite_Attack_B",
};

const char * g_szDinoHitEffect[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam		Knife				Handgun				Assult Rfl.			SMG				SniperRifle.		Shotgun			Grenade		Buff	
	"ExpHu01_0",			"ExpHu01_2",		"ExpDi01_1",		"ExpDi01_1",		"ExpDi01_1",	"ExpDi01_1",		"ExpDi01_1",  "ExpDi01_1",	"HMK_Dino_Elite_Attack_B",	
	"ExpHu01_0",			"ExpHu01_2",		"ExpDi01_1",		"ExpDi01_1",		"ExpDi01_1",	"ExpDi01_1",		"ExpDi01_1",  "ExpDi01_1",	"HMK_Dino_Elite_Attack_B",	
};

const char * g_szDinoHitEffect_Young[2][ CHARA_EFFECT_TYPE_COUNT] =		
{	// FIrst Person Cam	Knife					Handgun					Assult Rfl.				SMG					SniperRifle.			Shotgun				Grenade				Buff
	"ExpHu01_0_Young",	"ExpHu01_2_Young",		"ExpDi01_1_Young",		"ExpDi01_1_Young",		"ExpDi01_1_Young",	"ExpDi01_1_Young",		"ExpDi01_1_Young",  "ExpDi01_1_Young",	"HMK_Dino_Elite_Attack_B",
	"ExpHu01_0_Young",	"ExpHu01_2_Young",		"ExpDi01_1_Young",		"ExpDi01_1_Young",		"ExpDi01_1_Young",	"ExpDi01_1_Young",		"ExpDi01_1_Young",  "ExpDi01_1_Young",	"HMK_Dino_Elite_Attack_B",
};

#else

const char * g_szCharaHitEffect[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam			Knife			Handgun				Assult Rfl.			SMG				SniperRifle.		Shotgun			Grenade			Buff
	"ExpHu01_0",			"ExpHu01_2",		"ExpHu01_1",		"ExpHu01_1",		"ExpHu01_1",	"ExpHu01_1",		"ExpHu01_1",  "ExpHu01_1",		"HMK_Dino_Elite_Attack_B",
	"ExpHu01_0",			"ExpHu01_2",		"ExpHu02_1",		"ExpHu02_1",		"ExpHu02_1",	"ExpHu02_1",		"ExpHu02_1",  "ExpHu02_1",		"HMK_Dino_Elite_Attack_B",
};

const char * g_szCharaHitEffect_Young[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam	Knife					Handgun					Assult Rfl.				SMG					SniperRifle.			Shotgun		Grenade						Buff
	"ExpHu01_0_Young",	"ExpHu01_2_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",		"ExpHu01_1_Young",	"ExpHu01_1_Young",		"ExpHu01_1_Young",  "ExpHu01_1_Young",	"HMK_Dino_Elite_Attack_B",
	"ExpHu01_0_Young",	"ExpHu01_2_Young",		"ExpHu02_1_Young",		"ExpHu02_1_Young",		"ExpHu02_1_Young",	"ExpHu02_1_Young",		"ExpHu02_1_Young",  "ExpHu02_1_Young",	"HMK_Dino_Elite_Attack_B",
};

const char * g_szDinoHitEffect[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam		Knife				Handgun				Assult Rfl.			SMG				SniperRifle.		Shotgun			Grenade		Buff	
	"ExpHu01_0",			"ExpHu01_2",		"ExpDi01_1",		"ExpDi01_1",		"ExpDi01_1",	"ExpDi01_1",		"ExpDi01_1",  "ExpDi01_1",	"HMK_Dino_Elite_Attack_B",	
	"ExpHu01_0",			"ExpHu01_2",		"ExpDi02_1",		"ExpDi02_1",		"ExpDi02_1",	"ExpDi02_1",		"ExpDi02_1",  "ExpDi02_1",	"HMK_Dino_Elite_Attack_B",	
};

const char * g_szDinoHitEffect_Young[2][ CHARA_EFFECT_TYPE_COUNT] = 
{	// FIrst Person Cam	Knife					Handgun					Assult Rfl.				SMG					SniperRifle.			Shotgun		Grenade						Buff
	"ExpHu01_0_Young",	"ExpHu01_2_Young",		"ExpDi01_1_Young",		"ExpDi01_1_Young",		"ExpDi01_1_Young",	"ExpDi01_1_Young",		"ExpDi01_1_Young",  "ExpDi01_1_Young",	"HMK_Dino_Elite_Attack_B",
	"ExpHu01_0_Young",	"ExpHu01_2_Young",		"ExpDi02_1_Young",		"ExpDi02_1_Young",		"ExpDi02_1_Young",	"ExpDi02_1_Young",		"ExpDi02_1_Young",  "ExpDi02_1_Young",	"HMK_Dino_Elite_Attack_B",
};

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	��� �ǰ� ����
const char * g_szCharaHitSound[ WEAPON_EFFECT_TYPE_COUNT+1][3] = 
{	
	{	"M7_Hit_Human.wav",			"M7_Hit_Human.wav",			"M7_Hit_Human.wav"			},		//	Knife
	{	"HG_Hit_Human_01.wav",		"HG_Hit_Human_02.wav",		"HG_Hit_Human_02.wav"		},		//	Handgun
	{	"AR_Hit_Human_01.wav",		"AR_Hit_Human_02.wav",		"AR_Hit_Human_02.wav"		},		//	Assault
	{	"SMG_Hit_Human.wav",		"SMG_Hit_Human.wav",		"SMG_Hit_Human.wav"			},		//	SMG
	{	"SR_Hit_Human.wav",			"SR_Hit_Human.wav",			"SR_Hit_Human.wav"			},		//	SniperRifle
	{	"SG_Hit_Human.wav",			"SG_Hit_Human.wav",			"SG_Hit_Human.wav"			},		//	Shotgun
	{	"M7_Hit_Human.wav",			"M7_Hit_Human.wav",			"M7_Hit_Human.wav"			},		//	WEAPON_EFFECT_TYPE_RAPTOR
	{	"M7_Hit_Human.wav",			"M7_Hit_Human.wav",			"M7_Hit_Human.wav"			},		//	WEAPON_EFFECT_TYPE_RAPTOR_LEFT_SCRATCH
	{	"SG_Hit_Human.wav",			"SG_Hit_Human.wav",			"SG_Hit_Human.wav"			},		// Grenade
};

const WEAPON_EFFECT_TYPE s_WeaponRedirect[ WEAPON_CLASS_COUNT] =
{
	WEAPON_EFFECT_TYPE_UNKNOWN,
	WEAPON_EFFECT_TYPE_KNIFE,		//	������
	WEAPON_EFFECT_TYPE_HANDGUN,	//	����
	WEAPON_EFFECT_TYPE_ASSAULT,	//	����
	WEAPON_EFFECT_TYPE_SMG,		//	����ӽŰ�
	WEAPON_EFFECT_TYPE_SNIFER,	//	������
	WEAPON_EFFECT_TYPE_SHOTGUN,	//	��ź��
	WEAPON_EFFECT_TYPE_UNKNOWN,	//	��ô ����
	WEAPON_EFFECT_TYPE_UNKNOWN,	//	������
	WEAPON_EFFECT_TYPE_UNKNOWN,	//	�̼� ������
	WEAPON_EFFECT_TYPE_ASSAULT,	//	�ӽŰ�

	WEAPON_EFFECT_TYPE_UNKNOWN,	//  GrenadeShell
	WEAPON_EFFECT_TYPE_UNKNOWN,	//	BombTrigger

	WEAPON_EFFECT_TYPE_HANDGUN,	// CIC Handgun
	WEAPON_EFFECT_TYPE_HANDGUN,	// DualHandGun	
	WEAPON_EFFECT_TYPE_KNIFE,	// DualKnife

	WEAPON_EFFECT_TYPE_UNKNOWN,	// WEAPON_CLASS_ROCKET_LAUNCHER,
	WEAPON_EFFECT_TYPE_ASSAULT,	// WEAPON_CLASS_OBJECT_GUN,

	WEAPON_EFFECT_TYPE_SMG,		// WEAPON_CLASS_DUALSMG,

	WEAPON_EFFECT_TYPE_RAPTOR,	// WEAPON_CLASS_DINO,
	WEAPON_EFFECT_TYPE_RAPTOR_LEFT_SCRATCH,	// WEAPON_CLASS_DINO_LEFT_SCRATCH,
	WEAPON_EFFECT_TYPE_TREX,	// WEAPON_CLASS_TREX,
	WEAPON_EFFECT_TYPE_STING,
	WEAPON_EFFECT_TYPE_KNIFE,	//WEAPON_CLASS_KNUCKLE
	WEAPON_EFFECT_TYPE_ELITE,
	WEAPON_EFFECT_TYPE_ELITE_LEFT_SCRATCH,
	WEAPON_EFFECT_TYPE_UNKNOWN,	// BOW

	WEAPON_EFFECT_TYPE_UNKNOWN,	// ������ ��ô ����
	WEAPON_EFFECT_TYPE_UNKNOWN,	// ȸ���� ��ô ����

	// SubClass
	WEAPON_EFFECT_TYPE_ASSAULT,	// ���������� �Ʈ
	WEAPON_EFFECT_TYPE_SHOTGUN,	// ���������� ����
	WEAPON_EFFECT_TYPE_SNIFER,	// ���������� ����
	WEAPON_EFFECT_TYPE_SMG,		// ���������� SMG
	WEAPON_EFFECT_TYPE_UNKNOWN,	// ���������� ��ź	
	WEAPON_EFFECT_TYPE_KNIFE,	// ���������� BOW

	WEAPON_EFFECT_TYPE_SHOTGUN	// ��� ����

};

CEffectManager::CEffectManager(void)
{
	m_bForceEmit = true;
	
	i3mem::Fill( m_CharaHitIndex, -1, sizeof(m_CharaHitIndex));
	i3mem::Fill( m_CharaWaterHitIndex, -1, sizeof(m_CharaWaterHitIndex));
	i3mem::Fill( m_TerrainEffIndex, -1, sizeof(m_TerrainEffIndex));
	i3mem::Fill( m_BulletHoleEffIndex, -1, sizeof(m_BulletHoleEffIndex));
	i3mem::Fill( m_EnvironmentEffIndex, -1, sizeof(m_EnvironmentEffIndex));
	i3mem::Fill( m_BloodEffIndex, -1, sizeof(m_BloodEffIndex));
}

void CEffectManager::OnCreate()
{
	INT32 i, j;

	//////////////////////////////////////////////////////////////////
	// ������ Hit Effect�� ���� Index�� �˻��Ѵ�.

	for( i = 0; i < I3_TERRAIN_TYPE_COUNT; i++)
	{
		for( j = 0; j < WEAPON_EFFECT_TYPE_COUNT; j++)
		{
			//	������ �ǰ� ����Ʈ
			I3ASSERT( s_szTerrainEffect[i][j] != 0); 
			if( s_szTerrainEffect[i][j][0]  != 0)
			{
				m_TerrainEffIndex[i][j] = FindEffect( s_szTerrainEffect[i][j]);
				I3ASSERT( m_TerrainEffIndex[i][j] != -1);
			}

			//	������ ź�� ����Ʈ
			I3ASSERT( s_szBulletHoleEffect[i][j] != 0); 
			if( s_szBulletHoleEffect[i][j][0] != 0)
			{
				m_BulletHoleEffIndex[i][j] = FindEffect( s_szBulletHoleEffect[i][j]);
				I3ASSERT( m_BulletHoleEffIndex[i][j] != -1);
			}
		}
	}

	//////////////////////////////////////////////////////////////////	
	for( i = 0; i < ENV_EFFECT_TYPE_COUNT; i++)
	{
		//	ȯ�� ����Ʈ
		I3ASSERT( s_szEnvironmentEffect[i] != nullptr); 
		m_EnvironmentEffIndex[i] = FindEffect( s_szEnvironmentEffect[i]);
		I3ASSERT( m_EnvironmentEffIndex[i] != -1);
	}

	SetEffectCancelProc( __CancelEffectAtObject );
}

void CEffectManager::InitAgeLimitEffect()
{
	if( m_bInitedBloodEffect)
		return;

	i3mem::Fill( m_BloodEffIndex, -1, sizeof(INT32) * (BLOOD_TERRAIN_TYPE_COUNT*BLOOD_EFFECT_TYPE_COUNT) );
	i3mem::Fill( m_CharaHitIndex, -1, sizeof( INT32) * CHARA_EFFECT_TYPE_COUNT * 2);
	i3mem::Fill( m_CharaWaterHitIndex, -1, sizeof( INT32) * CHARA_EFFECT_TYPE_COUNT * 2);
	i3mem::Fill( m_DinoHitIndex, -1, sizeof( INT32) * CHARA_EFFECT_TYPE_COUNT * 2);

	INT32 i, j;
	for( i = 0; i < CHARA_EFFECT_TYPE_COUNT; i++)
	{
		//	��� �ǰ� ����Ʈ
		for( j = 0; j < 2; j++)
		{
#if defined( I3_RELEASE_PROFILE)
			if( 1)
#else 
			if( UserInfoContext::i()->IsAdultUser())
#endif
			{
				I3ASSERT( m_CharaHitIndex[j][ i] == -1);
				m_CharaHitIndex[j][i] = FindEffect( g_szCharaHitEffect[j][i]);
				if( m_CharaHitIndex[j][i] == -1 )
				{
					I3PRINTLOG(I3LOG_NOTICE, "Ż�� �̼� �ΰ� [ %s ] � ����Ʈ�� ����.", g_szCharaHitEffect[j][i]);
				}

				I3ASSERT( m_DinoHitIndex[j][i]  == -1);
				m_DinoHitIndex[j][i] = FindEffect( g_szDinoHitEffect[j][i]);
				if( m_DinoHitIndex[j][i] == -1 )
				{
					I3PRINTLOG(I3LOG_NOTICE, "Ż�� �̼� ���� [ %s ] � ����Ʈ�� ����.", g_szDinoHitEffect[j][i]);
				}
			}
			else
			{
				I3ASSERT( m_CharaHitIndex[j][i] == -1);
				m_CharaHitIndex[j][i] = FindEffect( g_szCharaHitEffect_Young[j][i]);
				if( m_CharaHitIndex[j][i] == -1 )
				{
					I3PRINTLOG(I3LOG_NOTICE, "Ż�� �̼� �ΰ� [ %s ] ���� ����Ʈ�� ����.", g_szCharaHitEffect_Young[j][i]);
				}

				I3ASSERT( m_DinoHitIndex[j][i] == -1);
				m_DinoHitIndex[j][i] = FindEffect( g_szDinoHitEffect_Young[j][i]);
				if( m_DinoHitIndex[j][i] == -1 )
				{
					I3PRINTLOG(I3LOG_NOTICE, "Ż�� �̼� ���� [ %s ] ���� ����Ʈ�� ����.", g_szDinoHitEffect_Young[j][i]);
				}
			}

			I3ASSERT( m_CharaWaterHitIndex[j][ i] == -1);
			m_CharaWaterHitIndex[j][i] = FindEffect( g_szCharaHitEffectWater[j][i]);
			if( m_CharaWaterHitIndex[j][i] == -1 )
			{
				I3PRINTLOG(I3LOG_NOTICE, "Ż�� �̼� �ΰ� [ %s ] ���� ����Ʈ�� ����.", g_szCharaHitEffectWater[j][i]);
			}
		}
	}

	/////////////////////////////////////////////////////////////////
	// ����
	for( i = 0; i < BLOOD_TERRAIN_TYPE_COUNT; i++ )
	{
		for( j = 0; j < BLOOD_EFFECT_TYPE_COUNT; j++ )
		{
			if( UserInfoContext::i()->IsAdultUser())
			{
				I3ASSERT( s_szBloodEffect[i][j] != nullptr); 
				I3ASSERT( m_BloodEffIndex[i][j] == -1); 
				m_BloodEffIndex[ i ][ j ] = FindEffect( s_szBloodEffect[ i ][ j ] );
			}
			else
			{
				I3ASSERT( s_szBloodEffect_Young[i][j] != nullptr); 
				I3ASSERT( m_BloodEffIndex[i][j] == -1); 
				m_BloodEffIndex[ i ][ j ] = FindEffect( s_szBloodEffect_Young[ i ][ j ] );
			}
		}
	}

	// ���� ó��
	if( !UserInfoContext::i()->IsAdultUser())
	{
		for( j = 0; j < WEAPON_EFFECT_TYPE_COUNT; j++)
		{
			//	������ �ǰ� ����Ʈ
			if( g_szCharaHitEffect_Young[ 0][j+1]  != 0)
			{
				m_TerrainEffIndex[ I3_TERRAIN_BLOOD][j] = FindEffect( g_szCharaHitEffect_Young[0][j+1]);
			}
		}
	}

	m_bInitedBloodEffect = true;
}

void CEffectManager::__SetTerrainTypeToDecal( i3TimeStamp * pEffect, I3_TERRAIN_TYPE type)
{
	// �ֿ� �ڵ尡 �ּ�ó���Ǿ� �־� ��ü������ �ּ�ó���մϴ�. ���ʿ��� �ڵ��� 2017.03.16. soon9
	//i3Node * pChild;
	//i3Particle * pParticle;

	//for( INT32 i = 0; i < pEffect->GetChildCount(); i++)
	//{
	//	pChild = pEffect->GetChild( i);
	//	if( i3::same_of<i3TimeSequence*>(pChild))
	//	{
	//		for( INT32 j = 0; j < pChild->GetChildCount(); j++)
	//		{
	//			if( !i3::same_of<i3Particle*>(pChild->GetChild( j)))
	//				continue;

	//			pParticle = (i3Particle*) pChild->GetChild( j);
	//			if( pParticle->GetParticleInfo()->GetType() != i3ParticleInfo::PARTICLE_DECAL)
	//				continue;
	//			
	//			//pParticle->setTerrainType( (UINT16) type);
	//		}
	//	}
	//}
}


void CEffectManager::AddEquipHitEffect( WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, bool bListener)
{
	// Redirect
	WEAPON_EFFECT_TYPE weaponEffectType = s_WeaponRedirect[ weaponType];

	if( weaponEffectType == WEAPON_EFFECT_TYPE_UNKNOWN)
	{
		// �ش� Effect�� ������ ����.
		return ;
	}

	I3ASSERT( g_pEnvSet != nullptr);
	if( !g_pEnvSet->m_bEnableTerrainEffect) return;

	INT32 wIdx = (INT32) weaponEffectType;

	if( wIdx < 0)								wIdx = 0;
	if( wIdx >= WEAPON_EFFECT_TYPE_COUNT )		wIdx = 4;	//	���Ĵ� Sniper �Ҹ���

	if( BulletType == BULLET_EFFECT_WATER)
		wIdx = WEAPON_EFFECT_TYPE_WATER;
	//	���� ����Ʈ
	if( m_TerrainEffIndex[ I3_TERRAIN_STEEL][ wIdx] != -1)
	{
		AddEffect( m_TerrainEffIndex[ I3_TERRAIN_STEEL][ wIdx], pPos, pNormal);
	}

	//��Ͽ� ���̹ٿ��� ���̸� ���� �ڵ��Դϴ�. - raja-
	INT32 nEquipType = 0;
	if( BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_BLUE)
	{
		if( bListener )		nEquipType = 0;			
		else nEquipType = 1;
	}
	else
	{
		if( bListener )		nEquipType = 1;			
		else nEquipType = 0;
	}

	if( weaponEffectType == WEAPON_EFFECT_TYPE_KNIFE)
	{
		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HELMET_KNIFE, nEquipType, pPos, bListener);
	}
	else
	{
		if( !bListener)
		{
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HELMET_ATTACK, nEquipType, pPos, false);
		}
		else
		{
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HELMET_DEFENCE, nEquipType, pPos, true);
		}
	}
}

void CEffectManager::AddBlowEffect( I3_TERRAIN_TYPE type, WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal)
{	//	�������� �Ҳɸ� Ƣ�� ����Ʈ
	//I3ASSERT( type >= I3_TERRAIN_NA && type < I3_TERRAIN_TYPE_COUNT);
	if( type < (INT32) I3_TERRAIN_NA || type >= I3_TERRAIN_TYPE_COUNT)
	{
		I3TRACE("--- [%s] Cannot play Blow Effect. Invalid terrain type!! (%d)\n", __FUNCTION__, type);
		return;
	}

	WEAPON_EFFECT_TYPE weaponEffectType = s_WeaponRedirect[ weaponType];
	if( weaponEffectType == WEAPON_EFFECT_TYPE_UNKNOWN)
	{
		// �ش��ϴ� Effect�� ����.
		return ;
	}

	I3ASSERT( g_pEnvSet != nullptr);
	if( g_pEnvSet->m_bEnableTerrainEffect)
	{
		INT32 wIdx = (INT32) weaponEffectType;

		if( wIdx < 0)								wIdx = 0;
		if( wIdx == WEAPON_CLASS_DUALKNIFE-WEAPON_CLASS_KNIFE)	wIdx = 0;
		if( wIdx >= WEAPON_EFFECT_TYPE_COUNT)		wIdx = 4;	//	���Ĵ� Sniper��

		if( BulletType == BULLET_EFFECT_WATER)
			wIdx = WEAPON_EFFECT_TYPE_WATER;

		//	���� ����Ʈ
		if( m_TerrainEffIndex[ type][ wIdx] != -1)
		{
			AddEffect( m_TerrainEffIndex[ type][ wIdx], pPos, pNormal);
		}
	}
}

i3TimeStamp * CEffectManager::AddEnvironmentEffect( ENV_EFFECT_TYPE effType, VEC3D * pPos, VEC3D * pNormal, MATRIX * pObjMat)
{
	i3TimeStamp * pNode = nullptr;

	if( effType < (INT32) 0 || effType >= ENV_EFFECT_TYPE_COUNT)
	{
		I3TRACE("--- [%s] Cannot play Environment Effect. Invalid terrain type!! (%d)\n", __FUNCTION__, effType);
		return nullptr;
	}

	I3ASSERT( g_pEnvSet != nullptr);

	if( !g_pEnvSet->m_bEnableTerrainEffect) return nullptr;
	
	//	���� ����Ʈ
	if( m_EnvironmentEffIndex[ effType] != -1)
	{
		pNode = AddEffect( m_EnvironmentEffIndex[ effType], pPos, pNormal, pObjMat);
	}	
	else
	{
		I3TRACE( "[WARN] ������ Environment Effect(%d)�� ����\n", effType);
	}	

	return pNode;
}



i3TimeStamp * CEffectManager::AddTerrainEffect( I3_TERRAIN_TYPE type, WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, VEC3D * pDir, REAL32 fRotateRadVal, i3Object * pObj, bool UnBlowFlag, bool UnDecalFlag)
{	//	������ �Ҳɰ� ź���� ����� ����Ʈ
	//I3ASSERT( type >= I3_TERRAIN_NA && type < I3_TERRAIN_TYPE_COUNT);	

	if( type < I3_TERRAIN_NA || type >= I3_TERRAIN_TYPE_COUNT)
	{
		I3TRACE("--- [%s] Cannot play Terrain Effect. Invalid terrain type!! (%d)\n", __FUNCTION__, type);
		return nullptr;
	}

	//	��Ʈ ����Ʈ 
	if(!UnBlowFlag)
		AddBlowEffect( type, weaponType, BulletType, pPos, pNormal);

	i3TimeStamp * pEffect = nullptr;
	if(!UnDecalFlag)
		pEffect = AddDecal( type, weaponType, BulletType, pPos, pNormal, pDir, fRotateRadVal, pObj);

	return pEffect;

}

i3TimeStamp * CEffectManager::AddDecal( I3_TERRAIN_TYPE type, WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, VEC3D * pDir, REAL32 fRotateRadVal, i3Object * pObj)
{
	
	I3ASSERT( g_pEnvSet != nullptr);
	if( !g_pEnvSet->m_bEnableTerrainEffect) return nullptr;

	i3TimeStamp * pEffect = nullptr;
	MATRIX * pAttachMtx = nullptr;

	WEAPON_EFFECT_TYPE weaponEffectType = s_WeaponRedirect[ weaponType];
	if( weaponEffectType == WEAPON_EFFECT_TYPE_UNKNOWN)
	{
		// �ش��ϴ� Effect�� ����.
		return nullptr;
	}

	INT32 wIdx = (INT32) weaponEffectType;

	if( wIdx < 0)							wIdx = 0;
	if( wIdx == WEAPON_CLASS_DUALKNIFE-WEAPON_CLASS_KNIFE)	wIdx = 0;
	if( wIdx >= WEAPON_EFFECT_TYPE_COUNT)	wIdx = 4;	//	���Ĵ� Sniper

	if(BulletType == BULLET_EFFECT_WATER)
	{
		wIdx = WEAPON_EFFECT_TYPE_WATER;
	}

	if( weaponType != WEAPON_CLASS_THROWING_CONTAIN && weaponType != WEAPON_CLASS_THROWING_ATTACK && 
		weaponType != WEAPON_CLASS_THROWING_HEAL  && weaponType != WEAPON_CLASS_MISSION )		// weaponType != WEAPON_CLASS_THROWING_GRENADE 
	{
		//	ź�� ����Ʈ
		if( m_BulletHoleEffIndex[ type][ wIdx] != -1)
		{
			VEC3D vAdd;
			i3Vector::Scale( &vAdd, pDir, DECAL_DEPTH );
			i3Vector::Add( &vAdd, &vAdd, pPos );

			if( pObj != nullptr)
			{
				// ������ �� �ִ� ź�� ����Ʈ(������Ʈ�� �پ��ִ�...)
				pAttachMtx = pObj->GetMatrix();
				
				// �ִϸ��̼��ϴ� ������Ʈ�� �ִϸ��̼ǵǴ� Matrix�� �ٿ��� �Ѵ�.
				m_TransformList.clear();
				i3Scene::FindNodeByType( pObj->getSceneObject(), i3Transform2::static_meta(), m_TransformList );

				for(size_t i = 0; i < m_TransformList.size(); i++)
				{
					i3Transform2 * pTrans = (i3Transform2*) m_TransformList[i];
					pAttachMtx = pTrans->GetCacheMatrix();
				}

				pEffect = AddEffect( m_BulletHoleEffIndex[ type][ wIdx], &vAdd, pNormal, pAttachMtx, I3_RAD2DEG( fRotateRadVal), pObj);

				// ������Ʈ ���¿� ���� ����Ʈ�� ������ �� �ֵ��� ����Ʈ�� ��Ƶд�.
				if( pEffect != nullptr)
				{
					m_ObjectEffectList.push_back( pEffect);
					__SetTerrainTypeToDecal( pEffect, type);
				}
			}
			else
			{
				// ������ �� ���� ź�� ����Ʈ(�� ��...)
				pEffect = AddEffect( m_BulletHoleEffIndex[ type][ wIdx], &vAdd, pNormal, I3_RAD2DEG( fRotateRadVal));
				if( pEffect != nullptr)
					__SetTerrainTypeToDecal( pEffect, type);
				
				//I3TRACE("[%s] %f, %f\n", __FUNCTION__, I3_RAD2DEG( fRotateRadVal), fRotateRadVal);
			}

		}
	}

	return pEffect;
}
i3TimeStamp* CEffectManager :: AddDecal( INT32 EffIdx, VEC3D * pPos, VEC3D * pNormal, VEC3D * pDir, REAL32 fRotateRadVal , i3Object * pObj )
{
	I3ASSERT( g_pEnvSet != nullptr);
	if( !g_pEnvSet->m_bEnableTerrainEffect) return nullptr;
	if( EffIdx <= -1)
	{
		// �ش��ϴ� Effect�� ����.
		return nullptr;
	}

	i3TimeStamp * pEffect = nullptr;
	MATRIX * pAttachMtx = nullptr;
	
	VEC3D vAdd;
	i3Vector::Scale( &vAdd, pDir, DECAL_DEPTH );
	i3Vector::Add( &vAdd, &vAdd, pPos );

	if( pObj != nullptr)
	{
		// ������ �� �ִ� ź�� ����Ʈ(������Ʈ�� �پ��ִ�...)
		pAttachMtx = pObj->GetMatrix();

		// �ִϸ��̼��ϴ� ������Ʈ�� �ִϸ��̼ǵǴ� Matrix�� �ٿ��� �Ѵ�.
		m_TransformList.clear();
		i3Scene::FindNodeByType( pObj->getSceneObject(), i3Transform2::static_meta(), m_TransformList );

		for(size_t i = 0; i < m_TransformList.size(); i++)
		{
			i3Transform2 * pTrans = (i3Transform2*) m_TransformList[i];
			pAttachMtx = pTrans->GetCacheMatrix();
		}

		pEffect = AddEffect( EffIdx, &vAdd, pNormal, pAttachMtx, I3_RAD2DEG( fRotateRadVal), pObj);

		//// ������Ʈ ���¿� ���� ����Ʈ�� ������ �� �ֵ��� ����Ʈ�� ��Ƶд�.
		//if( pEffect != nullptr)
		//{
		//	m_ObjectEffectList.push_back( pEffect);
		//	__SetTerrainTypeToDecal( pEffect, type);
		//}
	}
	else
	{
		// ������ �� ���� ź�� ����Ʈ(�� ��...)
		pEffect = AddEffect( EffIdx, &vAdd, pNormal, I3_RAD2DEG( fRotateRadVal));
		//if( pEffect != nullptr)
		//	__SetTerrainTypeToDecal( pEffect, type);

		I3TRACE("[%s] %f, %f\n", __FUNCTION__, I3_RAD2DEG( fRotateRadVal), fRotateRadVal);
	}


	return pEffect;
}

void CEffectManager::AddBloodEffect( BLOOD_TERRAIN_TYPE TerrainType, BLOOD_EFFECT_TYPE EffectType, VEC3D * pPos, VEC3D * pNormal, VEC3D * pDir, REAL32 fRotateRadVal, i3Object * pObj )
{
	//	�±� ���� ó��. �±� �̼����� �ǰ� �ƿ� ������ �ȵ˴ϴ�.
#if !defined (LOCALE_KOREA)
	if( !UserInfoContext::i()->IsAdultUser())		return;
#endif

	if( TerrainType < (INT32) 0 || TerrainType >= BLOOD_TERRAIN_TYPE_COUNT )
	{
		I3TRACE("--- [%s] Cannot play Blood Effect. Invalid terrain type!! (%d)\n", __FUNCTION__, TerrainType);
		return;
	}
	if( EffectType < (INT32) 0 || EffectType >= BLOOD_EFFECT_TYPE_COUNT)
	{
		I3TRACE("--- [%s] Cannot play Blood Effect. Invalid Effect type!! (%d)\n", __FUNCTION__, EffectType);
		return;
	}

	I3ASSERT( g_pEnvSet != nullptr);
	if( !g_pEnvSet->m_bEnableTerrainEffect) return;
	
	//	���� ����Ʈ
	if( m_BloodEffIndex[TerrainType][EffectType] != -1)
	{
		VEC3D vPos;
		i3Vector::Scale( &vPos, pDir, DECAL_BLOOD_DEPTH );
		i3Vector::Add( &vPos, pPos, &vPos );

		if( pObj != nullptr )
		{
			MATRIX * pAttachMtx;
			// ������ �� �ִ� ź�� ����Ʈ(������Ʈ�� �پ��ִ�...)
			pAttachMtx = pObj->GetMatrix();
			
			// �ִϸ��̼��ϴ� ������Ʈ�� �ִϸ��̼ǵǴ� Matrix�� �ٿ��� �Ѵ�.
			i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObj->getSceneObject() );
			if( pTrans != nullptr )
			{
				pAttachMtx = pTrans->GetCacheMatrix();
			}

			i3TimeStamp * pEffect = AddEffect( m_BloodEffIndex[ TerrainType ][ EffectType ], &vPos, pNormal, pAttachMtx, fRotateRadVal, pObj );

			// Effect�� ����ִ� ����, �ش� Object�� �޸� �����Ǵ� ��츦 ����..
			if( pEffect != nullptr)
			{
				m_ObjectEffectList.push_back( pEffect);
				__SetTerrainTypeToDecal( pEffect, I3_TERRAIN_NA);
			}
		}
		else
		{
			i3TimeStamp * pEffect = AddEffect( m_BloodEffIndex[ TerrainType ][ EffectType ], &vPos, pNormal, fRotateRadVal );
			if( pEffect != nullptr)
				__SetTerrainTypeToDecal( pEffect, I3_TERRAIN_NA);
		}

	}	
	else
	{
		I3TRACE( "[WARN] ������ Blood Effect(%d, %d)�� ����\n", TerrainType, EffectType);
	}	
}


void CEffectManager::AddCharaHitEffect( WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, REAL32 dist)
{	//	WEAPON_CLASS_UNKNOWN�� FPS ķ ���� ����Ʈ��
	//	�±� ���� ó��. �±� �̼����� �ǰ� �ƿ� ������ �ȵ˴ϴ�.
#if defined(LOCALE_VIETNAM)
	//	�߱��� ��� ���� ����Ʈ������ 1��Ī �ڽ��� ������ ���� ����Ʈ �ȳ��´�. 3��Ī ĳ���� ��Ʈ�� ���� ������
	if( weaponType == WEAPON_CLASS_UNKNOWN)	return;
#else
	#if !defined (LOCALE_KOREA)
	//	���� 
		if(UserInfoContext::i()->IsAdultUser() == false)		return;
	#endif

	I3ASSERT( g_pEnvSet != nullptr);
	if( g_pEnvSet->m_bEnableBlood == false) return;
#endif

	INT32 wIdx = (INT32) weaponType;

	if( wIdx == WEAPON_CLASS_MG)
		wIdx = 3;
		
	if( wIdx < 0)							wIdx = 0;
	if( wIdx >= CHARA_EFFECT_TYPE_COUNT)	wIdx = 4;	//	���Ĵ� Sniper

	INT32 idxDist = 0;
	
	if( dist > 17.0f)
		idxDist = 1;

	if(BulletType == BULLET_EFFECT_WATER)
	{
		if( m_CharaWaterHitIndex[idxDist][ wIdx] == -1 )
		{
			I3PRINTLOG(I3LOG_FATAL, "[ %s ] ���� ����Ʈ�� ����.", m_CharaWaterHitIndex[idxDist][ wIdx]);
		}
		AddEffect( m_CharaWaterHitIndex[idxDist][ wIdx], pPos, pNormal);	
	}
	else
	{
		if( m_CharaHitIndex[idxDist][ wIdx] == -1 )
		{
			I3PRINTLOG(I3LOG_FATAL, "[ %s ] ���� ����Ʈ�� ����.", m_CharaHitIndex[idxDist][ wIdx]);
		}
		AddEffect( m_CharaHitIndex[idxDist][ wIdx], pPos, pNormal);	
	}

}
void CEffectManager::AddDinoBuffEffect( VEC3D * pPos, VEC3D * pNormal)
{

	I3ASSERT( m_DinoHitIndex[0][ 8] != -1);
	AddEffect( m_DinoHitIndex[0][ 8],  pPos, pNormal);	

}
void CEffectManager::AddDinoHitEffect( WEAPON_CLASS_TYPE weaponType, BULLET_EFFECT_TYPE BulletType, VEC3D * pPos, VEC3D * pNormal, REAL32 dist)
{	//	WEAPON_CLASS_UNKNOWN�� FPS ķ ���� ����Ʈ��
	//	�±� ���� ó��. �±� �̼����� �ǰ� �ƿ� ������ �ȵ˴ϴ�.
	#if !defined (LOCALE_KOREA)
	if(UserInfoContext::i()->IsAdultUser() == false)		return;
	#endif

	I3ASSERT( g_pEnvSet != nullptr);
	if( g_pEnvSet->m_bEnableBlood == false) return;
	
	INT32 wIdx = (INT32) weaponType;

	if( wIdx == WEAPON_CLASS_MG)
		wIdx = 3;
		
	if( wIdx < 0)							wIdx = 0;
	if( wIdx >= CHARA_EFFECT_TYPE_COUNT)	wIdx = 4;	//	���Ĵ� Sniper

	INT32 idxDist = 0;
	
	if( dist > 17.0f)
		idxDist = 1;

	if( BulletType == BULLET_EFFECT_WATER)
	{
		if( m_CharaWaterHitIndex[idxDist][ wIdx] == -1 )
		{
			I3PRINTLOG(I3LOG_FATAL, " [ %s ] ���� ����Ʈ�� ����.", m_CharaWaterHitIndex[idxDist][ wIdx]);
		}
		AddEffect( m_CharaWaterHitIndex[idxDist][ wIdx], pPos, pNormal);	
	}
	else
	{
		if( m_DinoHitIndex[idxDist][ wIdx] == -1 )
		{
			I3PRINTLOG(I3LOG_FATAL, " [ %s ] ���� ����Ʈ�� ����.", m_DinoHitIndex[idxDist][ wIdx]);
		}
		AddEffect( m_DinoHitIndex[idxDist][ wIdx], pPos, pNormal);	
	}
}

void CEffectManager::OnUpdate( REAL32 rDeltaSeconds)
{
	i3EffectManager::OnUpdate( rDeltaSeconds);
	


	m_DeleteObjectList.clear();

	// ������� Object�� ���� Effect�� ó���մϴ�.
	for(size_t i=0; i< m_ObjectEffectList.size(); i++)
	{
		bool bDelete = false;
		i3TimeStamp * pEffect = m_ObjectEffectList[i];

		i3Node * pNode = pEffect->GetChild( 0);
		i3TimeSequence * pTimeSeq = nullptr;

		if( i3::same_of<i3TimeSequence*>(pNode))
		{
			pTimeSeq = (i3TimeSequence*)pNode;
			if( pTimeSeq->IsActive() == false)	bDelete = true;
		}
		
		i3Object * pObj = (i3Object*) pEffect->getRefObject();
		if( i3::kind_of<i3Object*>(pObj))
		{	
			// Object�� �پ��ִ� Effect�� ��Ȱ��ȭ �մϴ�.
			if( ((pObj->isEnable() == false) || (pObj->isVisible() == false)) || pObj->isChangedSg() )
				bDelete = true;
		}
		else
			bDelete = true;

		if( bDelete )
		{
			if( pObj != nullptr)	m_DeleteObjectList.push_back( pObj);
			pEffect->DeactivateNode();
			pEffect->setRefObject( nullptr);
			i3::vu::erase_index(m_ObjectEffectList, i);
			i--;
		}
	}

	for(size_t i = 0; i < m_DeleteObjectList.size(); i++)
	{
		i3Object * pObj = m_DeleteObjectList[i];
		pObj->resetChangedSgFlag();
	}
}

void CEffectManager::Reset()
{
	i3EffectManager::Reset();

	// ������� Object�� �پ��ִ� Effect Lsit�� �ʱ�ȭ�մϴ�.
	m_ObjectEffectList.clear();
}
