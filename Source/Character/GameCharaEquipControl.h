/**
 * \file	Character\GameCharaEquipControl.h
 *
 * Declares the game chara equip control class.
 */


#if !defined( __GAME_CHARA_EQUIP_CONTROL_H__ )
#define __GAME_CHARA_EQUIP_CONTROL_H__

#include "GameCharaControlBase.h"
#include "GameCharaEquipDef.h"

class CGameCharaEquip;

class CGameCharaEquipControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaEquipControl, CGameCharaControlBase );
protected:
	CGameCharaEquip *	getEquip( EQUIP::ePART type)							{ return getEquipContext()->getEquip( type); }
	void			setEquip( EQUIP::ePART type, CGameCharaEquip * pEquip)	{ getEquipContext()->setEquip( type, pEquip); }

public:

	/** \brief ��� �����մϴ�. */
	virtual bool	OnCreateEquip( CHARA_RES_ID CharaType, T_ItemID ItemID, TEAM_TYPE team, INT32 res_subtype);

	/** \brief ��� �����մϴ�. */
	virtual bool	OnDeleteEquip( EQUIP::ePART type);

	/** \brief ��� ĳ���Ϳ� ���Դϴ�. */
	virtual bool	OnAttach( CGameCharaEquip * pEquip);

	/** \brief ��� ĳ���Ϳ� ���Դϴ�. */
	virtual bool	OnAttach( EQUIP::ePART type, bool bReset);

	/** \brief ��� �����ϴ�. */
	virtual CGameCharaEquip *	OnDetach( EQUIP::ePART type);

	/** \brief ���� ��� �����ϰ� �� ���� �����մϴ� 
		\note lobby���� ����մϴ�. */
	virtual bool	OnSet( CGameCharaEquip * pEquip);

	/** \brief ��� �����ϴ�. */
	virtual CGameCharaEquip * OnDropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos);
	
};

#endif
