#include "pch.h"
#include "MODScriptSys_Glues.h"

extern "C"
{

//
// Prototype 
//		void addKillScore( TEAM team, INT32 score); 
// 
// Remark
//		������ ���� Kill Count�� ������Ų��. 
// 
// Parameters
//		team		Kill Count�� ������ų Team. 
//		score		���� ����. �Ϲ������� 1�� �־�����. ������ ��� �����ϴ�. 
// 
// Return
//		����.
// 
int MODScript::addScore( lua_State * pState)
{
	return 0;
}

// 
// Prototype
//		INT32 getKillScore( TEAM team); 
// 
// Remark
//		�ش� ���� Kill Count ������ ��ȯ�Ѵ�. 
// 
// Parameters
//		team		Kill Count�� ����� �ϴ� ��. 
// 
// Return 
//		�ش� ���� Kill Count. 
//
int MODScript::getKillScore( lua_State * pState)
{
	return 0;
}

// 
// Prototype
//		void setRoundTimer( INT32 secs) 
//  
// Remark
//		Round Time�� �����Ѵ�. 
// 
// Parameters
//		secs		������ Round Time�� �� ������ ����. 
// 
// Return
//		����. 
//
int MODScript::setRoundTimer( lua_State * pState)
{
	return 0;
}

// 
// Prototype
//		void killRoundTimer( bool ShowUI)
//  
// Remark
//		Round Timer�� �����Ѵ�.
// 
// Parameters
//		ShowUI		Round Time Board�� ǥ���� ������, �Ǵ� ������� �� ������ ����.
//					true�� ��� UI�� ��� ��Ÿ�� ������, false�� ��� �������.
// 
// Return
//		����. 
//
int MODScript::killRoundTimer( lua_State * pState)
{
	return 0;
}

int MODScript::setTimer( lua_State * pState)
{
	return 0;
}

int MODScript::killTimer( lua_State * pState)
{
	return 0;
}

int MODScript::setResapwnDelayTime( lua_State * pState)
{
	return 0;
}


}; // extern "C"