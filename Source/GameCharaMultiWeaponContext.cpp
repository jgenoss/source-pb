#include "pch.h"
#include "GameCharaMultiWeaponContext.h"
#include "GameCharaWeaponContext.h"
#include "UserInfoContext.h"
#include "UI/UIHudManager.h"
#include "UI/UIHudMultiWeapon.h"
#include "UserInfoContext.h"

#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CGameCharaMultiWeaponContext);

CGameCharaMultiWeaponContext::CGameCharaMultiWeaponContext() : m_pLeftWeaponSlot(nullptr), m_pRightWeaponSlot(nullptr), m_pCurrentWeaponSlot(nullptr), m_nQuickChangeIdx(-1), m_bPickupChange(false), m_bDropedChange(false), m_bHUDSelectedChange(false), m_bQuickChange(false), m_bDropProhibit(false), m_bPickupProhibit(false)
{
	m_MultiWeaponList.clear();
}

CGameCharaMultiWeaponContext::~CGameCharaMultiWeaponContext()
{
	I3TRACE("###################################################################\n");
	I3TRACE("CharaIdx : %d, CGameCharaMultiWeaponContext::OnDestroy\n", m_pOwner->getCharaNetIndex());

	OnDestroy();
}

void CGameCharaMultiWeaponContext::OnCreate(CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	CGameCharaContextBase::OnCreate(pOwner, defaultView, defaultNetwork);
}

void CGameCharaMultiWeaponContext::OnDestroy()
{
	ClearWeaponSlotList();

	CGameCharaContextBase::OnDestroy();
}

void CGameCharaMultiWeaponContext::ClearWeaponSlotList()
{
#if defined(I3_DEBUG)
	I3TRACE("CGameCharaMultiWeaponContext::ClearWeaponSlotList - %d\n", m_MultiWeaponList.size());
	I3TRACE("###################################################################\n");
#endif

	for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if (info->pWeapon != nullptr)
		{
			info->pWeapon->ReturnToReady();
			info->pWeapon->DetachNode();
		}
	}

	if ( m_pOwner->getWeaponContext() != nullptr)
	{
		m_pOwner->getWeaponContext()->_Delete_Weapon(WEAPON_SLOT_PRIMARY);
	}

	m_pCurrentWeaponSlot = nullptr;
	m_pLeftWeaponSlot = nullptr;
	m_pRightWeaponSlot = nullptr;

	// Context ���� ��, ��Ʋ ���� �� ����Ʈ ����
	i3::cu::for_each_SafeFree_clear(m_MultiWeaponList);
}

void CGameCharaMultiWeaponContext::InitMultiWeaponList()
{
	m_pOwner->getWeaponContext()->_Delete_Weapon( WEAPON_SLOT_PRIMARY);

	m_pCurrentWeaponSlot = nullptr;
	m_pLeftWeaponSlot = nullptr;
	m_pRightWeaponSlot = nullptr;

	m_MultiWeaponList.clear();

	ResetChange();

	EmptyQuickChangeIdx();
}

