#include "pch.h"
#include "GameLoginContext.h"

#if defined( LOCALE_JAPAN )
#include "Arario/Include/ararioapi2012.h"
#pragma comment(lib, "../lib/ArarioAPI2012.lib")
#include "URLUtil.h"
#endif // LOCALE_JAPAN

bool	GameLoginContext::ParseCommandLineForLogin( const char * pszCmdLine)
{
	if( pszCmdLine == nullptr || strlen(pszCmdLine) == 0)
		return false; 

#if defined(NC_BUILD)

	#if defined(LOCALE_RUSSIA) 

		const char * pStr1 = pszCmdLine;
		INT32 idx = 0;
		char szID[MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN];
		char szPass[MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN];

		memset(szID, 0, sizeof(szID));
		memset(szPass, 0, sizeof(szPass));

		while( *pStr1)
		{
			if( *pStr1 == ' ')
			{
				if(idx < MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN) {
					i3::string_ncopy_nullpad(szID, pszCmdLine, idx);
				}
				else {
					i3::string_ncopy_nullpad(szID, pszCmdLine, MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN-1);
				}
				i3::string_ncopy_nullpad(szPass, pStr1+1, MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN-1);

				break;
			}
			pStr1++;
			idx++;
		}

		SetID_AndPassForRussiaWebLogin(szID, szPass);

	#elif defined(LOCALE_LATIN_AMERICA)

		i3::vector<i3::string>	token_list;
		i3::strtok_fast(token_list, pszCmdLine, '|');
		const size_t token_size = token_list.size();

		if (token_size > 1) 
		{
			if (token_list[0].size() >= NET_PW_SIZE)
				token_list[0].resize(NET_PW_SIZE-1);
		
			if (token_list[1].size() >= NET_PW_SIZE)
				token_list[1].resize(NET_PW_SIZE-1);
		
			INT32 AuthCode = i3::atoi(token_list[1].c_str());
			SetID_AndPassForLatinWebLogin(token_list[0].c_str(), token_list[1].c_str(), AuthCode);
		}
		else if (token_size == 1)
		{
			if (token_list[0].size() >= NET_PW_SIZE)
				token_list[0].resize(NET_PW_SIZE-1);

			SetID_AndPassForLatinWebLogin(token_list[0].c_str(), "", 0);
		}
		else
		{
			SetID_AndPassForLatinWebLogin("", "", 0);
		}
	
	#elif defined (LOCALE_SINGMAL) || defined (LOCALE_PHILIPPINES) || defined (LOCALE_VIETNAM)
		if(!i3::contain_string(pszCmdLine,""))
			return false;
		else
		{
			const char* pStr1 = pszCmdLine;
			//bool bValue = false;
			char szToken[NET_ENCRYPTED_TOKEN_SIZE] = "";
			//char* pszToken = szToken;
			INT32 cnt = 0;
			INT32 paramCount = 0;

			//������ �Ķ���� ����
			// PointBlank.exe /GameID:��GarenaTH" /Token: "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx...xxxx"

			while(*pStr1)
			{	
				I3PRINTLOG(I3LOG_NOTICE,"ParamCount : %d", paramCount);

				if(paramCount >= 4)
					break;

				if( *pStr1 == '\"')
				{
					//bValue = false;
					//�Ķ���� ������ 2���̹Ƿ�, 
					//������ �Ķ���͸� �о �����ϸ� �ȴ�.
				}
				else if( *pStr1 == ':')
				{
					paramCount++;
				}
				else if (*pStr1 == ' ')
				{
					pStr1++;
					cnt++;
					continue;
				}
				else
				{
					if(paramCount == 2)
					{
						i3::generic_string_ncopy(szToken, pStr1, NET_ENCRYPTED_TOKEN_SIZE);
						break;
					}
				}

				pStr1++;
				cnt++;
			}
			SetToken_ForGarena(szToken);
	//		I3PRINTLOG(I3LOG_NOTICE,"%s", szToken);
	//		I3PRINTLOG(I3LOG_NOTICE,"Garena Token Size : %d", i3::generic_string_size(szToken));
		}

	#elif defined(LOCALE_KOREA)

		// �ڸ��� ��ó �Ķ���� ����
		// ��ó�Ķ���Ϳ� �˴ٿ��� �����ͱ��� ���ԵǾ� �̸�� ���� ���.(428byte~)
		// PointBlank.exe Drz8vkZxmXZ6655Zg7qPQi8Wu67MlXkcxyy3Re+oWrWwat/aYmsdNOEL2LLBqtYZsA6gqEvRAaFDSxPpaEboKWnOgcXcT

		if(!i3::contain_string(pszCmdLine,""))
		{
			return false;
		}
		else
		{
			//const char* pStr1 = pszCmdLine;

			i3::string str_cmd_line = pszCmdLine;
			str_cmd_line = str_cmd_line.substr(0, str_cmd_line.rfind(" ") );

			char szToken[NET_ENCRYPTED_TOKEN_SIZE] = "";
			i3::generic_string_ncopy(szToken, str_cmd_line.c_str(), NET_ENCRYPTED_TOKEN_SIZE);

			SetToken_ForGarena(szToken);

	//		I3PRINTLOG(I3LOG_NOTICE,"%s", szToken);
	//		I3PRINTLOG(I3LOG_NOTICE,"Garena Token Size : %d ", i3::generic_string_size(szToken));
		}


		//const char * pszParse = pszCmdLine;

		//char strSession[MAX_STRING_COUNT] = "";
		//UINT8 SessionKey[NET_SESSION_KEY_SIZE];

		//char strKey[MAX_STRING_COUNT] = "";
		//char strValue[MAX_STRING_COUNT] = "";
		//char * pstrKeyPtr = strKey;
		//char * pstrValuePtr = strValue;

		//bool bValue  = false;
		//INT32 cnt = 0;

		//memset(SessionKey, 0, sizeof(SessionKey));

		//// CommandLine ����
		//// ��C:\Program Files (x86)\PlayNC\PunchMonsterOBT\NClient.exe" /SessKey:"36226f9390befe45bd41633c283b8f13" /MacAddr:"001CC07AA05D" /UserNick:"CastleBuilder" /CompanyID:"0" /ChannelGroupIndex:"-1" /ServerAddr:"112.175.195.123" /StartGameID:"PunchMonsterOBT" /RepositorySub:" " /GamePath:"C:\Program Files (x86)\PlayNC\PunchMonsterOBT"
		//while( *pszParse)
		//{
		//	if( cnt >= 1024)
		//	{
		//		I3PRINTLOG(I3LOG_FATAL, "warning!! buffer overflow. please check!!");
		//		return false;
		//	}

		//	if( bValue)
		//	{
		//		//	������ �׸��� ���� (")�� ���� ����
		//		if( *pszParse == '\"')		
		//		{
		//			if( strlen( strValue) > 0)	// (") ���� ������ strValue�� ���� ��������� ���� ������
		//			{
		//				bValue = false;		

		//				if( i3::icontain_string_head(strKey, "SessKey") )
		//				{
		//					memcpy( strSession, strValue, sizeof(char) * strlen(strValue));
		//				}
		//				else if( i3::icontain_string_head(strKey, "ServerAddr") )
		//				{

		//				}
		//				else if( i3::icontain_string_head(strKey, "CompanyID") )
		//				{

		//				}
		//			}
		//		}
		//		else
		//		{
		//			if( &(*pstrValuePtr) == &strValue[MAX_STRING_COUNT-1])
		//			{
		//				I3PRINTLOG(I3LOG_FATAL, "warning!! buffer overflow. please check!!");
		//				return false;
		//			}

		//			*pstrValuePtr = *pszParse;		// ���� �д´�.

		//			pstrValuePtr++;
		//		}
		//	}
		//	// ������ �׸� ������
		//	else if( *pszParse == ' ')			
		//	{
		//		memset(strKey, 0, sizeof(strKey));
		//		memset(strValue, 0, sizeof(strValue));

		//		pstrKeyPtr = strKey;
		//		pstrValuePtr = strValue;
		//	}
		//	// (:)�� ������ �̸� ����. ������ ���� ���۵ȴ�
		//	else if( *pszParse == ':')		
		//	{
		//		bValue = true;
		//	}
		//	// ������ �׸��� �о�´�
		//	else
		//	{
		//		if( &(*pstrKeyPtr) == &strKey[MAX_STRING_COUNT-1])
		//		{
		//			I3PRINTLOG(I3LOG_FATAL, "warning!! buffer overflow. please check!!");
		//			return false;
		//		}

		//		*pstrKeyPtr = *pszParse;

		//		pstrKeyPtr++;
		//	}

		//	pszParse++;
		//	cnt++;
		//}

		//// TODO: strSession�� ���� �����Ѵ�. 32byte + 1byte(nullptr)
		//this->SetSessionID( strSession);

		//char* ptr = strSession;
		//char* desc = (char*)SessionKey;
		//*SessionKey = 0;

		//// ���ڿ� bytes���� ��ġ�� �����Ѵ�
		//for(INT32 i = 0; i < NET_SESSION_KEY_SIZE; i++)
		//{
		//	*desc = HexToInt(*ptr) * 16 + HexToInt(*(ptr + 1));

		//	ptr += 2;
		//	desc++;
		//}

		//this->SetSessionKey(SessionKey);

	#elif defined( LOCALE_JAPAN)

		if (!i3::contain_string(pszCmdLine, ""))
			return false;
	
		i3::string str_param = pszCmdLine;

		i3::string find_param = "/p2:";
		int start_pos = i3::contain_string(str_param, find_param);

		// �ش� ���� ������ ArGameLauncher�� ���� ����Ű�� ������ ���� ���Դϴ�.
		if (start_pos < 0)
			return false;

		str_param = str_param.substr(start_pos + find_param.size(), str_param.rfind(" ") - find_param.size());

		i3::wstring wstr_param;
		i3::mb_to_utf16(str_param, wstr_param);

		ArGameStartInfo AraGameInfo;

		if (GetGameParameterFromEncStr(wstr_param.c_str(), AraGameInfo) != ARTPRET_OK)
		{
			return false;
		}

		i3::string strSessionKey = AraGameInfo.ticket;
		strSessionKey = URL::Encode(strSessionKey);

		SetArarioSessionKey(strSessionKey.c_str());					// �α��� ���
		SetArarioUSN(static_cast<UINT32>(AraGameInfo.userIndex));	// ĳ�� ���� �������� ���.

	#endif	// LOCALE

#endif // NC_BUILD

	return true;
}

