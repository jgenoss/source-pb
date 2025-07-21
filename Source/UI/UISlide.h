#if !defined( __UI_SLIDE_H__)
#define __UI_SLIDE_H__

//
//  ������ �����������캰 �ϰ� �����̵� �Ǵ� ���� �� ������������ ��Ϻ� �����̵� ������� �����մϴ�..
//

typedef void (*cb_SlideEvent)( void *, void *);
typedef void (*cb_EndEvent)();

class UISlide;

class UISlide_Callback 
{ 
public: 
	virtual void	OnSlideInOnce() = 0;								// ù��° ������Ʈ��..
	virtual void	OnSlidingIn(REAL32 delta_time) = 0;					// ��� �����̵� ������Ʈ����
	virtual void	OnSlideInDone() = 0;

	virtual void	OnSlideOutOnce() = 0;								// ù��° ������Ʈ��..
	virtual void	OnSlidingOut(REAL32 delta_time) = 0;				// ��� �����̵� ������Ʈ����
	virtual void	OnSlideOutDone() = 0;
};

template<class T,	void (T::*S_IN_ONCE)(), void (T::*S_IN_ING)(REAL32), void (T::*S_IN_DONE)(), 
					void (T::*S_OUT_ONCE)(), void (T::*S_OUT_ING)(REAL32), void (T::*S_OUT_DONE)()>
class TUISlide_Callback : public UISlide_Callback
{
public:

	TUISlide_Callback() : m_this(0) {}
	TUISlide_Callback(T* obj) : m_this(obj) {}
	void		SetObj(T* obj) { m_this = obj; }

private:
	virtual void	OnSlideInOnce() { (m_this->*S_IN_ONCE)();  }
	virtual void	OnSlidingIn(REAL32 delta_time) { (m_this->*S_IN_ING)(delta_time); }
	virtual void	OnSlideInDone() { (m_this->*S_IN_DONE)(); }

	virtual void	OnSlideOutOnce() { (m_this->*S_OUT_ONCE)(); }	
	virtual void	OnSlidingOut(REAL32 delta_time) { (m_this->*S_OUT_ING)(delta_time); }		
	virtual void	OnSlideOutDone() { (m_this->*S_OUT_DONE)(); }

	T*		m_this;
};

//
// �Ʒ� �߻�Ŭ������ �����̵忡 ����ϸ� �����Լ��� ����... �־��� ratio(�����̵����϶� 0~1, �ƿ��϶� 1~0���� ����, 
// 0�̸� ȭ�� �ܺ� ����, 1�϶� Ÿ�������� ���ҽ��������ΰ����� �����ϰ� �۾��մϴ�...)
//

struct UISlideMtd 
{
	virtual void operator()( i3UIControl* ctrl, REAL32 ratio) = 0;
	virtual void self_destroy() {}
	virtual void setOriginalPos( REAL32 x, REAL32 y) {}
	virtual void setOriginalSize( REAL32 width, REAL32 height) {}
};

namespace detail { class UISlideProcess; class UISlideProcess_Sliding;	class UISlideProcess_Delay; 
					class UISlideProcess_WaitLoad;	class UISlideProcess_Done;  }


class UISlide 
{
public:
	enum slider_type { L_TO_R, R_TO_L, T_TO_B, B_TO_T, POPUP, slider_max };

	UISlide();
	~UISlide();
	void			AddFrameWnd(i3UIControl* pWnd, slider_type t = R_TO_L);		// �̰� ���� �̸� �غ�� ������� �����̵�..
	void			AddFrameWnd(i3UIControl* pWnd, UISlideMtd* mtd);			// �̰� ����, �־��� ratio�� �������� �ܺο��� ���� ������ ���̶� �������ϴ�..
	void			AddFrameWnds( i3UIScene* pScn, slider_type t = R_TO_L);
	
	void			Update(REAL32 rDeltaTime);

