#include "pch.h"
#include "ChannelContext.h"

#include "UserInfoContext.h"

void		ChannelContext::SetChannelUserCountArray( const UINT16* user_count_array, INT32 num)
{
	I3_BOUNDCHK( num, GAME_CHANNEL_COUNT+1);

	for( INT32 i = 0 ; i < num ; i++ )
	{
		_nUserCount[i] = user_count_array[i];
	}
	//i3::copy(user_count_array, user_count_array + num, _nUserCount);
}

#ifdef max
#undef max
#endif

ChannelContext::ChannelContext() : _nMaxUserForChannel(std::numeric_limits<INT32>::max())
{
	
}

void		ChannelContext::Reset()
{
	_nChannelCount = 0;
	_nMaxUserForChannel = std::numeric_limits<INT32>::max();	// hansoft 9288.
	for( INT32 i = 0; i < GAME_CHANNEL_COUNT; i++ ) _nUserCount[ i ] = 0;

	m_bMoveChannel = false;
}

bool _IsValidKillDeath(ChannelType channel, REAL32  Killcount, REAL32 Deathcount)
{
	const ConfigLogic::ChannelConditionInfo * pCondiInfo = 
		g_pConfigEx->GetLogic().GetChannelConditionInfo(channel);

	I3ASSERT(pCondiInfo != nullptr);
	I3ASSERT(pCondiInfo->condition == ConfigLogic::ChannelConditionInfo::KILLDEATH);

	REAL32 rate = pCondiInfo->value_min * 0.01f;

	if((Killcount + Deathcount) != 0)
	{
		if((Killcount / (Killcount + Deathcount)) > rate )
		{
			return false;
		}
	}
	return true;
}

bool _IsValidRank(ChannelType channel, UINT32 CurrentRank)
{
	const ConfigLogic::ChannelConditionInfo * pCondiInfo = 
		g_pConfigEx->GetLogic().GetChannelConditionInfo( channel );

	I3ASSERT(pCondiInfo != nullptr);
	I3ASSERT(pCondiInfo->condition == ConfigLogic::ChannelConditionInfo::RANK);

	const UINT32 LimitRankMin = static_cast<UINT32>(pCondiInfo->value_min);
	const UINT32 LimitRankMax = static_cast<UINT32>(pCondiInfo->value_max);

	if (LimitRankMin > CurrentRank || CurrentRank > LimitRankMax )
	{
		return false;
	}

	return true;
}


bool ChannelContext::IsValidChannelType(ChannelType channel) const
{
	if ( UserInfoContext::i()->CheckAuthLevel() ) return true;

	//EXPORT_AVALIABLE_CODE_VALUE
	switch(channel)
	{
	case CHANNEL_TYPE_BEGIN1:
		{
			REAL32  Killcount	= (REAL32)UserInfoContext::i()->GetMyUserInfoRecord()->_killcount;
			REAL32 Deathcount	= (REAL32)UserInfoContext::i()->GetMyUserInfoRecord()->_death;
			return _IsValidKillDeath( CHANNEL_TYPE_BEGIN1, Killcount, Deathcount );
		}
		break;

	case CHANNEL_TYPE_BEGIN2:
	case CHANNEL_TYPE_EXPERT:
	case CHANNEL_TYPE_EXPERT2:
		{
			// revision 30642 �ε��׽þ� �ű� ���� ���� ����
		#if defined( LOCALE_INDONESIA)
			if( channel == CHANNEL_TYPE_BEGIN2)
			{
				return true;
			}
		#endif
			UINT32 CurrentRank = UserInfoContext::i()->GetRealRank();
			return _IsValidRank( channel, CurrentRank );
		}
		break;

	case CHANNEL_TYPE_MIDDLE:
		{
			UINT32 CurrentRank = UserInfoContext::i()->GetRealRank();
	#if defined( LOCALE_TURKEY)	|| defined( LOCALE_INDONESIA) || defined( LOCALE_BRAZIL ) // revision 50299 Middle Channel ���� ���� üũ ��ƾ ���� (��Ű ������ ó��)
			return _IsValidRank( CHANNEL_TYPE_MIDDLE, CurrentRank );
	#else
			return _IsValidRank( CHANNEL_TYPE_EXPERT, CurrentRank );
	#endif
		}

	case CHANNEL_TYPE_CLAN:
	case CHANNEL_TYPE_PCCAFE_CLAN:	// revision 59499 Warnet Clan ���� Ÿ�� ���� �߰�.
		{
			// Clan Match Flag�� ����������, ��Ŭ������ ���� ����.(�뺴)
			if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) )
				return true;

			// ���Ե� Ŭ���� ������ ���� �Ұ�
			if (CLAN_MEMBER_LEVEL_UNKNOWN == UserInfoContext::i()->GetMyClanState())
				return false;
		}
		break;
	}

	return true;
}

