#ifndef __GAMECHATBOX_H__
#define __GAMECHATBOX_H__

#include "i3Base/itl/svector.h"

#define MAX_CHAT_BUFFER	100
#define MAX_MEGAPHONE_BUFFER 300

#define MAX_CHAT_HISTORY_COUNT	10

enum CHAT_SYS_TYPE
{
	CHAT_SYS_TYPE_NONE = 0,
	CHAT_SYS_TYPE_WHISPER,
	CHAT_SYS_TYPE_REPLY,
	CHAT_SYS_TYPE_ONETIME_TEAM,
	CHAT_SYS_TYPE_CLEAR,
	CHAT_SYS_TYPE_HELP,
	CHAT_SYS_TYPE_MEGAPHONE,

	CHAT_SYS_TYPE_COUNT,
};

//
// GAME_STRING�� ������� ��� rc_string���� �ٲ����� �𸥴�.. (�� ��� ���� �Ը� �� ���� ũ��..)
//

class CGameChatBox : public i3GameObjBase
{
	I3_CLASS_DEFINE( CGameChatBox, i3GameObjBase);

protected:
	// Buffer
	INT32			m_nCurrentBufferIndex;
	INT32			m_nBackUpBufferIndex;
	INT32			m_nDeleteBufferIndex;
	INT32			m_nCurrentWhisperBufferIndex;
	INT32			m_nWhisperBufferIndex;
	INT32			m_nCurrentMegaPhoneBufferIndex;
	INT32			m_nMegaPhoneBufferIndex;
	INT32			m_nBufferCount;
	INT32			m_nBackUpBufferCount;
	INT32			m_nWhisperBufferCount;
	INT32			m_nMegaPhoneBufferCount;
	REAL32			m_fBackUpBufferRatio;
																// �ƹ��� �ӵ��������ִ��� �迭�� �ϴ� ����..
	i3::svector<UINT16>				m_ChatBufferTypeList;		// MAX_CHAT_BUFFER���� �̸�Ȯ���ϰ� �迭ó�� ���.
	i3::svector<i3::rc_wstring>		m_ChatBufferList;			// MAX_STRING_COUNT + MAX_STRING_COUNT + 10 ���� ����..(Ư���� ���� ������ �����)
	i3::svector<i3::rc_wstring>		m_ChatBackupBufferList;		// MAX_STRING_COUNT + MAX_STRING_COUNT + 10 ���� ����..(Ư���� ���� ������ �����)
	i3::svector<i3::rc_wstring>		m_WhisperBufferList;		// ������ 5�� �����ֽ��ϴ�. (�����ڿ��� ó��)
	i3::svector<i3::rc_wstring>		m_MegaPhoneBufferList;		

	i3::rc_wstring						m_wstrChatTarget;			//
	i3::rc_wstring						m_wstrChatReplyTarget;		//

	i3::rc_wstring						m_wstrHistoryText;			// MAX_STRING_COUNT + MAX_STRING_COUNT + 10 ���� ����..(Ư���� ���� ������ �����)
	
	i3::rc_wstring					m_emptyString;

	bool			m_bWhisperEnable;
	bool			m_bRingBufferFull;

	// Chat History
	i3::svector<i3::rc_wstring>	m_HistoryBufferList;					// MAX_CHAT_HISTORY_COUNTũ�⸦ ������. (�����ڿ��� ó��)
	i3::svector<UINT16>			m_HistoryChatModeList;					// �ϰ��������� UINT16�� �����Ѵ�. (m_ChatBufferTypeList�� ���� ���̴�..)

	INT32			m_nCurrentHistoryBufferIndex;
	INT32			m_nHistoryBufferIndex;
	INT32			m_nHistoryBufferCount;
	INT32			m_nHistoryBufferOffset;

public:
	CGameChatBox(void);
	~CGameChatBox(void);

	// Buffer�� �ʱ�ȭ �մϴ�.
	void			ResetChatBuffer();
	void			ResetWhisperBuffer(void);
	void			ResetMegaPhoneBuffer(void);

	// ���±��� �Էµ� Chatting Buffer�� ������ index�� �����ɴϴ�.
	const i3::rc_wstring&	GetBufferText( INT32 index) const;				
	const i3::rc_wstring&	GetBackUpBufferText( INT32 index) const;		
	const i3::rc_wstring&	GetWhisperBufferText( INT32 index) const;
	const i3::rc_wstring&	GetMegaPhoneBufferText( INT32 index) const;
	void			GetHistoryBufferText( INT32 index, i3::rc_wstring& out_str, INT32 * pMode);	
	void			ResetHistoryBufferIndex(void);
	UINT16			getBufferType( INT32 index)				{ return m_ChatBufferTypeList[index];	}
	