void CGameCharaMultiWeaponContext::DeleteMultiWeapon( WeaponBase* pWeapon)
{
	MULTIWEAPON_INFO* pDeleteInfo = nullptr;
	UINT8 nDeleteIdx = 0;

	for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		// UI Index�� ��ġ�� ��� - ����
		// ����ġ������ UI Index�� ũ�� -1 ���ش�
		// �������� Skip
		if (info->pWeapon == pWeapon)
		{
#if defined( I3_DEBUG)
			i3::rc_wstring wstrWeaponName;
			info->pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
			I3TRACE("���߹��� %d���� ���� - %s\n", i, wstrWeaponName);
#endif

			if ( m_pCurrentWeaponSlot == nullptr)
			{
				m_pOwner->getWeaponContext()->NoUseWeapon(info->pWeapon);
				I3TRACE("���� �߻�! �� ���� �ٶ�\n");
			}
			
			EmptyQuickChangeIdx();

			pDeleteInfo = info;
			nDeleteIdx = static_cast<UINT8>(i);

			m_pCurrentWeaponSlot = nullptr;

			I3TRACE("���߹��⸮��Ʈ�� ��� ĳ���� Index :%d\n", m_pOwner->getCharaNetIndex());
		}
	}

	// ���� ���� ������ �����
	if (pDeleteInfo != nullptr)
	{
		I3MEM_SAFE_FREE(pDeleteInfo);
		i3::vu::erase_index(m_MultiWeaponList, static_cast<size_t>(nDeleteIdx));

		for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
		{
			MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

			if ( info->nSlotNumber > nDeleteIdx)
			{
				I3TRACE("%d Slot -> ", info->nSlotNumber);
				info->nSlotNumber -= 1;
				I3TRACE("%d Slot\n", info->nSlotNumber);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// ���⼭ ������ �����ϸ� �ȵ�! ������ ���° ��������  ���� ������ �����ص� ������...
	// 1���� ��� - Null ó��
	// ������ ������ ������ ��ȣ�� �����̾��� ��� 1�� ���� �������� ������ ����
	// �� ���� ��� ������ ���� ��ȣ�� �������� �����͸� ����
	if (m_MultiWeaponList.size() < 1)
	{
		ClearWeaponSlotList();
		I3TRACE("���߹��⸮��Ʈ�� ������ϴ�.\n");
	}
	else if ( nDeleteIdx == GetMultiWeaponCount() + 1)
	{
		SetMultiWeaponSlot(1);
	}
	else
	{
		SetMultiWeaponSlot( nDeleteIdx);
	}
}

// ���߹��⸦ �߰��ϴ� �Լ�
void CGameCharaMultiWeaponContext::AddMultiWeapon(WeaponBase* pWeapon, UINT8 nSyncNumber)
{
	I3ASSERT( pWeapon != nullptr);

	// ���� �ִ� ������ŭ ��� ���̸� �߰��� �� ����
	if ( CheckEmptySlot() == FALSE)
		return;

	MULTIWEAPON_INFO * pInfo = (MULTIWEAPON_INFO*)i3MemAlloc( sizeof(MULTIWEAPON_INFO));

	i3mem::FillZero( pInfo, sizeof(MULTIWEAPON_INFO));

	pInfo->nSlotNumber = (UINT8)m_MultiWeaponList.size() + 1;
	pInfo->pWeapon = pWeapon;
	pInfo->nSyncNumber = nSyncNumber;

	m_MultiWeaponList.push_back(pInfo);

	// ������ �� �ֹ��⸦ ���߹��⿡ ����� ���� �������� ����
	if ( m_pCurrentWeaponSlot == nullptr)
	{
		SetMultiWeaponSlot(1);
	}

#if defined(I3_DEBUG)
	i3::rc_wstring wstrWeaponName;
	pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
	I3TRACE("AddMultiWeapon %d Index - %s(%d)\n", pInfo->nSlotNumber, wstrWeaponName, nSyncNumber);
#endif
}

bool CGameCharaMultiWeaponContext::CheckEmptySlot()
{
	if ( UserInfoContext::i()->GetMaxMultiWeapon() <= GetMultiWeaponCount())
		return false;

	return true;
}

// ��Ʈ��ũ ������ ���߹��⸦ �����ϴ� �Լ�
void CGameCharaMultiWeaponContext::ChangeMultiWeapon(WeaponBase* pWeapon)
{
	I3ASSERT(pWeapon != nullptr);

	// �� ĳ������ ���߹��� ������ Cmd_ChangeWeapon���� ����
	if ( m_pOwner->isLocal())
		return;

	// ���� �ִ� ������ŭ ��� ���̸� �߰��� �� ����
	if ( CheckEmptySlot() == FALSE)
		return;

	// �����Ϸ��� ���⸦ ���߹��� ����Ʈ���� ã�ƺ��� 
	UINT8 nMultiWeaponIdx = SearchItemIDMultiWeapon( pWeapon);

	if ( nMultiWeaponIdx == 0)
	{
		// �ش��ϴ� ���� ������ ���� ��� ���߹��� ����Ʈ�� ���⸦ �߰� - �ٸ� ������ Sync ��ȣ�� ������ �ʿ� ����
		AddMultiWeapon( pWeapon);
		I3TRACE("���߹��⸦ �����Ϸ��� �Ͽ����� ����Ʈ�� �������� �ʾƼ� �߰�\n");
	}
	else
	{
		// ���� �����ϰ� �ִ� ���߹��Ⱑ ���� ��� ��������
		if ( m_pCurrentWeaponSlot != nullptr)
		{
			m_pOwner->getWeaponContext()->NoUseWeapon(m_pCurrentWeaponSlot->pWeapon);
		}

		// �߰��� ������ �������� ������ ����
		SetMultiWeaponSlot( nMultiWeaponIdx);
	}

#if defined(I3_DEBUG)
	i3::rc_wstring wstrWeaponName;
	pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
	I3TRACE("ChangeMultiWeapon - %s\n", wstrWeaponName);
#endif
}

UINT8 CGameCharaMultiWeaponContext::SearchItemIDMultiWeapon(WeaponBase* pWeapon)
{
	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( pWeapon->getWeaponInfo()->GetItemID() == info->pWeapon->getWeaponInfo()->GetItemID())
		{
			return info->nSlotNumber;
		}
	}

	return 0;
}

MULTIWEAPON_INFO* CGameCharaMultiWeaponContext::GetPrevMultiWeaponInfo(UINT8 idx)
{
	if ( GetMultiWeaponCount() <= 1)
		return nullptr;
	else if ( idx == GetMultiWeaponCount())
		return SearchIndexSlot( 1);
	else
		return SearchIndexSlot( idx + 1);
}

MULTIWEAPON_INFO* CGameCharaMultiWeaponContext::GetNextMultiWeaponInfo(UINT8 idx)
{
	if ( idx > 1)
		return SearchIndexSlot( idx - 1);
	else if ( idx == 1)
		return SearchIndexSlot( GetMultiWeaponCount());
	else
		return nullptr;
}

bool CGameCharaMultiWeaponContext::IsUsingMultiWeapon( WeaponBase* pWeapon)
{
	for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( info->pWeapon == pWeapon)
		{
			if ( info->pWeapon->getOwner() != nullptr)
			{
				//if ( info->pWeapon->getOwner()->isLocal())
				{
					return true;
				}
			}
		}
	}

	return false;
}

