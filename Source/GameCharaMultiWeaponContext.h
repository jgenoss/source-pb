#pragma once

#include "Weapon/WeaponBase.h"
#include "GameCharaContextBase.h"
#include "GameCharaBase.h"

/* ���߹��� ���� ���� - �������� ����� Sync�� ó���ϸ� Ŭ�� �������� UI Idx�� ó��*/
struct MULTIWEAPON_INFO
{
	WeaponBase *	pWeapon = nullptr;		// Weapon ����
	UINT8			nSlotNumber = 0;	// HUD ǥ�ÿ�
	UINT8			nSyncNumber = 0;	// ��Ʈ��ũ ��ũ��
};

class CGameCharaMultiWeaponContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaMultiWeaponContext, CGameCharaContextBase);

	friend class CGameCharaBase;

public:
	CGameCharaMultiWeaponContext();
	virtual ~CGameCharaMultiWeaponContext();

	virtual void	OnCreate(CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy(void) override;

	//	�װų� ��Ʋ ���� �� ���߹��� ������ �ʱ�ȭ
	void		InitMultiWeaponList();

	// ���߹��⸦ ����
	void		DeleteMultiWeapon( WeaponBase* pWeapon);

	// ���߹��⸦ �߰�
	void		AddMultiWeapon(WeaponBase* pWeapon, UINT8 nSyncNumber = 0);

	// ���߹��⸦ ����
	void		ChangeMultiWeapon(WeaponBase* pWeapon);

	MULTIWEAPON_INFO*	GetCurrSlotInfo()		{ return m_pCurrentWeaponSlot; }		// ���� ������ ���߹��� ����
	MULTIWEAPON_INFO*	GetPrevSlotInfo()		{ return m_pLeftWeaponSlot; }			// ������ ��ġ�� ���߹��� ����
	MULTIWEAPON_INFO*	GetNextSlotInfo()		{ return m_pRightWeaponSlot; }			// ������ ��ġ�� ���߹��� ����
	UINT8				GetMultiWeaponCount()	{ return static_cast<UINT8>( m_MultiWeaponList.size()); }

	void				HasPickupChange()		{ m_bPickupChange = true; }
	void				ResetPickupChange()		{ m_bPickupChange = false; }
	bool				IsPickupChange()		{ return m_bPickupChange; }

	void				HasDropedChange( UINT8 index = 0);
	bool				IsDropedChange()		{ return m_bDropedChange; }
	void				ResetDropedChange()		{ m_bDropedChange = false; m_nNextSlotIndex = 0; }
	UINT8				GetNextSlotIndex()		{ return m_nNextSlotIndex; }

	void				HasHUDSelectedChange()	{ m_bHUDSelectedChange = true; }
	void				ResetHUDSelectedChange(){ m_bHUDSelectedChange = false; }
	bool				IsHUDSelectedChange()	{ return m_bHUDSelectedChange; }

	void				HasQuickChange()		{ m_bQuickChange = true; }
	void				ResetQuickChange()		{ m_bQuickChange = false; }
	bool				IsQuickChange()			{ return m_bQuickChange; }
	INT32				GetQuickChangeIdx()		{ return m_nQuickChangeIdx; }
	void				EmptyQuickChangeIdx()	{ m_nQuickChangeIdx = -1; }

	void				SetDropProhibit(bool bProhibit)		{ m_bDropProhibit = bProhibit; }
	bool				IsDropProhibit()					{ return m_bDropProhibit; }
	void				SetPickupProhibit(bool bProhibit)	{ m_bPickupProhibit = bProhibit; }
	bool				IsPickupProhibit()					{ return m_bPickupProhibit; }

	void				ResetChange();

	// Drop �� ���߹��� ����Ʈ���� ����
	void		DropedMultiWeapon();

	void		SetQuickChangeIdx();
	WeaponBase* GetQuickChangeWeapon();

	WeaponBase* GetMultiWeapon( UINT8 index);
	WeaponBase* GetSwapMultiWeapon( bool bReset);

	MULTIWEAPON_INFO*	SearchIndexSlot( UINT8 uiIdx);
	MULTIWEAPON_INFO*	GetPrevMultiWeaponInfo(UINT8 idx);
	MULTIWEAPON_INFO*	GetNextMultiWeaponInfo(UINT8 idx);

	// Sync Index�� UI Index�� ã��
	UINT8				GetMultiWeaponIdx(UINT8 nSyncNum);

	// ItemID�� UI Index�� ã��
	UINT8				SearchItemIDMultiWeapon(WeaponBase* pWeapon);

	// �ش� Index �������� ���߹��� �����ϴ� �Լ�
	void				SetMultiWeaponSlot( UINT8 idx);

	// �� ĳ���Ͱ� ����ϴ� ���߹������� Ȯ���ϴ� �Լ�
	bool				IsUsingMultiWeapon( WeaponBase* pWeapon);

	// ���߹��� �� ������ �ִ��� üũ
	bool				CheckEmptySlot();

	void				ClearWeaponSlotList();

#if defined(I3_DEBUG)
	void				OutputMulitWeaponList();
#endif

protected:
	i3::vector<MULTIWEAPON_INFO*>	m_MultiWeaponList;

	MULTIWEAPON_INFO*	m_pLeftWeaponSlot;
	MULTIWEAPON_INFO*	m_pCurrentWeaponSlot;
	MULTIWEAPON_INFO*	m_pRightWeaponSlot;

	INT32				m_nQuickChangeIdx;				// ���� ���� ��ü��

	bool				m_bPickupChange;
	bool				m_bDropedChange;
	bool				m_bHUDSelectedChange;
	bool				m_bQuickChange;

	bool				m_bDropProhibit;
	bool				m_bPickupProhibit;

	UINT8				m_nNextSlotIndex;
};