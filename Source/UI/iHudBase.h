#pragma once

#include <unordered_map>

class UIHudManager;

class FrameList
{
public:
	void add(i3UIScene*, const char*);
	void add(i3UIFrameWnd*);

	void show();
	void show(size_t idx);
	
	void hide();
	void hide(size_t idx);

	void clear();

	const i3UIFrameWnd* getFrameWnd(size_t idx) const;
	i3UIFrameWnd* getFrameWnd(size_t idx);

private: 
	i3::vector< i3UIFrameWnd* > m_list;
};

class iHudBase
{
	friend class UIHudManager;

public:
	iHudBase(UIHudManager* parent);
	virtual ~iHudBase() {}

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) = 0;
	virtual void ClearAtUnload( i3UIScene * pScene) {}

	const i3::string & get_name() const { return m_name; }

	//���� ĳ���� ������
	void SetMyChara(CGameCharaBase* pMyChara) { s_pMyChara = pMyChara; }
	CGameCharaBase * GetMyChara(void)			{ return s_pMyChara; }

	//��Ʋ ���¿� ���� �ڵ� ȣ�� �Ǵ� �Լ��� ȣ�� �ϱ� ���鶧 ����ϸ� �ȴ�.
	virtual void SetParameter(INT32 arg1, INT32 arg2 = -1) {};

	//���� �۵� �� ȭ�� Ȱ��ȭ ���� - ���� �۵�(update, event, process_state ��..)�� ���Ѵ�.
	//SetEnable(false) �̸� ȭ�� ��Ȱ��ȭ �� ���� �۵� ��Ȱ��ȭ �ȴ�. 
	virtual void SetEnable(bool enable);
	virtual bool IsEnable() const { return m_enable; }

	//ȭ�鿡 �����ִ� ���� - ���� �۵�(update, event, process_state ��..) �� �Ѵ�.
	void SetVisible(bool visible);
	void SetVisibleFrame(bool visible, UINT32 idx);
	virtual bool IsVisible() const { return m_visivle; }

	//���� ������ hud�� ���� frame wnd �߰� �Լ�
	//�߰� �� �� hide �ȴ�.
	void AddFrameWnd(i3UIScene*, const char*);
	void AddFrameWnd(i3UIFrameWnd*);
	i3UIFrameWnd* GetFrameWnd(size_t idx = 0) { return m_FrameList.getFrameWnd(idx); }
	const i3UIFrameWnd* GetFrameWnd(size_t idx = 0) const { return m_FrameList.getFrameWnd(idx); }
	void ClearFrameWnd() { m_FrameList.clear(); }

public:
	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 Arg, const i3::user_data& UserData) {}
	virtual void Update( REAL32 rDeltaSeconds) {}
	virtual void OnRevive( i3RenderContext * pCtx) {}

public:
	void OnEvent(GAME_EVENT type, INT32, const i3::user_data& UserData);

protected:
	FrameList m_FrameList;		//���� hud main frame wnd
	i3::string m_name;	//lua �� �����ִ� �̸��̴�.

	UIHudManager* m_parent;
	bool m_enable;	//���� �۵� Ȱ��ȭ ����
	bool m_visivle;

	static CGameCharaBase* s_pMyChara;

protected:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) {}
	virtual void event_death_battle(INT32 arg, const i3::user_data& UserData) {}
	//�� �Լ��� event_respawn_battle �� �̺�Ʈ�� ó�� ȣ��Ǵ� �̺�Ʈ�̴�. ��, ���� ���۽ÿ� �ѹ��� ȣ��ȴ�.
	virtual void event_first_respawn_battle(INT32 arg, const i3::user_data& UserData) {} 
	virtual void event_respawn_battle(INT32 arg, const i3::user_data& UserData) {}
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) {}
	virtual void event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData) {}
	virtual void event_default(INT32 arg, const i3::user_data& UserData) {}

private:
	typedef void (iHudBase::*evt_fn)(INT32 arg, const i3::user_data& UserData);
	typedef i3::unordered_map< GAME_EVENT, evt_fn > container;
	static container m_evt_fn_map;

public:
	static void init_fn();
};


template <class T>
inline T FindControl(i3UIScene * pScene, const char* name)
{
	i3GameNode* node = pScene->FindChildByName( name );
	if( !node)
	{
		I3PRINTLOG(I3LOG_FATAL, "[%s] i3Uie ���� Ȯ��", name);
	}
	return static_cast<T>(node);
}
