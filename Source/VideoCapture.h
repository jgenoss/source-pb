#ifndef _VIDEOCAPTURE_H__
#define _VIDEOCAPTURE_H__

#include "bandicap.h"		// ������ ��ȭ - ���õ[2011.06.17]

enum HUD_MOVIE_CAPTURE_SYMBOL
{
	HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON = 0,
	HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF,

	HUD_MOVIE_CAPTURE_SYMBOL_CNT
};

class CVideoCapture : public i3ElementBase
{
	I3_CLASS_DEFINE(CVideoCapture, i3ElementBase);

private:
	CBandiCaptureLibrary *			m_pBandiCapture;		// ������ ��ȭ - ���õ[2011.06.17]

	//i3AttrSet*						m_pVCRoot;				// ��ȭ�� ���Ǵ� AttrSet
	i3Texture  *					m_pHud3Texture;			// ������ ���Ǵ� HUD ���� Texture;
	//i3Sprite2D *					m_pSpriteSymbol;		// ��ȭ�� ǥ�� �ɺ�
	//i3Sprite2D *					m_pSpriteString;		// ��ȭ�� ǥ�� ��Ʈ��

	REAL32							m_rMinCaptureTime;		// ���� ��ȭ �ּ� �ð� 
	REAL32							m_fNotifyDelay;			// ���� ��ȭ�� ���� ������
	
	INT32							m_nCaptureType;			// ���� ������

protected:
	bool CreateLibrary();									// �ݵ� ���̺귯�� ����
	
	void StartVideoCapture(CGameChatBox* pChatBox);
	void NotifyForVideoCapture(REAL32 rDelta);				// ���� ��ȭ �뺸 UI

public:
	CVideoCapture(void);
	~CVideoCapture(void);

	void InitHud3Texture(void);												// ���� ǥ�� UI �ʱ�ȭ
	void SaveVideoCapture(void);											// ���� ��ȭ
	void SetVideoCaptureInfo(INT32 iIdx);									// ���� �ػ� ���� 
	void SetVideoCapture(CGameChatBox* pChatBox);							// ���� ��ȭ ����
	void StopVideoCapture(CGameChatBox* pChatBox, bool bUserStop = true);	// ���� ��ȭ ����

	i3Texture* LoadHud3Texture(void);						// Ingame Hud3 return
	bool IsCapturing() const { if (m_pBandiCapture != nullptr) { return m_pBandiCapture->IsCapturing() ? true : false; } return false; }

	bool	CaptureImage( LPCSTR pszFilePath, INT nFileType, INT nQuality, INT nCaptureMode, bool bIncludeCursor );
	bool	CaptureImage( LPCWSTR pwszFilePath, INT nFileType, INT nQuality, INT nCaptureMode, bool bIncludeCursor );
};

#endif //_VIDEOCAPTURE_H__