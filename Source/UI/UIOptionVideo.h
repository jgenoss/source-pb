#if !defined( __UI_OPTION_Video_H__)
#define __UI_OPTION_Video_H__

#include "UIOptionBase.h"
#include "ProgressEdit.h"
#include "VideoUtil.h" 

class UIOptionVideo : public UIOptionBase
{
	I3_CLASS_DEFINE( UIOptionVideo, UIOptionBase );

	enum{ MAX_PE = 2, };

public:
	void clicked(int idx1, int idx2);

private:
	VideoUtil m_Video;

	//screen ����
	INT32 m_nScreenWidth;	//�ػ�
	INT32 m_nScreenHeight;
	BOOL m_bFullScreen; //��ü���

	INT32 m_AA;	//��Ƽ�ٸ����
	INT32 m_AAQ;
	BOOL m_bVSync;	//���� ����ȭ
	BOOL m_bTriLinearFilter;	//�ε巯�� ���� ó��

	//next time ����
	INT32 m_nTextureQualityType;	//�ؽ��� ����Ƽ
	INT32 m_nSpecularQualityType;	//���ݻ� ȿ��
	INT32 m_nEffectQuality;	//����Ʈ ǰ��
	BOOL m_bEnableNormalMap;	//���� ���� ǥ��

	//Shader�� �ش� GPU���� ��� �������� Ȯ���� �ʿ�!!
	INT32 m_nShadowQualityType;	//�׸��� ȿ��
	BOOL	m_bDynamicLight;	//����ȿ��

	//��� ����
	INT32 m_nVideoResolution; //������ �ػ�
	//INT32 m_Quick_Setup;		// ���� ǰ�� ����
	INT32 m_nQualityType;		// ���� ǰ�� ����

	REAL32 m_fGammaVal;	//ȭ�� ���
	REAL32 m_fFovValue;	//�þ߰�
	IntProgressEdit mPEdit[MAX_PE];
	
	BOOL m_bEnableBulletTrace;	//�Ѿ˱���
	BOOL m_bEnableBulletSmoke;	//�Ѿ˱��� ����Ʈ
	BOOL m_bEnableTerrainEffect;	//���� ����Ʈ
	BOOL m_bEnablePhysX;	//���� ȿ��
	// 
	BOOL m_bDisplayOtherParts; //���� ���� ǥ��
	BOOL m_bFixedViewCharacter; //���� ĳ���� ǥ��

	/*****************************/
	INT32 m_nScreenHz;
	INT32 m_ScreenDepth;
	BOOL m_b16BitsDepth;
	BOOL m_bEnableBloom;

private:
	//void set_quick_setup();
	void setupQuality(const INT32 type);
	void adjust_max_value();	//�׷��� ī�� �������ο� ���� �� ����

	void adjust_refresh_rate();	//�ػ󵵸� �����ϸ� �ش� �Լ��� �����������.

public:
	void check_quick_setup_level();

private:
	void cb_edit_change1( INT32 value );
	void cb_edit_change2( INT32 value );
	IntProgress_Callback< UIOptionVideo, &UIOptionVideo::cb_edit_change1 > m_Callback1;
	IntProgress_Callback< UIOptionVideo, &UIOptionVideo::cb_edit_change2 > m_Callback2;

private:
	virtual void load_value() override;
	void load_detail_value();

	virtual void save_value() override;
	
	virtual void default_value() override;
	void default_detail_value();
	
	virtual void apply_game() override;
	virtual void apply_lua() override;

public:
	UIOptionVideo();
	virtual ~UIOptionVideo();

	virtual void _InitializeAtLoad( i3UIScene * pScene ) override;
	virtual void OnCreate( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes() override;
	virtual bool OnKey_Enter() override;
	virtual bool OnKey_Escape() override;
	virtual void OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool is_modified() override;
	bool is_modified_screen() const;	//screen�� ������ �ִ� �� ����.. �������� �����.
	bool is_diff_default_RefreshRate() const;
	
	virtual bool key_escape() override;
	bool clicked_default();	//���ϰ��� false �̸� ���� �ൿ �Ѵ�.
	bool clicked_ok();	//���ϰ��� false �̸� ���� �ൿ �Ѵ�.
	bool clicked_cancel();	//���ϰ��� false �̸� ���� �ൿ �Ѵ�.
};

#endif
