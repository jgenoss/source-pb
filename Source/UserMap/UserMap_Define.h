#if !defined( __USERMAP_DEFINE_H)
#define __USERMAP_DEFINE_H

#include "UserMap_Block.h"
#include "UserMap_Block_Geometry.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Stage
//
namespace USMAP
{
	namespace STAGE
	{
		typedef UINT32		STYLE;
		
		const STYLE		STYLE_INTERCEPTABLE		= 0x00000001;				// ���� ����
		const STYLE		STYLE_ROUND				= 0x00000002;				// Round ��
		const STYLE		STYLE_RESPAWN_USER		= 0x00000004;				// �ݺ��ؼ� Respawn ��.

		struct INFO
		{
			INT8				m_ID[4];
			UINT64				m_MD5[2];
			
			char				m_Name[128];						// Map �̸�
			UINT32				m_Author;							// ������ ID
			UINT16				m_Version;							// Version Number (Ordinal)
			UINT32				m_PlayCount;						// Play ȸ��
			INT32				m_RegCount;							// Server ��� ȸ��
			INT8				m_LastEditDate[6];					// ���� ���� ����.

			STYLE				m_Style;

			UINT8				m_FrameCX;
			UINT8				m_FrameCZ;							// Map�� ũ��.

			INT16				m_BlockCount;						// ��ġ�� Block�� ����
			INT32				m_BlockOffset;						// Block Data�� ��� Offset

			INT16				m_ConditionCount;					// Condition�� ����
			INT32				m_ConditionOffset;					// Condition Data�� ��� Offset

			INT8				m_Sky;

			INT8				m_bFogEnable;						// Fog ��/��
			UINT32				m_FogColor;							// Fog Color
			REAL32				m_FogNear;							// Fog Near
			REAL32				m_FogFar;							// Fog Far

			REAL32				m_Gravity;							// �߷�
			INT8				m_MaxRedUserCount;					// RED Team �ִ� ���� ��
			INT8				m_MaxBlueUserCount;					// BLUE Team �ִ� ���� ��
			INT8				m_TimeLimit;						// ���� �ð� (�� ����)
			INT8				m_RoundLimit;						// ���� Round ��
			REAL32				m_DamageFactor;						// �������� ���� ����. (0~1)
			
			INT8				pad[44];							// 256 ũ�⸦ ���߱� ���� pad

			void				init(void);
		};
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Object
	//
	namespace OBJECT
	{
		typedef UINT16			STYLE;
		
		const STYLE		STYLE_DYNAMIC			= 0x0001;
		const STYLE		STYLE_TARGET_RED		= 0x0002;
		const STYLE		STYLE_TARGET_BLUE		= 0x0004;
		const STYLE		STYLE_NETSYNC			= 0x0008;
		const STYLE		STYLE_BREAKABLE			= 0x0010;
		const STYLE		STYLE_RESPAWN			= 0x0020;
		const STYLE		STYLE_NO_LIGHT			= 0x0040;

		struct INFO
		{
			UINT16						m_ID;						// Object�� Type ID
			STYLE						m_Style;					// Object�� ����
			INT16						m_Pos[3];					// ��ġ ����		(0-bits Fixed-Point format)
			INT8						m_Rot[3];					// ȸ�� ����		(8-bits Fixed-Point format)
			INT8						m_Scale[3];
			UINT8						m_TexID;					// Texture ID / Trigger ID / Respawn Team;
			INT8						m_Group;

			void	init(void);

			void	setScale( REAL32 sx, REAL32 sy, REAL32 sz);
			REAL32	getScale( INT32 idx)								{ return m_Scale[idx] * 0.5f; }
		};
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Action
	//
	namespace ACTION
	{
		enum ACTION_TYPE
		{
			ACTION_NONE = 0,
			ACTION_FIN_BATTLE,						// ���� ���� �� ���			(-)
			ACTION_FIN_ROUND,						// Round�� ����					(-)
			ACTION_BREAK_OBJECT,					// Ư�� Object�� �ı�			(Data[0] : Block ID)
			ACTION_RESPAWN,							// ���� User�� Respawn			(Data[0] : USMAP_COND_TARGET)
			ACTION_KILL,							// ���� User�� Kill				(Data[0] : USMAP_COND_TARGET)
			ACTION_ENABLE_TRIGGER,					// Ư�� Trigger�� Ȱ��ȭ		(Data[0] : Block(Trigger) ID)
			ACTION_DISABLE_TRIGGER,					// Ư�� Trigger�� ��Ȱ��ȭ		(Data[0] : Block(Trigger) ID)
			ACTION_ADD_TIME,						// ���� �ð��� ����/����		(Data[0] : ��ȭġ)
			ACTION_ADD_REDKILL,						// RED KillCount�� ����/����	(Data[0] : ��ȭġ)
			ACTION_ADD_BLUEKILL,					// BLUE KillCount�� ����/����	(Data[0] : ��ȭġ)
			ACTION_ADD_ALLYKILL,					// �Ʊ� KillCount�� ����/����	(Data[0] : ��ȭġ)
			ACTION_ADD_ENEMYKILL,					// �Ʊ� KillCount�� ����/����	(Data[0] : ��ȭġ)
			ACTION_ADD_NPC,							// NPC �߰�.
			ACTION_ALARM,							// Alarm �︮��					(Data[0] : Alarm ID)
			ACTION_SHOW_BOMB_INSTALL_UI,			// ��ź ��ġ UI ǥ��			(-)
		};
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Condition
	//
	namespace COND
	{
		typedef UINT16			STYLE;
	
		const STYLE		STYLE_START			= 0x0001;
		const STYLE		STYLE_ALWAYS		= 0x0002;
		const STYLE		STYLE_ROUNDEND		= 0x0004;

