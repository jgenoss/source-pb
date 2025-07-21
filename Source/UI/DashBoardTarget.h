#pragma once

namespace dboard //dash board
{
	struct target
	{
	public:
		target() {}
		virtual ~target() {}

		virtual void set_enable(bool enable) = 0;

		void init_control(i3UIScene* pScn, const char* parent, const char* targ, const char* bg, const char* progress, bool bMinimapUpdate = true);

		void update(REAL32 rDeltaSeconds);
		void update_warning(REAL32 rDeltaSeconds);
		void update_color();
		
		void set_bg_idx(INT32 idx, bool enable); //�� ��� �� ���� �� warning �� Ȱ��ȭ ����
		void set_target_normal_shape_idx(INT32 idx); //target ���� �ε���
		void set_target_die_shape_idx(INT32 idx); //target ���� �� �ε���
		void set_minimap_warning_idx(INT32 idx);
		void set_progress_invert();
		void set_enable_update(bool enable);
		void reset_target_shape();

		void set_hp(INT32 hp, INT32 hp_max);

	protected:
		i3UIFrameWnd* pImgSet_Wnd; //pImgSet_Bg, pImgSet_Target, pImgSet_Target �θ�
		i3UIButtonImageSet* pImgSet_Bg; //target�� ���� bg
		i3UIButtonImageSet* pImgSet_Target; //target (�Ѵ� ���)
		i3UIProgressBar* pProgressgBar;

		INT32 hp, max_hp, lastest_hp;
		INT32 delta_time, warning_cnt;
		INT32 minimap_idx; //warning ���� �� minimap ǥ�� index
		INT32 target_normal_shape_idx, target_die_shape_idx;	//progress bar�� 0 �� �Ŀ� target shape index

		bool enable_target;
		bool is_after_warning_bg_enable; //warning �� �� ������ ���� ���ؼ� turn �Ŀ� ǥ�� ����
		bool is_update_warning; //���� ���ϴ� ����
		bool is_finish_update; //update ����
		bool is_minimap_update; // warning�� minimap ǥ�� ����
	};

	struct target_1_5 : public target
	{
	public:
		target_1_5() {}
		virtual ~target_1_5() {}

		virtual void set_enable(bool enable);
	};

	struct target_1_0 : public target
	{
	public:
		target_1_0() {}
		virtual ~target_1_0() {}

		virtual void set_enable(bool enable);
	};
}