#include "pch.h"
#include "VideoCapture.h"
#include "GlobalInfoDef.h"

I3_CLASS_INSTANCE( CVideoCapture);//, i3ElementBase);

#define VIDEO_CAPTURE_HUD_SIZE			19.0f		// HUD �̹��� Width,Height Size
#define VIDEO_CAPTURE_HUD_STRING_WIDTH	25.0f		// HUD �̹��� "REC" String Width
#define VIDEO_CAPTURE_ON_HUD_U			458.0f		// "On" HUD u ��ǥ
#define VIDEO_CAPTURE_OFF_HUD_U			478.0f		// "Off" HUD u ��ǥ
#define VIDEO_CAPTURE_STRING_HUD_U		500.0f		// "REC" HUD u ��ǥ
#define VIDEO_CAPTURE_HUD_V				191.0f		// HUD �̹��� v ��ǥ


namespace
{
	bool	SetCaptureConfig( BCAP_CONFIG& cfg, INT32 nType )
	{
		switch(nType)
		{
		case 0 :
			cfg.Default();
			cfg.VideoQuality	= 80;
			cfg.VideoSizeW		= 480;
			cfg.VideoSizeH		= 272;
			cfg.VideoFPS		= 24.0f;
			break;

		case 1 :
			cfg.Default();
			cfg.VideoQuality	= 80;
			cfg.VideoSizeW		= 800;
			cfg.VideoFPS		= 24.0f;
			break;

		case 2 :
			cfg.Default();
			cfg.VideoQuality	= 90;
			cfg.VideoSizeW		= 1024;
			cfg.VideoFPS		= 24.0f;
			break;

		default:
			return false;
		}

		return true;
	}
}

CVideoCapture::CVideoCapture(void)
{
	m_pBandiCapture			= NULL;
	//m_pVCRoot				= NULL;
	m_pHud3Texture			= NULL;
	//m_pSpriteSymbol			= NULL;
	//m_pSpriteString			= NULL;
	m_rMinCaptureTime		= 0.0f;
	m_fNotifyDelay			= 0.0f;
	m_nCaptureType			= 0;
}

CVideoCapture::~CVideoCapture(void)
{
	// ���� ��ȭ�� ���� ���� �� ������ ���������� ����ǰ� ��
	if(IsCapturing())
	{
		StopVideoCapture(NULL , false);
	}

	//I3_SAFE_RELEASE( m_pVCRoot);
	//I3_SAFE_RELEASE( m_pSpriteSymbol);
	//I3_SAFE_RELEASE( m_pSpriteString);
	I3_SAFE_RELEASE( m_pHud3Texture);
	if( m_pBandiCapture)
	{
		m_pBandiCapture->Release();
		I3_SAFE_DELETE( m_pBandiCapture);
	}
}

bool CVideoCapture::CreateLibrary()
{
	if(m_pBandiCapture == NULL)
	{
		m_pBandiCapture = new CBandiCaptureLibrary();
		MEMDUMP_NEW( m_pBandiCapture, sizeof(CBandiCaptureLibrary));
		I3ASSERT( m_pBandiCapture != NULL );
	}

	if( m_pBandiCapture->IsCreated() == false)
	{
		m_pBandiCapture->Create(BANDICAP_RELEASE_DLL_FILE_NAME);
		m_pBandiCapture->Verify("ZEPETTO_POINTBLANK_20110812", "bb76cf9a");
	}

	return m_pBandiCapture->IsCreated() ? true : false;
}

void CVideoCapture::SetVideoCapture(CGameChatBox* pChatBox)
{
	// ��ġ�� ������ �ȵ� ���
	if(!CreateLibrary())
	{
		pChatBox->PutSystemChatting(GAME_RCSTRING("STBL_IDX_VIDEO_CAPTURE_FAIL_DEVICE")); // ������ ��ȭ�� ���� �� �� �����ϴ�.
		return;		
	}

	// ���� ��ġ�� ���� ��� 
	if(g_pViewer->GetSoundContext() == NULL)
	{
		pChatBox->PutSystemChatting(GAME_RCSTRING("STBL_IDX_VIDEO_CAPTURE_FAIL_SOUND")); // ���� ��ġ�� �����ϴ�. ������ ��ȭ�� ���� �� �� �����ϴ�.
		return;		
	}
	
	// ���� ��ȭ ����
	if(!m_pBandiCapture->IsCapturing())
	{
		StartVideoCapture(pChatBox);
	}
	else
	{
		StopVideoCapture(pChatBox);
	}
}


