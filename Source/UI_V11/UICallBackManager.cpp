#include "pch.h"

#include "UICallBackManager.h"

namespace UI
{

//�������� �ڵ����� Release �ϱ� ������, �޸� ���� ������ �� �ص� �ȴ�.
//��ϵ� CallBack �Լ��� �ݵ�� Release �ؾ� �Ǵ°� �ƴ�����
//�׷��� ��� �Ŀ� Release �ϸ� ���� �� ����.
CallBackManager::~CallBackManager()
{
	CallBackIntList::iterator int_end = mCallBackIntList.end();
	for(CallBackIntList::iterator it = mCallBackIntList.begin(); it != int_end;	++it)
	{
		I3PRINTLOG(I3LOG_NOTICE, "���� �� �� [ %d ] IntCallBack �̴�.", it->first );	
		delete it->second;
	}
	mCallBackIntList.clear();

	CallBackStringList::iterator str_end = mCallBackStringList.end();
	for(CallBackStringList::iterator it = mCallBackStringList.begin(); it != str_end;	++it)
	{
		I3PRINTLOG(I3LOG_NOTICE, "���� �� �� [ %s ] StringCallBack �̴�.", it->first.c_str() );
		delete it->second;
	}
	mCallBackStringList.clear();
}

void CallBackManager::insert(DWORD key, ICallback* pCallback)
{
	if( mCallBackIntList.find( key ) != mCallBackIntList.end() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "�̹� ��ϵ� [ %d ] CallBack �̴�.", key );
		return;
	}
	mCallBackIntList.insert( CallBackIntList::value_type( key, pCallback ) );
}

void CallBackManager::insert(const std::string & name, ICallback* pCallback)
{
	if( mCallBackStringList.find( name ) != mCallBackStringList.end() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "�̹� ��ϵ� [ %s] CallBack �̴�.", name.c_str() );
		return;
	}
	mCallBackStringList.insert( CallBackStringList::value_type( name, pCallback ) );
}

void CallBackManager::release(DWORD key)
{
	CallBackIntList::iterator it = mCallBackIntList.find( key );
	if( it != mCallBackIntList.end() )
	{
		delete it->second;
		mCallBackIntList.erase( it );
	}
}

void CallBackManager::release(const std::string & name)
{
	CallBackStringList::iterator it = mCallBackStringList.find( name );
	if( it != mCallBackStringList.end() )
	{
		delete it->second;
		mCallBackStringList.erase( it );
	}
}

bool CallBackManager::run(DWORD key, void* arg1, void* arg2)
{
	CallBackIntList::iterator it = mCallBackIntList.find( key );
	if( it == mCallBackIntList.end() )
		return false;

	it->second->run(arg1, arg2);
	return true;
}

bool CallBackManager::run(const std::string & name, void* arg1, void* arg2)
{
	CallBackStringList::iterator it = mCallBackStringList.find( name );
	if( it == mCallBackStringList.end() )
		return false;

	it->second->run(arg1, arg2);
	return true;
}

}