void GameLoginContext::GetIngameURLInfo(i3::vector<BANNER_URL_INFO>& out, URL_TYPE type)
{
	for each(BANNER_URL_INFO data in m_vecIngameURLInfo)
	{
		if (data.i32Type == type)
			out.push_back(data);
	}
}

void GameLoginContext::SetID_AndPassForRussiaWebLogin(char* pszID, char* pszPass)
{
	i3::string_ncopy_nullpad(m_szID_ForRussiaWebLogin, pszID, MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN-1);
	i3::string_ncopy_nullpad(m_szPass_ForRussiaWebLogin, pszPass, MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN-1);

	i3::string_ncopy_nullpad(m_GameLoginInfo.m_UserId, pszID, NET_ID_SIZE-1);
	i3::string_ncopy_nullpad(m_GameLoginInfo.m_UserPw, pszPass, NET_PW_SIZE-1);

	//I3PRINTLOG(I3LOG_NOTICE,"m_szID_ForRussiaWebLogin %s, m_szPass_ForRussiaWebLogin %s ", m_szID_ForRussiaWebLogin, m_szPass_ForRussiaWebLogin);
}

void GameLoginContext::SetID_AndPassForLatinWebLogin(const char* pszID, const char* pszPass,INT32 szAuthCode)
{
	i3::string_ncopy_nullpad(m_GameLoginInfo.m_UserId, pszID, NET_ID_SIZE-1);
	i3::string_ncopy_nullpad(m_GameLoginInfo.m_UserPw, pszPass, NET_PW_SIZE-1);


	m_nAuthCode=  szAuthCode ;

	//I3PRINTLOG(I3LOG_NOTICE,"m_szID_ForRussiaWebLogin %s, m_szPass_ForRussiaWebLogin %s ", m_szID_ForRussiaWebLogin, m_szPass_ForRussiaWebLogin);
}