void CVideoCapture::StartVideoCapture(CGameChatBox* pChatBox)
{
	BCAP_CONFIG cfg;

	// ���� �ػ� ����
	if( SetCaptureConfig(cfg, m_nCaptureType) == false)	// revision 44352
	{
		I3PRINTLOG(I3LOG_NOTICE,"Capture Type Unknown - %d", m_nCaptureType);
		SetCaptureConfig(cfg, 0);	// Ÿ���� �߸����õǾ� ������ �⺻ ������ ��ȭ
	}
	cfg.IncludeCursor = true;				  // Ŀ�� ǥ��
	m_pBandiCapture->CheckConfig(&cfg);       // �߸��� ���� �ٷ� ���

	m_pBandiCapture->SetConfig(&cfg);         // ���� ����
	m_pBandiCapture->SetMinMaxFPS(30, 60);    // �ּ�, �ִ� ������ ����

	// �켱���� 1�ܰ� ����		
	m_pBandiCapture->SetPriority(THREAD_PRIORITY_NORMAL);

	// ���� ����Ǵ� ��� ���� (MyDocument\PointBlank\Movie)
	wchar_t szCurDir[ MAX_PATH ] = L"";

	SHGetSpecialFolderPathW(NULL, szCurDir, CSIDL_PERSONAL, false);

	//document ��ġ
	std::wstring doc_path = szCurDir;

	//document/PointBlank ��ġ
	std::wstring doc_pb_path = doc_path + L"\\" + GAME_TITLE_NAME_W;
	FileUtil::create_directory( doc_pb_path );

	//document/PointBlank/Movie ��ġ
	i3::wstring doc_pb_movie_path = doc_pb_path +  L"\\" + L"Movie";
	FileUtil::create_directory( doc_pb_movie_path );

	//���ϸ�
	i3::wstring movie_file = FileUtil::wstring_by_date() + L"_" + FileUtil::wstring_by_time();
	movie_file.append( L".avi" );

	i3::wstring doc_pb_movie_file_path;
	doc_pb_movie_file_path = doc_pb_movie_path + L"\\" + movie_file;

	// Debug�� �׽�Ʈ �ڵ�
	// ���� ������
	/*{
		std::string szTemp;

		szTemp.assign(doc_pb_movie_file_path.begin(), doc_pb_movie_file_path.end());

		const char * pszTemp = szTemp.c_str();

		I3PRINTLOG(I3LOG_DEFALUT, "File Path : %s", pszTemp);

	}*/

	//// ĸó ����
	/*HRESULT hr =*/ m_pBandiCapture->Start(doc_pb_movie_file_path.c_str(), 
		g_hWnd, BCAP_MODE_D3D9_SCALE,
		(LONG_PTR)g_pViewer->GetRenderContext()->GetNativeDevice());

	//I3PRINTLOG(I3LOG_DEFALUT, "Banicap Message : %d", hr );

	m_rMinCaptureTime = 0.0f;
	m_fNotifyDelay = 0.0f;

	//m_pSpriteSymbol->SetEnable(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON, true);
	//m_pSpriteString->SetEnable( true);
	//m_pVCRoot->RemoveFlag( I3_NODEFLAG_DISABLE);


	pChatBox->PutSystemChatting(GAME_RCSTRING("STBL_IDX_VIDEO_CAPTURE_START"));	// ������ ��ȭ�� �����մϴ�.
}

void CVideoCapture::StopVideoCapture(CGameChatBox* pChatBox, bool bUserStop)
{
	// ���� �Է����� ���� ������ �ּ� ��ȭ �ð� 3��
	if(bUserStop)
	{
		if(m_rMinCaptureTime < 3.0f)
		{
			pChatBox->PutSystemChatting(GAME_RCSTRING("STBL_IDX_VIDEO_CAPTURE_TIME_LIMIT")); //������ ��ȭ �ð��� ª���ϴ�. (�ּ� ��ȭ �ð� :  3��)
			return;
		}
	}

	m_pBandiCapture->Stop();

	//for(UINT32 iIdx = 0 ; iIdx < HUD_MOVIE_CAPTURE_SYMBOL_CNT ; iIdx++) 
	//	m_pSpriteSymbol->SetEnable(iIdx, false);

	//m_pSpriteString->SetEnable( false);
	//m_pVCRoot->AddFlag( I3_NODEFLAG_DISABLE);

	if(pChatBox != NULL)
		pChatBox->PutSystemChatting(GAME_RCSTRING("STBL_IDX_VIDEO_CAPTURE_END")); // ������ ��ȭ�� �����մϴ�.

	// ĸ�� ���� ��, ��ü / ���̺귯�� ����.
	m_pBandiCapture->Destroy();
}


