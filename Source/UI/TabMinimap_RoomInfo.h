#pragma once
class TabMinimap_RoomInfo : public i3::shared_ginst<TabMinimap_RoomInfo>
{
public:
	TabMinimap_RoomInfo() {}
	~TabMinimap_RoomInfo() {}

	// ���� Kill Count�� �¸� ���ǿ� �ִٸ�, �ش� ������ ��ȯ�Ѵ�. �׷��� �ʴٸ� 0�� ��ȯ�Ѵ�.
	INT32				_GetLimitKillCount(void);
	// Round ���� Stage�� ���, ��ü Round ȸ���� ��ȯ�Ѵ�.
	INT32				_GetTotalRoundCount(void);
};