#if defined (LOCALE_SINGMAL) || defined (LOCALE_PHILIPPINES) || defined (LOCALE_KOREA) || defined (LOCALE_VIETNAM)
	
void GameLoginContext::SetToken_ForGarena(char* pszToken)
{

	i3::string_ncopy_nullpad(m_szToken_ForGarena, pszToken, NET_ENCRYPTED_TOKEN_SIZE-1 );
	i3::string_ncopy_nullpad(m_GameLoginInfo.m_szGarenaToken, pszToken, NET_ENCRYPTED_TOKEN_SIZE-1);
}

#elif defined( LOCALE_JAPAN)

void GameLoginContext::SetArarioSessionKey(const char * pszSessionKey)
{
	i3::string_ncopy_nullpad(m_szArarioSessionKey, pszSessionKey, NET_ENCRYPTED_TOKEN_SIZE - 1);
	i3::string_ncopy_nullpad(m_GameLoginInfo.m_szArarioSessionKey, pszSessionKey, NET_ENCRYPTED_TOKEN_SIZE - 1);
}

#endif


void	GameLoginContext::SetReportFTPAddress( const std::string& ip, UINT16 port )
{
	m_ReportIP = ip;
	m_ReportPort = port;
}

void	GameLoginContext::SetReportFTPTime( UINT32 serverTime )
{
	m_ReportServerTime = serverTime;
}

void	GameLoginContext::setWebLoginKey(const char* key)
{
	i3::generic_string_copy(m_strWebLoginKey, key);
}

const char* GameLoginContext::getWebLoginKey()
{
	return m_strWebLoginKey;
}