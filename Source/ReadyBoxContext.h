#pragma once
//----------------------------------------------------------------------------------//
//										ReadyBox														//
//----------------------------------------------------------------------------------//

class ReadyBoxContext : public i3::shared_ginst<ReadyBoxContext>
{
	//�ۼ��� : ������

public:
	//���� �����Ŀ� ��� �濡�� ReadyBox ȹ�� �� ��� ����
	//ä��â�� �˸��� ��� ��
	void SetReadyBoxMsg(UINT8 Type, UINT32 ServerIdx, UINT32 ChannelNum,
		UINT32 RoomIdx, T_ItemID ItemID, const char* NickName);

	bool EmptyReadyBoxMsg() const;
	size_t GetReadyBoxMsg_Size() const;

//	SUPPLY_BOX_TYPE GetReadyBoxMsg_Type(size_t index) const;				// �������� �Ⱦ��ٰ� ���� (2014.07.21.����)
	UINT32 GetReadyBoxMsg_ServerInx(size_t index) const;
	UINT32 GetReadyBoxMsg_ChannelNum(size_t index) const;
	UINT32 GetReadyBoxMsg_RoomIdx(size_t index) const;
	UINT32 GetReadyBoxMsg_ItemID(size_t index) const;
	const i3::rc_string & GetReadyBoxMsg_Nick(size_t index) const;

	//ReadyBox�� �ʱ�ȭ �Ѵ�.
	void ClearReadyBoxMsg();

	//���� ���� �� ��� â�� ǥ���ϱ� ���� �Լ�
	void SetReadyBoxPresented(UINT8 * PresentedSupplyBox);
	const UINT8* GetReadyBoxPresented() const;

	//����â�� ����.
	void	SetReadyBoxReward(UINT8 Type, T_ItemID ItemID);

//	SUPPLY_BOX_TYPE GetReadyBoxReward_Type() const;						// �������� �Ⱦ��ٰ� ���� (2014.07.21.����)
	UINT32	GetReadyBoxReward_ItemID() const;

	//ReadyBox�� �ʱ�ȭ �Ѵ�.
	void	ClearReadyBoxReward();


private:
	//ä��â�� �˸��� ��� ��
	struct MSG_CONTEXT
	{
		UINT8	 Type = 0;
		UINT32 ServerIdx = 0;
		UINT32 ChannelNum = 0;
		UINT32 RoomIdx = 0;
		T_ItemID ItemID = 0;
		i3::rc_string UserNick;
	};

//	typedef i3::vector< MSG_CONTEXT > Msg_List;
	i3::vector< MSG_CONTEXT > m_Msg_List;

	UINT8 m_PresentedReadyBox[SLOT_MAX_COUNT];

	//����â�� ����.
	UINT8	 m_RewardReadyBoxType;
	UINT32 m_RewardReadyBoxItemID;
};
