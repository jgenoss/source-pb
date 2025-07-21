#ifndef _ReadyPhaseBase_h
#define _ReadyPhaseBase_h

#include "PhaseBase.h"

class CReadyPhaseBase: public CPhaseBase
{
	I3_ABSTRACT_CLASS_DEFINE(CReadyPhaseBase);
public:
	CReadyPhaseBase();
	virtual ~CReadyPhaseBase();

	//virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify) = 0;
	virtual void OnGameEvent(INT32 event,INT32 arg);	

protected:
	//���� ���� ä�� �޼����� ����.
	void ApplyMsgReadyBox();

	//������ : �������� �̸��� �˷��ش�.
	//useRandomMap : ������ ��� ����
	//stageID : �˰� ���� �������� Id
	//strMapName : ��ȯ ������ �������� �̸�
	void GetStageMapName(BOOL useRandomMap, STAGE_ID stageID,
		std::string & strMapName);

	//������ : �������� ��带 �˷��ش�.
	//stageType : �˰� ���� �������� Type
	//strMapName : ��ȯ ������ �������� ���
	//bAllowChangeMode : ��ȯ ������ ���� ��� ����(?)
	void GetStageModeName(STAGE_TYPE stageType,
		std::string & strModeName, BOOL & bAllowChangeMode);

protected:
	STAGEID_INFO_LIST::EnableStageList m_EnableStageList;

	void GetEnableMap(STAGEID_INFO_LIST::EnableStageList & EnableMap, 
		STAGE_TYPE StageType, 
		V_STAGEOPTION StageOptionType);

	void InitEnableMapList(STAGE_TYPE type, i3GuiPopupList* m_pMapList);

protected:
	typedef std::map< STAGE_TYPE, std::vector< UINT > > LimitTimeList;
	LimitTimeList m_LimitTimeList;

	void InitStageLimitTimeList();
	UINT8 _RotateTimeIndex(STAGE_TYPE stageType, UINT8 subType, BOOL bNext);


protected:
	void createRandomMap(STAGE_TYPE type);
};

#endif //_ReadyPhaseBase_h
