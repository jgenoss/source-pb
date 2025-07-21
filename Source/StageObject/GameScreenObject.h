#if !defined( _GAME_SCREEN_OBJECT_H__)
#define _GAME_SCREEN_OBJECT_H__

class GameScreenObject : public i3Object
{
	I3_CLASS_DEFINE( GameScreenObject, i3Object);

protected:
	i3CameraObject * m_pCamObj;

	//volatile
	i3Camera			*	m_pCam;
	i3TextureBindAttr	*	m_pTargetTextureAttr;

public:
	GameScreenObject(void);
	virtual ~GameScreenObject(void);

	virtual void	OnBindResource(void) override;

protected:
	void	_BindCameraObj();

public:
	void	SetCameraObj( i3CameraObject * pObj);
	i3CameraObject * getCameraObj( void)	{	return m_pCamObj;}

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

#if defined(USE_EDITDLG)//XML
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

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
};

#endif