#pragma once

#include "MainWeapon_Throw.h"

class MainWeapon_ThrowKnife : public MainWeapon_Throw
{
public:
	MainWeapon_ThrowKnife(WeaponBase* p); 

	VEC3D *			getDirection( void)	 { return &m_ThrowData.m_vDirection; }
	VEC3D *			getRightVec( void)  { return &m_vRightDir; }

	void			ThrowArrow( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin, INT32 nNetSlotIdx, WeaponThrowKnife * pNewWeapon);
	void			Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx );

	void			OnUpdate_ThrowKnife( REAL32 rDeltaSeconds);

	REAL32			_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir);
	void			_ProcessBoundObjectForce();
	
private:

	virtual void	Reset() override;

	void			_ProcessHitWorld( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine* pLine);
	bool			SuccessiveStateFlagCheck(INT32 nState);

	void			ReturnInstance();


	void			UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange);
	void			CheckWorldCollision( REAL32 rDeltaSeconds);

	void			UpdateNetworkData( REAL32 rDeltatime);		// ������ �ʿ䰡 ���� virtual�� ������..

	bool			_CheckProcessUserLeave( void);

	// [Ż�� ���] �Ķ�����, ������̱� �߻� �� �Ⱥ��̴� ������ �ذ��ϱ� ���� �߰� // 2014-06-13 ENG_��뿵
	void			NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );

	VEC3D			m_vRightDir;

	bool			m_bSettedChecker;
	i3Transform *	m_pAttachObjectTransform;
	i3Object *		m_pAttachObject;
	REAL32			m_rBoundObjectLen;

	INT32			m_nStateFlag;
	typedef i3::vector<INT32> vStateFlag;
	vStateFlag		m_vThrowKnifeStateFlag;

	VEC3D			m_vObjectForce;	// �����̴� ������Ʈ�� ���� ���� ���� ���
};
