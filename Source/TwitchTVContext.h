#if !defined( __TWITCH_CONTEXT_H)
#define __TWITCH_CONTEXT_H

#include "twitchsdk.h" 

#ifdef  I3_DEBUG
#pragma comment(lib, "../lib/twitchsdk_32_Debug.lib")
#else
#pragma comment(lib, "../lib/twitchsdk_32_Release.lib")
#endif

#define MAX_FRAME_INDEX	4
#define MAX_VIDEO_INDEX 3
#define MAX_RESOULTION_INDEX 6
#define NUM_CAPTURE_SURFACES 4

#define TWITCHTV_INIT				0x01
#define TWITCHTV_GET_AUTH			0x02
#define TWITCHTV_GET_INGESTLIST		0x04
#define TWITCHTV_GET_USERINFO		0x08
#define TWITCHTV_GET_STREAMINFO		0x10
#define TWITCHTV_GET_INGESTSERVER	0x20
#define TWITCHTV_STREAMING			0x40
#define TWITCHTV_LOGIN				0x80

struct TWFRAME_INFO
{
	INT32			m_frame = 0;
	i3::wstring		m_sframe;
} ;

struct TWVIDEO_INFO
{
	i3::wstring		m_video;
} ;

struct TWRESOULTION_INFO
{
	INT32			m_width = 0;
	INT32			m_height = 0;
	INT32			m_recomendbitrate = 0;
} ;

class TwitchTVContext : public i3::shared_ginst<TwitchTVContext>
{
public:

	//Set,Get Function.
	bool	isTwitchLogin()	const								{ return isTwitchState(TWITCHTV_LOGIN); }
	bool	isBroadCasting() const								{ return isTwitchState(TWITCHTV_STREAMING); }
	bool	isMikeOn() const									{ return m_bMike; }
	void	SetMikeEnable(bool bmike)							{ m_bMike = bmike; }
	bool	isSpeakerOn() const									{ return m_bSpeaker; }
	void	SetSpeakerEnable(bool bspeaker)						{ m_bSpeaker = bspeaker; }
	bool	isMsgBox() const									{ return m_bMsgBox; }
	void	SetMsgBox(bool bMsg)								{ m_bMsgBox = bMsg; }
	bool	isFastStreamingStop()const							{ return m_bFastTwitchSreamingStop; }
	void	SetFastSreamingStop(bool bresult)					{ m_bFastTwitchSreamingStop = bresult; }

	void		 SetBroadTitle(const i3::wstring& stitle)		{ m_sBroadTitle = stitle; }
	i3::wstring	 GetBroadTitle() const							{ return m_sBroadTitle;   }

	void	SetFrame(INT32 nframe)								{ m_nFrameIndex = nframe; }
	INT32	GetFrame() const									{ return m_nFrameIndex;   }
	void	SetVideoGrade(INT32 nVideoGrade)					{ m_nVideoIndex = nVideoGrade; }
	INT32	GetVideoGrade()	const								{ return m_nVideoIndex;		   }
	void	SetMikeVolume(INT32 nMikeVolume)					{ m_nMikeVolume = nMikeVolume; }
	INT32	GetMikeVolume()	const								{ return m_nMikeVolume;		   }
	void	SetSpeakerVolume(INT32 nSpeakerVolume)				{ m_nSpeakerVolume = nSpeakerVolume; }
	INT32	GetSpeakerVolume() const							{ return m_nSpeakerVolume;			 }

	i3::wstring	GetFrameInfoString();
	INT32		GetFrameInfoToIndex(INT32 index); 
	i3::wstring GetVideoInfoString();
	i3::wstring GetVideoInfoToIndex(INT32 index);
	i3::wstring GetResoultionInfoString();
	i3::wstring GetResoultionInfoToIndex(INT32 index);

	void		SetLastTaskResult(TTV_ErrorCode result)				{ m_LastTaskResult = result; }
	void		SetWaitForCallback(bool result)						{ m_bWaitingForCallback = result; }
	
	void		AddTwitchState( UINT8 state )						{ m_ui8TwitchState |= state; }
	void		RemoveTwitchState(UINT8 state )						{ m_ui8TwitchState &= ~state; }
	bool		isTwitchState( UINT8 state ) const					{ return (m_ui8TwitchState & state) != 0; }	
	
	void		SetRTMPState( uint64_t state )						{ m_ui64RTMPState = state; }
	uint64_t	GetRTMPState()										{ return m_ui64RTMPState;  }
	void		SetTotalSent( uint64_t state )						{ m_ui64TotalSent = state; }
	uint64_t	GetTotalSent()										{ return m_ui64TotalSent;  }

	i3::vector<unsigned char*> GetFreeBufferList()					{ return m_FreeBufferList; }
	unsigned char* GetNextFreeBuffer()
	{
		unsigned char * pBuffer = m_FreeBufferList.back();
		if( m_bWaitingForCallback == false )
		{
			m_bWaitingForCallback = true;
			m_FreeBufferList.pop_back();
		}
		return pBuffer;
	}

public:
	TwitchTVContext();
	~TwitchTVContext();

	void Init();

	bool TTVLogin(const i3::wstring& id, const i3::wstring& pw);
	bool TTVStreamStart();
	bool TTVStreamStop();
	bool TTVStreaming();
	bool TTVOptionChange( bool IsNeedRestart );
	