void CVideoCapture::SaveVideoCapture(void)
{
	if(m_pBandiCapture == NULL)
		return;
	if(!m_pBandiCapture->IsCapturing())
		return;

	REAL32 rDelta = g_pViewer->getDeltaTime();

	NotifyForVideoCapture(rDelta);

	HRESULT hr = m_pBandiCapture->Work((LONG_PTR)g_pViewer->GetRenderContext()->GetNativeDevice());

	if(hr != BC_OK)
	{
		I3PRINTLOG(I3LOG_DEFALUT, "[BandiCapture Error] Save Video Capture (%d)", hr);
		StopVideoCapture(NULL , false);
		I3PRINTLOG(I3LOG_DEFALUT, "[BandiCapture Error] System Stop Video Capture");
	}

	m_rMinCaptureTime += rDelta;

}

void CVideoCapture::NotifyForVideoCapture(REAL32 rDelta)
{
	//m_fNotifyDelay += rDelta;

	//if(m_fNotifyDelay > 0.5f)
	//{
	//	if(m_pSpriteSymbol->GetEnable(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON))
	//	{	
	//		m_pSpriteSymbol->SetEnable(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON, false);
	//		m_pSpriteSymbol->SetEnable(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF, true);
	//	}
	//	else
	//	{
	//		m_pSpriteSymbol->SetEnable(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON, true);
	//		m_pSpriteSymbol->SetEnable(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF, false);
	//	}

	//	m_fNotifyDelay = 0.0f;
	//}
}

void CVideoCapture::InitHud3Texture(void)
{
#if defined (MULTYLANGUAGE)
	i3::stack_string strLoadFilePath;
	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/HUD_03.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	m_pHud3Texture	= g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
#else
#if defined (USE_LOCALE_UI)
	m_pHud3Texture	= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Hud_03.i3i");
#else
	m_pHud3Texture	= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/hud3.i3i");
#endif
#endif
	I3ASSERT( m_pHud3Texture != NULL);
//
//	REAL32 rStartX = g_pViewer->GetViewWidth() * 0.925f;
//	REAL32 rStartY = g_pViewer->GetViewHeight() * 0.05f;
//
//	m_pSpriteSymbol = i3Sprite2D::new_object();
//	I3ASSERT( m_pSpriteSymbol != NULL );
//
//	m_pSpriteSymbol->SetTexture(m_pHud3Texture);
//
//	m_pSpriteSymbol->Create(HUD_MOVIE_CAPTURE_SYMBOL_CNT, true, true);
//
//	m_pSpriteSymbol->SetEnable(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON, false);
//	m_pSpriteSymbol->SetColor(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON, 255,255,255,255);
//	m_pSpriteSymbol->SetRect( HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON, rStartX, rStartY, VIDEO_CAPTURE_HUD_SIZE, VIDEO_CAPTURE_HUD_SIZE);
//	// UV ��ǥ 
//	m_pSpriteSymbol->SetTextureCoord( HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON,
//			VIDEO_CAPTURE_ON_HUD_U, VIDEO_CAPTURE_HUD_V, VIDEO_CAPTURE_ON_HUD_U + VIDEO_CAPTURE_HUD_SIZE, VIDEO_CAPTURE_HUD_V + VIDEO_CAPTURE_HUD_SIZE);
//
//	m_pSpriteSymbol->SetEnable(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF, false);
//	m_pSpriteSymbol->SetColor(HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF, 255,255,255,255);
//	m_pSpriteSymbol->SetRect( HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF,rStartX, rStartY, VIDEO_CAPTURE_HUD_SIZE, VIDEO_CAPTURE_HUD_SIZE);
//	
//	m_pSpriteSymbol->SetTextureCoord( HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF,
//			VIDEO_CAPTURE_OFF_HUD_U, VIDEO_CAPTURE_HUD_V, VIDEO_CAPTURE_OFF_HUD_U + VIDEO_CAPTURE_HUD_SIZE, VIDEO_CAPTURE_HUD_V + VIDEO_CAPTURE_HUD_SIZE);
//
//	rStartX += 21.0f;
//
//	m_pSpriteString = i3Sprite2D::new_object();
//	I3ASSERT( m_pSpriteString != NULL );
//
//	m_pSpriteString->Create( 1, true, true );
//	m_pSpriteString->SetTexture( m_pHud3Texture );
//	m_pSpriteString->SetEnable( false);
//	m_pSpriteString->SetRect( 0, rStartX, rStartY, VIDEO_CAPTURE_HUD_STRING_WIDTH, VIDEO_CAPTURE_HUD_SIZE);
//	m_pSpriteString->SetColor( 0, 255,255,255,255);
//	
//	m_pSpriteString->SetTextureCoord( 0, VIDEO_CAPTURE_STRING_HUD_U, VIDEO_CAPTURE_HUD_V, 
//				VIDEO_CAPTURE_STRING_HUD_U + VIDEO_CAPTURE_HUD_STRING_WIDTH, VIDEO_CAPTURE_HUD_V + VIDEO_CAPTURE_HUD_SIZE);
//
//	if( m_pVCRoot == NULL)
//	{
//		m_pVCRoot = i3AttrSet::new_object();
//
//		if( m_pSpriteSymbol != NULL)  m_pVCRoot->AddChild( m_pSpriteSymbol);
//		if( m_pSpriteString != NULL)  m_pVCRoot->AddChild( m_pSpriteString);
//
//		{
//			i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
//
//			pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
//			pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
//
//			m_pVCRoot->AddAttr( pAttr);
//		}
//
//		m_pVCRoot->AddFlag( I3_NODEFLAG_DISABLE);
//		g_pFramework->getGuiLayer()->GetParent()->AddChild(m_pVCRoot);
//	}
}