ChannelType ChannelContext::GetChannelType(INT32 serverIdx, INT32 channelIdx) const
{ 
	I3_BOUNDCHK(serverIdx, MAX_GAME_SERVER_COUNT);
	I3_BOUNDCHK(channelIdx, GAME_CHANNEL_COUNT);
	
	if (serverIdx < 0)
		return CHANNEL_TYPE_NONE;

	return static_cast<ChannelType>(m_channelTypeList[serverIdx][channelIdx]); 
}	


ChannelType ChannelContext::GetConnectedChannelType( void ) const
{
	INT32 ChannelNumber = g_pServerInfo->GetCurChannelNumber();
	if ( ChannelNumber != INVALID_INDEX )
	{
		return ChannelContext::GetChannelType(g_pServerInfo->GetCurGameServerIndex(), ChannelNumber);
	}
	return CHANNEL_TYPE_NONE;
}

void ChannelContext::LoadServerTextFromFile()
{
	ChannelType nMode = CHANNEL_TYPE_NONE;
	WCHAR16 szwReadLine[512] = L"";
	char filePath[MAX_PATH] = "";

	i3FileStream file;

#if defined ( MULTYLANGUAGE)
	i3::sprintf( filePath, "Locale%sString/ServerSelect_Ver2.txt", g_pStringFilePath.c_str());
#else
	//	multi path
	ConvertLocalePath( filePath, "Script/String/ServerSelect_Ver2.txt", g_pStringFilePath.c_str());
#endif

	if( false == file.Open( filePath, STREAM_READ | STREAM_SHAREREAD) )
	{
		I3ASSERT_0;	//	�ݵ�� �־�� ��. ��ŵ�ϸ� ��ũ��Ʈ�� �ȳ���. komet
	}

	//	Check unicode
	{
		UINT32 rc = file.Read( szwReadLine, sizeof( WCHAR16));
		if( rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "ServerSelect file load fail !! (%s)", filePath);
			return;
		}
		else
		{
			szwReadLine[rc] = 0;

			//	���⿡ �ɸ��� ��ũ��Ʈ ������ �����ڵ尡 �ƴմϴ�. �ݵ�� �����ڵ� �����̾�� �մϴ�. komet
			if (!CHECK_UNICODE(szwReadLine[0]))
			{
				I3PRINTLOG(I3LOG_FATAL, "ServerSelect file have not unicode !! (%s)", filePath);
				return;
			}
		}
	}

	bool bContinue = true;
	while( bContinue )
	{
		UINT32 wBufSize = sizeof( szwReadLine);
		i3mem::FillZero( szwReadLine, wBufSize);

		// �����ڵ�� �� �ؽ�Ʈ�� ���پ� �н��ϴ�.
		UINT32 wReadSize = file.ReadLineW( szwReadLine, wBufSize);

		if( wReadSize == 0)	continue;

		I3ASSERT( wReadSize < wBufSize);


		// �̺�Ʈ ���� ���� �ڵ�� ���ܵӴϴ�. ���߿� ���� ���ɼ� ���
		//{
		//	i3::stack_wstring wstr_temp;

		//	//	�̺�Ʈ ����
		//	for(INT32 i=0; i<10; i++)
		//	{
		//		i3::sprintf( wstr_temp, L"[STR_SERVER_SELECT_CATEGORY_EVENT%d]", i);

		//		if( i3::contain_string( szwReadLine, wstr_temp ) >= 0 )
		//		{
		//			nMode = SS_MAINTYPE_EVENT0 + i;	// �̺�Ʈ
		//			I3_BOUNDCHK( nMode, SS_MAINTYPE_MAX);
		//			break;
		//		}
		//	}
		//}


		// ����
		if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_NORMAL]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_NORMAL;	// �Ϲ�
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_BEGINNER1]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_BEGIN1;	// �ʺ�
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_BEGINNER2]" ) >= 0)
		{
			nMode = CHANNEL_TYPE_BEGIN2;	// �ʺ� 2
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_CLAN]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_CLAN;	// Ŭ��
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_EXPERT1]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_EXPERT;	// ���
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_EXPERT2]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_EXPERT2;	// ��� 2
		}
		else if ( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_CHAMPIONSHIP]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_CHAMPIONSHIP;
		}
		else if ( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_NORMAL2]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_NORMAL2;
		}
		else if ( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_NATIONAL]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_NATIONAL;
		}
		else if ( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_MIDDLE]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_MIDDLE;	//�߰�
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_GWARNET]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_PCCAFE_SILVER;
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_GWARNET_GOLD]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_PCCAFE_GOLD;
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_GWARNET_CLAN]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_PCCAFE_CLAN;
		}
		else if( i3::contain_string( szwReadLine, L"[STR_SERVER_SELECT_CATEGORY_AZERBAIJAN]" ) >= 0 )
		{
			nMode = CHANNEL_TYPE_AZERBAIJAN;
		}
		else if( i3::generic_is_iequal( szwReadLine, L"[ServerSelectEnd]" )  )
		{
			bContinue = false;
			continue;
		}
		else if (szwReadLine[0] != L';')
		{
			i3::rc_wstring wstrlog;
			char_traits<wchar_t> wstrTest;
			wstrTest;

			if (i3::contain_string(szwReadLine, L"[CATEGORY_TEXT]") >= 0)
			{
				// Ŭ���� ���� �� ũ���� Ȯ�ο� �α�
				size_t length = wstrTest.length(szwReadLine);
				i3::sprintf(wstrlog, L"type %d / size %d / %s", (int)nMode, (int)length, szwReadLine);
				I3PRINTLOG(I3LOG_NOTICE, wstrlog);

				m_wstrChannelTypeString[nMode][SS_SUBTYPE_CATEGORY_TEXT] = (szwReadLine + 15);
			}
			else if (i3::contain_string(szwReadLine, L"[CATEGORY_SLOT_TEXT]") >= 0)
			{
				// Ŭ���� ���� �� ũ���� Ȯ�ο� �α�
				size_t length = wstrTest.length(szwReadLine);
				i3::sprintf(wstrlog, L"type %d / size %d / %s", (int)nMode, (int)length, szwReadLine);
				I3PRINTLOG(I3LOG_NOTICE, wstrlog);

				m_wstrChannelTypeString[nMode][SS_SUBTYPE_CATEGORY_SLOT_TEXT] = (szwReadLine + 20);
			}
			else if (i3::contain_string(szwReadLine, L"[CATEGORY_SLOT_FULL_TEXT]") >= 0)
			{
				// Ŭ���� ���� �� ũ���� Ȯ�ο� �α�

				size_t length = wstrTest.length(szwReadLine);
				i3::sprintf(wstrlog, L"type %d / size %d / %s", (int)nMode, (int)length, szwReadLine);
				I3PRINTLOG(I3LOG_NOTICE, wstrlog);

				m_wstrChannelTypeString[nMode][SS_SUBTYPE_CATEGORY_SLOT_FULL_TEXT] = (szwReadLine + 25);
			}
			else if (i3::contain_string(szwReadLine, L"[CATEGORY_READY_SLOT_TEXT]") >= 0)
			{
				// Ŭ���� ���� �� ũ���� Ȯ�ο� �α�
				size_t length = wstrTest.length(szwReadLine);
				i3::sprintf(wstrlog, L"type %d / size %d / %s", (int)nMode, (int)length, szwReadLine);
				I3PRINTLOG(I3LOG_NOTICE, wstrlog);

				//GetMainFrame()->setChannelSlotText(nMode, &szwReadLine[26]);
				m_wstrChannelTypeString[nMode][SS_SUBTYPE_CATEGORY_READY_SLOT_TEXT] = (szwReadLine + 26);
			}
		}
	}

	file.Close();
}

void ChannelContext::GetServerChannelText( i3::rc_wstring& output, ChannelType channel, SS_SUBTYPE subtype)
{
	if(channel < 0 || channel >= CHANNEL_TYPE_MAX ) return;
	if(subtype < 0 || subtype >= SS_SUBTYPE_COUNT ) return;	// �⺻ �ڼ��� �ȵ��ִ� ���·� ¥�� �ε����ڵ带 �ϴ� ������..Ŀ����Ƽ�� �ùٸ� ����.

	output = m_wstrChannelTypeString[channel][subtype];
}