		enum PARAM
		{
			PARAM_TIME			= 0,			// PlayTime�� �ʴ���. ���� Round����� Round Play Time���� ����.	(Value[0])
			PARAM_ALIVECOUNT,					// ����ִ� ���� ��													(Target[0], Value[0])
			PARAM_DEADCOUNT,					// ���� ���� ��														(Target[0], Value[0])
			PARAM_RESPAWNCOUNT,					// ���� User�� Respawn ȸ��											(Target[0], Value[0])
			PARAM_ROUNDCOUNT,					// ������ Round ȸ��												(Value[0])
			PARAM_TARGETHP,						// Target Object�� HP												(Target[0], Value[0])
			PARAM_USERHP,						// ���� User�� HP													(Target[0], Value[0])
			PARAM_TARGETCOUNT,					// Target Object ��, �ı����� ���� Object�� ����					(Target[0], Value[0])
			PARAM_USERTRIGGER,					// ���� User�� ������ Trigger ID									(Target[0], Value[0])
			PARAM_OBJECTTRIGGER,				// ���� Object�� ������ Trigger ID									(Target[0], Value[0])
			PARAM_KILLCOUNT,					// Kill Count														(Target[0], Value[0])
			PARAM_TOTALKILLCOUNT,				// ��ü�� Kill Count												(Target[0], Value[0])
			PARAM_INSTALLED_BOMB_COUNT,			// ��ġ�� ��ź�� ����												(Value[0])
			PARAM_DEFUSED_BOMB_COUNT,			// ������ ��ź�� ����												(Value[0])
		};

		enum TARGET
		{
			TARGET_PLAYER = 1,					// Player���� ����Ǵ� ����
			TARGET_ALL_ALLY,					// �ڽ��� ������ ��� �Ʊ��� ����Ǵ� ����
			TARGET_ALL_ENEMY,					// ��� �������� ����Ǵ� ����
			TARGET_RED_TARGET,					// RED Team Target Object�� ����Ǵ� ����.
			TARGET_BLUE_TARGET,					// BLUE Team Target Object�� ����Ǵ� ����.
			TARGET_ALLY_TARGET,					// �Ʊ� Target Object�� ����Ǵ� ����
			TARGET_ENEMY_TARGET,				// �Ʊ� Target Object�� ����Ǵ� ����
			TARGET_DEAD_ALLY,					// ���� �Ʊ� ��, ù ��° 1��
			TARGET_ALL_HEAD_ALLY,				// ���� ��� �Ʊ�
			TARGET_DEAD_ENEMY,					// ���� ���� ��, ù ��° 1��
			TARGET_ALL_DEAD_ENEMY,				// ���� ��� ����.
		};

		struct INFO
		{
			STYLE							m_Style;				// ����
			INT8							m_AndLink;				// Single-Linked-List�� Next Index. (-1 == ���� ����)
			PARAM							m_Param;
			TARGET							m_Target;
			INT32							m_Value[2];
			USMAP::ACTION::ACTION_TYPE		m_Action[4];
			TARGET							m_ActionTarget[4];
			INT32							m_ActionData[4];

			void				init(void);
		};
	};

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Blocks
	namespace BLOCK
	{
		const INT32				ICON_CX		= 38;
		const INT32				ICON_CY		= 37;

		typedef UINT32			EDIT_STYLE;

		const EDIT_STYLE		EDIT_STYLE_DISABLE_MOVE_X		= 0x00000001;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_MOVE_Y		= 0x00000002;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_MOVE_Z		= 0x00000004;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_ROTATE_X		= 0x00000008;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_ROTATE_Y		= 0x00000010;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_ROTATE_Z		= 0x00000020;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_SCALE_X		= 0x00000040;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_SCALE_Y		= 0x00000080;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_SCALE_Z		= 0x00000100;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_TEX			= 0x00000200;

		typedef UINT16			ID;

		const ID				ID_NONE							= 0;
		const ID				ID_CUBE							= 1;
		const ID				ID_SPHERE						= 2;
		const ID				ID_SLOPE						= 3;
		const ID				ID_TRAP							= 4;
		const ID				ID_PYRAMID						= 5;
		const ID				ID_ROAD1						= 6;
		const ID				ID_HOLE1						= 7;
		const ID				ID_WALL_DOOR1					= 8;
		const ID				ID_WALL_HOLE1					= 9;
		const ID				ID_WALL_WINDOW1					= 10;
		const ID				ID_STEP1						= 11;
		const ID				ID_STEP2						= 12;
		const ID				ID_BARICATE						= 13;
		const ID				ID_CAR1							= 14;
		const ID				ID_DRUM1						= 15;
		const ID				ID_CONTAINER1					= 16;
		const ID				ID_BOX_WOOD						= 17;

		struct INFO
		{
			char				m_szName[64];
			char				m_szRes[ 128];
			ID					m_ID;
			i3GameResObject *	m_pRes;
			CUserMap_Block *	m_pBlock;
			UINT32				m_EditStyle;

			char				m_szTex[128];
			i3Texture *			m_pIconTex;
			VEC2D				m_UV;
		};

		INT32					getCount(void);
		INFO *					getInfo( INT32 idx);
		INFO *					FindInfo( UINT16 id);
	};

	//////////////////////////////////////////////////////////////////////////////////
	// Sky
	namespace SKY
	{
		typedef INT32			TYPE;

		typedef struct _tagInfo
		{
			char				m_szRes[128];
			char				m_szName[128];
			i3SceneGraphInfo *	m_pSg;
			COLORREAL			m_LightDiffuse;
			COLORREAL			m_LightSpecular;
			VEC3D				m_LightDir;
		} INFO;

		INT32					getCount(void);
		INFO *					getInfo( INT32 idx);
	};
};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
