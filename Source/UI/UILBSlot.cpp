#include "pch.h"
#include "UILBSlot.h"

#include "UIPhaseShop.h"
#include "UIMainFrame.h"

#include "ItemImageMgr.h"

/////////////////////////////////////////////////////////

namespace
{
	class UILBSlot_Callback_None : public UILBSlot_Callback
	{
	public:
		virtual void	OnLoadSlot( INT32 slot_idx, i3UIScene* slot_scn ) {}				// �ε�� ���, �ʱ�ȭ�� ȣ�� ��ġ..
		virtual void	OnUnloadSlot( INT32 slot_idx, i3UIScene* slot_scn) {}				// ��ε�� ��� ������ ȣ�� ��ġ..
		virtual void	OnEnabledSlot( INT32 slot_idx, i3UIScene* slot_scn) {}			// enableȣ��� ��� ������Ʈ�� ������ ä�� �Լ�..
		virtual void	OnDisabledSlot( INT32 slot_idx, i3UIScene* slot_scn) {}
		virtual void	OnEnterSlot( INT32 slot_idx, i3UIScene* slot_scn) {} 
		virtual void	OnLeaveSlot( INT32 slot_idx, i3UIScene* slot_scn) {}
		virtual bool	EnableMouseOver() { return true; }
	} g_callback_none;
}


UILBSlotSet::UILBSlotSet() : m_parent(nullptr), m_callback(&g_callback_none), m_offset_scn_idx(0), m_numRow(0), m_numCol(0), m_numSlot(0), m_numActiveSlot(0),
m_idx_slot_mouseover(-1), m_isEnableMouseOver(true), m_isEnableMouseOverFromCB(true), m_skip_slot(true)
{
	m_prevMousePos.x = m_prevMousePos.y = LONG_MAX; 
	m_offsetPos.x = 0;
	m_offsetPos.y = 0;
}

UILBSlotSet::~UILBSlotSet()
{
	
}

void	UILBSlotSet::OnCreate(UIBase* parent, INT32 row, INT32 col)
{
	m_parent = parent;	m_numRow = row;	m_numCol = col;	m_numSlot = row * col;
}

void	UILBSlotSet::RegisterSlotScene( const char* szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded )		// ���̸��� ��� �Լ� ���..
{
	m_offset_scn_idx = m_parent->GetSceneCount();
	for (INT32 i = 0 ; i < m_numSlot ; ++i )
	{
		m_parent->AddScene(szSlotScene, Lua_Func, bLoaded, !bLoaded);					// ������ �Ѳ����� �ε��Ҷ� ���� �������� �ϴ� �����ҽ����ϰ� ��������� �ߺ��ε��Ǵ°� �ƴѰ� �ϴ� ��..
	}
}

void	UILBSlotSet::SetVisible(bool bEnable)
{
	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		i3UIFrameWnd* pWnd = static_cast<i3UIFrameWnd*>( scn->getFirstChild() );
	//	pWnd->SetVisible(bEnable);
		pWnd->EnableCtrl(bEnable, true);
	}
}

void UILBSlotSet::OnLoadAllScenes( INT32 start_x,  INT32 start_y, UINT32 offset_x,  UINT32 offset_y)
{
	m_skip_slot = false;

	m_offsetPos.x = offset_x;
	m_offsetPos.y = offset_y;

	UpdatePosition( start_x, start_y, offset_x, offset_y);
	
	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		if (scn->isEnable() == false)
			scn->EnableScene(true);
	}

	for (INT32 i = 0 ; i < m_numSlot ; ++i )
	{
		m_callback->OnLoadSlot(i, m_parent->GetScene(m_offset_scn_idx + i) );
	}
}

