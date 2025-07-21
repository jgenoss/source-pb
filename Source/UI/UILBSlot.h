#if !defined( __UI_LB_SLOT_H__)
#define __UI_LB_SLOT_H__

//
//
//  �켱, �����뽽���� UIBase�� i3GameNode���� ��� ������Ű�� ��������� �߰���ϵǴ� ���Ծ����κ��� Ŭ������ �и��ؼ� �����մϴ�...
//  �ݹ�Ŭ������ �ΰ�, ����ó���� ������ �۾�Ŭ������ �ǵ����� ����� �����غ��Ҵµ�...��������, ��ȸ�Ҹ��� ��ġ�� ���� �ʾƺ�����...�ٽ� ������ �����...

class UIBase;

class UILBSlot_Callback
{
public:
	virtual void	OnLoadSlot( INT32 slot_idx, i3UIScene* slot_scn ) = 0;				// �ε�� ���, �ʱ�ȭ�� ȣ�� ��ġ..
	virtual void	OnUnloadSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0;				// ��ε�� ��� ������ ȣ�� ��ġ..
	virtual void	OnEnabledSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0;			// enableȣ��� ��� ������Ʈ�� ������ ä�� �Լ�..
	virtual void	OnDisabledSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0;
	virtual void	OnEnterSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0; 
	virtual void	OnLeaveSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0;
	virtual bool	EnableMouseOver() = 0;
};

template<class T,
void (T::*ON_LOAD_SLOT)(INT32, i3UIScene*),
void (T::*ON_UNLOAD_SLOT)(INT32, i3UIScene*),
void (T::*ON_ENABLED_SLOT)(INT32, i3UIScene*),
void (T::*ON_DISABLED_SLOT)(INT32, i3UIScene*),
void (T::*ON_ENTER_SLOT)(INT32, i3UIScene*),
void (T::*ON_LEAVE_SLOT)(INT32, i3UIScene*),
bool (T::*ENABLE_MOUSEOVER)()
>

class TUILBSlot_Callback : public UILBSlot_Callback
{
public:
	TUILBSlot_Callback() : m_obj(nullptr) {}
	void			SetObj(T* obj) { m_obj = obj; }
private:
	virtual void	OnLoadSlot( INT32 slot_idx, i3UIScene* slot_scn ) override {	(m_obj->*ON_LOAD_SLOT)(slot_idx, slot_scn); }
	virtual void	OnUnloadSlot( INT32 slot_idx, i3UIScene* slot_scn) override {  (m_obj->*ON_UNLOAD_SLOT)(slot_idx, slot_scn); }
	virtual void	OnEnabledSlot( INT32 slot_idx, i3UIScene* slot_scn) override {  (m_obj->*ON_ENABLED_SLOT)(slot_idx, slot_scn); }
	virtual void	OnDisabledSlot( INT32 slot_idx, i3UIScene* slot_scn) override { (m_obj->*ON_DISABLED_SLOT)(slot_idx, slot_scn);  }
	virtual void	OnEnterSlot( INT32 slot_idx, i3UIScene* slot_scn) override { (m_obj->*ON_ENTER_SLOT)(slot_idx, slot_scn);  }
	virtual void	OnLeaveSlot( INT32 slot_idx, i3UIScene* slot_scn) override {  (m_obj->*ON_LEAVE_SLOT)(slot_idx, slot_scn); }
	virtual bool	EnableMouseOver() override { return (m_obj->*ENABLE_MOUSEOVER)();  }
	T*				m_obj;
};


class UILBSlotSet
{
public:
	UILBSlotSet();
	virtual ~UILBSlotSet();
	void SetEnableMouseOver(bool bEnable);
	bool IsEnableMouseOver() const { return m_isEnableMouseOver && m_isEnableMouseOverFromCB; }
	void	 SetCallback(UILBSlot_Callback* callback);
	void OnCreate(UIBase* parent, INT32 row, INT32 col);
	void RegisterSlotScene( const char* szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded = false);		// ���̸��� ��� �Լ� ���..

	void SetVisible(bool bEnable);
	void SetSkip(bool bSkip) { m_skip_slot = bSkip; }

	void	 OnLoadAllScenes( INT32 start_x,  INT32 start_y, UINT32 offset_x = 0,  UINT32 offset_y = 0);
	void	 OnLoadAllScenes(i3UIControl* attach_pos,  UINT32 offset_x = 0,  UINT32 offset_y = 0);

	void OnUnloadAllScenes();
	void	 OnUpdate();				// OnMouseMove�� ���� ������ �ܺ� �ݹ��� ������ �ʾƼ�, ���⼭ ���� �������� �����մϴ�...

	void	UpdatePosition( INT32 start_x,  INT32 start_y, UINT32 offset_x = 0,  UINT32 offset_y = 0);
	void	 UpdateList(const INT32 size_list, const INT32 idx_scroll_start);

	void	 SetEnable(INT32 slot, bool bEnable);

	bool HitTestSet(INT32 x, INT32 y, INT32& outSlotIdx);
	bool HitTestSlot(INT32 slot, INT32 x, INT32 y);

	INT32 GetRowCount() const { return m_numRow; }
	INT32 GetColCount() const { return m_numCol; }
	INT32 GetSlotCount() const { return m_numSlot; }
	INT32 GetActiveSlotCount() const { return m_numActiveSlot; }
	INT32 GetMouseOverSlot() const { return m_idx_slot_mouseover; }

	i3UIScene* GetSlotScene(INT32 slot) const;				// �ݹ����� ���� Enable, Load, Unload�� �����ϰ�� ���� ���� �����ؼ� ó���ϵ��� �մϴ�...

	const VEC2D&	GetSlotSize() const;
	const VEC3D&	GetSlotPos(INT32 slot) const;

	void MoveBackScene();		//��� slot�� �ֻ����� �����Ѵ�.
	void ReverseScene();		//��� slot�� ���� �������� �����Ѵ�.(slot �ȿ� combo box�� ���� ��쿡 ����ϸ� ����.)

	void	ResetMouseoverSlot() { m_idx_slot_mouseover = -1; m_prevMousePos.x = m_prevMousePos.y = LONG_MAX; }

protected:
	void			_ProcessEnableMouseOver(bool bEnable);
	UIBase*				m_parent;
	UILBSlot_Callback*	m_callback;
	INT32			m_offset_scn_idx;					// Parent�κ��� ���� ���� �����ϱ� ������ �� �̻� �迭�� ������ �ʾƵ� �ȴ�..
	INT32			m_idx_slot_mouseover;
	INT32			m_numRow, m_numCol, m_numSlot, m_numActiveSlot;
	POINT			m_prevMousePos;
	bool			m_isEnableMouseOver;
	bool			m_isEnableMouseOverFromCB;
	bool m_skip_slot;
	POINT m_offsetPos;
};

#endif