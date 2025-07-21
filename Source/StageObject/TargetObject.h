#if !defined( __TARGET_OBJECT_H)
#define __TARGET_OBJECT_H

class TargetObject : public i3Object
{
	I3_CLASS_DEFINE( TargetObject, i3Object);
public:
	enum TYPE
	{
		TYPE_ENEMY = 0,
		TYPE_FRIEND,
		TYPE_RANDOM
	};

	enum APPEAR_TYPE
	{
		APT_POPUP = 0,
		APT_SLIDE_LEFT,
		API_SLIDE_RIGHT,
	};

	enum STATE
	{
		STATE_SLEEP = 0,				// ���� ���� (��Ȱ��ȭ ����)
		STATE_START_ACTIVE,				// �Ͼ�� ����.
		STATE_ACTIVE,					// ������ �Ͼ ����
		STATE_START_SLEEP,				// �ı����� ���� ���¿��� ���� ����.
		STATE_DESTROY,
		STATE_FINISH,
	};

protected:
	// Persistant members
	TYPE			m_Type;
	APPEAR_TYPE		m_Appear;
	REAL32			m_timeOffset;
	REAL32			m_timeStart;
	REAL32			m_timeKeep;
	REAL32			m_timeEnd;
	
	// Volatile members
	TYPE			m_ActualType;			// Random������...
	STATE			m_State;
	STATE			m_OldState;
	REAL32			m_timeLocal;
	bool			m_bSmall;
	bool			m_bDestroyed;

	i3Animation *	m_pAnimStart;
	i3Animation *	m_pAnimKeep;
	i3Animation *	m_pAnimEnd;

protected:
	void			_FindAnimForPopup(void);
	void			_FindAnimForSlide(void);

public:
	TargetObject(void);
	virtual ~TargetObject(void);

	TYPE			getType(void)						{ return m_Type; }
	void			setType( TYPE type)					{ m_Type = type; }

	APPEAR_TYPE		getAppearType(void)					{ return m_Appear; }
	void			setAppearType( APPEAR_TYPE type)	{ m_Appear = type; }

	REAL32			getOffsetTime(void)					{ return m_timeOffset; }
	void			setOffsetTime( REAL32 tm)			{ m_timeOffset = tm; }

	REAL32			getStartTime(void)					{ return m_timeStart; }
	void			setStartTime( REAL32 tm)			{ m_timeStart = tm; }

	REAL32			getKeepTime(void)					{ return m_timeKeep; }
	void			setKeepTime( REAL32 tm)				{ m_timeKeep = tm; }

	REAL32			getEndTime(void)					{ return m_timeEnd; }
	void			setEndTime( REAL32 tm)				{ m_timeEnd = tm; }

	STATE			getState(void)						{ return m_State; }

	void			Start(void);
	void			Reset(void);

	virtual void	Damage( REAL32 dmg, i3GameObj * pEventSender, i3PhysixShapeSet * pHitShapeSet) override;

	void			OnSleep( REAL32 tm, bool bFirst);
	void			OnStartActive( REAL32 tm, bool bFirst);
	void			OnActive( REAL32 tm, bool bFirst);
	void			OnStartSleep( REAL32 tm, bool bFirst);
	void			OnDestroy( REAL32 tm, bool bFirst);
	
	virtual void	OnBindResource(void) override;
	virtual void	OnChangeTime( REAL32 tm) override;
	virtual void	OnUpdate( REAL32 tm) override;
	virtual	void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

#if defined(I3_DEBUG)//XML
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
#endif

	//
	// Level Tool���� ���� �������� �߰� Data�� �����ϱ� ���� ȣ���Ѵ�.
	// OnQueryGameData() �Լ� �������� �ʿ��� �޸𸮸� �Ҵ��� ��ȯ�ؾ� �ϸ�
	// ������ �Ҵ�� �޸��� ũ�⸦ ��ȯ�Ѵ�.
	//
	// Level Tool�� ��ȯ�� �޸𸮸� i3MemFree() �Լ��� ���� �����ϱ� ������
	// �ݵ�� i3MemAlloc() �Լ��� ���� �Ҵ�� �޸𸮿��� �Ѵ�.
	//
	// Paramters
	//		pData	[OUT]		������ �����͸� ���� �Ҵ��� �޸� ����. �Լ� ������ �Ҵ�ȴ�.
	//
	// Return
	//		�Ҵ�� �޸� ������ ũ�⸦ Byte ������ ��ȯ�Ѵ�.
	//
	virtual UINT32		OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;

	//
	// i3GameSceneInfo Class���� ���ӿ� Instance�� Loading�ϴ� �߿� ȣ���Ѵ�.
	// OnQueryGameData() �Լ��� ���޵� �Ͱ� ������ ó���� �ؾ��Ѵ�.
	// ������ �о���� �޸𸮸� ��ȯ�ؾ� �ϸ�, ���� ó�� �� Error�� �߻��ߴٸ�
	// STREAM_ERR�� ��ȯ�ؾ� �Ѵ�.
	virtual UINT32		OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
};

#endif
