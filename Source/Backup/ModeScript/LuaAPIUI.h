#pragma once

#include "LuaAPIDef.h"

extern CSampleClass g_kSample; 

namespace LuaAPIUI
{
	void	Init(lua_State* L);

	//
	// Called : Client
	// 
	LVOID	hideKillScoreUI(void);			// Kill Count Score board�� ������� �Ѵ�.
	LVOID	hideTeamScoreUI(void);			// Team Score Board�� ������� �Ѵ�.
	LVOID	mapHighlight( PVEC3D pos );		// �ش� ��ġ�� ���� �󿡼� Highlight ��Ų��.
	LVOID	msgChat( const char* msg );		// ä�� â�� System Message�� ����ϴ� �Լ�.
	LVOID	msgHelp( const char* msg );		// ������ ������ �����ִ� �޽����� ���.�� �Լ��� ��µǴ� �޽����� ��ġ�� ���� ���̳�, �ƴϳĿ� ���� �ٸ� �� �ִ�. ������ ���� ���� �߿� �� �Լ��� �޽����� ȭ�� �ߴܿ� ǥ���Ѵ�. �׷��� ��Ÿ ȭ�鿡���� ������ ������ ���õ� UI ������ ǥ�õ� �� �ִ�.
	LVOID	msgPopup( const char* msg );	// Popup Message â�� ����. User�� OK ��ư�� Ŭ���ؾ� �ϴ� UI.
	LVOID	showKillScoreUI(void);			// ȭ�� �ֻ���� Kill Count Score�� ���̵��� �����Ѵ�. �� UI�� �׻� Kill Count�θ� ���Ǹ�, Kill Counting�� Script�� �ƴ�, Game Engine���� �ڵ����� ���ŵȴ�.
	LVOID	showTeamScoreUI(void);			// Team Score Board�� ��Ÿ���� �Ѵ�. Team Score�� finishRound �Լ��� ���� �¸��� ȸ���� �ڵ����� �����ϰ� ǥ���ȴ�.
};