// idx�� ���� ���� �������� �����͸� �����ϴ� �Լ� - Drop, Add, Select
void CGameCharaMultiWeaponContext::SetMultiWeaponSlot(UINT8 idx)
{
	// Ȥ�� �� ���� ������ ���� ���� �� �����͸� �ʱ�ȭ
	m_pCurrentWeaponSlot = nullptr;
	m_pLeftWeaponSlot = nullptr;
	m_pRightWeaponSlot = nullptr;

	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		// ���� ���߹��� ����
		if ( info->nSlotNumber == idx)
		{
			m_pCurrentWeaponSlot = info;

			if ( m_pOwner->isLocal())
			{
				UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
				I3ASSERT( pHud != nullptr);

				pHud->SetCurrentSlotIdx( idx);
			}
		}

		// ���� ���߹��� ����(2�� �̻� ���߹��Ⱑ ���� ��츸 ����) - ���� ��ȣ
		if ( GetMultiWeaponCount() > 1)
		{
			// ������ ������ ������ ��� 1�� ������ ����
			// �� �ܿ��� ���� Idx + 1 ������ ����
			if ( idx == GetMultiWeaponCount() && info->nSlotNumber == 1)
				m_pLeftWeaponSlot = info;
			else if ( idx != GetMultiWeaponCount() && info->nSlotNumber == idx + 1)
				m_pLeftWeaponSlot = info;
		}

		// ���� ���߹��� ����(3�� �̻� ���߹��Ⱑ ���� ��츸 ����) - ���� ��ȣ
		if ( GetMultiWeaponCount() > 2)
		{
			// ������ 1�� ������ ��� ������ ������ ����
			// �� �ܿ��� ���� Idx - 1 ������ ����
			if ( idx == 1 && info->nSlotNumber == GetMultiWeaponCount())
				m_pRightWeaponSlot = info;
			else if ( idx != 1 && info->nSlotNumber == idx - 1)
				m_pRightWeaponSlot = info;
		}
	}
}

