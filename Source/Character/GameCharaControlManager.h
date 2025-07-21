#if !defined( __GAME_CHARA_CONTROL_MANAGER_H__)
#define __GAME_CHARA_CONTROL_MANAGER_H__

#include "GameCharaControl.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//		Control Manager
// desc : ���Ŀ� CharaBase�� �ű�ϴ�. ������������ �� Ŭ������ ����մϴ�.

class CGameCharaBase;

class CGameCharaControlManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameCharaControlManager);
protected:
	INT32		m_n1PV;				// 0:3PV, 1:1PV
	INT32		m_nLocal;			// 0:network, 1:local

	CGameCharaBase *				m_pOwner;
		
	
public:


public:
	CGameCharaControlManager();
	virtual ~CGameCharaControlManager();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0);
	virtual void	OnDestroy();

	// change view
	virtual void	OnChange1PV();
	virtual void	OnChange3PV();

	virtual void	OnChangeLocal();
	virtual void	OnChangeNetwork();
	
	virtual void	OnUpdate( REAL32 rDeltatime);
};

#endif