void CVideoCapture::SetVideoCaptureInfo(INT32 iIdx)
{
	m_nCaptureType	= iIdx;

	//// �ػ� ���濡 ���� HUD ��ǥ ����
	//REAL32 rStartX = g_pViewer->GetViewWidth() * 0.925f;
	//REAL32 rStartY = g_pViewer->GetViewHeight() * 0.05f;

	//m_pSpriteSymbol->SetRect( HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON, rStartX, rStartY, VIDEO_CAPTURE_HUD_SIZE, VIDEO_CAPTURE_HUD_SIZE);
	//m_pSpriteSymbol->SetRect( HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF, rStartX, rStartY, VIDEO_CAPTURE_HUD_SIZE, VIDEO_CAPTURE_HUD_SIZE);
	//
	//rStartX += 21.0f;
	//m_pSpriteString->SetRect( 0, rStartX, rStartY, VIDEO_CAPTURE_HUD_STRING_WIDTH, VIDEO_CAPTURE_HUD_SIZE);
}

i3Texture* CVideoCapture::LoadHud3Texture(void)
{
	if (m_pHud3Texture)
	{
		m_pHud3Texture->AddRef();
		return m_pHud3Texture;
	}
	
	return NULL;
}

bool	CVideoCapture::CaptureImage( LPCSTR pszFilePath, INT nFileType, INT nQuality, INT nCaptureMode, bool bIncludeCursor )
{
	if( !CreateLibrary() )
	{
		return false;
	}

	LONG_PTR ptr = (LONG_PTR)g_pViewer->GetRenderContext()->GetNativeDevice();

	if( NULL == ptr )
	{
		return false;
	}

	HRESULT Result = m_pBandiCapture->CaptureImage( pszFilePath, nFileType, nQuality, nCaptureMode, bIncludeCursor, ptr ) ;

	// ���� ��ȭ���� �ƴҶ��� ��ü / ���̺귯���� ����. 
	if(!m_pBandiCapture->IsCapturing())
	{
		m_pBandiCapture->Destroy();
	}

	return S_OK == Result;
}

bool	CVideoCapture::CaptureImage( LPCWSTR pwszFilePath, INT nFileType, INT nQuality, INT nCaptureMode, bool bIncludeCursor )
{
	if( !CreateLibrary() )
	{
		return false;
	}

	LONG_PTR ptr = (LONG_PTR)g_pViewer->GetRenderContext()->GetNativeDevice();

	if( NULL == ptr )
	{
		return false;
	}

	HRESULT Result = m_pBandiCapture->CaptureImage( pwszFilePath, nFileType, nQuality, nCaptureMode, bIncludeCursor, ptr );

	// ���� ��ȭ���� �ƴҶ��� ��ü / ���̺귯���� ����. 
	if(!m_pBandiCapture->IsCapturing())
	{
		m_pBandiCapture->Destroy();
	}

	return S_OK == Result;
}
