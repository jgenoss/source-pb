#pragma once

#include "iHudBase.h"
#include "UIHudDefine.h"

#include "../GameCharaMultiWeaponContext.h"

class UIHudManager;
class UIImgSetCtrl;

class UIHudMultiWeapon : public iHudBase
{
public:
	UIHudMultiWeapon(UIHudManager* p);
	virtual ~UIHudMultiWeapon();

	virtual void InitializeAtLoad(i3UIScene * pScene) override;
	virtual void ClearAtUnload(i3UIScene * pScene) override;
	virtual void Update(REAL32 rDeltaSeconds) override;
	virtual void SetEnable(bool enable) override;

	// Context���� ����� ������ UI�� ����
	void UpdateMultiWeaponSlot();

	// HUD�� On/Off ���ش�
	void ShowMultiWeaponSlot( bool bAuto = false);

	// Slot�� ����(bForward�� TRUE�� ��� �������� �̵�)
	void SetMultiWeaponSlotImage(BOOL bForward = TRUE);
	
	void OffAutoClosed();

	void SetCurrentSlotIdx( UINT8 idx)		{ m_nCurrentSlotIdx = idx; }
	UINT8 GetCurrentSlotIdx()				{ return m_nCurrentSlotIdx; }

	void SetEnableFire( BOOL enable)		{ m_bEnableFire = enable; }
	BOOL GetEnableFire()					{ return m_bEnableFire; }

	bool GetAutoView()						{ return m_bAutoView; }

private:
	UIImgSetCtrl *			m_pCurrMultiWeapon;	// ���� ��ġ�� ���߹��� Slot(���� ������ ����)
	i3UIButtonImageSet *	m_pCurrSlotNumber;	// ���� ��ġ�� Slot ��ȣ

	UIImgSetCtrl *			m_pNextMultiWeapon;	// ���� ��ġ�� ���߹��� Slot(������ �����ߴ� ����)
	i3UIButtonImageSet *	m_pNextSlotNumber;	// ���� ��ġ�� Slot ��ȣ

	UIImgSetCtrl *			m_pPrevMultiWeapon;	// ���� ��ġ�� ���߹��� Slot(������ ������ ���߹���)
	i3UIButtonImageSet *	m_pPrevSlotNumber;	// ���� ��ġ�� Slot ��ȣ 

	bool		m_bAutoView;			// ���� �������� ���� �ڵ� HUD����
	BOOL		m_bEnableFire;			// HUD off ���� �ٷ� �߻� �Ұ�
	REAL32		m_fLifeTime;			// ���� �������� ���� �ڵ� HUD�� off ��Ű�� ���� �ð�
	UINT8		m_nCurrentSlotIdx;		// HUD On ������ ��쿡�� ���� ���߹���

	// UI Slot ����
	void _SetWeaponShapeImage(UIImgSetCtrl* ctrl, T_ItemID itemID);

	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;
};