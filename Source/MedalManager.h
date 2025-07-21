#pragma once

#include "MedalDef_Client.h"

class MedalSetBase;
class CMedalSystemFile;
class CMedalManager;

#define MEDAL_REWARD_TIME_OUT			20.0f
#define MAX_QUICK_MEDAL_REWARD_COUNT	20

#define MEDAL_SLOT_COL		2		// �޴� ���� ��
#define MEDAL_SLOT_ROW		6		// �޴� ���� ��
#define Medal_SLOT_TOTAL	MEDAL_SLOT_COL * MEDAL_SLOT_ROW	// �޴� ���� UI ���̴� ����

enum MEDAL_SORT_TYPE				// �޴� ���� Ÿ��
{
	MEDAL_SORT_NON			= -1,	// ���۵���
	MEDAL_SORT_DEFAULT		= 0,	// �⺻ ����(���� �ε���)
	MEDAL_SORT_ACQUIRE,				// ȹ���� �޴� �켱
	MEDAL_SORT_NON_ACQUIRE,			// ��ȹ�� �޴� �켱
	MEDAL_SORT_NO_GET_REWARD,		// ����ȹ�� ���� �޴� �켱
	MEDAL_SORT_MAX,
};

enum MEDALTAB
{
	TAB_NONE			= -1,
	TAB_BASIC			= 0,	// �Ϲ� �޴�
	TAB_EVENT,					// �̺�Ʈ �޴�
	TAB_DAILY,					// ���� �޴�
	TAB_CLAN,					// Ŭ�� �޴�
	TAB_MAX,
};

class MedalViewInfo
{
private:
public:
	UINT16		medal_Idx;
	UINT8		medal_lv;
	INT32		medal_viewIdx;
	INT32		medal_tabIdx;
	T_ItemID	medal_restriction_item;

	MedalViewInfo() : medal_Idx(0), medal_viewIdx(-1), medal_tabIdx(-1), medal_restriction_item(0), medal_lv(0) {}
	~MedalViewInfo()							{}

	INT32	getImageIdx() const;	
	UINT8	getLevel() const { return medal_lv; }				
	INT32	getLevelProcessCnt(UINT8 lv)const;	
	INT32	getMedalProcessCnt()const;		
	UINT8	getCurrentLevel() const;
	UINT8	getMaxLevel() const;  			
	float	getLevelRate(UINT8 lv) const;		

	//bool	isActivated(INT32 _tab, INT32 _view)
	bool	isAcquired() const;					
	bool	isChecked()	const;					
	bool	isNew() const; 						
	void	setNew(bool bEnable);			

	i3::rc_wstring	getMedalName() const;			
	const char*	getMedalDesc() const;			

	bool	isGetReward(UINT8 lv) const;			
	const MQF_REWARD_INFO* getReward(UINT8 lv);

	bool	isReceivableReward(void) const;		// �޴� ������ ���� �� �ִ��� üũ

	void			setRestrictionItem(T_ItemID ItemID)	{	medal_restriction_item = ItemID;	}
	T_ItemID		getRestrictionItem(void) const		{ return medal_restriction_item; }

};

class CMedalManager : public i3::shared_ginst< CMedalManager >
{
public:
	CMedalManager(void);
	virtual ~CMedalManager(void);

