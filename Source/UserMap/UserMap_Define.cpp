#include "pch.h"
#include "UserMap_Define.h"

void USMAP::STAGE::INFO::init(void)
{
	i3mem::FillZero( this, sizeof(USMAP::STAGE::INFO));

	i3mem::Copy( m_ID, "USI1", 4);
	
	m_Version			= 1;							// Version Number (Ordinal)

	m_Style				= STYLE_INTERCEPTABLE | STYLE_RESPAWN_USER;

	m_bFogEnable		= TRUE;						// Fog ��/��
	m_FogColor			= 0xFF7F7F7F;				// Fog Color
	m_FogNear			= 1.0f;						// Fog Near
	m_FogFar			= 100.0f;					// Fog Far

	m_Gravity			= 9.8f;						// �߷�
	m_MaxRedUserCount	= 8;						// RED Team �ִ� ���� ��
	m_MaxBlueUserCount	= 8;						// BLUE Team �ִ� ���� ��
	m_TimeLimit			= 10;						// ���� �ð� (�� ����)
	m_RoundLimit		= 5;						// ���� Round ��
	m_DamageFactor		= 1.0f;						// �������� ���� ����. (0~1)
}

void USMAP::OBJECT::INFO::init(void)
{
	i3mem::FillZero( this, sizeof(USMAP::OBJECT::INFO));

	m_Scale[0] = 2;
	m_Scale[1] = 2;
	m_Scale[2] = 2;
}

void USMAP::OBJECT::INFO::setScale( REAL32 sx, REAL32 sy, REAL32 sz)
{
	m_Scale[0] = (INT8) i3Math::Clamp( (sx / 0.5f), -128.0f, 127.0f);
	m_Scale[1] = (INT8) i3Math::Clamp( (sy / 0.5f), -128.0f, 127.0f);
	m_Scale[2] = (INT8) i3Math::Clamp( (sz / 0.5f), -128.0f, 127.0f);
}

void USMAP::COND::INFO::init(void)
{
	i3mem::FillZero( this, sizeof(USMAP::COND::INFO));

	m_Style				= STYLE_ALWAYS;			// ����
	m_AndLink			= -1;					// Single-Linked-List�� Next Index. (-1 == ���� ����)
}

static USMAP::BLOCK::INFO		s_BlockInfo[] =
{
	{	// 0
		"ť��",	"MapEditor/res/block_box.i3Obj", USMAP::BLOCK::ID_CUBE,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL,	{ 2.0f,		353.0f}
	},

	{	// 1
		"��", "MapEditor/res/block_sphere.i3Obj", USMAP::BLOCK::ID_SPHERE,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL,	{ 43.0f,		353.0f}
	},

	{	// 2
		"����", "MapEditor/res/block_tri.i3Obj", USMAP::BLOCK::ID_SLOPE,	NULL, 	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL,	{ 84.0f,		353.0f}
	},

	{	// 3
		"����", "MapEditor/res/block_trap01.i3Obj", USMAP::BLOCK::ID_TRAP,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL,	{ 125.0f,		353.0f}
	},

	{	// 4
		"�Ƕ�̵�",	"MapEditor/res/block_pyramid.i3Obj", USMAP::BLOCK::ID_PYRAMID,	NULL, NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 167.0f,		353.0f}
	},

	{	// 5
		"���",	"MapEditor/res/block_road01.i3Obj", USMAP::BLOCK::ID_ROAD1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 208.0f,		353.0f}
	},

	{	// 6
		"ȯ�ⱸ", "MapEditor/res/block_hole01.i3Obj", USMAP::BLOCK::ID_HOLE1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 249.0f,		353.0f}
	},

	{	// 7
		"��(��)", "MapEditor/res/block_wall_door01.i3Obj", USMAP::BLOCK::ID_WALL_DOOR1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 289.0f,		353.0f}
	},

	{	// 8
		"��(����)", "MapEditor/res/block_wall_hole01.i3Obj", USMAP::BLOCK::ID_WALL_HOLE1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 330.0f,		353.0f}
	},

	{	// 9
		"��(â)", "MapEditor/res/block_wall_window01.i3Obj", USMAP::BLOCK::ID_WALL_WINDOW1,	NULL,	NULL, 0,
		"MapEditor/UseMapSource01.i3i", NULL, { 371.0f,		353.0f}
	},

	{	// 10
		"���1", "MapEditor/res/block_step01.i3Obj", USMAP::BLOCK::ID_STEP1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 2.0f,		434.0f}
	},

	{	// 11
		"���2", "", USMAP::BLOCK::ID_STEP2,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 43.0f,		434.0f}
	},

	{	// 12
		"�ٸ�����Ʈ", "MapEditor/res/block_baricate.i3Obj", USMAP::BLOCK::ID_BARICATE,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 84.0f,		434.0f}
	},

	{	// 13
		"�ڵ���", "MapEditor/res/block_car01.i3Obj", USMAP::BLOCK::ID_CAR1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 125.0f,		434.0f}
	},

	{	// 14
		"�巳��", "MapEditor/res/block_drum01.i3Obj", USMAP::BLOCK::ID_DRUM1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 166.0f,		434.0f}
	},

	{	// 15
		"�����̳�", "MapEditor/res/block_container01.i3Obj", USMAP::BLOCK::ID_CONTAINER1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 207.0f,		434.0f}
	},

	{	// 16
		"����(����)", "MapEditor/res/block_woodbox.i3Obj", USMAP::BLOCK::ID_CONTAINER1,	NULL,	NULL, USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX,
		"MapEditor/UseMapSource01.i3i", NULL, { 247.0f,		434.0f}
	}
};

INT32 USMAP::BLOCK::getCount(void)
{
	return GET_ARRAY_COUNT(s_BlockInfo);
}

USMAP::BLOCK::INFO * USMAP::BLOCK::getInfo( INT32 idx)
{
	I3_BOUNDCHK( idx, getCount());

	return & s_BlockInfo[ idx];
}

USMAP::BLOCK::INFO * USMAP::BLOCK::FindInfo( UINT16 id)
{
	INT32 i;

	for( i = 0; i < getCount(); i++)
	{
		if( s_BlockInfo[i].m_ID == id)
			return & s_BlockInfo[i];
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////
//
// Sky
static USMAP::SKY::INFO		s_SkyTable[] =
{
	{	"MapEditor/res/Sky_CloudDayLight.i3s",		"�� �� (����)",	NULL,
		{ 1.2f,	1.2f, 1.2f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.2f, 0.8f, 0.2f }
	},

	{	"MapEditor/res/Sky_BecomingRedSky.i3s",		"���� ����",	NULL,
		{ 1.2f,	1.0f, 1.0f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.3f, -0.8f, 0.3f }
	},

	{	"MapEditor/res/Sky_BecomingRedSky2.i3s",		"���� ���� (����)",	NULL,
		{ 1.0f,	0.8f, 0.8f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.2f, -0.8f, -0.4f }
	},

	{	"MapEditor/res/Sky_RedSky.i3s",				"����",	NULL,
		{ 1.0f,	0.95f, 0.8f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.4f, -0.7f, -0.5f }
	},

	{	"",				"",	NULL,
		{ 0.0f,	0.0f, 0.8f, 1.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.4f, -0.7f, -0.5f }
	},
};

INT32 USMAP::SKY::getCount(void)
{
	INT32 i;

	for( i = 0; s_SkyTable[i].m_szName[0] != 0; i++);

	return i;
}

USMAP::SKY::INFO * USMAP::SKY::getInfo( INT32 idx)
{
	return & s_SkyTable[idx];
}