UINT8 CGameCharaMultiWeaponContext::GetMultiWeaponIdx(UINT8 nSyncNum)
{
	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( info->nSyncNumber == nSyncNum)
			return info->nSlotNumber;
	}

	return 0;
}

void CGameCharaMultiWeaponContext::SetQuickChangeIdx()
{
	if ( GetCurrSlotInfo() == nullptr)
		return;

	m_nQuickChangeIdx = GetCurrSlotInfo()->nSlotNumber;
	I3TRACE("SetQuickChangeIdx : %d\n", m_nQuickChangeIdx);
}

WeaponBase* CGameCharaMultiWeaponContext::GetQuickChangeWeapon()
{
	if ( SearchIndexSlot( static_cast<UINT8>(GetQuickChangeIdx())) != nullptr)
	{
		return SearchIndexSlot( static_cast<UINT8>(GetQuickChangeIdx()))->pWeapon;
	}

	return nullptr;
}

WeaponBase* CGameCharaMultiWeaponContext::GetMultiWeapon( UINT8 index)
{
	// ���� üũ �߰�

	for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if (info->nSlotNumber == index)
			return info->pWeapon;
	}

	return nullptr;
}

WeaponBase* CGameCharaMultiWeaponContext::GetSwapMultiWeapon( bool bReset)
{
	// ���ⱳü ����� ���� ���߹��Ⱑ �޶�����

	// 1. �ֹ��� ȹ������ ���� ���� ��ü�� ��� - ������  ���߹��� ����Ʈ���� �������� ��� �ִ� ���߹���
	// HUD �������� ��ü �� ���� - HUDChange ����
	// �Ϲ����� ���� ��ü �� ���� - ����� ���� ����
	// ���� ���� ��ü �� ���� - QuickChange ����
	// �ֹ��� ��� ��ü �� ����������... - DropedChange ����
	if ( IsPickupChange())
	{
		UINT8 nLastMultiWeaponSlot = GetMultiWeaponCount();

		if ( bReset)
		{
			I3TRACE("OnSelect GetSwapMultiWeapon - PickupChange\n");

			ResetChange();

			SetMultiWeaponSlot( nLastMultiWeaponSlot);

			if ( m_pOwner->isLocal())
			{
				UIHudMultiWeapon* pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
				I3ASSERT(pHud != nullptr);

				pHud->UpdateMultiWeaponSlot();
				pHud->ShowMultiWeaponSlot(true);
			}
		}
		else
		{
			I3TRACE("PreOnSelect GetSwapMultiWeapon - PickupChange\n");
		}

		return GetMultiWeapon( nLastMultiWeaponSlot);
	}

	// 2. ���߹��� HUD���� �������� ���� ���� ��ü�� ��� - HUD�� ����Ű�� ������ ���߹���
	// �ֹ��� ȹ�� ��ü �� ����������... - PickupChange ����
	// �Ϲ����� ���� ��ü �� ������(�ֹ���� ��ü)���� ���������� - ����� ���� ����
	// �ֹ��� ��� ��ü �� ����������... - DropedChange ����
	// ���� ���� ��ü �� ����������... - QuickChange ����
	if ( IsHUDSelectedChange())
	{
		UIHudMultiWeapon* pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
		I3ASSERT( pHud != nullptr);

		if ( bReset)
		{
			I3TRACE("OnSelect GetSwapMultiWeapon - HUDSelectedChange\n");

			ResetChange();

			SetMultiWeaponSlot( pHud->GetCurrentSlotIdx());
		}
		else
		{
			I3TRACE("PreOnSelect GetSwapMultiWeapon - HUDSelectedChange\n");
		}

		return GetMultiWeapon( pHud->GetCurrentSlotIdx());
	}

	// 3. �ֹ��� ������� ���� ���� ��ü�� ��� - ��� �� ���õ� ������ ���߹���
	// �ֹ��� ȹ�� ��ü �� ���� - PickupChange ����
	// �Ϲ����� ���� ��ü �� ���� - ����� ���� ����
	// ���� ���� ��ü �� ���� - QuickChange ����
	// HUD �������� ��ü �� ���� - HUDChange ����
	if ( IsDropedChange())
	{
		UINT8 nNextMultiWeaponSlot = GetNextSlotIndex();

		// ���߹��Ⱑ ���µ� ���߹���� ��ü �õ�
		if ( nNextMultiWeaponSlot == 0)
		{
			if ( bReset)
			{
				ResetChange();
			}

			return getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY);
		}

		if ( bReset)
		{
			I3TRACE("OnSelect GetSwapMultiWeapon - DropedChange\n");

			ResetChange();

			SetMultiWeaponSlot( nNextMultiWeaponSlot);
		}
		else
		{
			I3TRACE("PreOnSelect GetSwapMultiWeapon - DropedChange\n");
		}

		return GetMultiWeapon( nNextMultiWeaponSlot);

	}

	// 4. ���� ���� ��ü�� ���� ���� ��ü�� ���(�ֹ���-�ֹ��� ��ü��) - ������ ������ ���߹���
	if ( IsQuickChange())
	{
		if ( bReset)
		{
			I3TRACE("OnSelect GetSwapMultiWeapon - QuickChange\n");

			ResetChange();

			UINT8 nQuickChangeIdx = static_cast<UINT8>(GetQuickChangeIdx());
			m_pOwner->getMultiWeaponContext()->SetQuickChangeIdx();

			SetMultiWeaponSlot( nQuickChangeIdx);

			return GetMultiWeapon( nQuickChangeIdx);
		}
		else
		{
			I3TRACE("PreOnSelect GetSwapMultiWeapon - QuickChange\n");
		}

		return GetMultiWeapon( static_cast<UINT8>( GetQuickChangeIdx()) );
	}

	// 5. �Ϲ����� ���ⱳü�� ���� ���߹��� ��ü
	// �ֹ��� ȹ�� ��ü �� �Ұ����� ��
	// �ֹ��� ��� ��ü �� �Ұ����� ��
	// ���� ���� ��ü �� �Ұ����� ��
	// HUD �������� ��ü �� �Ұ����� ��
	if ( bReset)
	{
		I3TRACE("OnSelect GetSwapMultiWeapon - WeaponChange\n");
		ResetChange();
	}
	else
	{
		I3TRACE("PreOnSelect GetSwapMultiWeapon - WeaponChange\n");
	}

	return getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY);
}

