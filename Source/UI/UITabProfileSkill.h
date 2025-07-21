#if !defined( __UI_TAB_PROFILE_SKILL_H__)
#define __UI_TAB_PROFILE_SKILL_H__

#include "UITabProfileBase.h"
#include "ClientDef.h"
#include "UIImgSetCtrl.h"
#include "UIFlowEditBox.h"

#define MAX_SKILLSLOT 6

class UIImgSetCtrl;
class UIFlowEditBox;

class UITabProfileSkill : public UITabProfileBase
{
	I3_CLASS_DEFINE( UITabProfileSkill, UITabProfileBase);
public:
	enum SKILL_STATE
	{
		SKILL_STATE_OFF = 0,
		SKILL_STATE_ON,
		SKILL_STATE_SELECT,
	};

	struct SKILLSLOTDATA
	{
		SKILL_TYPE		SkillType;			//��ųŸ��
		INT32			iSkillIndex;		//��ų�ε���
		INT32			iCurrentExp;		//���� ����ġ
		INT32			iLevel;				//MAX����
		INT32			iMaxLevel;			//Max����
		UINT32			iCurrentRank;		//���緹��
		REAL32			iCurrentPlaytime;	//���緹���ð�
		REQUISTE		NextType;			//��������
		INT32			iNextRank;			//��������
		REAL32			iNextPlaytime;		//�����÷���Ÿ��
		SKILL_STATE		SkillState;			//��ų��밡������
		bool			bFocus;				//��ų ��Ŀ�� ����
		REAL32			rPlayTime;			//���� �÷��̽ð�
		bool			bApply;				//��ų������
		bool			bNew;				//���ο� ��ų ����
		SKILLSLOTDATA(): SkillType(SKILLLIST_UNKNOWN), iSkillIndex(-1), iLevel(0), rPlayTime(0),SkillState(SKILL_STATE_OFF), bApply(false), iNextPlaytime(-1), iCurrentExp(0), iCurrentPlaytime(0), bNew(false)
		{

		}
	};

	struct CHARASLOTDATA
	{
		i3::rc_wstring			wstrName;    //     [NET_CHARA_NICK_NAME_SIZE];
		INT32					iCharaType;
	};

private:
	i3::vector<SKILLSLOTDATA>			m_SlotSkill[CATEGORYLIST_MAX];
	SKILLSLOTDATA						m_SelectSkill[CATEGORYLIST_MAX];
	class i3UIButtonImageSet*			m_apImgSetCtrl[MAX_SKILLSLOT];	// ���� �̹���
	class i3UIButtonImageSet*			m_SelectImgSetCtrl;	// ���� �̹���
	INT32								m_iRank;
	i3::vector<CHARASLOTDATA>			m_SlotChara;
	
	CHARA_SLOT_SKILL			m_Classtype;
	
	//UI ���� ����
	INT32						m_iCharaCount;
	INT32						m_CurrentAvatarSlot;
	INT32						m_iCurrentCategory;
	INT32						m_iCurrentIndex;
	INT32						m_iCurrentScroll;
	i3UIScrollBar*				m_pSlotScrollbar;
	bool						m_biscancel;
	UINT8						m_iCharaClass;
	INT32						m_iFocusSlot[CATEGORYLIST_MAX];

	REAL32						m_fProgressTimer;
	REAL32						m_fMaxLevel;			// ���� ǥ�� ��ǥ �ִ�ġ
	REAL32						m_fMotifyLevel;			// �����Ǿ��� ���� ��ǥ
	REAL32						m_fCurLevel;			// ���� ���� ��ǥ
	INT32						m_iCurSlotMaxLevel;		// ���� ������ �ִ� ����
	INT32						m_iOriginLevel;
	INT32						m_iLevelButtonCount;

	REAL32						m_fSpeedRate;

	UIFlowEditBox				m_FlowEditBox;

	i3UIButtonImageSet*			m_pLevelImageSet;
	i3UIProgressBar*			m_pLevelProgressBar;
	i3UIImageBox*				m_pLevelProgressLine;
	i3UIButton*					m_pCurLevelButton;
	
public:
	UITabProfileSkill();
	virtual ~UITabProfileSkill();
 
