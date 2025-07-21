#if !defined( __CREATEROOM_STAGE_H__)
#define __CREATEROOM_STAGE_H__

#include "CreateRoom_Base.h"

class CreateRoom_Stage : public CreateRoom_Base
{
	enum
	{
		MAPLIST_SLOT_COUNT = 3,
	};

public:
	CreateRoom_Stage(i3UIScene* pBase);
	~CreateRoom_Stage();

public:

	virtual void	InitData(bool isCreateRoom) override;
	virtual void	UpdateforRule(CSI_RULE* rule, bool firstupdate = false) override;
	virtual bool	ConfimRoomInfo(ROOM_INFO_ALL &info) override;

	CSI_STAGE*		GetSelectedStage();

	void			ClickMapSlot(int nSlot);
	void			MapListDBClick(int nSlot);
	void			MapListScroll();
	void			MapListWheel(int zDelta);
	void			OnMouseOverSlot(int nSlot, bool bMouseOver);

	void			ClickRandomMap();
	void			CancelRandomMap();

	void			FocuseStageWheel();

private:

	void			InitRandomMapList();
	void			StageUIUpdate();

private:

	i3UIScrollBar*	m_pMapScrollbar;
	UIMapSlot		m_MapSlot[MAPLIST_SLOT_COUNT];

	//�꿡 ���� �׻� ���� ������ �ϸ�, ����� ����.
	i3::vector<CSI_STAGE*> m_vStages;
	i3::vector<CSI_STAGE*> m_vRandomStages;

	CSI_STAGE*		m_pSelectedStage;

	//�������� ������� ���� UI������ ���� �� On/Off ���� ����.
	bool			m_bCurRandomMap;
};

#endif