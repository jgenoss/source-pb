#if !defined( __LINK_OBJECT_TRIGGER_H__)
#define __LINK_OBJECT_TRIGGER_H__

class CLinkObjTrigger : public i3Trigger
{
	I3_CLASS_DEFINE( CLinkObjTrigger, i3Trigger);
protected:
	i3::vector<i3Object*>			m_ObjList;

public:
	CLinkObjTrigger(void);
	virtual ~CLinkObjTrigger(void);

	INT32			getLinkObjCount(void)					{ return (INT32)m_ObjList.size(); }
	i3Object *		getLinkObj( INT32 idx)					{ return m_ObjList[ idx]; }
	void			addLinkObj( i3Object * pObj);
	void			removeLinkObj( i3Object * pObj);
	void			removeAllLinkObj( void);

	virtual void	OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest) override;
	virtual void	OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest) override;
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

};

#endif