	CHAT_SYS_TYPE	ParsingText(const i3::wliteral_range& wTextRng);

	bool			FindTarget( const i3::wliteral_range& wTextRng);
	void			SetHelpText(void);
	
	const i3::rc_wstring&	GetChatTarget(void) const		{ return m_wstrChatTarget;			}	// ��ġ�°� �Ұ����غ��δ�..(�ڲ� �����μ��� void* �μ��Լ��� �Ѿ��.)
	void			SetChatTarget(const i3::rc_wstring& chat_target)	{	m_wstrChatTarget = chat_target; }			// ��ġ�°� �Ұ����غ��δ�..(�ڲ� �����μ��� void* �μ��Լ��� �Ѿ��.)

	const i3::rc_wstring&	GetChatReplyTarget(void) const	{ return m_wstrChatReplyTarget;	}		// ��ġ�°� �Ұ����غ��δ�..(�ڲ� �����μ��� void* �μ��Լ��� �Ѿ��.)
	const i3::rc_wstring&	GetHistoryText(void) const				{ return m_wstrHistoryText;		}		// ��ġ�°� �Ұ����غ��δ�..(�ڲ� �����μ��� void* �μ��Լ��� �Ѿ��.)
	INT32			GetBufferCount()						{ return m_nBufferCount;				}
	INT32			GetWhisperBufferCount()					{ return m_nWhisperBufferCount;			}
	INT32			GetMegaPhoneBufferCount()				{ return m_nMegaPhoneBufferCount;       }
	INT32			GetBackUpBufferCount()					{ return m_nBackUpBufferCount;			}
	INT32			GetCurrentBufferIndex()					{ return m_nCurrentBufferIndex;			}
	INT32			GetCurrentWhisperBufferIndex()			{ return m_nCurrentWhisperBufferIndex;	}
	INT32			GetCurrentMegaPhoneBufferIndex()		{ return m_nCurrentMegaPhoneBufferIndex;}
	INT32			GetMegaPhoneBufferIndex()				{ return m_nMegaPhoneBufferIndex;		}
	INT32			GetBackUpBufferIndex()					{ return m_nBackUpBufferIndex;			}
	REAL32			GetBackUpBufferRatio()					{ return m_fBackUpBufferRatio;			}
	void			SetChatReplyTarget(const i3::rc_wstring& wstrTarget);
	void			CalcRatio( INT32 nExceptionValue);
	void			DeleteBufferIndex(void);
	void			ResetBackupBuffer(void);
	void			IncreaseBackupBuffer( INT32 nExceptionValue = 0);
	void			DecreaseBackupBuffer( INT32 nExceptionValue = 0);
	void			CalcBackupBufferByRatio( REAL32 fVal, INT32 nExceptionValue = 0);

	//
	INT32			GetHistoryBufferCount()					{ return m_nHistoryBufferCount;			}
	INT32			GetHistoryBufferIndex()					{ return m_nCurrentHistoryBufferIndex;	}
	INT32			GetHistoryBufferOffset()				{ return m_nHistoryBufferOffset;		}
	void			SetHistoryBufferIndex(INT32 nIndex)		{ m_nCurrentHistoryBufferIndex = nIndex;}

	void			PutUserChatting(const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng, const bool bIsGM, UINT16 nType = 0, UINT8 NickColor = 0);
	void			PutUserWhisper(const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng, const bool bIsGM);	
	void			PutUserMegaPhone(const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng );
	void			PutSystemChatting(const i3::rc_wstring& str, UINT16 nType = 0);
	void			PutHistory( const i3::rc_wstring& str, INT32 ChatMode);			
	void			OnUpdate( REAL32 rDeltaSeconds );

	void			setWhisperEnable( bool bEnable)			{ m_bWhisperEnable = bEnable;			}
	bool			getWhisperEnable(void)					{ return m_bWhisperEnable;				}

	virtual	void	SetEnable( bool bTrue = true, REAL32 tm = 0.0f ) override;

private:
	void			_PutStringBuffer(const i3::rc_wstring& str, UINT16 nType = 0);
	void			_PutStringWhisperBuffer(const i3::rc_wstring& str);
	void			_PutStringMegaPhoneBuffer(const i3::rc_wstring& str);
};





#endif // __GAMECHATBOX_H__