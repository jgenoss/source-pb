#ifndef _OUTGAMECOMMANDSET_H__
#define _OUTGAMECOMMANDSET_H__

class COutGameQACommandSet :	public i3ElementBase
{
	I3_CLASS_DEFINE(COutGameQACommandSet, i3ElementBase);

	enum COMMAND_TYPE
	{
		COMMAND_RANDOMBOX,
	};

	struct ItemInfo
	{
		UINT32 m_itemID = 0;
		i3::rc_wstring m_itemName;
	};

public:
	~COutGameQACommandSet();

	void ProcessCommand(REAL32 rDeltaSeconds);

	//�����ڽ� �׽�Ʈ Ŀ�ǵ�
	bool CheckRandomBoxCommand(const i3::wliteral_range& wImeBufRng);

	
private:
	//���� �Լ� �̺�Ʈ ����.
	//�̺�Ʈ Ÿ�԰� ���ڸ� �Ѱܹ޴´�. �̺�Ʈ�� ������ �Ķ���͵��� ���̽����� �ٸ��Ƿ�
	//�� ���Ŀ� �߰��ϴ� ������� �˾Ƽ� �����ϰ�.
	void SetEventForCommand(COMMAND_TYPE commandType, const void * pArg = nullptr, const void * pArg1 = nullptr, const void * pArg2 = nullptr, LPCSTR szFile = nullptr, int nLine = 0);

	//�����ڽ� �׽�Ʈ Ŀ�ǵ�
	bool	m_bIsRandomBoxTestEnabled = false;
	UINT32	m_itemID = 0;
	T_ItemDBIdx	m_itemDBIdx = 0;
	UINT32	m_iterationCount = 0;
	REAL32	m_timeBuffer = 0.0f;

	i3::vector<ItemInfo> resultBuffer;
	void	ProcessRandomBoxCommand(REAL32 rDeltaSeconds);
	void    SetCSVLog();

public:
	bool	isRandomBoxTestEnabled()	{return m_bIsRandomBoxTestEnabled;}
	void	AddResultToBuffer(UINT32 itemID, const i3::rc_wstring& itemName);
};

#endif