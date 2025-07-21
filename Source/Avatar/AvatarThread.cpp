#include "pch.h"
#include "AvatarThread.h"
#include "GameCharaManager.h"

void AVATAR_LOAD_INFO::Done()
{
	Reset();
}

void AVATAR_LOAD_INFO::Flush( void)
{
	if( _pLoadedAvatar != nullptr)
	{	// �ν��Ͻ� ��ȯ���� �������Ѿ� �Ѵ�.
		_pLoadedAvatar->ReturnInstance();
		//I3TRACE( "ref : %d\n", _pLoadedAvatar->GetRefCount());
	}

	Reset();
}

I3_CLASS_INSTANCE( cThreadAvatar);

cThreadAvatar::cThreadAvatar()
{
	m_pTimer = i3Timer::new_object();
}

cThreadAvatar::~cThreadAvatar()
{
	Stop();
	while( m_pAvatar != nullptr && _enableExit() == false)
	{
		i3Timer::Sleep(1);
	}
	I3ASSERT( m_pAvatar == nullptr);
	Destroy();

	I3_SAFE_RELEASE( m_pTimer);
}

bool cThreadAvatar::SetAvatarInfo( AVATAR_LOAD_INFO * pInfo)
{
	I3_REF_CHANGE( m_pAvatar, pInfo->_pLoadedAvatar);
	m_pAvatar = pInfo->_pLoadedAvatar;

	SetAvatarState( ATS_RUN);

	Wakeup();

	return true;
}

void cThreadAvatar::Run( void * pData)
{
	if( m_pAvatar != nullptr)
	{
		UINT32 cnt = 0;
		while( m_pAvatar->OnUpdateAvatarGen_InThread() == false)
		{
			i3Timer::Sleep( 1);
			cnt++;
			if( cnt > 10000)
			{	// 10�ʰ� �ε带 ���ߴٸ�??
				I3PRINTLOG(I3LOG_FATAL,  "Avatar load fail!!!!!");
				break;
			}

			if( _isRun() == false)
			{// �߰��� ������ ����Ǿ� Thread�� �����ϴ� ����̴�.
				break;
			}
			I3ASSERT( m_pAvatar != nullptr);
		}

		m_eState = ATS_WAIT;
		//I3TRACE( "Cleared Avatar %d : %d\n", m_pAvatar->getCharaNetIndex(), m_pAvatar->GetRefCount());
		I3_SAFE_RELEASE( m_pAvatar);
	}

	if( m_eState == ATS_WAIT)
	{
		if( m_pTimer != nullptr)
			m_SleepTime += m_pTimer->GetDeltaSec();
	}
}

