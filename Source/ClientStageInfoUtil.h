#pragma once
#ifndef __CStageInfoUtil__
#define __CStageInfoUtil__

#include "CSI_TypeDef.h"

namespace CStageInfoUtil
{
	void	CreateCSIContextMapData();
	void	AddRuleDatatoCSICotext(MAP_RULE_DATA* ruledata);
	void	AddStageListtoCSICotext(MAP_STAGE_MATCHING* matchingdata);


	i3::vector< CSI_RULE* >&		GetRuleList(bool custom_list = false);
	CSI_STAGE_LIST					GetStageList(INT32 ruleidx);
	CSI_STAGE_LIST					GetStageListofEvent();
	CSI_STAGE_LIST					GetStageListofEvent(INT32 ruleidx);

	CSI_MAP*				GetMapData(STAGE_UID	stage_uid);
	CSI_MAP*				GetMapData(STAGE_ID		stageid	 );
	CSI_MAP*				GetMapData(const char*	map_code_name);
	
	CSI_RULE*				GetRuleData(INT32		ruleidx	 );
	CSI_RULE*				GetRuleData(STAGE_ID	stageid, bool isStageid = true);
	
	CSI_STAGE*				GetStageData(STAGE_ID	stageid	 );
	
	const INT32				GetServerRuleMaxIdx();

	/** \breif �Ű������� ���Ϳ� �ش� ��(UID)�� ����(����)�� ����� ���δ� PushBack ���ش�. */
	void					GatherRuleDatasAtUID(i3::vector< CSI_RULE*>* vec, STAGE_UID stage_uid);



	/****** CSI_STAGE������ �Ǻ��� �� �ִ� ���� �������� ���׵��� ����� ��� ���´�. ******/
	
	/** \breif ���Ը�带 Maxperson���� �Ǻ��Ͽ� �˷��ش�. */
	STAGE_SLOT_MODE	GetSlotMode(CSI_STAGE stage);

	/** \breif ��� ���� �����ϴ� ���� �ִ� ������ ���ø� �˴ϴ�.*/
	bool	IsAvailableStageMode(STAGE_MODE stagemode);

	/** \breif stageid�� eventŸ�� �˻��ϱ�*/
	bool	IsStageEventforStageId(STAGE_ID stageid, STAGE_EVENT eventtype);

	/** \breif �游��� �˾����� ������ ���� �⺻ ��带 ������ ��尡 ������ �ȵǴ� ������ �˻��ϴ� �Լ�. */
	bool	IsDiableChaneMode(CSI_STAGE stage);
	bool	IsDiableChaneMode(CSI_RULE  rule );

	/** \breif ��� �� ����� ���� �Ǵ� ���� �˻� �Լ�. */
	bool	IsModeRoundType(CSI_STAGE stage);

	/** \breif �������� �������� �Ǵ� ���� �˻� �Լ�. (IsRoundTypeandEternerlRespawn �� ����) */
	bool	IsEternerlRespawn(CSI_STAGE stage);

	/** \breif ���� ��� �� �������� �������� �Ǵ� ���� �˻� �Լ�. ex) ���̼�. */
	bool	IsRoundTypeandEternerlRespawn(CSI_STAGE stage);

	/** \breif ���������� ���� ������ ���ϴ� ���� �˻� �Լ�. */
	bool	IsFixedRoundTypeMode(CSI_STAGE stage);

	/** \breif �̼� ������Ʈ�� ����ϴ� ��� �˻� �Լ�. */
	bool	IsUsingMissionObject(CSI_STAGE stage);

	/** \brief ������ ���⿡ ���� ó�� ���� */
	bool	IsDeletableDroppedWeapon(CSI_STAGE stage);

	/** \brief Observer Mode ���� ����
		\note �ɼǿ� ���� Observer���·� �� �� �ִ� ���(GM Observer)�� �Ǵ��Ѵ�. */
	bool	IsObserverModeStage(CSI_STAGE stage);

	/** \brief Observer�� stage ���� 
		\note �������� �ѹ� ���� ���� ��带 �ɷ����ٰ� �����ϸ� �ȴ�. */
	bool	IsObserverModeStageByOnlyMode(CSI_STAGE stage);

	/** \breief KillMode? ���� ������ �̰ɷ� �� �Ÿ���. */
	bool	IsKillMode(CSI_STAGE stage);

	/** \breief ModeOption�� ���� SUB_TYPE ���� �Լ�. ( �̼�, �ð�) */
	UINT8	getMissionRoundType(CSI_STAGE stage);
};

#endif