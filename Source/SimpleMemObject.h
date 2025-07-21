#ifndef __SIMPLEMEMOBJECT_H__
#define	__SIMPLEMEMOBJECT_H__

// 
// ���� �ڵ� ���� �Ǵ� �Լ� ��������
// �޸𸮸� �Ҵ��ϰ� ���� ������ ��ü���� �ñ�� ���ؼ� ���� Ŭ����.
// ** ���۷��� ī������ �ȵ˴ϴ�.
//

template <typename T>
class CSimpleMemObject
{
public:
	CSimpleMemObject(T * p = nullptr, bool IsArray = false) : m_Ptr(p), m_IsArray(IsArray) {}
	~CSimpleMemObject(void)
	{
		Release();
	}

public:
	void	Set(T * p)
	{
		Release();
		m_Ptr = p;
	}

	operator T* ()
	{
		return m_Ptr;
	}

	template <typename _T>
	operator _T * ()
	{
		return reinterpret_cast<_T*>(m_Ptr);
	}

	void	Release()
	{
		if ( ! m_Ptr )
			return;
		if ( m_IsArray )
		{
			I3_SAFE_DELETE_ARRAY( m_Ptr);
		}
		else
		{
			I3_SAFE_DELETE( m_Ptr);
		}
	}

private:
	T * m_Ptr;
	bool m_IsArray;
};


#endif	// __SIMPLEMEMOBJECT_H__