void UILBSlotSet::UpdatePosition( INT32 start_x,  INT32 start_y, UINT32 offset_x,  UINT32 offset_y)
{
	I3ASSERT( m_parent != nullptr);

	i3UIControl* item_ctrl = static_cast<i3UIControl*>(m_parent->GetScene(m_offset_scn_idx)->getFirstChild());
	const VEC2D& item_size = *item_ctrl->getSize();

	REAL32 offsetX = item_size.x + offset_x;
	REAL32 offsetY = item_size.y + offset_y;

	for (INT32 i = 0, count = 0; i < m_numRow ; ++i )
	{
		for (INT32 j = 0; j < m_numCol; ++j, ++count)
		{
			item_ctrl = static_cast<i3UIControl*>(m_parent->GetScene(m_offset_scn_idx + count)->getFirstChild());
			item_ctrl->setPos(start_x + offsetX * REAL32(j), start_y + offsetY * REAL32(i));
		}
	}
}

void	UILBSlotSet::OnLoadAllScenes(i3UIControl* attach_pos, UINT32 offset_x,  UINT32 offset_y)
{
	// ������������ ��� ���� ��..
	attach_pos->CalcAccumulatedPos();

	UINT32 start_x = (UINT32)attach_pos->getAccumulatedPos()->x;
	UINT32 start_y = (UINT32)attach_pos->getAccumulatedPos()->y;

	OnLoadAllScenes( start_x, start_y, offset_x, offset_y );
}

void	UILBSlotSet::OnUnloadAllScenes()
{
	// �� �ȿ����� ����� ������ �ٸ����� �Ұ�ó�� �����ϰ� �ݹ�..

	if (m_idx_slot_mouseover != -1)			// �ҰŵǱ� ���� ���콺 ������ �����ִٸ� leave��ȣ�� �ֵ��� �Ѵ�..(������ �����̵� �ƿ� �����̹Ƿ� �ݿ��Ǳ�� ��ƴ�..)
	{
		INT32 prev_slot_mouseover = m_idx_slot_mouseover;
		m_idx_slot_mouseover = -1;
		m_numActiveSlot = 0;

		m_callback->OnLeaveSlot(prev_slot_mouseover, UILBSlotSet::GetSlotScene(prev_slot_mouseover) );
	}

	for (INT32 i = 0 ; i < m_numSlot ; ++i )
	{
		m_callback->OnUnloadSlot(i, m_parent->GetScene(m_offset_scn_idx + i) );
	}

	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		if(scn && scn->isEnable() == true)
			scn->EnableScene(false);
	}
}

void	UILBSlotSet::OnUpdate()
{
	if ( m_isEnableMouseOver == false) 	return;
	if( m_skip_slot )	return;
	
	bool res = m_callback->EnableMouseOver();
	if (res != m_isEnableMouseOverFromCB)
	{
		m_isEnableMouseOverFromCB = res;
		UILBSlotSet::_ProcessEnableMouseOver(res);
	}
	
	if (res == false)
		return;

	POINT pt = i3UI::getManager()->getCurrentMousePosition();
	
	if (pt.x != m_prevMousePos.x || pt.y != m_prevMousePos.y)		// OnMouseMove�� �ش��ϴ� ���콺 �̵� ����..
	{
		m_prevMousePos.x = pt.x;	m_prevMousePos.y = pt.y;

		INT32 prev_slot_mouseover = m_idx_slot_mouseover;
		
		UILBSlotSet::HitTestSet(pt.x, pt.y, m_idx_slot_mouseover);
		
		if ( m_idx_slot_mouseover != prev_slot_mouseover )
		{
			if ( prev_slot_mouseover != -1 )	// leaveó��..
			{
				m_callback->OnLeaveSlot(prev_slot_mouseover, 
					UILBSlotSet::GetSlotScene(prev_slot_mouseover) );
			}
			
			if ( m_idx_slot_mouseover != -1 )	// enteró��..
			{
				m_callback->OnEnterSlot(m_idx_slot_mouseover, 
					UILBSlotSet::GetSlotScene(m_idx_slot_mouseover) );
			}
		}
	}
}

