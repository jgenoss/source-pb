/**
 * \file	GameCharaContextBase.h
 *
 * Declares the game chara context base class.
 */


#if !defined( __GAME_CHARA_CONTROL_CONTEXT_H__)
#define __GAME_CHARA_CONTROL_CONTEXT_H__

class CGameCharaBase;
class CGameCharaWeaponContext;
class CGameCharaActionContext;
class CGameCharaAnimContext;
class CGameCharaBoneContext;
class CGameCharaUpdateContext;
class CGameCharaSoundContext;
class CGameCharaInputContext;
class CGameCharaMaterialContext;
class CGameCharaCameraContext;
class CGameCharaMoveContext;
class CGameCharaCollisionContext;
class CGameCharaEquipContext;
class CGameCharaHUDContext;

class CGameCharaContextBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameCharaContextBase, i3ElementBase );
protected:
	VAR_LR(CGameCharaBase*,233)	m_pOwner = nullptr;


protected:
	CGameCharaWeaponContext *	getWeaponContext(void);
	CGameCharaActionContext *	getActionContext(void);
	CGameCharaAnimContext *		getAnimContext(void);
	CGameCharaBoneContext *		getBoneContext( void);
	CGameCharaUpdateContext *	getUpdateContext( void);
	CGameCharaSoundContext *	getSoundContext( void);
	CGameCharaInputContext *	getInputContext( void);
	CGameCharaMaterialContext *	getMaterialContext( void);
	CGameCharaCameraContext *	getCameraContext( void);
	CGameCharaMoveContext *		getMoveContext( void);
	CGameCharaCollisionContext * getCollisionContext( void);
	CGameCharaEquipContext *	getEquipContext( void);
	CGameCharaHUDContext *		getHUDContext( void);

	WeaponBase *				getCurrentWeapon( void);
	WeaponBase *				getUsedWeapon( void)	{ if( getCurrentWeapon() == nullptr) return nullptr; return getCurrentWeapon()->getUsedWeapon(); }

public:
	virtual ~CGameCharaContextBase();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0);

	virtual void	OnDestroy( void)		{}

	virtual void	OnReset( void);

	/** \brief 1��Ī ĳ���ͷ� ���� */
	virtual void	OnChange1PV( void) {}

	/** \brief 3��Ī ĳ���ͷ� ���� */
	virtual void	OnChange3PV( void) {}

	/** \brief Local ĳ���ͷ� ���� */
	virtual void	OnChangeLocal( void) {}

	/** \brief Network ĳ���ͷ� ���� */
	virtual void	OnChangeNetwork( void) {}

	/** \brief NPC ĳ���ͷ� ���� */
	virtual void	OnChangeNPC( void) {}

	/** \brief AI ĳ���ͷ� ���� */
	virtual void	OnChangeAI( void){}

	/** \brief Profile ĳ���ͷ� ���� */
	virtual void	OnChangeProfile( void) {}

	virtual void	OnChangeFullbody( void) {}

	/** \brief Bone ������ ����Ǹ� ȣ��ȴ�. */
	virtual void	OnChangeBoneHierarchy( i3Body * pBody) {}

	/** \brief ���� ĳ���ͷ� ����
		\param[in] bTRex TRex ���� */
	//virtual void	OnChangeDino( bool bTRex) {}
	virtual void	OnChangeTrex() {}
	virtual void	OnChangeSting() {}
	virtual void	OnChangeRaptor() {}

	/** Update ���� ���� ȣ��˴ϴ�. */
	virtual void	OnFirstUpdate( REAL32 rDeltatime) {}
	virtual void	OnUpdate( REAL32 rDeltatime) {}
	virtual void	OnLastUpdate( REAL32 rDeltatime) {}
};

#endif
