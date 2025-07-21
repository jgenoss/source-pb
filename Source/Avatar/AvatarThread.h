#if !defined( __AVATAR_THREAD_H__)
#define __AVATAR_THREAD_H__

#include "Avatar.h"
#include "GameThread.h"

class cThreadAvatar;

enum eAVATAR_STATE
{
	E_AVATAR_STATE_NA = 0,			// �ƹ��� ���µ� �ƴϴ�.
	E_AVATAR_STATE_COMMAND,			// �ε� ����� ���� ����
	E_AVATAR_STATE_LOADING,			// �ε� ���̴�.
	E_AVATAR_STATE_ERROR,			// �ε� ����
	E_AVATAR_STATE_LOADED,			// �ε� �Ϸ�
	E_AVATAR_STATE_FLUSH,			// �ε��������� ���� �ʴ´� ������������
};

enum AVATAR_THREAD_STATE
{
	ATS_WAIT,
	ATS_RUN,
	ATS_LOST,	// Lost Device ���� �ε��� �����.
	ATS_CLEAR,	// �ܺο��� �����ϴ� ���
	ATS_FLUSH,	// �߰��� ���������� ��� (ReturnInstance�� �ؾ��Ѵ�)
};

struct AVATAR_LOAD_INFO
{
	INT32						_netIndex = -1;
	GLOBAL_CHARA_INFO 			_CharaInfo;		// �ε���
	GLOBAL_CHARA_INFO *			_pCharaInfo = nullptr;	// �ܺ� CharaInfo
	Avatar*						_pLoadedAvatar = nullptr;
	cThreadAvatar *				_pThread = nullptr;
	eAVATAR_STATE				_state = E_AVATAR_STATE_NA;
	i3Stage *					_pStage = nullptr;		//Avatar�� InstanceManager�� ������ ���θ� �Ǵ��ϱ� ����. InstanceManager�� �����Ǿ��ٸ� �ƹ�Ÿ�� ������ �ٷ� �������Ѿ��Ѵ�.

	void	Reset()
	{
		_state = E_AVATAR_STATE_NA;
		_netIndex = -1;
		_CharaInfo.Reset();
		_pCharaInfo = nullptr;
		_pThread = nullptr;
		_pLoadedAvatar = nullptr;
		_pStage = nullptr;
	}

	void	Done( void);
	void	Flush( void);
};

class cThreadAvatar : public CGameThread
{
	I3_CLASS_DEFINE( cThreadAvatar, CGameThread);

public:
	cThreadAvatar();
	virtual ~cThreadAvatar();

	virtual void	Run( void *pData) override;

	bool			SetAvatarInfo( AVATAR_LOAD_INFO * pInfo);

	Avatar*			GetAvatar()									{ return m_pAvatar; }

	void			SetAvatarState(AVATAR_THREAD_STATE eState)	{ m_eState = eState; }
	AVATAR_THREAD_STATE	GetAvatarState()						{ return m_eState; }

	REAL32			GetSleepTime( void)							{ return m_SleepTime; }

private:
	Avatar*			m_pAvatar = nullptr;
	AVATAR_THREAD_STATE	m_eState = ATS_WAIT;

	i3Timer *		m_pTimer;			// [initialize at constructor]
	REAL32			m_SleepTime = 0.0f;	// ���ڰ� �ִ� �ð�
};

#endif