void	UILBSlotSet::SetEnableMouseOver(bool bEnable)
{
	if (m_isEnableMouseOver == bEnable ) return;

	m_isEnableMouseOver = bEnable;

	UILBSlotSet::_ProcessEnableMouseOver(bEnable);
}

void	UILBSlotSet::_ProcessEnableMouseOver(bool bEnable)
{
	if (!bEnable)	// false  ���� ���, ���콺 �»��¶�� Leaveó���� ����..
	{
		if (m_idx_slot_mouseover != -1)
		{
			m_callback->OnLeaveSlot(m_idx_slot_mouseover, 
				UILBSlotSet::GetSlotScene(m_idx_slot_mouseover) );

			m_idx_slot_mouseover = -1;
		}
	}
	else	// ���� Ű�� ���, �׽�Ʈ�Ͽ� ������ ��� Enteró���� ����.
	{
		INT32 prev_slot_mouseover = m_idx_slot_mouseover;
		POINT pt = i3UI::getManager()->getCurrentMousePosition();
		UILBSlotSet::HitTestSet(pt.x, pt.y, m_idx_slot_mouseover);
		m_prevMousePos.x = pt.x;	m_prevMousePos.y = pt.y;

		if ( m_idx_slot_mouseover != prev_slot_mouseover )				
		{
			if ( prev_slot_mouseover != -1 )	// leaveó��..
			{
				m_callback->OnLeaveSlot(prev_slot_mouseover, 
					UILBSlotSet::GetSlotScene(prev_slot_mouseover) );
			}

			if ( m_idx_slot_mouseover != -1 )	// enteró��..
			{
				m_callback->OnEnterSlot(m_idx_slot_mouseover, 
					UILBSlotSet::GetSlotScene(m_idx_slot_mouseover) );
			}
		}
	}
}

void	UILBSlotSet::UpdateList(const INT32 size_list, const INT32 idx_scroll_start)
{
// �ܺ� ����Ʈ�� ���� ���� �� ���踦 �� �Ǻ��ؼ� enable, disable�� ��� ���� �����ؼ� ����..	
	const INT32 numSlot = m_numSlot;
	const bool  Is_Any_Empty_Slot =  (size_list - idx_scroll_start < numSlot);
	const INT32 Num_Visible = (Is_Any_Empty_Slot) ? size_list - idx_scroll_start : numSlot;
	
	m_numActiveSlot = Num_Visible;

	INT32 i = 0;		

	for ( i = 0; i < Num_Visible ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		m_callback->OnEnabledSlot(i, scn);
	}
	for ( ; i < numSlot; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		m_callback->OnDisabledSlot(i, scn);
	}
}

void	UILBSlotSet::SetEnable(INT32 slot, bool bEnable)
{
	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + slot);

	if ( bEnable )
		m_callback->OnEnabledSlot(slot, scn);		//enable�� ��쿡�� ������ �ݹ鿡��..�����ǵ� �ᱹ �� �ȿ��� �����ؼ� ó���� ��...
	else
		m_callback->OnDisabledSlot(slot, scn);
}

i3UIScene*	UILBSlotSet::GetSlotScene(INT32 slot) const
{ 
	return m_parent->GetScene(m_offset_scn_idx + slot); 
}


