#if !defined( _USER_GUIDE_MGR_H__)
#define _USER_GUIDE_MGR_H__

#include "UI/UIUserGuideSlot.h"
#include "UI/UIDefine.h"

// ���̵� �ý��� 
enum GUIDE_SYSTEM
{
	GS_NULL		= -1,
	GS_JOIN_ROOM,				// ��Ʋ ����
	GS_CREATEROOM_DEATHMATCH,	// �� ����� - ��ü��
	GS_CHANGE_WEAPON,			// ���̵� �޴� - ���� ��ü
	GS_CHANGE_CHARACTER_NAME,	// ĳ���� �г��� ��ü
	GS_CHANGE_CHARACTER,		// ĳ���� �� - ĳ���� ��ü
	GS_CREATEROOM_BOMB,			// �游��� - ���Ĺ̼�
	GS_ADD_FRIEND,				// ģ�� �߰�
	GS_REQUEST_JOIN_CLAN,		// Ŭ�� ���� ��û�� ������
	GS_SEND_MAIL,				// ���� ������
	GS_RECEIVE_PRESENT,			// ���� �ޱ�
	GS_BUYANDEQUIP_WEAPON,		// ���� ����&����
	GS_EQUIP_SKILL,				// ��ų �ý��� - ��ų ����
	GS_BUY_CHARACTER,			// ĳ���� ���� ����
	GS_RECEIVE_QUEST_REWARD,	// ����Ʈ ���� ȹ��
	GS_CHANGE_ROOMSET,			// �� ���� �ٲٱ� - �������� ��ü

	GS_MAX_COUNT
};


struct RecvEventInfo
{
	UINT32 _ui32EventIdx;
	UINT32 _ui32RewordPoint;

	RecvEventInfo(UINT32 ui32EveneIdx, INT32 ui32Point)
	{
		_ui32EventIdx = ui32EveneIdx;
		_ui32RewordPoint = ui32Point;
	}
};

struct GuideTargetInfo
{
	GuideTargetAttr _GuideTargetAttr;
	bool			_bCleard;

	GuideTargetInfo(GuideTargetAttr & pAttr, bool bCleard)
	{
		_GuideTargetAttr = pAttr;
		_bCleard = bCleard;
	}

	bool isSameAttr(const GuideTargetAttr * pAttr)
	{
		return _GuideTargetAttr.IsSameAttr(pAttr);
	}

	bool isSameEvent(INT32 i32EventIdx)
	{
		return (_GuideTargetAttr._i32EventIdx == i32EventIdx);
	}

	bool isSameGuideType(INT32 i32Type)
	{
		return (_GuideTargetAttr._i32GuideType == i32Type);
	}

	bool isSameGuideNumber(INT32 i32Number)
	{
		return (_GuideTargetAttr._i32GuideNum == i32Number);
	}
};

struct GuideSlotInfo
{
	UIUserGuideSlot *	_pSlot;
	bool				_bSendEvent;

	GuideSlotInfo(UIUserGuideSlot * pSlot)
	{
		_pSlot = pSlot;
		_bSendEvent = false;
	}
};

// Key : User Guide System
class UserGuideMgr : public i3::shared_ginst<UserGuideMgr>
{
private:
	i3::vector<RecvEventInfo>	m_EventQueue;	
	i3::vector<GuideTargetInfo> m_GuideTargetList;
	i3::vector<GuideSlotInfo>	m_GuideSlotList;

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
	bool				m_SkipCondition;
#endif

protected:
	void _AddGuideTargetInfo(GuideTargetAttr & pAttr);
	void _RemoveGuideTargetInfo(GuideTargetAttr & pAttr);

	GuideTargetInfo * _FindGuideTargetInfo(INT32 i32Type, INT32 i32Number);

	bool _IsPossibleGuide(GuideTargetAttr & pAttr);
	bool _IsClearGuide(UINT64 ui64UserData, INT32 i32EventNum);
	bool _IsClearPrevGuide(INT32 i32Type, INT32 i32Number);
	bool _IsSameScene(const char * pStr1, const char * pStr2);

	void _FindTargetCrtlFromScene(GuideTargetAttr & pAttr);
	void _FindTargetCtrlFromListView(i3UIListView_Box * pLvBox,  GuideTargetAttr & pAttr);

	void _ChangeTargetScene(GuideTargetAttr & pAttr);

	bool _CheckDuplicateCtrl(i3UIControl * pCtrl);
	bool _CheckGuideCondition(GuideTargetAttr & pAttr);

	void _PlayStartMessage(INT32 i32GuideType, INT32 i32GuideNum);
	
	GuideSlotInfo * _GetGuideSlotInfo(UINT32 i32Type)
	{
		I3ASSERT( i32Type < m_GuideSlotList.size() )
		return	&m_GuideSlotList[i32Type];
	}

	UIUserGuideSlot * _GetGuideSlot(UINT32 i32Type)
	{
		return _GetGuideSlotInfo(i32Type)->_pSlot;
	}

public:
	UserGuideMgr(void);
	virtual ~UserGuideMgr(void);

	void InitUserGuideList(void);
	void RemoveUserGuideList(void);

	void OnLoadGuideTargetInfo(UIBase * pTargetFrame);
	void UnLoadGuideTargetInfo(UIBase * pTargetFrame);

	void PlayGuideSystem(INT32 i32GuideType);
	void StopGuideSystem(UIBase * pTargetFrame);

	void ChangeNextGuide(GuideSlotInfo * pSlotInfo);
	void DisableUserGuideAll(void);

	void OnUpdate(REAL32 rDeltaSeconds);
	void OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2);
	
	void OnClick(i3UIControl * pCtrl, bool bDBclick);
	void OnScroll(i3UIControl * pCtrl);
	
	void ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData);

	void SetReceivedGuideData(UINT32 ui32Event, UINT32 ui32Point)
	{
		m_EventQueue.push_back( RecvEventInfo(ui32Event, ui32Point) );
	}

	UIMainFrame * GetMainFrame(void) { return g_pFramework->GetUIMainframe(); }
};

#endif // _USER_GUIDE_MGR_H__