	void			ResetOriginalPos();
	void			ClearFrameWndList();
	void			RemoveFrameWnd(i3UIControl* pWnd);
	void			RemoveFrameWnds(i3UIScene* pScn);

	void			Start(bool Is_In);
	void			ForceEnd();
	void			ReadyToStart(bool Is_In);			// Ư���� ���� �ƴϸ� ���� �ʽ��ϴ�.. �ʱ�ȭ�� ��Ű�� ���� ����..
	void			Restore();

	void			AddSyncSlide(UISlide* slide);	

	void			ResetTime();
	void			SetTargetTime(bool slide_in, REAL32 target_time) { m_target_time[slide_in] = (target_time > 0.f) ? target_time : 0.f; }
	void			SetInitDelayTime(bool slide_in, REAL32 delay_time) { m_init_delay_time[slide_in] = ( delay_time > 0.f) ? delay_time : 0.f; }
	void			SetCallback(UISlide_Callback* callback) { m_callback = callback; }
	void			ResetCallback();
	float			GetCurrRatio() const { return m_curr_ratio; }

	static UISlide_Callback*	GetCallback_Null();
	
	bool			IsSlideDelayWaitLoad() const;

	/** \brief ���� ��ġ�� �����Ѵ�. */
	void			SetOriginalPos( REAL32 x, REAL32 y);

	/** \brief ���� ũ�⸦ �����Ѵ� */
	void			SetOriginalSize( REAL32 width, REAL32 height);

private:

	void			_Init_Pos();
	
	UISlide_Callback*				m_callback;
	REAL32								m_target_time[2];
	REAL32								m_init_delay_time[2];

	i3::vector<i3UIControl*>			m_ctrl_list;
	i3::vector<UISlideMtd*>				m_mtd_list;
	
	UISlide*							m_parent_slide;
	i3::vector<UISlide*>				m_sync_slide_list;			// �ε��� ���ҽ���ٸ��� �ð��� ����ȭ�� ��������..

	REAL32								m_curr_time;
	REAL32								m_curr_ratio;
	class detail::UISlideProcess*		m_slider;
	bool								m_is_slide_in;
	bool								m_is_delay_waitload;
	bool								m_waitload_all_done;

	friend class	detail::UISlideProcess_Sliding;
	friend class	detail::UISlideProcess_Delay;
	friend class	detail::UISlideProcess_WaitLoad;
	friend class	detail::UISlideProcess_Done;

};


namespace UI {

//slide �� frame wnd�� �Ӽ��� ��Ÿ����.
struct tmSlideAtt
{
	i3UIFrameWnd* frame;
	i3UI3DViewBox * _3dBox;
	INT32 origin_x, origin_y;
	INT32 target_x, target_y;
	REAL32 origin_size_x, origin_size_y;
	REAL32 target_size_x, target_size_y;
	
	cb_SlideEvent	cbSlideEvent;
	cb_EndEvent		cbEndEvent;


	tmSlideAtt(i3UIFrameWnd* frm) : frame(frm), _3dBox(nullptr), cbSlideEvent(nullptr), cbEndEvent(nullptr)
	{
		origin_x = (INT32)frm->getPos()->x;	origin_y = (INT32)frm->getPos()->y;
		origin_size_x = frm->getSize()->x;	origin_size_y = frm->getSize()->y;
	}

	tmSlideAtt(i3UIFrameWnd* frm, void * pBox, cb_SlideEvent evt1 = nullptr, cb_EndEvent evt2 = nullptr) : frame(frm)
	{
		_3dBox = static_cast<i3UI3DViewBox*>(pBox);
		origin_x = (INT32)frm->getPos()->x;	origin_y = (INT32)frm->getPos()->y;
		origin_size_x = frm->getSize()->x;	origin_size_y = frm->getSize()->y;
		setCBSlideEvent(evt1);
		setCBEndEvent(evt2);
	}

