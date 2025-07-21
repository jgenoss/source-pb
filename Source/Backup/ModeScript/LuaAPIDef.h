#pragma once

#include <string.h>

class CSampleClass
{
public:
	CSampleClass() {
		strcpy_s(m_szName, 32, "CSampleClass");
		m_nHP = 120;
	}
	const char* GetName() { return m_szName; }
	unsigned int GetHP() { return m_nHP; }
protected:
	char m_szName[32];
	unsigned int m_nHP;
};

extern CSampleClass g_kSample;

typedef CSampleClass* HOBJECT;
typedef CSampleClass* HPLAYER;
typedef CSampleClass* HWEAPON;

typedef UINT32 WEAPON_ID;
typedef UINT32 CLAN_ID;
typedef VEC3D* PVEC3D;
typedef INT32 RANK;

typedef INT32 LVOID;
#define LNULL 0;

typedef INT32 TIMER;

#include "../../CommonSource/CommonDef.h"

enum TEAM{
	RED,	// RED ���� �ǹ��ϴ� ���.
	BLUE,	// BLUE ���� �ǹ��ϴ� ���.
	BOTH,	// �� �� ��θ� �ǹ��ϴ� ���.
};

enum ROLE {
	HUMAN,	// �Ϲ����� ������. �⺻ ROLE�̴�.
	BOSS,	// ���� ������ �ϴ� ROLE.
	MONSTER,// �Ϲ� ���� ������ �ϴ� ROLE.
};