	virtual void		OnCreate( i3GameNode * pParent ) override;
	virtual bool		OnEntranceStart( void * pArg1, void * pArg2 ) override;
	virtual void		OnEntranceEnd() override;
	virtual bool		OnExitStart() override;
	virtual void		OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void		ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual void		SetInputDisable(bool bDisable) override;

	virtual bool		RequestChangeTab() override;
	virtual bool		RequestExitTab() override;
	virtual void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		_ClearAtUnload(i3UIScene * pScene) override;


public:
	INT32				GetCurrentAvatar()	{ return m_CurrentAvatarSlot;  }

public:
	//UI���� ���� �Դϴ�.
	void				SetLuaCharaInfo(void);						//ĳ���� ���� LUA ����
	void				SetLuaCharaSelect(INT32 idx);				//�ɸ��� �����̺�Ʈ	
	void				SetLuaSkillSlotInfo(void);					//��ų������ ����Ÿ LUA ����
	void				SetLuaApplyClass(bool bEnable);				//��ų CLASS LUA ����
	void				SetCurrentCategory(INT32 idx);				//ī�װ��̵� ��ưŬ�� �̺�Ʈ
	void				SetCurrentScroll(INT32 idx);				//��ų ������ ��ũ�� �̺�Ʈ 
	void				SlotWheel(INT32 zDelta);					//��ų ������ �� �̺�Ʈ
	void				SelectSlot(INT32 slot);						//��ų ������ ������ ������ LUA ����
	void				SlotNewCheck(INT32 slot);					//��ų ������ ������ ������ LUA ����
	void				SetLuaSkillApplyInfo(void);					//���� �������� ��ų LUA ����
	void				SetSkillDesc(INT32 Idx, REAL32 & outVal);	//������ ��ų�� Description ���� 

	void				ApplySlot();						//��ų ����â�� ��ų ������ư �̺�Ʈ
	void				ApplySlot_DoubleClick(INT32 slot);
	void				SelectedLevel(INT32 level);					// ��ų�� ������ ����
	void				ChangeLevel(INT32 level);		//����â�� ���� ����Ʈ �ڽ� �̺�Ʈ
	void				CancelSlot(INT32 slot);						//������ų â������ ��ų ���� �̺�Ʈ
	void				SelectClass(INT32 _index);					//Ŭ�����̼��ý� Ŭ���� ���� �̺�Ʈ
	void				ApplyClass(void);							//Ŭ���� ���� Ŭ�� �̺�Ʈ
	static void			CbApplyClass(void* pThis, INT32 nParam);	//Ŭ���� ���� Ŭ�� �̺�Ʈ
	void				SetAllSlotData(void);						// �������� ���� ��ų���� ���ù� PEF���� â�������ѽ�ų���� ����
	INT32				GetSlotSkillCount()
	{
		INT32 icount = 0;
		for( UINT32 i = 0; i < m_SlotSkill[m_iCurrentCategory].size(); i++)
		{
			if( m_SlotSkill[m_iCurrentCategory].at(i).SkillType == SKILLLIST_UNKNOWN)
				continue;
			icount++;
		}
		return icount;
		
	}

private:
	void				_SetSkillSlotData();
	void				_SetSkillActivate(void);

	void				_SetCurLevelButtonPos(INT32 leveltype, INT32 curlevel);

	void				InitLevelProgressBar();
	void				LevelProgressBarUpdate( REAL32 rDeltaSeconds);

	void				GetWeaponTypeStr(CHARA_CLASS weapontype, i3::rc_wstring& out_wstr);
	
	void				GetSkillNameTypeStr(INT32 category, INT32 index, i3::rc_wstring& out);
	void				GetSkillTypeDescripStr(INT32 index, i3::rc_wstring& out_wstr, bool bMax = false);
	void				GetSendData(PACKET_SKILL_SET_REQ *_data);
};

#endif //end of __UI_TAB_PROFILE_SKILL_H__