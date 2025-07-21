#ifndef		__SERVER_INFO_H__
#define		__SERVER_INFO_H__


class CServerIPList;

class	CServerInfo	: public i3ElementBase
{
	I3_CLASS_DEFINE( CServerInfo, i3ElementBase);
public:
	CServerInfo();


public:
	UINT32			GetNumber() const			{return m_Number;}
	UINT32			GetIP() const				{return m_ServerIP;}
	const char *	GetIPStr() const			{return m_ServerIPStr;}
	UINT16			GetPort() const				{return m_ServerPort;}
	SChannelType	GetType() const				{return m_Type;}
	UINT32			GetUserCount() const		{return m_UserCount;}
	UINT32			GetMaxUserCount() const		{return m_MaxUserCount;}
	bool			GetActive() const			{return m_Active;}

	void Set(UINT32 Number, const GAME_SERVER_INFO & ServerInfo);


private:
	UINT32			m_Number;
	UINT32			m_ServerIP;
	char			m_ServerIPStr[64];
	UINT16			m_ServerPort;
	SChannelType	m_Type;
	UINT32			m_UserCount;
	UINT32			m_MaxUserCount;
	bool			m_Active;
};


class	CServerInfoContext	: public i3ElementBase
{
	I3_CLASS_DEFINE( CServerInfoContext, i3ElementBase);
public:
	CServerInfoContext();
	~CServerInfoContext();

public:
	INT32	GetServerNumber( void );

	bool	Initialize();

	bool		IsUseServerSelectPage() const	{return (bool)m_IsUseServerSelect;}
	UINT32		GetServerCount()	 			{return m_Info[S_CHANNEL_TYPE_ALL].size();}

	// Ư�� Ÿ���� ���� ������ ��ȯ�Ѵ�.
	INT32		GetServerTypeCount(SChannelType SType = S_CHANNEL_TYPE_ALL) const;
	const i3::vector<CServerInfo*>&	GetServerInfoList(SChannelType SType = S_CHANNEL_TYPE_ALL) const	{return m_Info[SType];}
	const CServerInfo *		GetServerInfo(UINT32 ServerIndex, SChannelType SType = S_CHANNEL_TYPE_ALL) const;

	void	CreateServerInfo(bool UseServerSelect, UINT32 ServerCount);
	bool	UpdateServerInfo(UINT32 ServerCount, GAME_SERVER_INFO * pInfoArr);
	void	Clear();
	void	ClearServerInfos();

	const char *	GetUseServerSet() const;
	INT32			GetUseIPCount();
	INT32			GetCurrentIPIdx();
	void			SetCurrentIPIdx(INT32 idx);
	
	

	UINT32			GetLogInServerIP() const;
	const char *	GetLogInServerIPStr(INT32 idx) const;
	const char *	GetLogInServerIPStr() const;
	char *			GetCompareLauncherStr() const;
	UINT16			GetLogInServerPort() const;

	INT32					GetGameServerIndex(INT32 ServerIndex, SChannelType SType = S_CHANNEL_TYPE_ALL);

	bool					SetGameServer(INT32 ServerIndex, SChannelType SType = S_CHANNEL_TYPE_ALL);

	// ���� ���õ� ���� ����.
	// ����� �����̰ų� �����Ϸ��� ������ �����̴�.
	const CServerInfo *		GetCurGameServer() const;
	INT32					GetCurGameServerIndex() const;
	INT32					GetCurGameServerIndexByType() const		{ return m_GameServerIndexByType; }
	SChannelType			GetCurGameServerType() const			{ return m_GameServerType; }
	void					SetCurGameServerType(SChannelType eType) 			{ m_GameServerType = eType; }
	void					GetIndexByTypeAndType(UINT32 uiAll_Index, UINT32& uiIndex, SChannelType& eType);

	INT32	GetCurChannelNumber() const			{ return m_CurChannelNumber; }
	void	SetCurChannelNumber(INT32 val)		{ m_CurChannelNumber = val; }

private:
	// �������� �޾ƿ��� ���� ����.
	bool				m_IsUseServerSelect;		// ���� ����Ʈ �������� ����ϴ��� ���� ( 0 or 1 )

	// �� ���� ������ ���� ������ ���� ���� ����Ʈ.
	i3::vector<CServerInfo*>	m_Info[S_CHANNEL_TYPE_MAX];


	CServerIPList *		m_pServerIPManager;
	INT32				m_GameServerIndexByType;
	SChannelType		m_GameServerType;

	INT32				m_CurChannelNumber;
	
	//** xxx.xxx.xxx.xxx������ IP ���� Ȯ�� �մϴ�.
	bool				_IsDottedAddressIP( const char* pAddr ) const;
	//** ������ ������ IPstr�� ��ȯ �մϴ�. 
	const char*			_ConvertDomainName2IP( const char* pAddr ) const;
};


#endif	// __SERVER_INFO_H__