	INT32		getMedalTotalCount(INT32 _Type);
	INT32		getMedalSetCount(INT32 _Type);
	UINT16		getMedalIdx(INT32 _Type,INT32 idx);	// MQF �޴� �ε���(MedalSetBase[setidx]->getmedalidx
	INT32		getImageIdx(INT32 _Type,INT32 idx);	// MQF �̹��� �ε���
	INT32		getImageIdx(UINT8 _Type,UINT16 idx);// MQF �̹��� �ε���
	i3::rc_wstring	getMedalName(INT32 _Type, INT32 idx) const;						// �޴� �̸�
	const char*	getMedalDesc(INT32 _Type, INT32 idx);						// �޴� ����
	const MQF_REWARD_INFO*	getReward(INT32 _Type, INT32 idx, UINT8 lv);	// �޴� ���� ����
	bool		isGetReward(INT32 _Type, INT32 idx, UINT8 lv);				// �޴� ���� ȹ�� ����
	bool		isReceivableReward(INT32 _Type, INT32 idx);					// �޴� ������ ���� �� �ִ��� üũ
	INT32		getLevelProcessCnt(INT32 _Type, INT32 idx, UINT8 lv);		// �� ������ ���� Ƚ��
	INT32		getMedalProcessCnt(INT32 _Type, INT32 idx);					// ������� ������ �ӹ� ���� Ƚ��(���� ����)
	UINT8		getCurrentLevel(INT32 _Type,INT32 idx);						// �޴� ���� ����
	UINT8		getMaxLevel(INT32 _Type,INT32 idx);							//���� �Ŵ��� ���� �ִ� ������ �������ݴϴ�.
	float		getLevelRate(INT32 _Type,INT32 idx, UINT8 lv);				// �޴� ���� ����ī��Ʈ ����
	bool		isActivated(INT32 _Type,INT32 idx);
	bool		isAcquired(INT32 _Type,INT32 idx);							//
	bool		isChecked(INT32 _Type,INT32 idx);							// �˸��� ��� üũ ����
	bool		isNew(INT32 _Type,INT32 idx);								// ���� ȹ���� �޴�����..
	void		setNew(INT32 _Type,INT32 idx, bool bEnable);
	bool		hasNew(INT32 _Type);

public:
	MedalSetBase*		getMedalSet(INT32 _type)	{ return MedalSetList[_type];	}
	INT32				getMedalNormalTypeByIndex(UINT16);
	INT32				getMedalSetTypeByIndex(UINT16);

public:
	NOTIFY_MEDAL*		getNotifyMedal(UINT32 idx)	{ return &notifyMedalList[idx];	}
	void				setNotifyMedal(UINT32 idx, NOTIFY_MEDAL *);

	const char*			getNotifyMedalDesc(UINT8, UINT16);
	const char*			getNotifyMedalCountDesc(UINT8, UINT16);

public:
	void		ProcessCompleteMedalInfo(const MEDAL_STATE & data);
	void		ReplaceBattleInfo(const MEDAL_STATE & data);
	bool		IsCompleteMedalInfo(const MEDAL_STATE & data) const;

	void		PushMedalMessageInfo(const MEDAL_STATE & data)	{	m_qMedalMessage.push(data);	}
	INT32		getMedalMessageCnt(){	return m_qMedalMessage.size();}
	void		getMedalMessageFront(MEDAL_STATE *pData);

	void		PushCompleteMedalInfo(const MEDAL_STATE & data);
	INT32		getCompleteMedalCnt(){	return m_qCompleteMedal.size();}
	void		getCompleteMedalFront(MedalCompleteInfo *pData);

	bool		IsPerfectClearMedal(MedalCompleteInfo *pData) const;

	MedalViewInfo* GetMedalViewIdx(INT32 idx) const;

	void		MakeViewList(MEDALTAB eTab)	;
	void		MakeRewardList();
	static bool	_sort_medal(const MedalViewInfo * s1, const MedalViewInfo * s2);
	i3::vector<MedalViewInfo *>* GetViewList(){return &m_viewList;}
	i3::vector<MedalViewInfo *>* GetRewardList(){return &m_rewardList;}

	void		SetItemRestriction(T_ItemID ItemID);

	REAL32		GetAchievementRate() const { return m_fAchievementRate; }
	
private:
	std::queue<MEDAL_STATE>			m_qMedalMessage;
	std::queue<MedalCompleteInfo>	m_qCompleteMedal;
	
	i3::vector<MedalViewInfo *>		m_viewList;
	i3::vector<MedalViewInfo *>		m_rewardList;
	MedalViewInfo					m_viewInfo[MAX_MEDAL_COUNT];
	MedalViewInfo					m_rewardInfo[MAX_MEDAL_COUNT*4];

