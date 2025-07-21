#pragma once

#include "UIHUDBase.h"
#include "UISecCount.h"
#include "UIBattleDef.h"

enum BATTLE_FORCIBLYREMOVE_RESULT;
enum FORCE_REMOVE_UI_TYPE;

class UIHUDForceRemove : public UIHUDBase
{
	friend class UIBattlePopupAccuseCrime;

	I3_CLASS_DEFINE( UIHUDForceRemove, UIHUDBase);

public:
	UIHUDForceRemove();
	virtual ~UIHUDForceRemove();

	virtual void OnCreate( i3GameNode * pParent) override;
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void OnLoadAllScenes() override;
	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;
	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool OnKeyInput( i3InputDeviceManager * pInputMng) override;
	virtual void _InitializeAtLoad(i3UIScene * pScene) override;

private:
	void	Vote( bool bAgree);
	void	EndVote();

private:
	void to_Lua_Switch(FORCE_REMOVE_UI_TYPE type);
	void to_lua_VoteGuide();	// ��ǥ ����(�ȳ�)
	void to_Lua_VoteStart();	// ��ǥ ����. �ĺ��� �г���, ���� ǥ��
	void to_Lua_VoteCount();	// ��ǥ ���� ǥ��(����,�ݴ�)

	void to_Lua_VoteResult(BATTLE_FORCIBLYREMOVE_RESULT Res); // ��ǥ ���
	void to_Lua_VoteCancel(); //��ǥ ���

private:
	i3::rc_wstring m_wstrAppNickName, m_wstrCandidateNickName;
	i3::rc_wstring m_wstrRemainTimeStr, m_wstrRemainSecondStr;
	
	FORCEREMOVE_CAUSE m_Reason;

	INT32 m_VoteTime;
	INT32 m_numAgree, m_numDisagree;			// �ڱ� �ڽ��� ��ǥ��Ȳ��Ŷ�� ���ޱ� ������ ���������մϴ�...
	bool m_bStartVote;			//��ǥ ���� ����(��ǥ �ϳ�? �� �ϳ�?)
	bool m_bVoted;				//���� ���� ��ǥ ����(�߳�? �� �߳�?)
	bool m_bMyVoteResult;		//���� ���� ��ǥ��(����/�ݴ�) - ������ �޸� 1�� ����, 0�� �����Դϴ�.
	bool m_bReporter;			// �Ű��� Flag
	BATTLE_FORCIBLYREMOVE_RESULT m_VoteResult;
	bool m_bUISTART_finished;	// UI START�� ��� �ð�(3)�� ��������

	// UI �ϴ� ���α׷����� ����
	i3UIProgressBar* m_ProgressBar[3];
	float			 m_progressValue;

	// ������ �����̵� ���
	enum { MAX_SLIDE_WND = 4 };
	i3UIFrameWnd*	m_pMainFmw[MAX_SLIDE_WND];

	UISecCount				m_SecCount;			// �ð���� ��ƿ..
	INT32	m_CurUIState;		// ���� UIâ ����
	INT32	m_NextUIState;		// UIâ ���¸� �ٲٱ� ������ ���� to do sliding.

public:
	void			OnSecCount_Start(INT32 result_time);
	void			OnSecCount_Change(INT32 result_time);
	void			OnSecCount_DoneOrStop(INT32 result_time);

private:
	// �Ϲ������� START - GUIDE - VOTE - RESULT ������ UI�� ����˴ϴ�
	// �Ű����� ��� START - VOTE - RESULT
	void			ChangeUIState(INT32 _State);
	void			SetProgressBar(REAL32 rDeltaSeconds);
	void			SetPopupSlide(REAL32 rDeltaSeconds);

	void	ResetVoteWnd();	
	void	SetVoteWnd_Close(INT32 ui_type, REAL32 rDeltaSeconds, bool half);	// �����̵�� �ݱ�	// half:�����̵� �ݸ� �ݱ� ����
	void	SetVoteWnd_Open(INT32 ui_type, REAL32 rDeltaSeconds);	// �����̵�� ����

	TUISecCallback<UIHUDForceRemove, 
		&UIHUDForceRemove::OnSecCount_Start,
		&UIHUDForceRemove::OnSecCount_Change,
		&UIHUDForceRemove::OnSecCount_DoneOrStop,
		&UIHUDForceRemove::OnSecCount_DoneOrStop
	> m_SecCountCB;

	//----------------------------------------------------------------------//
	//						Received Game Eent Functions					//
	//----------------------------------------------------------------------//
private:
	typedef void	(UIHUDForceRemove::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_KickVote_SuggestResult( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Start( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Update( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Count( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Cancel( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_KickVote_Result( INT32 arg, const i3::user_data&);	

};