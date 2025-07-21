/**
 * \file	Character\GameCharaWeaponControl.h
 *
 * Declares the game chara weapon control class.
 */


#if !defined( __GAME_CHARA_WEAPON_CONTROL_H__ )
#define __GAME_CHARA_WEAPON_CONTROL_H__

#include "GameCharaControlBase.h"
#include "GameCharaWeaponControlDef.h"

#define WEAPON_CHECKTAKING_TIME		0.25f	// ��� ���� �˻� (�ʴ� 4ȸ �˻�)

class CGameCharaWeaponManager;

class CGameCharaWeaponControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl, CGameCharaControlBase);

protected:
	inline WEAPON_SLOT_TYPE	getCurrentSlotNumber( void)			{ return getWeaponContext()->getCurrentSlotNum(); }
	inline void		setCurrentSlotNumber( WEAPON_SLOT_TYPE nSlot)	{ getWeaponContext()->setCurrentSlotNum( nSlot); }
	inline WEAPON_SLOT_TYPE	getOldSlotNumber( void)				{ return getWeaponContext()->getOldSlotNum(); }
	inline void		setOldSlotNumber( WEAPON_SLOT_TYPE nSlot)	{ getWeaponContext()->setOldSlotNum( nSlot); }
	inline WEAPON_SLOT_TYPE	getSwapSlotNumber( void)			{ return getWeaponContext()->getSwapSlotNum(); }
	inline void		setSwapSlotNumber( WEAPON_SLOT_TYPE nSlot)	{ getWeaponContext()->setSwapSlotNum( nSlot); }

	inline WeaponBase *	getWeapon( WEAPON_SLOT_TYPE nSlot)
	{
		I3_BOUNDCHK( nSlot, WEAPON_SLOT_COUNT);
		return getWeaponContext()->getWeapon( nSlot);
	}

	inline void		setWeapon( WEAPON_SLOT_TYPE nSlot, WeaponBase * pWeapon)
	{
		I3_BOUNDCHK_RETURN( nSlot, WEAPON_SLOT_COUNT);
		getWeaponContext()->setWeapon( nSlot, pWeapon);
	}

	inline WeaponBase *	getCurrentWeapon( void)			{ return getWeaponContext()->getCurrent(); }

	inline i3Node *	getWeaponAttachRoot( void)			{ return getWeaponContext()->getAttachRoot(); }

	inline WeaponBase *	getPurchasePrimWeapon( void)	{ return getWeaponContext()->getBackpack(); }
	inline void		setPurchasePrimWeapon( WeaponBase * pWeapon)	{ getWeaponContext()->setBackpack( pWeapon); }
	inline i3Node *	getDummyPrimPurchase( void)		{ return getWeaponContext()->getDummyPrimPurchase(); }


protected:
	bool			_ProcessDropedWeaponSearching( void);
	bool			_ProcessFireForNetwork( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER fireOrder = WEAPON::PRIMARY_FIRE);

	/** \brief ���⸦ ������ ó���ϴ� �Լ�
		\note ���� ��ü�� �����մϴ�. */
	void			_AfterProcessDropWeapon( WeaponBase * pWeapon, bool bDirectPickup, bool bChangeNextWeapon = true);

	/** \brief ������� �ʴ� ����� �����Ѵ�.
		\note ���� Physic shape�� �������� �ű�� �۾��� �Ѵ�. */
	void			_NoUseWeapon( WeaponBase * pWeapon);

