#if !defined( __UI_LB_ProgressBar_H__)
#define __UI_LB_ProgressBar_H__

#include <set>

/*	ProgressBar�� Ư�� ������ ���� ������ �����ϴ� Ŭ����
 **/
class iColorProgressBar
{
public:
	iColorProgressBar();
	virtual ~iColorProgressBar();

	void SetControl(i3UIScene * pScene, const char* progressbar);
	void SetControl(i3UIControl* progressbar);
	void SetEnable(bool enable);

	void AddMaxColorSpot(int r, int g, int b); //�ִ밪 ����
	void AddColorSpot(int split_spot, int r, int g, int b);	//split_spot ����(����) �ش� �������� ����(�Ʒ���)

protected:
	const COLOR* find_color(int spot);

protected:
	i3UIProgressBar* m_ProgressBar;

protected:
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
	list m_list;
};

/*****************************/
//IntColorProgressBar
class IntColorProgressBar : public iColorProgressBar
{
public:
	IntColorProgressBar();
	virtual ~IntColorProgressBar() {}

	void SetRange(int min, int max);
	void SetNumber(int num);		//SetRange �� �Լ��� ���� �Ǿ�� �Ѵ�.
	void SetNumber(int num, int min, int max);

private:
	INT32 m_Min, m_Max, m_CurrNum;
};

/*****************************/
//RealColorProgressBar
class RealColorProgressBar : public iColorProgressBar
{
private:
	enum {FLOAT_FACTOR = 1000, FLOAT_INT = 100,};

public:
	RealColorProgressBar();
	virtual ~RealColorProgressBar() {}

	void SetRange(REAL32 min, REAL32 max);
	void SetNumber(REAL32 num);		//SetRange �� �Լ��� ���� �Ǿ�� �Ѵ�.
	void SetNumber(REAL32 num, REAL32 min, REAL32 max);

private:
	int m_Min, m_Max, m_CurrNum;
};


#endif