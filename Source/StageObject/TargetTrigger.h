#if !defined( __TARGET_TRIGGER_H)
#define __TARGET_TRIGGER_H

#include "TargetObject.h"
#include "StageDef.h"

class TargetTrigger : public i3Trigger
{
	I3_CLASS_DEFINE( TargetTrigger, i3Trigger);
protected:
	i3::vector<TargetObject*>			m_ObjList;
	TEAM_TYPE		m_Team;

public:
	TargetTrigger(void);
	virtual ~TargetTrigger(void);

	INT32			getTargetCount(void)					{ return (INT32)m_ObjList.size(); }
	TargetObject *	getTarget( INT32 idx)					{ return m_ObjList[ idx]; }
	void			addTarget( TargetObject * pObj);
	void			removeTarget( TargetObject * pObj);
	void			removeAllTarget(void);

	TEAM_TYPE		getTeam(void)							{ return m_Team; }
	void			setTeam( TEAM_TYPE team)				{ m_Team = team; }

	virtual void	OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest) override;
	virtual void	OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

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
