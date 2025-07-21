#if !defined( __UI_LB_NumberCount_H__)
#define __UI_LB_NumberCount_H__

/*	i3UIButtonImageSet�� �̿��ؼ� number�� ǥ���Ѵ�.
 **/
class Number
{
	i3UIButtonImageSet* m_ImgBox;
	bool m_Enable;
	
public:
	Number();
	~Number();

	void SetControl(i3UIScene * pScene, const char* name);
	void SetControl(i3UIButtonImageSet* ctrl);
	i3UIButtonImageSet* GetControl()	{ return m_ImgBox; }

	void SetNumber(INT32 num);
	INT32 GetNumber() const;

	void Enable(bool enable);
	bool IsEnable() const;
};

/*	i3UIButtonImageSet�� �̿��ؼ� number�� ǥ���Ѵ�.
 *		AddNumberControl�� ���� �ڸ����� ��Ÿ����. ù��°�� 1�� �ڸ�, �ι�°�� 10�� �ڸ��̴�.
 *		������ ��ġ�� �ڵ� ��ġ ���� �ȴ�.
 *		EnableShowFrontZero �Լ� : //001 -> 1 �� ǥ�� ����
 *		EnableShowZero �Լ� : 0 �� ǥ�� ����
  **/
class NumberCount
{
public:
	NumberCount();
	virtual ~NumberCount();

	void ClearNumber();

	void SetVisible(bool visible);

	void AddNumberControl(i3UIScene * pScene, const char* name);
	void AddNumberControl(i3UIButtonImageSet* ctrl);

	void RemoveNumberControl(size_t idx);
	Number* DetchNumberControl(size_t idx);
	void AttachNumberControl(size_t idx, Number* ctrl);

	void SetNumber(INT32 num);
	INT32 GetNumber();
	void ResetDisplay() { m_CurrNum = INT_MIN; }

	void EnableShowFrontZero(bool show) { m_is_show_front_zero = show; }	//1 -> 001 �� ǥ�� ����
	void EnableShowZero(bool show) { m_is_show_zero = show; }	//0(zero) �� ǥ�� ����
	
	void EnableDynamicCenter(); //center�� �߽����� number �� ��������.
	void DisableDynamicCenter();

	void SetWidthOffset(INT32 offset); //Control ���� ����.
	void InitialPosOffset(INT32 x); //��� Control�� �̵��Ѵ�.

	void ShowRound();
	void HideRound();

	void SetColor(UINT8 r, UINT8 g, UINT8 b);
	void SetAlpha(UINT8 alpha); //0(����) ~255(������) ��

	const VEC3D* GetNumberPos() const;
	REAL32 GetNumberHeight() const;

private:
	void get_initial_position();
	void re_position();

private:
	i3::vector< Number* > m_list;	//0�ε����� 1�ڸ�
	Number* m_Round_ctrl;
	INT32 m_CurrNum;
	bool m_is_show_zero;	//0 ǥ�� ����
	bool m_is_show_front_zero;	//01, 1 ǥ�� ����
	bool m_is_center_pos;

	struct context
	{
		INT32 mini;
		INT32 maxi;
	};
	context m_range;
	INT32 m_max_width;
	INT32 m_initial_offset, m_width_offset;
};

#include <set>

class ColorNumberCount : public NumberCount
{
public:
	ColorNumberCount();
	virtual ~ColorNumberCount();

public:
	void SetRange(int min, int max);
	void SetNumber(int num);		//SetRange �� �Լ��� ���� �Ǿ�� �Ѵ�.
	void SetNumber(int num, int min, int max);

	void AddMaxColorSpot(int r, int g, int b); //�ִ밪 ����
	void AddColorSpot(int split_spot, int r, int g, int b);	//split_spot ����(����) �ش� �������� ����(�Ʒ���)

private:
	const COLOR* find_color(int spot);

private:
	struct spot
	{
		spot(int s, int r, int g, int b) : split_spot(s) { col.a = 255; col.r = (UINT8)r; col.g = (UINT8)g; col.b = (UINT8)b; }
		int split_spot; COLOR col;
	};

	struct comp
	{
		bool operator()( const spot& lhs, const spot & rhs ) const 
		{ return lhs.split_spot < rhs.split_spot; }
	};

	typedef std::set< spot, comp > list;
	list m_color_list;

private:
	INT32 m_Min, m_Max, m_CurrNum;
};

#endif