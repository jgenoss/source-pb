/**
 * \file	GlobalInfoDef.h
 *
 * Declares the global information def class.
 */

#if !defined( __GLOBAL_INFO_DEF_H__)
#define __GLOBAL_INFO_DEF_H__

#pragma pack(push,8)

struct GAME_LOGIN_INFO
{
	// �ڽ��� ���̵�		(G->C)	
	char				m_UserId[NET_ID_SIZE] = { 0 };
	// �ڽ��� �н�����		(G->C)
	char				m_UserPw[NET_PW_SIZE] = { 0 };
	// UID
	INT64				m_UserUID = -1;
	// �ڽ��� �г���		(G->C)	
	i3::rc_wstring		m_wstrUserNick;
	// ���� ���� ���̵�
	char				m_BillingID[BILLING_ID_SIZE] = { 0 };

#if defined(LOCALE_SINGMAL) || defined (LOCALE_PHILIPPINES) || defined(LOCALE_KOREA) || defined(LOCALE_VIETNAM)
	char				m_szGarenaToken[NET_ENCRYPTED_TOKEN_SIZE] = { 0 };
#elif defined(LOCALE_JAPAN)
	char				m_szArarioSessionKey[NET_ENCRYPTED_TOKEN_SIZE] = { 0 };
#endif

	// �ڽ��� ��Ʈ�� ī���� Mac Address 
	UINT64				m_MacAddress = 0;
	// Web Session Key
	UINT8				m_SessionKey[NET_SESSION_KEY_SIZE] = { 0 };

	char				m_strExtUID[EXT_UID_SIZE] = { 0 };
	UINT8				m_LinkMethod = 0;

	// Web Session ID
	char				m_SessionID[33] = { 0 };
	// Web Session Login
	bool				m_bWebLogin = false;

	/** \brief ���� �ʱ�ȭ �Լ�*/
	void	Init( void);

	/** \brief �� string�� ���̸� ��ȯ�Ѵ�. */
	INT32	GetUserIDLength( void) const		{ return i3::generic_string_size( m_UserId); }
	INT32	GetUserPwLength( void) const		{ return i3::generic_string_size( m_UserPw); }
//	INT32	GetUserNickLength( void) const		{ return i3::generic_string_size( m_UserNick); }

	void	SetUserUID(INT64 UID);
	/** \brief UserID�� ����*/
	void	SetUserID( const char * pID);
	/** \brief UserPassword�� ���� */
	void	SetUserPassword( const char * pPW);
	/** \brief UserNick�� ����*/
	void	SetUserNick( const i3::rc_wstring& wstrNick);
	void	SetExtUID(const char* pExtUID);

	void	SetBillingID(const char* pUID);
	void	SetSessionKey( UINT8 * pKey);
	void	SetSessionID( const char * pID);

	void	SetLinkMethod(UINT8 value);
};

struct BANNER_URL_INFO
{
	INT32			i32Type = 0;
	INT32			i32Data = 0;
	i3::rc_wstring	wsURL;
};

#pragma pack(pop)

namespace FileUtil{

size_t get_file_size(const std::string & full_path_name);
size_t get_file_size(const std::wstring & full_path_name);

bool is_exist_file(const i3::string & full_path_name);
bool is_exist_file(const i3::wstring & full_path_name);

bool is_exist_dir(const i3::string & full_path);
bool is_exist_dir(const i3::wstring & full_path);

void create_directory(const i3::string & full_path);
void create_directory(const i3::wstring & full_path);

i3::string string_by_date(const i3::string & split = "_");
i3::wstring wstring_by_date(const i3::wstring & split = L"_");

i3::string string_by_time(const i3::string & split = "_");
i3::wstring wstring_by_time(const i3::wstring & split = L"_");
};

#endif
