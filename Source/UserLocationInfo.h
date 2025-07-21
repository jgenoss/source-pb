#ifndef		__USER_LOCATION_INFO_H__
#define		__USER_LOCATION_INFO_H__



// ������ ����, ä��, �� ��ġ�� ��Ÿ���� Ŭ����
class	CUserLocationInfo
{
public:
	CUserLocationInfo();
	CUserLocationInfo(UINT64 FriendState);

	void SetState(UINT64 State) { m_State = State; }
	FRIEND_STATE	GetState() const;
	UINT32			GetMatch() const;
	UINT32			GetServerNumber() const;
	UINT32			GetChannelNumber() const;
	UINT32			GetRoomNumber() const;
	UINT32			GetDisplayServerNumber() const;
	UINT32			GetDisplayChannelNumber() const;
	UINT32			GetDisplayRoomNumber() const;
	UINT32			GetDisplayServerChannelNumber() const;
	bool			IsMatch() const;

private:
	UINT64	m_State;
};


#endif
