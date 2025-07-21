#pragma once
//////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------//
//								Random Map system									//
//----------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////

#define	RANDOMMAP_MAX_COUNT			30			// ������ �ִ� ���� - ���ڸ��� �÷������(PopupRandomMap�� üũ�ڽ��� ����)

#include "ClientStageInfoUtil.h"

class RandomMapContext : public i3::shared_ginst<RandomMapContext>
{

public:
	RandomMapContext();
	~RandomMapContext();

	void			ClearStageList();
	void			AddStage(CSI_STAGE* stage);
	void			BackupStageList();

	bool			IsStageListEmpty() const;
	bool			Contain(CSI_STAGE* stage) const;
	size_t			GetStageListSize() const;
	CSI_STAGE*		GetStage(size_t idx) const;

	// �����κ��� ���� ������ �������� ����Ʈ ����(���� �ΰ� �Ǵ� �Խ�Ʈ�� �뿡 ���� ��)
	void			TakeOverStageList();
	bool			IsStageListChanged() const;

	STAGE_MODE		GetRandomStageType() const;
	void			SetRandomStageType(STAGE_MODE val);

	STAGE_MODE		GetPrevStageType() const;
	void			SetPrevStageType(STAGE_MODE val);

	void			setPrevIndex(INT32 Prev);
	INT32			getPrevIndex() const;

private:
	i3::vector<CSI_STAGE*> m_StageOrdList;
	i3::vector<CSI_STAGE*> m_OldStageOrdList;
	
	STAGE_MODE		m_RandomStage;
	STAGE_MODE		m_PrevStage;

	INT32			m_iPrevMap;
};