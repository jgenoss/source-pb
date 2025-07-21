#pragma once

#define MAX_WEAPON_COMMAND_COUNT			8

#define MAX_EMPTYBULLETSOUND_COUNT			32					// ź�� ����� ���� ī��Ʈ
#define MAX_TERRAINSOUND_COUNT				32					// ���� Ÿ�� ����� ���� ī��Ʈ
#define BLOODEFFECT_HIT_LENGTH				5.f					// ������ ���� �ٴ� �Ÿ�
#define DELAY_EMPTYBULLETSOUND				0.7f				// ź�ǰ� Ƣ�� �Ҹ��� ������ Ÿ��

#define		WEAPON_BOMBSTATE_THROWING		0x00000001			// ������ ����
#define		WEAPON_BOMBSTATE_SETTLED		0x00000002			// ������ ����
#define		WEAPON_BOMBSTATE_ATTACHOBJECT	0x00000004			// ������Ʈ�� ���� ����
#define		WEAPON_BOMBSTATE_TIMESTART		0x00000008			// �ݹ�(�Ϻ� ����� ���� �ð� �Ŀ� �ݹ��� �ȴ�. ��.C5)
#define		WEAPON_BOMBSTATE_ATTACHWALL		0x00000010			// ���� �پ���.
#define		WEAPON_BOMBSTATE_FREEZE			0x00000020			// in world
#define		WEAPON_BOMBSTATE_EXPLOSION		0x00000040			// explosion
#define		WEAPON_BOMBSTATE_HITBOUND		0x00000080			// �����ΰ��� �浹 �� �ٿ�� �� ���� ( ������̱�, �������Ұ� ���� ����� ���� ���� ó�� ������ ���� ) // 2014.06.12 ENG_��뿵

typedef UINT32 WEAPON_FLAG;
#define WEAPON_FLAG_MULTIPLE_EXTENSION		0x00000001			// ���� extension ����
#define WEAPON_FLAG_ENABLE_FIRE_SOUND		0x00000002			// �߻� ���� ��� ���� ����
#define WEAPON_FLAG_ATTACHED				0x00000004			// �پ� �ִ��� üũ?
#define WEAPON_FLAG_TAKE_CHARA				0x00000008			// ĳ���Ͱ� ���� ����

enum WEAPON_ATTACH_TYPE
{
	WEAPON_ATTACH_NONE = 0,
	WEAPON_ATTACH_GRIP,						//	�� ������
	WEAPON_ATTACH_BARREL,					//	�ѿ� ��������
};

enum WEAPON_DUAL_MAGAZINE_TYPE
{
	WEAPON_DUAL_MAGAZINE_NONE = 0,	// �Ϲ� �ѱ�
	WEAPON_DUAL_MAGAZINE_IDLE,		// ���źâ
	WEAPON_DUAL_MAGAZINE_RIGHT,		// ���źâ ����
};

enum WEAPON_SNIPER_DUAL_MAGAZINE_TYPE
{
	WEAPON_SNIPER_DUAL_MAGAZINE_NONE = 0,
	WEAPON_SNIPER_DUAL_MAGAZINE_SWAP,
	WEAPON_SNIPER_DUAL_MAGAZINE_RELOAD,
};

enum WEAPON_MOVE_SPEED_TYPE
{
	WEAPON_MOVE_SPEED_NORMAL = 0,
	WEAPON_MOVE_SPEED_FIRE,
	WEAPON_MOVE_SPEED_SIT,
	WEAPON_MOVE_SPEED_ZOOM,
};

enum WEAPON_BULLET_HIT_STATE
{
	WEAPON_BULLET_HIT_STATE_NONE = 0,
	WEAPON_BULLET_HIT_STATE_NO_HIT,				///< Collision�� �ȵǾ���.
	WEAPON_BULLET_HIT_STATE_LESSRANGE,			///< Collision�� �Ǿ����� ���� ��Ÿ��� �ȵȴ�.
	WEAPON_BULLET_HIT_STATE_HIT,				///< �Ϲ� Hit
	WEAPON_BULLET_HIT_STATE_PENETRATE,			///< ���� Hit
};

namespace WEAPON {

	enum FIRE_SOUND_TYPE
	{
		FIRE_SOUND_NORMAL		= 0,	//�⺻ �߻� �Ҹ�
		FIRE_SOUND_NORMAL_2,				//�⺻ �߻� �Ҹ� 2
		FIRE_SOUND_SCOPE,				//�ܻ��¿����� �߻�Ҹ�
		FIRE_SOUND_MUFFLER,				//������ ���������� �߻�Ҹ�
		FIRE_SOUND_EXT,					// ��ź �߻� �Ҹ�
		FIRE_SOUND_NORMAL_3PV,		//�߻� �Ҹ� 3��Ī

		FIRE_SOUND_COUNT
	};

