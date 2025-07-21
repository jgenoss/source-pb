#pragma once

enum ITEM_QUERY_STYLE
{
	IQS_USE = 0, IQS_REMOVE,
};

//	�����ۻ�� �˾� Ÿ��
enum USE_ITEM_POPUP_TYPE
{
	USE_ITEM_UNKNOWN = -1,
	USE_ITEM_QUERY = 0,				//	����� ��� ����
	USE_ITEM_COMPLETE,				//	��� �Ϸ�

	USE_ITEM_CHANGE_NICK,			//	�г��� ����
	USE_ITEM_CHANGE_CLAN_NAME,		//	Ŭ���̸� ����
	USE_ITEM_COLOR_NICK,			//	�÷� �г���
	USE_ITEM_COLOR_CLAN_NAME,		//	�÷� Ŭ���̸�
	USE_ITEM_COLOR_CROSSHAIR,		//	�÷� ũ�ν����
	USE_ITEM_DISGUISE_NICK,			//	�ӽ� �г���
	USE_ITEM_DISGUISE_RANK,			//	���� ���
	USE_ITEM_CHANGE_CLAN_MARK,		// Ŭ�� ��ũ ����
	USE_ITEM_COLOR_FIRE_MUZZLE,	// �÷� �ѱ� ȭ�� �����

	USE_ITEM_POPUP_MAX
};

enum ITEM_USE_METHOD
{
	USE_METHOD_UNKNOWN = -1,

	USE_METHOD_DEFAULT_AUTH,		//DefaultAuth,
	USE_METHOD_DISGUISE_NICK,		//DisguiseNick,
	USE_METHOD_DISGUISE_RANK,		//DisguiseRank,
	USE_METHOD_COLOR_NICK,			//ColorNick,
	USE_METHOD_COLOR_CLAN,			//ColorClan,
	USE_METHOD_COLOR_CROSS_HAIR,	//ColorCrossHair,
	USE_METHOD_CHANGE_NICK,			//ChangeNick,
	USE_METHOD_CHANGE_CLAN_NAME,	//ChangeClanName,
	USE_METHOD_CHANGE_CLAN_MARK,	//ChangeClanMark,
	USE_METHOD_BATTLE_FIELD_MOVE,	//BattleFieldMove,
	USE_METHOD_COLOR_FIRE_MUZZLE,	//ColorFireMuzzle

	MAX_USE_METHOD
};

enum ITEM_RESULT_METHOD
{
	RESULT_METHOD_UNKNOWN = -1,

	RESULT_METHOD_COLOR_NICK,		//ColorNick,
	RESULT_METHOD_COLOR_CLAN,		//ColorClan,
	RESULT_METHOD_NICK,				//Nick,
	RESULT_METHOD_RANK,				//Rank,
	RESULT_METHOD_COLOR_CROSS_HAIR,	//ColorCrossHair
	RESULT_METHOD_CLAN_NAME,		//ClanName,
	RESULT_METHOD_CLAN_MARK,		//ClanMark,
	RESULT_METHOD_CLAN_PERSON_PLUS,	//ClanPersonPlus
	RESULT_METHOD_GRAB_BAG,			//GrabBag,
	RESULT_METHOD_POINT,			//Point,
	RESULT_METHOD_COLOR_FIRE_MUZZLE, // ColorFireMuzzle

	MAX_RESULT_METHOD
};

namespace use_item
{
	enum UseType
	{
		ut_None= 0,

		ut_OtherView, //���� ���� ����

		vut_Duplicate,	//�ߺ� ����� �� ����.
	};

	ITEM_USE_METHOD UseMethodStringToEnum( T_ItemID item_id);
	ITEM_RESULT_METHOD ResultMethodStringToEnum( T_ItemID item_id);

}