public:

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	/*virtual*/ void	OnUpdate( REAL32 rDeltatime);

	// ���� ����(WeaponManager���� �޾ƿɴϴ�.) - nMultiWeaponSync ���� ���⿡���� ��� �ʿ� ������ Default ��
	virtual bool	OnCreateWeapon( WEAPON_SLOT_TYPE nSlot, T_ItemID Itemid, UINT8 nMultiWeaponSync = 0);
	// ���� ����(WeaponManager�� ��ȯ�մϴ�.)
	/*virtual*/ void	OnDeleteWeapon( WEAPON_SLOT_TYPE nSlot);

	void			OnDeleteMultiWeapon( WEAPON_SLOT_TYPE nSlot);

	/** \brief ���⸦ ĳ���Ϳ� ���Դϴ�.
		\return ���� ���� pointer */
	virtual WeaponBase * OnAttachWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode);
	// ���⸦ ĳ���Ϳ��� ������ϴ�.

	/*virtual*/ WeaponBase * OnAttachBackWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode);
	// ���⸦ ĳ���Ϳ��� ������ϴ�.
	virtual WeaponBase * OnDetachWeapon( WEAPON_SLOT_TYPE nSlot);

	// ���� �ʱ�ȭ
	/*virtual*/ void	OnResetWeapon( void);

	/** \brief ������� ���濡 ���� ĳ���� �⺻ �ڼ��� ���� */
	/*virtual*/ void	OnAttackIdleWeapon( void);

	// � �ֹ��⸦ ���Դϴ�.
	virtual bool	OnAttachPurchasePrimWeapon( WEAPON_SLOT_TYPE nSlot);
	// � ���� �ֹ��⸦ ������ϴ�.
	virtual void	OnDetachPurchasePrimWeapon( void);
	// �������� �������⸦ ���Դϴ�.
	virtual bool	OnAttachPurchaseSideWeapon( WEAPON_SLOT_TYPE nSlot);
	// �������� ���� �������⸦ ������ϴ�.
	virtual void	OnDetachPurchaseSideWeapon( void);

	/** \brief �ش� ���� ����� ��ü
		\note ���� �����ӿ����� ��ü���� �ʰ� ���� �����ӿ� ��ü�˴ϴ�.
		\return ��ü ����*/
	/*virtual*/ bool	OnPreSelectWeapon( WEAPON_SLOT_TYPE nSlot);

	// �ش� ���� ����� ��ü(���� �����ӿ��� ��ü�˴ϴ�.)
	// return true �ش� ����� ��ü ����
	virtual bool	OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound = true);

	// 1/3��Ī�� ���� ���� ���μ��� ó��
	virtual bool	OnChangeViewWeapon( void)		{ return false; }

	/** \brief ���⸦ ���忡 �����ϴ�. */
	/*virtual*/ bool	OnWeaponDropToWorld( WEAPON_SLOT_TYPE nSlot, VEC3D * pPos, bool bChangeNextWeapon = true, bool bDirectPickup = false);

	/** \brief ���⸦ �ݴ´�. */
	/*virtual*/ bool	OnWeaponTake( WeaponBase * pWeapon, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList);

	/** \brief ���忡 ������ ���⸦ ã���ϴ�. */
	/*virtual*/ bool	OnDropedWeaponSearching( REAL32 rDeltatime);

	/** \brief ���� �߻縦 ó���մϴ�. */
	/*virtual*/ bool	OnFire( VEC3D * pStartPos = nullptr, VEC3D * pTargetPos = nullptr);

	/** \brief ���� �߻縦 ó���մϴ�. */
	/*virtual*/ bool	OnSecFire( VEC3D * pStartPos = nullptr, VEC3D * pTargetPos = nullptr);

	/** \brief ���� �غ� ó���մϴ�. */
	/*virtual*/ bool	OnFireReady( void);

	/** \brief ���� ������ ó���մϴ� .*/
	virtual bool	OnZoom( void)					{ return false; }

	/** \brief Extension Ȱ��ȭ�� �����մϴ�. */
	virtual bool	OnExtActivate( void);

	/** \brief Extension ��Ȱ��ȭ�� �����մϴ�. */
	virtual bool	OnExtDeactivate( void);

	/** \brief ������ �����մϴ�. */
	virtual bool	OnLoadMagazine( void);

	/** \brief Dual źâ�� ���ε带 �����մϴ�. */
	/*virtual*/ bool	OnLoadMagazineDual( void);

	/** \brief ��ǿ� ź���� �����ϵ��� �����մϴ�. */
	/*virtual*/ bool	OnLoadBullet( void);

	/** \brief ��ǿ��� Ź���� �����մϴ�.
		\note 1��Ī Animation�� �ֽ��ϴ�. */
	virtual bool	OnRemoveShell( void)			{ return false;}

	/** \brief ĳ���� �������� ���� ȣ��˴ϴ�.
		\note �������� 0�� ����� ��ü�ϴ� �����Ѵ�. */
	virtual bool	OnProcessRespawn( void);

	/** \brief ���� �غ� �����մϴ�. */
	virtual bool	OnLoadMagazine_Ready( void);

	bool			SetBlendAlpha( WEAPON_SLOT_TYPE nSlot, REAL32 fVal);
	void			SetBlendAlphaAll( REAL32 fVal);	
	bool			SetMaterialColor( WEAPON_SLOT_TYPE nSlot, COLORREAL * pfDiffuse, COLORREAL * pfSpecular);
	void			SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular);

	void			NoUseWeapon(WeaponBase* pWeapon);

};

#endif
