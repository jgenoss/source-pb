/**
 * \file	GameCharaEquipContext.h
 *
 * Declares the game chara equip context class.
 */


#if !defined( __GAME_CHARA_EQUIP_CONTEXT_H__)
#define __GAME_CHARA_EQUIP_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaEquipDef.h"
#include "GameCharaDef.h"

class CGameCharaEquipControl;
class CGameCharaEquipControl1PV;
class CGameCharaEquipControl3PV;

class CGameCharaBase;
class CGameCharaEquip;
class CEquipInfo;

class CGameCharaEquipContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaEquipContext, CGameCharaContextBase);

protected:
	CGameCharaEquipControl *		m_pEquipCurrentControl = nullptr;
	CGameCharaEquipControl1PV *		m_pEquipControl1PV = nullptr;
	CGameCharaEquipControl3PV *		m_pEquipControl3PV = nullptr;
	
	//	���ǰ
	CGameCharaEquip	*	m_pCharaEquip[ EQUIP::ePART_COUNT];		// [initialize at constructor]
	CEquipInfo *		m_pEquipInfo[ EQUIP::ePART_COUNT];		// [initialize at constructor]

	// �Ϻ� ���� BoneRef�� ���δ�.
	i3BoneRef *			m_pDummy[ EQUIP::ePART_COUNT];			// [initialize at constructor]

	// ��Ʈ�� ����� ��� üũ (��� �����ؼǿ��� �Ǻ��ϱ� ���� �뵵)
	bool				m_bHelmetProtect = false;

	// ��ǥ���� �۸� ����
	bool				m_bEnableTargetChaseHelmet = false;
	REAL32				m_rTargetChaseC5 = 0.0f;


public:
	/** \brief ��� ��ȯ�մϴ�. */
	CGameCharaEquip *	getEquip( EQUIP::ePART part)	{ return m_pCharaEquip[ part]; }

	/** \brief ��� �����մϴ�. */
	void				setEquip( EQUIP::ePART part, CGameCharaEquip * pEquip)	{ m_pCharaEquip[ part] = pEquip; }

	/** \brief ����� ������ ��ȯ�մϴ�.
		\note ������ ���� ������ �ٸ� �� �ֽ��ϴ�.
			�ƹ�Ÿ �ý��ۿ��� ���Ƿ� �𵨸��� �ٲ��� ���� �ֽ��ϴ�. ������ ������ ������ ������ ����� ���� �ֽ��ϴ�.*/
	const CEquipInfo *	getEquipInfo( EQUIP::ePART part)		{ return m_pEquipInfo[ part]; }

	/** \brief ����� ������ �����մϴ�. */
	void				setEquipInfo( EQUIP::ePART part, CEquipInfo *pInfo)	
	{ 
		I3ASSERT_RETURN(part != EQUIP::ePART_UNKNOWN);
		m_pEquipInfo[ part] = pInfo; 
	}

	i3BoneRef *			getDummy(EQUIP::ePART part) const { I3ASSERT_RETURN(part >= 0, nullptr);  return m_pDummy[part]; }
	void				setDummy( EQUIP::ePART part, i3BoneRef * pNode)	{ I3_REF_CHANGE( m_pDummy[ part], pNode); }
	

	/** \brief ���� ĳ���Ϳ� �پ� �ִ��� ���� */
	bool				isAttachOnChara( EQUIP::ePART part);

	/** \brief �Ӹ� ��ȣ ���θ� ��ȯ�մϴ�. */
	bool				getHelmetProtect( void)						{ return m_bHelmetProtect; }

	/** \brief �Ӹ� ��ȣ ���θ� �����մϴ�. */
	void				setHelmetProtect( bool bVal)				{ m_bHelmetProtect = bVal; }

	/** \brief ��ǥ ���� ��� ��� ���θ� ��ȯ�մϴ�. */
	REAL32				getTargetChaseC5( void)						{ return m_rTargetChaseC5; }
	void				setTargetChaseC5( REAL32 range)				{ m_rTargetChaseC5 = range; }

	/** \brief ��� ���� �ɷ�ġ�� ��ȯ�մϴ�. */
	REAL32				GetAbility( EQUIP::eABILITY ability);

	/** \brief �Ӹ� ��ȣ���� ��ȯ�մϴ�. */
	REAL32				GetHeadDefenseRate();

	/** \brief �Ӹ� ��ȣ �ð��� ��ȯ�մϴ�. */
	REAL32				GetHeadProtectionTime();


public:
	CGameCharaEquipContext();
	virtual ~CGameCharaEquipContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV( void) override;
	virtual void	OnChange3PV( void) override;

	virtual void	OnChangeLocal( void) override;
	virtual void	OnChangeNetwork( void) override;
	virtual void	OnChangeNPC( void) override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief ��� ���忡 ������.
		\param[in] type ��� Ÿ��
		\param[in] pVecDir �������� ���� */
	void			DropToWorld( EQUIP::ePART type, VEC3D * pVecDir);

	/** \brief ����� ������ ���� */
	void			SetMaterialColor( EQUIP::ePART part, COLORREAL * pDiff, COLORREAL * pSpec, bool bOnChara);

	/** \brief ����� ���İ��� �����Ѵ�.
		\param[in] type ��� Ÿ��
		\param[in] fAlpha alpha value
		\param[in] bOnChara ĳ���Ϳ� ���� ���� */
	void			SetBlendAlpha( EQUIP::ePART type, REAL32 fAlpha, bool bOnChara);

	/** \brief ��� �����ϰ� �޸𸮿��� �����մϴ�.
		\param[in] type ��� Ÿ��*/
	void			ReleaseEquip( EQUIP::ePART type);

	/** \brief ����� �����մϴ�.
		\param[in] charaResType Resource type
		\param[in] EquipID Resource id*/
	void			CreateEquip( CHARA_RES_ID charaResType, T_ItemID ItemID, INT32 res_subtype);

	/** \brief ��� �����ϴ�.
		\param[in] type ��� Ŭ���� Ÿ��
		\param[in] pSceneNode ���� �ٿ��� scene  node
		\param[in] pGameNode ���� �ٿ��� game node
		\param[in] pVecPos ��� ��ġ*/
	bool			DropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos);

	/** \brief ��� ĳ���Ϳ� ���Դϴ�.
		\param[in] type ��� Ŭ���� Ÿ��
		\param[in] bReset ��� �ʱ�ȭ ���� */
	bool			Attach( EQUIP::ePART type, bool bReset = true);

	/** \brief ��� ��� ���ϴ�.
		\note ���峪 ĳ���Ϳ��� ���� �˴ϴ�.
		\param[in] type ��� Ŭ���� Ÿ�� */
	bool			Detach( EQUIP::ePART type);

	void			CreateEquipmentDummy( void);

private:
	void			__SetDummyBoneRef(const char* szBoneName, i3BoneRef* destBoneRef);
};

#endif
