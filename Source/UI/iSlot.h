#if !defined( __UI_interface_Slot_H__)
#define __UI_interface_Slot_H__

#include "UIBase.h"
#include "UILBScrollSlot.h"

class UIImgSetCtrl;

namespace UI	{

enum {NON_SLOT_INDEX = -1,};

/* slot�� Ưȭ�� Ŭ���� �Դϴ�.
	1. UILBScrollSlotSet(�ڵ� ��ũ�ѹ�), UIImgSetCtrl(�̹���), ���콺 Ŀ�� slot ��ġ�� �����ϴ� Ŭ����
	2. ��� �޾Ƽ� ����ϸ� �˴ϴ�.
 **/
class iSlot
{
	typedef iSlot me;

public:
	iSlot();
	virtual ~iSlot();

protected:
	UILBScrollSlotSet mSlot;
	UIImgSetCtrl** m_ppImgSet;

private:
	INT32 m_old_slot_idx, m_now_slot_idx;
	i3::vector<bool> mSelectedList;

protected:
	void set_select(INT32 idx);
	void reset_select(size_t size);

	INT32 apply_select();
	bool is_selected(size_t idx) const	
	{
		if( mSelectedList.empty() == true ) return false;
		return mSelectedList[idx];	
	}

	INT32 get_now_slot() const { return m_now_slot_idx; }
	INT32 get_old_slot() const { return m_old_slot_idx; }
	INT32 get_able_select_slot_size() const { return mSelectedList.size(); }

protected:
	//iSlot �����Լ�
	virtual void cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn ) {}
	virtual void cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn ) {}
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) {}
	virtual void cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) {}
	virtual void cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn ) {}
	virtual void cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn ) {}

private:
	//slot �ݹ� �Լ�
	void cb_Load( INT32 slot_idx, i3UIScene* slot_scn);
	void cb_Unload( INT32 slot_idx, i3UIScene* slot_scn);
	void cb_Enabled( INT32 slot_idx, i3UIScene* slot_scn);
	void cb_Disabled( INT32 slot_idx, i3UIScene* slot_scn);
	void cb_Enter( INT32 slot_idx, i3UIScene* slot_scn);
	void cb_Leave( INT32 slot_idx, i3UIScene* slot_scn);
	bool cb_Istooltip();

	TUILBSlot_Callback< me, 
		&me::cb_Load, &me::cb_Unload, 
		&me::cb_Enabled, &me::cb_Disabled,
		&me::cb_Enter, &me::cb_Leave, 
		&me::cb_Istooltip > mSlotCb;

private:
	//scroll bar �ݹ� �Լ�
	INT32 _notifyScroll( INT32 idx, INT32 old_idx );

	typedef TUILBScrollSlot_Callback< iSlot, &iSlot::_notifyScroll > ScrollCallback;
	friend class ScrollCallback;
	ScrollCallback m_ScrollCallback;
};

}	//end of namespace UI_SMenu
#endif
