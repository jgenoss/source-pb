#pragma once

#include "MainWeapon_Throw.h"

#define NET_GRENADE_OFFTIME		15.f				// ��Ʈ��ũ�� 15�ʰ� �� ������ ����

class MainWeapon_Grenade : public MainWeapon_Throw
{
public:
	MainWeapon_Grenade(WeaponBase* p); 
	
	virtual void	OnInitVariable() override;
	virtual void	OnCreate( bool bCreateInWeapon) override;
	virtual void	Reset() override;

	virtual void	OnEndFire(i3AIContext * pCtx, REAL32 tm) override;

	virtual void	ChainExplosion(void);
	virtual void	OnUpdate_Grenade( REAL32 rDeltaSeconds);
	virtual void	SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo);

//// �ӽ÷� �񰡻����� �ۼ�..	

	void	NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );

////
	//  �ϴ� �ӽ÷� �ۼ�...���� �Լ��� ���°� ���� �ʾƺ���..
	virtual void	CheckWorldCollision( REAL32 rDeltaSeconds);
	virtual void	UpdateNetworkData( REAL32 rDeltatime);
	virtual void	UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange);

	virtual void	Explosion();

	bool	SuccessiveStateFlagCheck(INT32 nState);

	void			_ProcessTakeObject();		// �ϴ� public����..(�Űܾ���)
	REAL32			_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir);
	void			_ProcessBoundObjectForce();

	void			CheckBullet();

/// �ӽ� �Լ�..
	REAL32			GetElapsedExplosionTime() const { return m_rElapsedExplosionTime; }
	void			SetElapsedExplosionTime(REAL32 fTime) { m_rElapsedExplosionTime = fTime; }

	void			Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin, INT32 nNetSlotIdx);
	
	virtual bool	isPlayRadioSound(void)	{ return true; }
	virtual bool	isUseTraceEffect(void)	{ return true; }

protected:

	// ������ ���� ��� �ش� ���⸦ ������� �ϴ� ��� ȣ��
	bool			_CheckProcessUserLeave( void);
	virtual void	_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0);

	virtual WeaponGrenade *	_CreateThrowWepaon(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx);
	virtual void			_SetThrowData(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed);

	virtual void			_TakeObject(void);
	virtual void			_UpdateTakeObjectTransform(void);

private:
	virtual	bool	_CheckThrow(REAL32 LocalTime);		

protected:
	REAL32			m_rElapsedExplosionTime;		// �ٴڿ� �ε����� ����������� ��� �ð�

	UINT32			m_uCount;
	VEC3D			m_vObjectForce;	// �����̴� ������Ʈ�� ���� ���� ���� ���


	bool			m_bCheckFloor;	// �����ϰ� �ٴ��� üũȮ��
	i3Transform *	m_pAttachObjectTransform;
	i3Object *		m_pAttachObject;
	
	REAL32			m_rBoundObjectLen;
	INT32			m_nFireEffectCount;

	INT32			m_nStateFlag;
	typedef i3::vector<INT32> vStateFlag;
	vStateFlag		m_vGrenadeStateFlag;

	bool			m_bResetBullet;
};

