/**
 * \file	GameCharaWeaponContext.h
 *
 * Declares the game chara weapon context class.
 */


#if !defined( __GAME_CHARA_WEAPON_CONTEXT_H__)
#define __GAME_CHARA_WEAPON_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaWeaponControlDef.h"


class CGameCharaWeaponControl;
class CGameCharaWeaponControl1PV;
class CGameCharaWeaponControl3PV;

class CGameCharaWeaponContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaWeaponContext, CGameCharaContextBase);

	friend class CGameCharaBase;
	friend class CGameCharaMaterialContext;

	friend class CGameCharaWeaponControl;
	friend class CGameCharaWeaponControl1PV;
	

protected:
	// WeaponController
	VAR_LR(CGameCharaWeaponControl*,66)	m_pCurrentWeaponControl = nullptr;
	VAR_LR(CGameCharaWeaponControl1PV*,67)	m_p1PVWeaponControl = nullptr;
	VAR_LR(CGameCharaWeaponControl3PV*,68)	m_p3PVWeaponControl = nullptr;

	VAR_LR(WeaponBase*, 69)	m_pWeapon[WEAPON_SLOT_COUNT] = { nullptr, };
	VAR_LR(WeaponBase*,70)	m_pStaticCurrentWeapon = nullptr;

	VAR_LR(WEAPON_SLOT_TYPE,71) m_nCurWeaponSlotNum = WEAPON_SLOT_PRIMARY;
	VAR_LR(WEAPON_SLOT_TYPE,72) m_nOldWeaponSlotNum = WEAPON_SLOT_UNKNOWN;
	VAR_LR(WEAPON_SLOT_TYPE,73) m_nSwapWeaponSlotNum = WEAPON_SLOT_PRIMARY;

	VAR_LR(WeaponBase*,74) m_pPurchasePrimWeapon = nullptr;		// ���� � �Ű� �ִ� ����

	i3Node *			m_pDummyRightHand = nullptr;			//	������ �ٴ� ����
	i3Node *			m_pDummyLeftSideHand = nullptr;		//  ������ �׸��� ���� ����(�޼�)
	i3Node *			m_pDummyLeftHand = nullptr;			//	�޼� �ٴ� ����
	i3Node *			m_pDummyRightSideHand = nullptr;		// �޼� �׸��� ���� ����(������)
	i3Node *			m_pDummyPrimPurchase = nullptr;		// �ֹ��� �ٴ� ����
	i3Node *			m_pWeaponAttachRoot = nullptr;		// ���� �پ� �ִ� ����
	CHARA_WEAPON_ATTACH_HAND	m_nWeaponAttachHand = CHARA_WEAPON_GRIP_RIGHT_HAND;

	REAL32				m_rDropedWeaponCheckTime = 0.0f;	// ��� ������ ���� üũ Ÿ��
	typedef std::deque<INT32> deqThrowWeaponIdxCon;
	deqThrowWeaponIdxCon m_deqThrowList;

	bool				m_bForcedChange = false;			// ���߹��Ⳣ�� ���� ���ⱳü�� ��� ������ ����
	UINT8				m_nPrevMultiWeaponIdx = 0;			// ���߹��� ��ü �� ���������� �����ϱ� ���Ͽ� ���� ������ ���߹��� �ε����� ����

public:
	
	// �ش� ������ ����
	WeaponBase *		getWeapon( WEAPON_SLOT_TYPE nSlotNum);
	void				setWeapon( WEAPON_SLOT_TYPE nSlotNum, WeaponBase * pWeapon)
	{
		I3_BOUNDCHK( nSlotNum, WEAPON_SLOT_COUNT);
		I3_REF_CHANGE( m_pWeapon[ nSlotNum], pWeapon);
	}

	// ���� ��� �ִ� ����
	WeaponBase *		getCurrent(void)						{ I3_BOUNDCHK( m_nCurWeaponSlotNum, WEAPON_SLOT_COUNT);	return getWeapon( m_nCurWeaponSlotNum);	}
	// ���� ��� �ִ� ���� ����
	WEAPON_SLOT_TYPE	getCurrentSlotNum( void )					{ return m_nCurWeaponSlotNum;								}
	void				setCurrentSlotNum( WEAPON_SLOT_TYPE nSlot)	{ m_nCurWeaponSlotNum = nSlot; }
	// ���� ���� ����
	WEAPON_SLOT_TYPE	getOldSlotNum( void )					{ return m_nOldWeaponSlotNum;								}
	void				setOldSlotNum( WEAPON_SLOT_TYPE nSlot)	{ m_nOldWeaponSlotNum = nSlot; }
	// ���� �����ӿ� �ٲ� ���� ����
	WEAPON_SLOT_TYPE	getSwapSlotNum( void )					{ return m_nSwapWeaponSlotNum;								}
	void				setSwapSlotNum( WEAPON_SLOT_TYPE nSlot)	{ m_nSwapWeaponSlotNum = nSlot; }

	// � �Ű� �ִ� ����
	WeaponBase*			getBackpack( void )						{ return m_pPurchasePrimWeapon; }
	void				setBackpack( WeaponBase * pWeapon)		{ m_pPurchasePrimWeapon = pWeapon; }
	
	// � ���� ������ ��ġ ���
	i3Node *			getDummyPrimPurchase( void)				{ return m_pDummyPrimPurchase; }

	WeaponBase *		getStaticCurrentWeapon(void)			{ return m_pStaticCurrentWeapon;}
	void				setStaticCurrentWeapon(WeaponBase * pW)	{ m_pStaticCurrentWeapon = pW;}

	void				setForcedChange( bool bForced)			{ m_bForcedChange = bForced; }
	bool				getForcedChange(void)					{ return m_bForcedChange; }

	void				setPrevMultiWeaponIdx(UINT8 idx)		{ m_nPrevMultiWeaponIdx = idx; }
	UINT8				getPrevMultiWeaponIdx()					{ return m_nPrevMultiWeaponIdx; }

	// �������� ����
	/*WeaponBase*			getSidepack( void )						{ return m_pPurchaseSideWeapon; }
	void				setSidepack( WeaponBase * pWeapon)		{ m_pPurchaseSideWeapon = pWeapon; }*/
	
	// �������� ��ġ ���
	//i3Node *			getDummySidePurchase( void)				{ return m_pDummySidePurchase; }

	i3Node *			getDummyLeftHand( void)					{ return m_pDummyLeftHand; }
	i3Node *			getDummyRightHand( void)				{ return m_pDummyRightHand; }
	// ���� �پ� �ִ� ������ ���
	i3Node *			getAttachRoot( void)					{ return m_pWeaponAttachRoot;			}
	void				setAttachHand( CHARA_WEAPON_ATTACH_HAND nAttachHand)
						{	
							switch( nAttachHand)
							{
							case CHARA_WEAPON_GRIP_RIGHT_HAND:	
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
								I3TRACE("setAttachHand : CHARA_WEAPON_GRIP_RIGHT_HAND\n");
#endif
									m_pWeaponAttachRoot = m_pDummyRightHand;		
								}break;
							case CHARA_WEAPON_BARREL_LEFT_HAND:
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
									I3TRACE("setAttachHand : CHARA_WEAPON_BARREL_LEFT_HAND\n");
#endif
							#if defined( USE_RELOAD_NEWANIMATION)
									m_pWeaponAttachRoot = m_pDummyLeftHand;
							#else
									m_pWeaponAttachRoot = m_pDummyLeftSideHand;
							#endif
								}break;
							case CHARA_WEAPON_GRIP_LEFT_HAND :	
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
									I3TRACE("setAttachHand : CHARA_WEAPON_GRIP_LEFT_HAND\n");
#endif
									m_pWeaponAttachRoot = m_pDummyLeftHand;			
								}break;
							case CHARA_WEAPON_BARREL_RIGHT_HAND :	
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
									I3TRACE("setAttachHand : CHARA_WEAPON_BARREL_RIGHT_HAND\n");
#endif
									m_pWeaponAttachRoot = m_pDummyRightSideHand;	
								}break;
							default: 
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
									I3TRACE("setAttachHand : CHARA_WEAPON_BARREL_RIGHT_HAND\n");
#endif
									m_pWeaponAttachRoot = nullptr;		
								}break;
							}
							m_nWeaponAttachHand = nAttachHand;
						}
	CHARA_WEAPON_ATTACH_HAND getAttachHand( void)						{ return m_nWeaponAttachHand;			}

	// ����� ���� üũ Ÿ��
	REAL32				getDropedWeaponCheckTime( void)					{ return m_rDropedWeaponCheckTime; }
	void				setDropedWeaponCheckTime( REAL32 tm)			{ m_rDropedWeaponCheckTime = tm; }

	WEAPON_SLOT_TYPE	getNextUsableWeaponSlot(void);
	WEAPON_SLOT_TYPE	getPrevUsableWeaponSlot(void);
	bool				checkUsableWeaponSlot( WEAPON_SLOT_TYPE slot, bool bCheckMultiWeapon);

	void				NoUseWeapon( WeaponBase* pWeapon);