bool	UILBSlotSet::HitTestSet(INT32 x, INT32 y, INT32& outSlotIdx)
{
	// ���� ���� �ȿ��� ���÷� ��°� ���ҵ�... (���� ���� �����Ӱ� ������踦 ������ ������... 0�� ���� �������� ���;��Ѵ�..)
	outSlotIdx = -1;

	//�幰�� m_skip_slot���� ����� ������ �ȵǾ� OnUpdate()���� ����� �Ѿ���� ��찡 �ִ� �� �����ϴ�
	if (m_parent == nullptr)
	{
		I3PRINTLOG(I3LOG_VIEWER, "UILBSlotSet Parent is null");
		return false;
	}

	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx);
	if (scn == nullptr) 
		return false;
	
	i3UIControl* slot_frame = static_cast<i3UIControl*>(scn->getFirstChild());
	if ( slot_frame->isIgnoreInput() )			// ù���� ��ǲ ��Ȱ��ȭ���� ����..(���������Ե� ��Ȱ��ȭ�Ȱ����� ����)
		return false;

	const VEC3D& offsetPos = *slot_frame->getAccumulatedPos();			// 0������ ��ġ..
	const VEC2D& slot_size = *slot_frame->getSize();
	REAL32 size_x = slot_size.x + m_offsetPos.x;	//offst ũ�⸦ �߰��Ѵ�.
	REAL32 size_y = slot_size.y + m_offsetPos.y;

	REAL32 tmp = 0.0f;


	INT32 calc_col_idx = INT32(floorf( (REAL32(x) - offsetPos.x) / size_x ));		// f -> i ���� ���� ����..
	if ( calc_col_idx < 0 || calc_col_idx >= m_numCol ) 
		return false;

	tmp = offsetPos.x + (size_x*calc_col_idx);
	if( REAL32(x) - (tmp+slot_size.x) >= 0.0f)
		return false;

	INT32 calc_row_idx = INT32(floorf((REAL32(y) - offsetPos.y) / size_y));		// 
	if ( calc_row_idx < 0 || calc_row_idx >= m_numRow )
		return false;

	tmp = offsetPos.y + (size_y*calc_row_idx);
	if( REAL32(y) - (tmp+slot_size.y) >= 0.0f)
		return false;


	INT32 result_idx = calc_row_idx * m_numCol + calc_col_idx ;
	if (result_idx >= m_numActiveSlot) return false;

	if ( m_parent->GetScene(m_offset_scn_idx + result_idx)->isEnable() == false)
		return false;
	
	outSlotIdx = result_idx;

	return true;
}

bool	UILBSlotSet::HitTestSlot(INT32 slot, INT32 x, INT32 y)
{
	if (slot >= m_numActiveSlot)
		return false;

	i3UIScene* scn = GetSlotScene(m_offset_scn_idx + slot);

	if ( scn == nullptr) return false;

	if ( scn->isEnable() == false)
		return false;
	
	i3UIControl* pCtrl = static_cast<i3UIControl*>(m_parent->GetScene(m_offset_scn_idx + slot)->getFirstChild());
	
	if (pCtrl->isIgnoreInput())
		return false;

	VEC3D* ctrlPos = pCtrl->getAccumulatedPos();
	VEC2D* ctrlSize = pCtrl->getSize();

	REAL32 right	= ctrlPos->x + ctrlSize->x;
	REAL32 bottom	= ctrlPos->y + ctrlSize->y;

	if( REAL32(x) >=  ctrlPos->x && REAL32(x) <= right )
	{
		if( REAL32(y) >= ctrlPos->y && REAL32(y) <= bottom )
		{
			return true;
		}
	}
	return false;
}


const VEC2D&	UILBSlotSet::GetSlotSize() const
{
	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx);
	if( scn != nullptr)
		return	*static_cast<i3UIControl*>(scn->getFirstChild())->getSize();

	static VEC2D v;
	
	return v;
}

const VEC3D&	UILBSlotSet::GetSlotPos(INT32 slot) const
{
	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + slot);	
	return *static_cast<i3UIControl*>(scn->getFirstChild())->getAccumulatedPos();
}

void		UILBSlotSet::SetCallback(UILBSlot_Callback* callback) 
{ 
	m_callback = callback; 
	bool res = callback->EnableMouseOver();
	if ( res != m_isEnableMouseOverFromCB )
	{
		m_isEnableMouseOverFromCB = res;
		UILBSlotSet::_ProcessEnableMouseOver(res);
	}
}

void UILBSlotSet::MoveBackScene()
{
	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		scn->Attach(false);
	}
}

void UILBSlotSet::ReverseScene()
{
	for (INT32 i = m_numSlot-1 ; i >= 0; --i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		scn->Attach(false);
	}
}