void CGameCharaMultiWeaponContext::ResetChange()
{
	ResetPickupChange();
	ResetHUDSelectedChange();
	ResetDropedChange();
	ResetQuickChange();
	SetDropProhibit(false);
	SetPickupProhibit(false);
}

void CGameCharaMultiWeaponContext::HasDropedChange(UINT8 index /* = 0 */)
{
	m_bDropedChange = true;
	m_nNextSlotIndex = index;
}

// ���� ������ ���߹��⸦ �����ϴ� �Լ�
void CGameCharaMultiWeaponContext::DropedMultiWeapon()
{
	I3TRACE("CGameCharaMultiWeaponContext::DropedMultiWeapon() - ");
	// ���� ���� ���� ���߹��Ⱑ ���� ��� ����
	if ( m_pCurrentWeaponSlot == nullptr)
		return;

	UINT8 idx = m_pCurrentWeaponSlot->nSlotNumber;
	I3TRACE("������ UI Idx : %d\n", idx);

	MULTIWEAPON_INFO* pDeleteInfo = nullptr;
	size_t nDeleteIdx = 0;

	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		I3TRACE("%d��° ���� �˻� : %d\n", info->nSlotNumber);

		// UI Index�� ��ġ�� ��� - ����
		// ����ġ������ UI Index�� ũ�� -1 ���ش�
		// �������� Skip
		if ( info->nSlotNumber == idx)
		{
#if defined( I3_DEBUG)
			i3::rc_wstring wstrWeaponName;
			info->pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
			I3TRACE("���߹��� %d���� ���� - %s\n", idx, wstrWeaponName);
#endif

			if ( m_pCurrentWeaponSlot == nullptr)
			{
				m_pOwner->getWeaponContext()->NoUseWeapon( info->pWeapon);
				I3TRACE("���� �߻�! �� ���� �ٶ�\n");
			}

			if ( GetQuickChangeIdx() == idx)
				EmptyQuickChangeIdx();

			pDeleteInfo = info;
			nDeleteIdx = i;

			m_pCurrentWeaponSlot = nullptr;

			I3TRACE("���߹��⸮��Ʈ�� ��� ĳ���� Index :%d\n", m_pOwner->getCharaNetIndex());
		}
		else if ( info->nSlotNumber > idx)
		{
			// �����ϴ� ���Ժ��� ���Թ�ȣ�� Ŭ ��� 1�� �ٿ���
			I3TRACE("%d Slot -> ",info->nSlotNumber);
			info->nSlotNumber -= 1;
			I3TRACE("%d Slot\n", info->nSlotNumber);
		}
	}

	// ���� ���� ������ �����
	if ( pDeleteInfo != nullptr)
	{
		I3MEM_SAFE_FREE( pDeleteInfo);
		i3::vu::erase_index( m_MultiWeaponList, nDeleteIdx);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// ���⼭ ������ �����ϸ� �ȵ�! ������ ���° ��������  ���� ������ �����ص� ������...
	// 1���� ��� - Null ó��
	// ������ ������ ������ ��ȣ�� �����̾��� ��� 1�� ���� �������� ������ ����
	// �� ���� ��� ������ ���� ��ȣ�� �������� �����͸� ����
	if ( m_MultiWeaponList.size() < 1)
	{
		HasDropedChange();
		I3TRACE("���߹��⸮��Ʈ�� ������ϴ�.\n");
	}
	else if( idx  == GetMultiWeaponCount() + 1)
	{
		HasDropedChange(1);
	}
	else
	{
		HasDropedChange(idx);
	}
}

