#if !defined( __CTRACER_BULLET_H)
#define __CTRACER_BULLET_H

//#define DEBUG_TRACER_BULLET

enum TRACER_TYPE
{
	TRACER_TYPE_UNKNOWN		= -1,
	TRACER_TYPE_BULLET = 0,			//	�Ϲ� �Ѿ� ����
	TRACER_TYPE_HAND_GRENADE,		//	����ź ����
	TRACER_TYPE_SHELL_GRENADE,		//	��ź ����
	TRACER_TYPE_ARROW,				//	ȭ�� ����
	TRACER_TYPE_SMOKE,				//	����ź ����
	TRACER_TYPE_FLASH,				//	����ź ����
	TRACER_TYPE_BULLET_SMOKE,		//	�Ϲ� �Ѿ� ����

	TRACER_TYPE_MAX
};

struct TracerBulletInfo
{
	TRACER_TYPE _Type = TRACER_TYPE_UNKNOWN;			//	Ÿ�� (��� ���)
	UINT32 _Style = 0;	
	VEC3D _vBeginPos;			//	���� ��ġ (��� ���)
	VEC3D _vDestPos;			//	�� ��ġ (��� ���)
	I3COLOR _color;				//	�ؽ�ó ���� (��� ���)
	REAL32 _LifeTime = 0.0f;			//	���� �ð� (����: ��) (TRACER_TYPE_BULLET�� ��� ����)
	REAL32 _Thickness = 0.0f;			//	�β� (����: m) (��� ���)
	REAL32 _GrowThickness = 0.0f;		//	���İ� ������� ���� ���Ʒ��� ������ �β� (����: m)	(TRACER_TYPE_BULLET_SMOKE�� �����)
	REAL32 _StartSpeed = 0.0f;			//	������ �̵� �ӵ� (�ѱ�) (����: m/s)	(TRACER_TYPE_BULLET_SMOKE�� �����)
	REAL32 _EndSpeed = 0.0f;			//	���� �̵� �ӵ� (Ÿ����) (����: m/s) (��� ���)
	REAL32 _Length = 0.0f;				//	���� (����: m) (TRACER_TYPE_BULLET_SMOKE�� ��� ����)
	REAL32 _RepeatTexSize = 0.0f;		//	�ؽ�ó�� Ÿ�ϸ� �Ǵ� ���� ���� (����: m) (TRACER_TYPE_BULLET_SMOKE�� �����)
};


class CTracerBullet : public i3GameObj
{
	I3_CLASS_DEFINE( CTracerBullet, i3GameObj);

protected:

	TracerBulletInfo	m_Info;

	INT32			m_nIdx = 0;			//	�Ѿ� �ε���
	INT32			m_nSrcAlpha = 255;	//	������ ���� ���İ�
	bool			m_bActive = false;		//	Ȱ��ȭ ����

	VEC3D			m_vDir;			//	����
	VEC3D			m_vCurStart;	//	���� ���ư��� �Ѿ� ���� ����
	VEC3D			m_vCurEnd;	//	���� ���ư��� �Ѿ� �� ����
	
	REAL32			m_fCurThickness = 0.0f;	//	�β�
	REAL32			m_fDestLengthSq = 0.0f;
	REAL32			m_fCurLengthSq = 0.0f;
	REAL32			m_fGapLengthSq = 0.0f;
	REAL32			m_fCurTime = 0.0f;

	i3Billboard *	m_pBillboard = nullptr;

public:
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	bool	Create( INT32 Index);
	bool	Start( i3Billboard * pImage, TracerBulletInfo * pInfo);	

	void	SetColor( COLOR * pColor);
	void	SetColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a);
	void	SetAlpha(UINT8 a);
	void	Reset( bool bEnable = false);

	TracerBulletInfo * GetInfo(void)				{	return &m_Info;						}
	INT32			GetIdx( void)					{	return m_nIdx;						}
	VEC3D *			GetDir( void)					{	return &m_vDir;						}
	//	Total �Ѿ��� ���ư����ϴ� �Ÿ�
	REAL32			GetDestLength( void)			{	return i3Math::Sqrt( m_fDestLengthSq);		}
	REAL32			GetCurLength( void);			//	���������� ������� ���ư� �Ÿ�
	REAL32			GetRemainLength( void)			{	return (GetDestLength() - GetCurLength());	}
	
	bool			IsActive( void)					{	return m_bActive;					}
	//	�Ѿ��� �����ߴ��� ����
	bool			IsArrive( void)					
	{	
		switch( m_Info._Type)
		{
		case TRACER_TYPE_BULLET:		
			{
				return (m_fDestLengthSq <= m_fCurLengthSq);	
			}
			break;
		case TRACER_TYPE_HAND_GRENADE:
		case TRACER_TYPE_SHELL_GRENADE:
		case TRACER_TYPE_ARROW:
		case TRACER_TYPE_SMOKE:
		case TRACER_TYPE_FLASH:
		case TRACER_TYPE_BULLET_SMOKE:
			{
				return (m_nSrcAlpha <= 0);	
			}
			break;
		}	

		return false;	
	}

protected:
	void	OnActive( REAL32 tm);

};

#endif