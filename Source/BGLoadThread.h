#if !defined( __BG_LOAD_THREAD_H__)
#define __BG_LOAD_THREAD_H__

#include "GameThread.h"

class BGLoadThread : public CGameThread
{
	I3_CLASS_DEFINE( BGLoadThread, CGameThread);
private:
	enum LOADTHREAD_STATE
	{
		LT_NONE,
		LT_LOADING,
		LT_LOADED
	};

	struct	sLoadQue {
		LOADTHREAD_STATE		m_State = LT_NONE;		// �ε� Ŀ��带 �־����� Ȯ��
		char					m_szPath[MAX_PATH] = { 0 };
		INT32					m_IOIdx = 0;
		i3GameRes *				m_pLoadedRes = nullptr;

		i3Animation *		m_pAnim = nullptr;		// Animation BGLoad
		i3::vector<i3ElementBase*> _OwnList;	// animation�� �����ϰ� �ִ� ������
	};

	sLoadQue		m_Que[2];		// 2���� ���� ���۸� �������

	INT32			m_writeIdx = 0;

public:
	BGLoadThread();

	virtual void	Run( void * pData) override;

	void			SetLoadCmd( const char * pszPath);

	bool			IsFree( void);
	
	i3GameRes *		GetRes( void);
};

#endif
