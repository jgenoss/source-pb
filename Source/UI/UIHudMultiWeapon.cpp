#include "pch.h"
#include "UIHudMultiWeapon.h"
#include "UIImgSetCtrl.h"
#include "ItemImageMgr.h"
#include "../MyRoomInfoContext.h"
#include "../GameCharaMultiWeaponContext.h"

UIHudMultiWeapon::UIHudMultiWeapon(UIHudManager* p) : iHudBase(p), m_bAutoView(false), m_fLifeTime(0.f), m_pCurrMultiWeapon(nullptr), m_pNextMultiWeapon(nullptr), m_pPrevMultiWeapon(nullptr), m_pPrevSlotNumber(nullptr), m_pNextSlotNumber(nullptr), m_pCurrSlotNumber(nullptr), m_nCurrentSlotIdx(1), m_bEnableFire(TRUE)
{
}

UIHudMultiWeapon::~UIHudMultiWeapon()
{
	m_pCurrMultiWeapon = nullptr;
	m_pCurrSlotNumber = nullptr;

	m_pNextMultiWeapon = nullptr;
	m_pNextSlotNumber = nullptr;

	m_pPrevMultiWeapon = nullptr;
	m_pPrevSlotNumber = nullptr;
}

void	UIHudMultiWeapon::Update(REAL32 rDeltaSeconds)
{
	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		if ( GetMyChara()->getMultiWeaponContext()->GetCurrSlotInfo() == nullptr
			|| GetMyChara()->getMultiWeaponContext()->IsHUDSelectedChange())
		{
			OffAutoClosed();
			SetEnable(false);
		}

		if ( m_bAutoView)
		{
			m_fLifeTime += rDeltaSeconds;

			if ( m_fLifeTime > 2.0f)
			{
				OffAutoClosed();
				ShowMultiWeaponSlot();
			}
		}

		if ( GetMyChara()->isCharaStateMask(CHARA_STATE_DEATH) && IsEnable())
		{
			SetEnable(false);
		}
	}
}

void UIHudMultiWeapon::SetEnable(bool enable)
{
	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		if ( enable == false)
		{
			OffAutoClosed();
		}
		else
		{
			if (GetMyChara()->getMultiWeaponContext()->GetCurrSlotInfo() == nullptr)
				return;

			UpdateMultiWeaponSlot();
		}
	}

	iHudBase::SetEnable( enable);
}

void UIHudMultiWeapon::OffAutoClosed()
{
	m_bAutoView = false;
	m_fLifeTime = 0.f;
}

void UIHudMultiWeapon::InitializeAtLoad(i3UIScene * pScene)
{
	iHudBase::AddFrameWnd( pScene, "MultiWeapon");

	ItemImageMgr* mgr = GetItemImageMgr();

	m_pCurrMultiWeapon = mgr->CreateImgSetCtrl( pScene, "CurrWeapon");
	m_pPrevMultiWeapon = mgr->CreateImgSetCtrl( pScene, "PrevWeapon");
	m_pNextMultiWeapon = mgr->CreateImgSetCtrl( pScene, "NextWeapon");

	m_pCurrSlotNumber = FindControl<i3UIButtonImageSet*>( pScene, "CurrSlotNumber");
	m_pPrevSlotNumber = FindControl<i3UIButtonImageSet*>( pScene, "PrevSlotNumber");
	m_pNextSlotNumber = FindControl<i3UIButtonImageSet*>( pScene, "NextSlotNumber");
}

void UIHudMultiWeapon::ClearAtUnload(i3UIScene * pScene)
{
	I3_SAFE_RELEASE( m_pCurrMultiWeapon);
	I3_SAFE_RELEASE( m_pNextMultiWeapon);
	I3_SAFE_RELEASE( m_pPrevMultiWeapon);
}

// ���� �����͸� ������ HUD�� �������ִ� �Լ�
void UIHudMultiWeapon::UpdateMultiWeaponSlot()
{
	MULTIWEAPON_INFO* pMultiWeapon = GetMyChara()->getMultiWeaponContext()->GetCurrSlotInfo();

	// ���� ���� ���� ����
	if ( pMultiWeapon)
	{
		_SetWeaponShapeImage( m_pCurrMultiWeapon, pMultiWeapon->pWeapon->getWeaponInfo()->GetItemID());
		m_pCurrSlotNumber->SetShapeIdx( pMultiWeapon->nSlotNumber - 1);
		m_pCurrSlotNumber->SetVisible();
		m_pCurrMultiWeapon->SetEnable( TRUE);
	}
	else
	{
		m_pCurrSlotNumber->SetVisible(FALSE);
		m_pCurrMultiWeapon->SetEnable( FALSE);
	}

	pMultiWeapon = GetMyChara()->getMultiWeaponContext()->GetNextSlotInfo();

	// ���� ���� ���� ����
	if ( pMultiWeapon)
	{
		_SetWeaponShapeImage( m_pNextMultiWeapon, pMultiWeapon->pWeapon->getWeaponInfo()->GetItemID());
		m_pNextSlotNumber->SetShapeIdx( pMultiWeapon->nSlotNumber - 1);
		m_pNextSlotNumber->SetVisible();
		m_pNextMultiWeapon->SetEnable( TRUE);
	}
	else
	{
		m_pNextSlotNumber->SetVisible(FALSE);
		m_pNextMultiWeapon->SetEnable( FALSE);
	}

	pMultiWeapon = GetMyChara()->getMultiWeaponContext()->GetPrevSlotInfo();

	// ���� ���� ���� ����
	if ( pMultiWeapon)
	{
		_SetWeaponShapeImage( m_pPrevMultiWeapon, pMultiWeapon->pWeapon->getWeaponInfo()->GetItemID());
		m_pPrevSlotNumber->SetShapeIdx( pMultiWeapon->nSlotNumber - 1);
		m_pPrevSlotNumber->SetVisible();
		m_pPrevMultiWeapon->SetEnable( TRUE);
	}
	else
	{
		m_pPrevSlotNumber->SetVisible(FALSE);
		m_pPrevMultiWeapon->SetEnable( FALSE);
	}
}