	tmSlideAtt(i3UIFrameWnd* frm, i3UI3DViewBox * pBox) : frame(frm), _3dBox(pBox)
	{
		origin_x = (INT32)frm->getPos()->x;	origin_y = (INT32)frm->getPos()->y;
		origin_size_x = frm->getSize()->x;	origin_size_y = frm->getSize()->y;
		cbSlideEvent= nullptr;
		cbEndEvent	= nullptr;
	}

	tmSlideAtt(i3UIFrameWnd* frm, INT32 targ_x, INT32 targ_y) : frame(frm), target_x(targ_x), target_y(targ_y), cbSlideEvent(nullptr), cbEndEvent(nullptr)
	{	origin_x = (INT32)frm->getPos()->x;	origin_y = (INT32)frm->getPos()->y;	}

	tmSlideAtt(i3UIFrameWnd* frm, INT32 ori_x, INT32 ori_y, INT32 targ_x, INT32 targ_y) : frame(frm), origin_x(ori_x), origin_y(ori_y), target_x(targ_x), target_y(targ_y), cbSlideEvent(nullptr), cbEndEvent(nullptr)
	{}

	void set_target(INT32 x, INT32 y)	{	target_x = x;		target_y = y;	}
	void set_target_by_offset(INT32 offset_x, INT32 offset_y)	{	target_x = origin_x + offset_x;		target_y = origin_y + offset_y;	}
	void set_target_by_offset_and_size(INT32 offset_x, INT32 offset_y, REAL32 size_x, REAL32 size_y)
	{
		target_x = origin_x + offset_x;		target_y = origin_y + offset_y;
		target_size_x = size_x;				target_size_y = size_y;
	}

	void			setCBSlideEvent(cb_SlideEvent cb)	{	cbSlideEvent= cb;	}
	void			setCBEndEvent(cb_EndEvent cb)		{	cbEndEvent	= cb;	}
	cb_SlideEvent *	getCBSlideEvent()					{	return &cbSlideEvent;}
	cb_EndEvent*	getCBEndEvent()						{	return &cbEndEvent;	}
};

//slide ���
struct tmSlideMtd 
{
	virtual void operator()( tmSlideAtt* slide_att, REAL32 ratio) = 0;
};
//���� slide ���
struct tmLinearSlideMtd : public tmSlideMtd
{
	bool invert;
	virtual void operator()( tmSlideAtt* slide_att, REAL32 ratio);
};

//Size ���� �˾� ���
struct tmPopupSlideMtd : public tmSlideMtd
{
	bool invert;
	virtual void operator()(tmSlideAtt* slide_att, REAL32 ratio);
};

//slide manager�� �ϰ� ��� container
struct tmSlideAttContainer
{
	typedef i3::vector< tmSlideAtt* > SlideAttList;
	SlideAttList mList;	
	i3UIScene* scene;

	tmSlideAttContainer(i3UIScene* s) : scene(s) {}

	void add(const char* frm_name);
	void add(const char* frm_name, void * pBox, cb_SlideEvent, cb_EndEvent);

	void set_origin(INT32 x, INT32 y, bool move = true);
	void set_origin_by_offset(INT32 offset_x, INT32 offset_y, bool move = true);

	void set_target(INT32 x, INT32 y);
	void set_target_by_offset(INT32 offset_x, INT32 offset_y);

	void set_target_by_offset_and_size(INT32 offset_x, INT32 offset_y, REAL32 targetsize_x, REAL32 targetsize_y);
};

/*	Slide �Ϸ��Ŀ� ȣ��ȴ�.
*/
struct SCallBack
{ 
	virtual void Call() = 0;
	virtual void operator = ( const SCallBack& CF) { this->pObj = CF.pObj; }
	void* pObj;
	void set_obj(  void* obj) { this->pObj = obj;}
};

template <class T, void (T::*CallBack)()>
struct TCallBack : SCallBack
{		
//	typedef void (T::*Func)();
	void Call()
	{ 
		T* pThis = static_cast<T*>(pObj);
		(pThis->*CallBack)();
	}
};

/*	tmSlideAtt�� �����ϴ� Ŭ����
 *		start
 **/
class tmSlide
{
	typedef i3::vector< tmSlideAtt* > SlideAttList;
	SlideAttList mSlideAttList;