MULTIWEAPON_INFO* CGameCharaMultiWeaponContext::SearchIndexSlot(UINT8 uiIdx)
{
	I3TRACE("SearchIndexSlot\n");
	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( info->nSlotNumber == uiIdx)
			return info;
	}

	return nullptr;
}

#if defined(I3_DEBUG)
void CGameCharaMultiWeaponContext::OutputMulitWeaponList()
{
	I3TRACE("\n##################################\nm_MultiWeaponList Size : %d\n", m_MultiWeaponList.size());

	I3TRACE("[%d] ĳ���� : ", m_pOwner->getCharaNetIndex());
	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( info == nullptr)
			continue;

		i3::rc_wstring wstrWeaponName;
		info->pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);

		I3TRACE("[%d ���� - %s (%d)] ", (INT32)info->nSlotNumber, wstrWeaponName, info->nSyncNumber);
	}
	I3TRACE("\n");

	if ( m_pCurrentWeaponSlot != nullptr)
	{
		i3::rc_wstring wstrWeaponName;
		m_pCurrentWeaponSlot->pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
		I3TRACE("Current Weapon - %s\n", wstrWeaponName);
	}
	else
	{
		I3TRACE("���� ���߹��� ������ �������!!!\n");
	}
	I3TRACE("##################################\n\n");
}
#endif