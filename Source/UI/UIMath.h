#pragma once

namespace uiMath{

//�ּҺ��� ������ �ּҰ� ��ȯ, �ִ뺸�� ũ�� �ִ밪 ��ȯ
template <typename T>
T limited(T val, T min_val, T max_val) 
{
    return std::max<T>(min_val, std::min<T>(max_val, val));
}

//�ּҺ��� ������ �ִ밪 ��ȯ, �ִ뺸�� ũ�� �ּҰ� ��ȯ
template <typename T>
T roundlimited(T val, T min_val, T max_val) 
{
	if( val > max_val )
		return min_val;
	else if( val < min_val )
		return max_val;

	return std::max<T>(min_val, std::min<T>(max_val, val));
}

//�ּ�, �ִ� �����ȿ��� dir ����(����(false), ����(true))���� step �̵��Ѵ�.
template <typename T>
T move_number(T val, T min_val, T max_val, bool dir, T step_val = 1, bool round = true) 
{
	if( max_val == min_val ) return min_val;

	if( round == true )
	{
		T tmp;
		if( dir == false ) tmp = val - step_val;
		else tmp = val + step_val;

		T result = limited(tmp, min_val, max_val);
		if( tmp == result ) return result;

		T ab;
		if( min_val > tmp )
		{
			ab = abs( min_val - tmp ) - 1;
			return max_val - ab;
		}

		ab = abs( tmp - max_val ) - 1;
		return min_val + ab;
	}

	if( dir == false ) return limited(val - step_val, min_val, max_val);
	return limited(val + step_val, min_val, max_val);
}

 
//���� ��ȯ �Ѵ�. (0~1���� ��, �ּ�, �ִ�) (0.5f, 10.0f, 20.0f) -> 15.0f
template <typename T>
T interpolate(float t, T min_val, T max_val) 
{
	return (T)(min_val*(1.-t) + max_val*t);
}
 
//0~1 ������ ��ȯ (��, �ּ�, �ִ�) (15.0f, 10.0f, 20.0f) -> 0.5f
template <typename T>
float inverse_interpolate(T val, T min_val, T max_val) 
{
    return 1.0f - ( (val-max_val) / (float)(min_val - max_val));
}

//Ư�� ��ġ�� ������ ��ȯ(���ο� �ּҰ�, ���ο� �ִ밪, ���� ��, ���� �ּҰ�, ���� �ִ밪
//(20.0f, 40.0f, 15.0f, 10.0f, 20.0f) -> 30.0f
template <typename T>
T remap_ranges(T new_min_val, T new_max_val, T old_middle_val, T old_min_val, T old_max_val) 
{
    float t = inverse_interpolate(old_middle_val, old_min_val, old_max_val);
    return  interpolate(t, new_min_val, new_max_val);
}

}