	INT32							m_iCompletedMedalCount;
	REAL32							m_fAchievementRate;

private:
	MedalSetBase *					MedalSetList[MEDAL_SET_TYPE_MAX];
	NOTIFY_MEDAL					notifyMedalList[MAX_NOTIFY_MADAL_COUNT];

/***************************************************************************************
****						Medal Reward Popup Information							****
***************************************************************************************/
public:
	struct CurrentRewardInfo
	{
		INT32			level_ = 0;
		MedalViewInfo*	medal_info_ = nullptr;
	};

private:
	CurrentRewardInfo	current_;

public:
	void				setCurrentRewardInfo(INT32 lv, MedalViewInfo* medal_info)
	{
		current_.level_ = lv;
		current_.medal_info_ = medal_info;
	}
	const CurrentRewardInfo*	getCurrentRewardInfo() { return &current_; }

	void				ClearCurrentRewardInfo()
	{
		current_.level_ = -1; current_.medal_info_ = nullptr;
	}


/***************************************************************************************
****								Medal Sort Info									****
***************************************************************************************/
private:
	INT32						m_nSortType;

public:
	INT32						GetSortType(void) const			{ return m_nSortType; }
	void						SetSortType(INT32 SortType)		{ m_nSortType = SortType; }

	void						GetSortTypeStr(i3::rc_wstring&, MEDAL_SORT_TYPE);

/***************************************************************************************
****						Medal Reward Resqust Info								****
***************************************************************************************/

private:
	i3::vector<MedalRewardResult>	m_vRewardResultList;		// �޴� ���� ��û ����Ʈ
	REAL32							m_rRewardResultWaitTime;	// ���� ��û ��� �ð�
	bool							m_bIsWaitResult;			// ��û ��� �÷���

	i3::vector<MedalCompleteInfo>	m_vRewardMedalList;

public:

	void						PushRewardMedal(MedalViewInfo data);
	void						PopRewardMedal(const MedalCompleteInfo* info);
	const MedalCompleteInfo *	getRewardMedal(INT32 idx) const;
	const INT32					getRewardMedalListCount(void) const { return m_vRewardMedalList.size(); }
	void						ClearRewardMedalList(void)	{ m_vRewardMedalList.clear(); }

	void						BuildMedalRewardPacket(MEDAL_REWARD * pOut, const MedalCompleteInfo * pInfo);
	void						ClearMedalRewardResultList(void) { m_vRewardResultList.clear(); }

	void						setMedalRewardResult(PACKET_D_GET_MEDAL_REWARD * pData);
	const MedalRewardResult *	getMedalRewardResult(INT32 idx)
	{
		I3_BOUNDCHK(idx, (INT32)m_vRewardResultList.size()); 
		return &m_vRewardResultList[idx]; 
	}

	const MQF_REWARD_INFO *		getMedalRewardInfo(INT32 idx);
	i3::rc_wstring				getMedalRewardNameForFirstFail(void);
	INT32						getMedalRewardResultFailCount(void);
	INT32						getMedalRewardResultCount(void) {return m_vRewardResultList.size(); }

	void						StartRewardResultWait(void);
	void						EndRewardResultWait(void);
	void						UpdateRewardResultWait(REAL32 dt);

	void						setWaitResult(bool flag)			{ m_bIsWaitResult = flag; }
	bool						isWaitResult(void)					{ return m_bIsWaitResult; }


/***************************************************************************************
****								Quick Medal Reword								****
***************************************************************************************/
	

private:
	i3::vector<MedalCompleteInfo>	m_vQuickRewardList;

protected:
	bool						_CheckCompleteMedal(const MEDAL_STATE & data, MedalCompleteInfo & out);

public:
	void						PushQuickRewardMedal(const MEDAL_STATE & data);
	void						SortQuickRewardList(void);
	void						ClearQuickRewardList(void)	{ m_vQuickRewardList.clear(); }

	const MEDAL_INFO *			getMedalFromQuickRewardList(INT32 idx);
	const MedalCompleteInfo *	getQuickRewardMedal(INT32 idx);
	INT32						getQuickRewardLevel(INT32 idx)		
	{ 
		I3_BOUNDCHK(idx, (INT32)m_vQuickRewardList.size()); 
		return m_vQuickRewardList[idx]._level; 
	}
	INT32						getQuickRewardCount(void)	{ return m_vQuickRewardList.size(); }

/***************************************************************************************/

//For MQF File
public:
	char*	getMedalFileName(INT32);
	bool	LoadMedalData();

};

