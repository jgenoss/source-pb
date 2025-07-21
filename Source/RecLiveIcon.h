#ifndef _RECLIVEICON_H__
#define _RECLIVEICON_H__

class CRecLiveIcon : public i3ElementBase
{
	I3_CLASS_DEFINE(CRecLiveIcon, i3ElementBase);
private:
	bool m_bEnable;
	REAL32 m_fNotifyDelay;	

	i3UIImageBox * m_pImageBox1;			// �ձ� �� �����ִ� �̹���
	i3UIImageBox * m_pImageBox2;			// �ձ� �� �����ִ� �̹���
	i3UIStaticText * m_pStaticText;			// REC, LIVE, REC & LIVE	

	i3UIScene *	   m_pScene;
public:
	CRecLiveIcon(void);
	~CRecLiveIcon(void);

	void InitIcon();
	void OnUpdate( bool isTwitchStreaming, bool isVideoCapture );
};

#endif