	enum eSTATE
	{
		STATE_READY,						// ��� ����
		STATE_WAIT,						// �߻簡 �ǰ� ���� �߻縦 ��ٸ��� ����
		STATE_FIRE,						// �߻� �ִϸ��̼���
		STATE_FIRING,					// �߻簡 ������ ���� ������ ���� �߻� �ִϸ��̼����̴�.
		STATE_LOADBULLET,				// �븮�踦 �ڷ� ���� ����
		STATE_LOADMAG,					// Magazine�� ���� ����� ����
		STATE_NEEDTO_FIRE,				// �߻縦 �䱸�ϴ� ����(����ÿ� ���)
		STATE_NEEDTO_LOADBULLET,			// Chara�� Reload�� �ؾ��ϴ� ����
		STATE_NEEDTO_LOADMAG,			// Chara�� Magazine�� ���� ������ �ϴ� ����
		STATE_EMPTY_MAGAZINE,			// ź���� �� ����
		STATE_NEEDTO_QUICKLOAD,			// ��� źâ�� ��ȯ�� �䱸�ϴ� ����
		STATE_NEEDTO_LOADMAGREADY,		// �Ű����� ��ȯ�ϱ� �� ������ �ʿ�(���ǿ� źȯ �ֱ� �� ����) - ������
		STATE_NEEDTO_REMOVESHELL,		// �Ű����� ���� ����
		STATE_NEEDTO_DROPWEAPON,			// ���⸦ ������ ����
		STATE_NEEDTO_LOAD_B_MAG,			// ���� �������� �� B �Ű����� �ʿ��� ��
		STATE_LOADMAG_READY,				// Reload �غ� ���� (�Ϸ�Ǹ� LOADMAG�� �����Ѵ�)
	};


	enum AI
	{
		AI_CURRENT = 0,

		AI_FIRE,							//	Action Fire
		AI_FIRE1PV,
		AI_FIRE3PV,

		AI_FIREMALE,
		AI_FIREFEMALE,

		AI_SECFIRE,						//	Action Secondary Fire
		AI_SECFIRE3PV,
		AI_IDLE,							// Action Idle
		AI_IDLE1PV,
		AI_IDLE3PV,

		AI_LOADBULLET,
		AI_LOADBULLET1PV,
		AI_LOADBULLET3PV,

		AI_LOADMAG_S,

		AI_LOADMAGAZINE,

		AI_LOADMAGAZINEDUAL,				//	Action DualHandGun loadmagazine
		AI_LOADMAGAZINEDUAL1PV_LEFT,
		AI_LOADMAGAZINEDUAL1PV_RIGHT,
		AI_LOADMAGAZINEDUAL3PV_LEFT,
		AI_LOADMAGAZINEDUAL3PV_RIGHT,

		AI_LOADMAGAZINEDUAL1PV_SWAP,		// DSR-1 ��� źâ
		AI_LOADMAGAZINEDUAL3PV_SWAP,
		AI_LOADMAGAZINEDUAL1PV_RELOAD,
		AI_LOADMAGAZINEDUAL3PV_RELOAD,

		AI_LOADMAGAZINE_1PV,
		AI_LOADMAGAZINE_3PV,

		// Dual ����
		AI_LOADMAGAZINE_DUALMAG_ALL,			// ��� źâ �������� �������� ��ü ��ü

		AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE,
		AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE,
		AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE,
		AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE,	

		AI_LOADMAGAZINE_EXTENSION,
		AI_LOADMAGAZINE1PV_EXTENSION,
		AI_LOADMAGAZINE3PV_EXTENSION,

		//ĳ���ͳ��౸�� 
		//���ε�
		AI_LOADMAGAZINE_1PV_MALE,
		AI_LOADMAGAZINE_3PV_MALE,
		AI_LOADMAGAZINE_1PV_FEMALE,
		AI_LOADMAGAZINE_3PV_FEMALE,

		//�ͽ��ټ� ���ε�
		AI_LOADMAGAZINE1PV_EXTENSION_MALE,
		AI_LOADMAGAZINE3PV_EXTENSION_MALE,
		AI_LOADMAGAZINE1PV_EXTENSION_FEMALE,
		AI_LOADMAGAZINE3PV_EXTENSION_FEMALE,
		
		//�ͽ��ټ� ������ ���ε�
		AI_LOADMAGAZINE_MASTER1PV_EXT,
		AI_LOADMAGAZINE_MASTER3PV_EXT,

		AI_LOADMAG_E,

		AI_LOADMAG_MASTER_S,

		// ������ ����.
		AI_LOADMAGAZINE_MASTER1PV,
		AI_LOADMAGAZINE_MASTER3PV,
		AI_LOADMAGAZINE_RIGHT_MASTER1PV,		// Dual Magazine ���⿡�� Ext�� źâ�� �ٲ� ���¿��� ���ε�
		AI_LOADMAGAZINE_RIGHT_MASTER3PV,		// Dual Magazine ���⿡�� Ext�� źâ�� �ٲ� ���¿��� ���ε�

		AI_LOADMAG_MASTER_E,

		AI_CHANGEDUAL,					// Daul Weapon Change
		AI_CHANGEDUAL_LEFT,
		AI_CHANGEDUAL_RIGHT,
		AI_CHANGEDUAL1PV_LEFT,
		AI_CHANGEDUAL1PV_RIGHT,
		AI_CHANGEDUAL3PV_LEFT, 
		AI_CHANGEDUAL3PV_RIGHT,

		AI_MOVE,
		AI_CHANGE,
		AI_CHANGE1PV,
		AI_CHANGE3PV,
		AI_DAMAGE,
		AI_JUMPEND,

		// Extension animation
		AI_EXTENSION_ACTIVATE,
		AI_EXTENSION_DEACTIVATE,
		AI_IDLE_EXTENSION,
		AI_FIRE_EXTENSION,
		AI_LOADBULLET_EXTENSION,
		
		AI_EXTENSION_ACTIVATE3PV,
		AI_EXTENSION_DEACTIVATE3PV,

		// Folded weapon
		AI_IDLE_FOLDED,
		AI_FIRE_FOLDED,
		AI_IDLE_FOLDED_CHANGE,

		//�ε�淿
		AI_LOADBULLET1PV_MALE,
		AI_LOADBULLET3PV_MALE,
		AI_LOADBULLET1PV_FEMALE,
		AI_LOADBULLET3PV_FEMALE,

		AI_FIREBULLET1PV_MALE,
		AI_FIREBULLET3PV_MALE,
		AI_FIREBULLET1PV_FEMALE,
		AI_FIREBULLET3PV_FEMALE,

		AI_ATTACKREADY_1PV,
		AI_ATTACKREADY_3PV,

		AI_GRENADEATTACKREADY_1PV,
		AI_GRENADEATTACKREADY_3PV,
		
		AI_UI_IDLE_A,
		AI_UI_IDLE_B,

		AI_EXT_IDLE_1PV,
		AI_EXT_IDLE_3PV,

		//UTS��. �¿� ������ �ֱ�(�̷�..)
		AI_LOADBULLET_LEFT_MALE,
		AI_LOADBULLET_LEFT_FEMALE,
		AI_LOADBULLET_RIGHT_MALE,
		AI_LOADBULLET_RIGHT_FEMALE,

		AI_MAX,
	};

	enum ROTATE_ANGLE // KEYWORD : ROTATE_GRENADE
	{
		WALL_X = 0,
		WALL_Y,
		FLOOR_X,
		FLOOR_Y,
	};
};

struct WEAPON_TERRAIN_SOUND : i3::class_storage_pool<WEAPON_TERRAIN_SOUND>
{
	WEAPON_TERRAIN_SOUND(WEAPON::FIRE_ORDER FireOrder, I3_TERRAIN_TYPE TerrainType, 
							REAL32 Delay, const VEC3D& Pos) :
		_nFireOrder(FireOrder), _nTerrainType(TerrainType), 
		_rDelay(Delay), _vPos(Pos) {}

	WEAPON::FIRE_ORDER	_nFireOrder;
	I3_TERRAIN_TYPE		_nTerrainType;
	REAL32				_rDelay;
	VEC3D				_vPos;
};

typedef WEAPON_TERRAIN_SOUND WEAPON_EMPTYBULLET_SOUND;


struct WEAPON_DROPINFO
{
	INT32		_nUserTeam;				// ����� ������ ��
	bool		_bDropping;				// �����(��Ʈ��ũ ó������ �մϴ�)
	bool		_bDroped;				// ����� ����(���⸦ �ֿ�� �ִ� ����)
	bool		_bContact;				// ���� ����� �ø����� ����
	bool		_bSleep;				// ����ǰ� ������ ����
	REAL32		_rSleepTimer;			// ����� �� physix�� ���������� ���� Ÿ�̸�
	VEC3D		_vDropDir;				// ����� ����

	void		Reset( void)
	{
		_nUserTeam	= 0;
		_bDropping	= false;
		_bDroped	= false;
		_bContact	= false;
		_bSleep		= false;
		_rSleepTimer = 0.f;
		i3Vector::Zero( &_vDropDir);
	}

	void  ReturnToReady( void)
	{
		_bDropping = false;
		_bDroped = false;
		_bContact = false;
		_bSleep  = false;
		_rSleepTimer = 0.f;
		i3Vector::Zero( &_vDropDir);
	}
};

struct WEAPON_TERRAIN_EFFECT_INFO
{
	I3_TERRAIN_TYPE	_terrainType;
	VEC3D *			_pPos;
	VEC3D *			_pNormal;
	VEC3D *			_pDir;
	REAL32			_rRotate;
	i3Object *		_pObj;
};