	static void CbFastStopStreaming(void* pThis, INT32 nParam);
private:
	
	void Init_FrameInfo();
	void Init_VideoInfo();
	void Init_ResultionInfo();

	bool FindTwitchFile();
	bool OpenTwitchFile();
	void LoadTwitchFile(void);

	bool CreateTwitchFile();
	bool SaveTwitchFile();
	bool DeleteTwitchFile();
	void MakeTwitchMD5Key(void);
	char* GetTwitchMD5Key(void)
	{  
		// �����߿� ShopFile�� ����� �� �� ������ �ٽ��ѹ� MD5Key�� �����մϴ�.
		MakeTwitchMD5Key(); 
		return m_strTwitchMD5Key; 
	};

	
private:
	uint64_t m_ui64TotalSent;						//Ʈ��ġ poolstats�� ����
	uint64_t m_ui64RTMPState;						//Ʈ��ġ poolstats�� ����

	UINT8		m_ui8TwitchState;					//Ʈ��ġ ���� ���� ����ũ

	TTV_ErrorCode	m_LastTaskResult;				//Ʈ��ġ Ƽ�� ������ ��� ��.
	bool	   m_bWaitingForCallback;				//Ʈ��ġ Ƽ�� �ݹ� �Լ� ���� ����.

	i3::wstring	 m_sBroadTitle;						//Ʈ��ġ Ƽ�� ��� ����
	INT32	m_nFrameIndex;							//Ʈ��ġ Ƽ�� ��� ���� ������
	INT32	m_nVideoIndex;							//Ʈ��ġ Ƽ�� ��� ���� ��������Ƽ
	INT32	m_nResoultionIndex;						//Ʈ��ġ Ƽ�� ��� ���� �ػ�
	bool	m_bMike;								//Ʈ��ġ Ƽ�� ����ũ on/off
	bool	m_bSpeaker;								//Ʈ��ġ Ƽ�� ����Ŀ on/off
	INT32	m_nMikeVolume;							//Ʈ��ġ Ƽ�� ����ũ ����
	INT32	m_nSpeakerVolume;						//Ʈ��ġ Ƽ�� ����Ŀ ����

	i3::vector<TWFRAME_INFO *> m_FrameList;				//Ʈ��ġ Ƽ�� ��� ������ ���� ����Ÿ
	i3::vector<TWVIDEO_INFO *> m_VideoList;				//Ʈ��ġ Ƽ�� ��� ���� ǰ�� ���� ����Ÿ
	i3::vector<TWRESOULTION_INFO *> m_ResoultionLsit;	//Ʈ��ġ Ƽ�� ��� �ػ� ���� ����Ÿ

	TTV_AuthToken		m_AuthToken;				//Ʈ��ġ Ƽ�� ���� ��ū
	TTV_IngestList		m_IngestList;				//Ʈ��ġ Ƽ�� ���� ���� ���� ����Ʈ
	TTV_UserInfo		m_UserInfo;					//Ʈ��ġ Ƽ�� ���� ����
	TTV_StreamInfo		m_StreamInfo;				//Ʈ��ġ Ƽ�� ��Ʈ�� ����
	TTV_ChannelInfo     m_Channelnfo;				//Ʈ��ġ Ƽ�� ä�� ����
	TTV_IngestServer	m_IngestServer;				//Ʈ��ġ Ƽ�� ����� ����
	float				m_fServerBitrate;			//Ʈ��ġ Ƽ�� ����� ���� ��Ʈ����Ʈ

	i3::vector<unsigned char*> m_FreeBufferList;		//Ʈ��ġ Ƽ�� �����ؾߵǴ� ����.
	
	IDirect3DTexture9* m_CaptureTexture;				
	IDirect3DSurface9* m_CaptureSurface;
	IDirect3DSurface9* m_ResizeSurface[NUM_CAPTURE_SURFACES];
	IDirect3DQuery9*   m_CaptureQuery[NUM_CAPTURE_SURFACES];		
	
	INT32 m_CaptureTextureWidth;
	INT32 m_CaptureTextureHeight;
	INT32 m_CaptureGet;
	INT32 m_CapturePut;

	uint64_t m_LastCaptureTime;
	
	bool    m_bMsgBox;
	bool	m_bFastTwitchSreamingStop;
	HANDLE	m_hTwitchShopFile;
	char	m_strTwitchMD5Key[NET_MD5_KEY_SIZE+1];
	
private:
	//Ʈ��ġ �۵� �Լ���..
	TTV_ErrorCode TwitchTVAuthenticate(TTV_AuthToken* authToken, const i3::wstring& id, const i3::wstring& pw);
	TTV_ErrorCode TwitchTVLogin(TTV_AuthToken* authToken, TTV_ChannelInfo* channelInfo);
	TTV_ErrorCode TwitchTVGetIngetList(const TTV_AuthToken* authToken, TTV_IngestList* ingestList);
	TTV_ErrorCode TwitchTVFoundIngestServer();
	TTV_ErrorCode TwitchTVStreamingStart();
	TTV_ErrorCode TwitchTVStreamingStop();
	TTV_ErrorCode TwitchTVSubmitFrame(unsigned char* pBgraFrame);
	TTV_ErrorCode TwitchTVChangeOption(bool IsNeedRestart );

	bool CaptureFrame(int captureWidth, int captureHeight, unsigned char*& outBgraFrame);
};

#endif