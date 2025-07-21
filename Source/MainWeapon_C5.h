#pragma once

#include "MainWeapon_Throw.h"

//
//  ����ź�� �����غ�������, �������� �ٸ� ���� ���� �ʾƼ� ����ź���� ��ӹޱ� ��ƴ�..
//

class MainWeapon_C5 : public MainWeapon_Throw
{
public:
	MainWeapon_C5(WeaponBase* p);

	void			OnUpdate_C5(REAL32 rDeltaSeconds);
	void			Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed);

	void			NET_Bound_C5( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );		// �ϴ� �񰡻�ó��..WeaponC5�� �����Լ��κ��� ���� ȣ��ȴ�.
	void			Explosion_C5();					// �����Լ��� �ʿ䰡 ���� �񰡻����� ������, C5���̾� �ٿ� ����.
	
	virtual void	ChainExplosion(void) override;
	virtual VEC3D*	GetThrowInfo_Normal(void) override { return &m_vAttachNormal; }

	void			RemoveInvalidBomb(void);

private:
	virtual void	OnInitVariable() override;
	virtual void	OnCreate( bool bCreateInWeapon) override {}
	virtual void	Reset() override;

	REAL32			_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir);
	void			_ProcessBoundObjectForce();

	void			UpdateFlying( REAL32 rDeltaSeconds);
	void			CheckWorldCollision( REAL32 rDeltaSeconds);				// ����źŬ�����鿡���� �����Լ�������, C5������ �񰡻�


	void			_TakeObject( i3Object * pObject);
	void			_TakeObject( I3_PHYSIX_HIT_RESULT * pResult );

	void			_CollisionTestForAttach( void);
	void			_ProcessOnTakeObject( void);

	void			UpdateNetworkData( REAL32 rDeltatime);					// ������ �ʿ䰡 ���� virtual�� ������..

	// ������ ���� ��� �ش� ���⸦ ������� �ϴ� ��� ȣ��
	bool			_CheckProcessUserLeave( void);

	REAL32			m_rThrowLen;					// �Ÿ��� ���� ���� �ٴ� ����� ����
	VEC3D			m_vAttachNormal;				// ���� ���� ���
	i3Transform *	m_pAttachObjectTransform;		// ������Ʈ�� �ٴ� ����� transform
	i3Object *		m_pAttachObject;
	MATRIX			m_AttachLocalMatrix;

	REAL32			m_rElapsedExplosionTime;		// �ݹ��̵ǰ� ���� ���� �ð�
	REAL32			m_rExplosionDelay;
	REAL32			m_rExplosionDelayTimer;
	REAL32			m_rRotateY;

	bool			m_bFirstBound;

	REAL32			m_rFloorCheckTime;

	bool			m_bCheckAttachFloor;
	bool			m_bAttchedObject;

	VEC3D			m_vObjectForce;
	VEC3D			m_vObjectForceNormal;
	REAL32			m_rBoundObjectLen;
	i3Object *		m_pBoundObject;
};