	REAL32 m_accumulate_time;
	REAL32 m_play_time;
	tmSlideMtd * m_slideMtd;

	SCallBack* m_pCallFunc;

public:
	tmSlide();
	~tmSlide();

	void add(const tmSlideAttContainer & list);
	tmSlideAtt* add(tmSlideAtt* att);
	void clear();

	// mtd : slide ���(tmSlideMtd�� ��� �޾Ƽ� ���ǵ� ����ü)
	// play_time : slide �ð�
	void start(tmSlideMtd * mtd, REAL32 play_time);  
	void OnUpdate(REAL32 rDeltaTime);

	void move_origin(); // ��ϵ� tmSlideAtt�� �������� �̵��Ѵ�.
	void move_target(); // ��ϵ� tmSlideAtt�� ��ǥ������ �̵��Ѵ�.
	void move_origin_and_size();
	void move_target_and_size();

	void update_origin_pos(INT32 x, INT32 y, bool move = true); // ��ϵ� tmSlideAtt�� ������ �����Ѵ�.
	void update_origin_pos_by_offset(INT32 offset_x, INT32 offset_y, bool move = true); // ��ϵ� tmSlideAtt�� ������ ���� ��ġ�� offset ��ŭ���� �����Ѵ�.

	void update_target_pos(INT32 x, INT32 y, bool move = true); // ��ϵ� tmSlideAtt�� ��ǥ���� �����Ѵ�.
	void update_target_pos_by_offset(INT32 offset_x, INT32 offset_y, bool move = true); // ��ϵ� tmSlideAtt�� ��ǥ���� ��ǥ�� ��ġ�� offset ��ŭ���� �����Ѵ�.

	void SetSlideEndCallBack( SCallBack* pCallFunc) { m_pCallFunc = pCallFunc; }
	void ResetSlideEndCallBack();
};


/*	tmSlideAtt�� �����ϴ� Ŭ����
 *		start
 **/
struct multi_slide_CallBack
{ 
	virtual void finish_multi_slide(size_t idx) = 0;
	void operator = ( const multi_slide_CallBack& CF) { this->pObj = CF.pObj; }
	void* pObj;
};

template <class T, void (T::*CallBack)(size_t idx)>
struct T_multi_slide_CallBack : multi_slide_CallBack
{		
	typedef void (T::*Func)(size_t idx);
	void finish_multi_slide(size_t idx)
	{ 
		T* pThis = static_cast<T*>(pObj);
		(pThis->*CallBack)(idx);
	}
};

struct multi_slide_att
{
	REAL32 play_time;
	tmSlideMtd* play_mtd;
};

class multi_tmSlide
{
public:
	multi_tmSlide();
	~multi_tmSlide();

public:
	tmSlide& get_slide() { return m_slide; } //tmSlide �� ���� operator�� ���� �ʴ´�. �� �Լ��� �޾Ƽ� ó������.
	void add_slide_att(tmSlideMtd* play_mtd, REAL32 play_time);

	void OnUpdate(REAL32 rDeltaTime);
	
	void start();

	void force_begin();
	void force_end();
	
	void SetSlideEndCallBack( multi_slide_CallBack* pCallFunc) { m_pCallFunc = pCallFunc; }

private:
	void apply_slide_att();

private:
	void slide_finish();
	TCallBack<multi_tmSlide, &multi_tmSlide::slide_finish> m_slide_callback;

private:
	tmSlide m_slide;
	size_t slide_att_index;
	i3::vector< multi_slide_att > multi_slide_att_list;
	multi_slide_CallBack* m_pCallFunc;
};

}	//end of namespace UI

#endif