// HUD ������ �ٲ��ִ� �Լ�
void UIHudMultiWeapon::SetMultiWeaponSlotImage(BOOL bForward)
{
	CGameCharaMultiWeaponContext* pMultiWeaponContext = GetMyChara()->getMultiWeaponContext();
	
	if ( pMultiWeaponContext->GetMultiWeaponCount() <= 1)
		return;

	if ( m_bAutoView)
		OffAutoClosed();

	UINT8 idx = GetCurrentSlotIdx();

	if ( bForward)
	{
		if (pMultiWeaponContext->GetMultiWeaponCount() == idx)
			SetCurrentSlotIdx(1);
		else
			SetCurrentSlotIdx(idx + 1);
	}
	else
	{
		if ( idx == 1)
			SetCurrentSlotIdx( pMultiWeaponContext->GetMultiWeaponCount());
		else
			SetCurrentSlotIdx(idx - 1);
	}

	idx = GetCurrentSlotIdx();

	m_pCurrSlotNumber->SetVisible(FALSE);
	m_pCurrMultiWeapon->SetEnable( FALSE);
	m_pPrevSlotNumber->SetVisible(FALSE);
	m_pPrevMultiWeapon->SetEnable( FALSE);
	m_pNextSlotNumber->SetVisible(FALSE);
	m_pNextMultiWeapon->SetEnable( FALSE);

	MULTIWEAPON_INFO* pInfo = nullptr;

	pInfo = pMultiWeaponContext->SearchIndexSlot(idx);
	if (pInfo)
	{
		_SetWeaponShapeImage( m_pCurrMultiWeapon, pInfo->pWeapon->getWeaponInfo()->GetItemID());
		m_pCurrSlotNumber->SetShapeIdx( pInfo->nSlotNumber - 1);

		m_pCurrSlotNumber->SetVisible();
		m_pCurrMultiWeapon->SetEnable( TRUE);
	}

	if ( pMultiWeaponContext->GetMultiWeaponCount() < 2)
		return;

	pInfo = pMultiWeaponContext->GetPrevMultiWeaponInfo( idx);
	if ( pInfo)
	{
		_SetWeaponShapeImage( m_pPrevMultiWeapon, pInfo->pWeapon->getWeaponInfo()->GetItemID());
		m_pPrevSlotNumber->SetShapeIdx( pInfo->nSlotNumber - 1);

		m_pPrevSlotNumber->SetVisible();
		m_pPrevMultiWeapon->SetEnable(TRUE);
	}

	if ( pMultiWeaponContext->GetMultiWeaponCount() < 3)
		return;

	pInfo = pMultiWeaponContext->GetNextMultiWeaponInfo( idx);
	if ( pInfo)
	{
		_SetWeaponShapeImage( m_pNextMultiWeapon, pInfo->pWeapon->getWeaponInfo()->GetItemID());
		m_pNextSlotNumber->SetShapeIdx( pInfo->nSlotNumber - 1);

		m_pNextSlotNumber->SetVisible();
		m_pNextMultiWeapon->SetEnable( TRUE);
	}
}

// HUD On/Off �Լ�
void UIHudMultiWeapon::ShowMultiWeaponSlot( bool bAuto /* = false*/)
{
	I3ASSERT( GetMyChara() != nullptr);

	// �̹� ���� ���¿� �ڵ����� ���� ��� �ð��� ����
	if ( IsEnable() && bAuto)
	{
		m_fLifeTime = 0.f;
		return;
	}

	m_bAutoView = bAuto;

	SetEnable( !IsEnable());

	if ( IsEnable())
	{
		if ( GetMyChara()->getMultiWeaponContext()->GetCurrSlotInfo() == nullptr)
			SetEnable(false);
	}
	else
	{
		if ( GetMyChara()->getMultiWeaponContext()->GetCurrSlotInfo() != nullptr)
		{
			m_nCurrentSlotIdx = GetMyChara()->getMultiWeaponContext()->GetCurrSlotInfo()->nSlotNumber;
		}
		else
		{
			m_nCurrentSlotIdx = 1;
		}
	}
}

void UIHudMultiWeapon::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	SetEnable(false);
}

void UIHudMultiWeapon::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	SetEnable(false);
}

// �ش� Slot�� itemID�� �´� Shape ���
void UIHudMultiWeapon::_SetWeaponShapeImage(UIImgSetCtrl* ctrl, T_ItemID itemID)
{
	GetItemImageMgr()->SelectImage(ctrl, itemID);
}