public:
	CGameCharaWeaponContext( void);
	virtual ~CGameCharaWeaponContext( void);

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	// change view
	virtual void	OnChange1PV() override;
	virtual void	OnChange3PV() override;

	virtual void	OnChangeLocal() override;
	virtual void	OnChangeNetwork() override;
	virtual void	OnChangeNPC() override;

	/** \brief Attach Node�� ã�´�. */
	virtual void	OnChangeBoneHierarchy( i3Body * pBody) override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief ������ ���Ἲ �˻縦 �մϴ�.
		\note ���� ĳ���Ϳ� �ش� ���Ⱑ ���� ��� �����ؼ� ��� �ֵ��� �Ѵ�. */
	void			WeaponValidCheck( WEAPON_CLASS_TYPE nClassType, WEAPON::RES_ID WeaponNum, WEAPON_SLOT_TYPE slot, UINT8 nMultiWeaponIndex, bool bSyncChange);
	WeaponBase *	FindWeaponByExtensionItemID( T_ItemID extItemID);
	
public:
	// **\brief ���� ��忡 ���� ���⸦ �����Ѵ�. */
	WEAPON_SLOT_TYPE	CreateWeaponByGameMode( UINT8 WeaponFlag);

	INT32			ChangeToEquipedWeapon( INT32 nSlot, UINT8 weapponLimitFlag);

	/** \brief ������ �ϸ� ȣ��˴ϴ�. */
	bool	_Respawn( void);
	void	PushThrowWeaponIndex(INT32 nIdx);
	INT32	GetThrowWeaponIndex();
	void	PopThrowWeaponIndex();

public:
	/** \brief ������ ������ �����Ѵ�.
		\param[in] pDiff Diffuse color
		\param[in] pSpec Specular color
		\param[in] alpha Alpha value */
	bool	_Change_Color( COLORREAL * pDiff = nullptr, COLORREAL * pSpec = nullptr, REAL32 alpha = 1.f);

	/** \brief ���⸦ �����Ѵ�.
		\param[in] slot ���� ���� ��ȣ
		\param[in] bDirect ���� �����ӿ��� ���� ����
		\note bDirect�� false��� next frame���� ���� ��ü�� �̷�����ϴ�.
			�ٸ� ĳ���ʹ� �̹� ���� ������ �ִϸ��̼����� ���� frame���� �����˴ϴ�. */
	bool	_ChangeWeapon( WEAPON_SLOT_TYPE slot, bool bDirect = false);

	/** \brief ���⸦ �����Ѵ�.
		\param[in] slot ���� ���� ��ȣ
		\param[in] T_ItemID ���� ItemID */
	bool	_Create_Weapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid);

	/** \brief �ش� ������ ���⸦ �����Ѵ�.
		\param[in] slot ���� ���� ��ȣ */
	void	_Delete_Weapon( WEAPON_SLOT_TYPE slot);

	/** \brief ���⸦ ����.
		\param[in] slot ���� ����		*/
	void	_Detach_Weapon( WEAPON_SLOT_TYPE slot);

	/** \brief ���⸦ ����.
		\param[in] pWeapon ���� pointer */
	void	_Detach_Weapon( WeaponBase * pWeapon);

	/** \brief ���� ���⸮
		\param[in] slot ���� ���� ��ȣ
		\param[in] pPos ��ġ
		\param[in] bNextWeapon ���� ����� �ڵ� ���� ���� */
	bool	_Drop( WEAPON_SLOT_TYPE slot, VEC3D * pPos, bool bNextWeapon, bool bDirectPickup);

	/** \brief Extension Ȱ�� ���� */
	bool	_Extension( bool bEnable);

	/** \brief ���� �߻�
		\param[in] order ���� ����
		\param[in] pPos ĳ���� ���� ��ġ
		\param[in] pTarget ��ǥ�� */
	bool	_Fire( WEAPON::FIRE_ORDER order, VEC3D * pPos = nullptr, VEC3D * pTarget = nullptr);

	/** \brief ���� ���⸦ ��밡�� ���·� �����Ѵ�. */
	void	_Idle( void);

	/** \brief ź�� ���� */
	bool	_Load_Bullet( void);

	/** \brief źâ ���� */
	bool	_Load_Magazine( void);

	/** \brief Quick load */
	bool	_Load_Magazine_Dual( void);

	/** \brief ���� */
	bool	_Load_Magazine_Ready( void);

	/** \brief ���⸦ �ݴ´�.
		\param[in] pWeapon weapon pointer */
	bool	_Pickup( WeaponBase * pWeapon, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList);

	/** \brief ��ǿ��� ź���� ����
		\note ��ź�� �ش�˴ϴ�. */
	bool	_Remove_Shell( void);

	/** \brief ��� ���⸦ �ʱ�ȭ�Ѵ�. */
	void	_Reset_Weapons( void);

	/** \brief ���� ���⸦ �����Ѵ�.
		\note ���� ��ü�Ǵ� ���Ⱑ �����Ǹ� �ٲ��ݴϴ�. */
	void	_SyncWeapon( void);

	/** \brief Zoom */
	bool	_Zoom( void);
};

#endif	// __GAME_CHARA_WEAPON_CONTEXT_H__
