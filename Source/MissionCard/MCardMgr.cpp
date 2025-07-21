#include "pch.h"
#include "MCardMgr.h"

#include "i3Base/itl/range/algorithm/find.h"
#include "i3Base/itl/range/algorithm/rfind.h"

#include "i3Base/itl/unordered_vector.h"

#include "MCardSetInfo.h"
#include "MCardInfo.h"
#include "MCardQuestInfo.h"
#include "MCardActionInfo.h"

#include "MCardSet.h"
#include "MCard.h"
#include "MCardQuest.h"
#include "MCardCallback.h"


MCardMgr::MCardMgr() : m_selCardSetIndex(-1), m_pCardInfoForReward(nullptr), 
	m_bIsCardRewardExist(false), m_bIsCardRewardFinal(false), m_bIsAnyQuestCompleted(false)
{
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		m_apMCardSet[i] = new MCardSet(i);
	}
}

MCardMgr::~MCardMgr()
{
	i3::cu::for_each_delete(m_vecCardSetInfoByID);
	i3::for_each(m_apMCardSet, m_apMCardSet+MAX_CARDSET_PER_USER, i3::fu::delete_fun() );
}

// �ϴ� ���� �����س���...���߿� config.h�� ImportStringFile�� �Բ� �����丵�Ͽ� �����ϴ°� ���ڴ�..
namespace
{

	i3RegKey *	_FindKey(const i3::literal_range& strKeyName, i3RegKey * pRoot, char KeySep)
	{
		typedef i3::literal_range::iterator		iterator;
		
		i3::literal_range strTest = strKeyName;
		i3::stack_string strFound;

		i3RegKey * pFoundKey = pRoot;

		while( !strTest.empty() )
		{
			iterator it_found = i3::range::find( strTest, KeySep );
			
			strFound.assign(strTest.begin(), it_found);
		
			pFoundKey = (i3RegKey *)i3TreeNode::FindNodeByName( pFoundKey, strFound.c_str() );
			if (pFoundKey == nullptr)
				return nullptr;
			
			if ( it_found == strTest.end() )
				break;

			strTest.set_begin(++it_found);
		}

		return pFoundKey;
	}

	bool ImportStringFile(i3RegKey* pRegRoot, const i3::literal_range& fileName)
	{
		// �ӹ�ī��� ��ũ��Ʈ �ؽ�Ʈ�� pef�� ������Ŵ...
		i3FileStream file;
		if( !file.Open( fileName.c_str(), STREAM_READ | STREAM_SHAREREAD))
		{
			return false;	//String ������ ����.
		}
		
		i3RegKey * pFindKey = nullptr;
		i3RegData * pFindData = nullptr;
		WCHAR16 wszReadLine[2048] = L"";

		i3::rc_wstring	wstrLocal;
		i3::string		strRegKey;
		i3::string		strRegData;

		file.SetPosition(sizeof(WCHAR16), STREAM_BEGIN);

		while( file.GetLastError() != i3Stream::STREAM_EOS)
		{
			INT32 nReadLength = file.ReadLineW(wszReadLine, sizeof(wszReadLine) );
			if( nReadLength == 0 )
				continue;

			//Label �� Local�� �и��Ѵ�.

			std::size_t wszReadLineLen = i3::generic_string_size(wszReadLine);		// nReadLength ����������� �ŷ��ϱⰡ ��������δ�..
			i3::const_wchar_range rng( wszReadLine, wszReadLine + wszReadLineLen );
			if (rng.empty())
				continue;

			i3::const_wchar_range::iterator it_sep = i3::range::find( rng, L'\t');
			if( it_sep == rng.end() )
				continue;

			i3::const_wchar_range wLabelRng( rng.begin(), it_sep);
			wstrLocal.assign( ++it_sep, rng.end() );

			if (wstrLocal.empty())
				continue;

			//Label���� RegKey�� RegData�� �и��Ѵ�.

			i3::const_wchar_range::iterator it_dir_sep = i3::range::find( wLabelRng, L'\\' );		

			if ( it_dir_sep != wLabelRng.end() )		// �ּ��� 1���� Ű�� �����ؾߵ�..
			{
				i3::const_wchar_range test_rng(++it_dir_sep, wLabelRng.end() );

				it_dir_sep = i3::range::rfind(test_rng, L'\\');				// ���� ������ ������ ���翩����..�����ϸ� RegKey�� RegData��Ʈ�� ���� ��� �����Ѵ�..

				i3::utf16_to_mb( i3::const_wchar_range(wLabelRng.begin(), it_dir_sep), strRegKey);

				if ( it_dir_sep != test_rng.end() )
				{
					i3::utf16_to_mb( i3::const_wchar_range(++it_dir_sep, test_rng.end()), strRegData);
				}
				else
				{
					strRegData.clear();
				}
			}
			else
			{
				strRegKey.clear();	strRegData.clear();
			}

			//i3RegistrySet���� �ش� Registry Ű�� ã�´�.
			pFindKey = _FindKey(strRegKey.c_str(), pRegRoot, '\\');

			if( pFindKey)
			{
				//ã�� Registry Ű���� �ش� Registry Data�� ã�´�.
				pFindData = pFindKey->FindDataByName( strRegData.c_str() );
				//������ �����. (����� Ÿ���� i3RegString���� ����...)
				if ( pFindData == nullptr )
				{
					i3RegString * pRegStr = i3RegString::new_object();
					pRegStr->SetName(strRegData);
					pFindKey->AddData(pRegStr);

					pFindData = pRegStr;
				}		

				//���� �־��.
				switch( pFindData->getDataType())
				{
				case I3REG_TYPE_STRING:
					{
						if( i3::generic_string_size( wstrLocal ) > 0)
						{
							//	���� �������߿� ������ '\\n'�� ������ �ƽ�Ű '\n'�� �ٽ� ��ȯ�ؾ� �Ѵ�.
							i3::const_wchar_range::iterator it = i3::range::find( wstrLocal, L'\\');
							if ( it != wstrLocal.end() )
							{
								++it; 
								if ( it != wstrLocal.end() && *it == L'n')
								{
									i3::wstring temp = wstrLocal;
									i3::generic_string_replace_all( temp, L"\\n", L"\n");
									wstrLocal = temp;
								}
							}
						}

						//	ã�Ƽ� ���� �ִ´�.
						((i3RegString*) pFindData)->setValue( wstrLocal);
					}
					break;
				case I3REG_TYPE_INT32:
					{
						INT32 val = i3::atoi(wstrLocal);

						//	ã�Ƽ� ���� �ִ´�.
						((i3RegINT32*) pFindData)->setValue( val);
					}
					break;
				case I3REG_TYPE_REAL32:
					{
						REAL32 val = (REAL32) i3::atof(wstrLocal);

						//	ã�Ƽ� ���� �ִ´�.
						((i3RegREAL32*) pFindData)->setValue( val);
					}
					break;
				default:
					{
						I3PRINTLOG(I3LOG_NOTICE,"i3RegistrySet::ImportStringFile invliad type : %d", pFindData->getDataType());
					}
					break;
				}
			}	
		
		}
		
		return true;
	}

	bool	FillMCardQuest(const QUEST_CTX* ctx, MCardQuestInfo* outQuestInfo)
	{
		bool bResult = true;
	

		switch(ctx->m_ID)
		{
		case 0:
			bResult = false;
			break;
		case 1:		// 0 - Action, 1 - Stage, 2 - Count, 3 - Acc(����, ����), 4 - Team, 5 - ��� ī��Ʈ
			{

				ResetMCardQuestInfo(outQuestInfo);
				outQuestInfo->m_QuestID = ctx->m_ID;
				outQuestInfo->m_ActionID = (ACTION_ID)ctx->m_Arg[0];

				outQuestInfo->m_StageID = getQuestStageID(STAGE_ID(ctx->m_Arg[1]) );	// QuestDef.h�� �Լ�.
				outQuestInfo->m_TargetCompleteCount = ctx->m_Arg[2];
				outQuestInfo->m_Team   = TEAM_TYPE(ctx->m_Arg[4]);
				outQuestInfo->m_bAccum = (ctx->m_Arg[3] != 0);
			}
			break;
		case 2:	   //0 - Action, 1 - Stage, 2 - Count, 3 - ������, 4 - ����ѹ�, 5 - ��� ī��Ʈ (����)	
			{

				ResetMCardQuestInfo(outQuestInfo);
				outQuestInfo->m_QuestID = ctx->m_ID;
				outQuestInfo->m_ActionID = (ACTION_ID)ctx->m_Arg[0];

				outQuestInfo->m_StageID = getQuestStageID(STAGE_ID(ctx->m_Arg[1]) );
				outQuestInfo->m_TargetCompleteCount = ctx->m_Arg[2];
				outQuestInfo->m_WeaponClassType = WEAPON_CLASS_TYPE(ctx->m_Arg[3]);
				outQuestInfo->m_WeaponNumber    = ctx->m_Arg[4];
				outQuestInfo->m_bAccum = true;
			}
			break;
		case 3:   // 0 - Action, 1 - Stage, 2 - Count, 3 - ������, 4 - ����ѹ�, 5 - ��� ī��Ʈ (����)
			{
				ResetMCardQuestInfo(outQuestInfo);
				outQuestInfo->m_QuestID = ctx->m_ID;
				outQuestInfo->m_ActionID = (ACTION_ID)ctx->m_Arg[0];

				outQuestInfo->m_StageID = getQuestStageID( STAGE_ID(ctx->m_Arg[1]) );
				outQuestInfo->m_TargetCompleteCount = ctx->m_Arg[2];	
				outQuestInfo->m_WeaponClassType = WEAPON_CLASS_TYPE(ctx->m_Arg[3]);
				outQuestInfo->m_WeaponNumber    = ctx->m_Arg[4];
				outQuestInfo->m_bAccum = false;
			}
			break;
		default:
			bResult = false;
			break;
		}

		return bResult;
	}

	void	FillMCardPrizeInfo_Mqf(const QUEST_ITEM_INFO_NEW& itemInfo, MCardPrizeInfo_Mqf* outPrize)
	{
		i3::svector<MCardPrizeInfo_Mqf_ItemInfo>& vecPrizeItem = outPrize->vecPrizeItem;

		outPrize->point = itemInfo.m_ui32Point;
		outPrize->exp = itemInfo.m_ui32Exp;
		outPrize->prize = itemInfo.m_ui32Prize;

		INT32 itemCount = 0;
		for (INT k = 0; k <MAX_QUEST_ITEM_CARD ; ++k)
		{
			// ��ȿ�� ���� ã�´�..
			if (itemInfo.m_Item[k].m_ui32ItemID > 0 && itemInfo.m_Item[k].m_i8AuthType > 0 && itemInfo.m_Item[k].m_ui32Arg > 0)
				++itemCount;
		}

		vecPrizeItem.resize(itemCount);

		itemCount = 0;

		for (INT k = 0; k <MAX_QUEST_ITEM_CARD ; ++k)
		{
			// ��ȿ�� ���� ã�´�..
			if (itemInfo.m_Item[k].m_ui32ItemID > 0 && itemInfo.m_Item[k].m_i8AuthType > 0 && itemInfo.m_Item[k].m_ui32Arg > 0)
			{
				vecPrizeItem[itemCount].m_ui32ItemID = itemInfo.m_Item[k].m_ui32ItemID;
				vecPrizeItem[itemCount].m_ui32Arg = itemInfo.m_Item[k].m_ui32Arg;
				vecPrizeItem[itemCount].m_i8AuthType = itemInfo.m_Item[k].m_i8AuthType;
				++itemCount;
			}
		}
	}

}

#define MAX_QUEST_REWARD_ITEM_COUNT		4

bool		MCardMgr::LoadRegistryFile(const i3::rc_string& strFileName, 
										   const i3::rc_string& scriptTextFileTitle)
{
	i3RegistrySet regFile;
	INT32 nReadBytes = regFile.LoadRegistrySet(strFileName.c_str(), REGISTRY_FILE_BINARY);

	if (nReadBytes == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Failed to load %s", strFileName );		
		return false;
	}
	
	i3RegKey * pRootRegKey = regFile.getRoot();
	i3RegKey* pMainKey = nullptr;
	{
		pMainKey = static_cast<i3RegKey*>(pRootRegKey->FindChildByName("Quest"));
		if (pMainKey == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Can't Find i3RegKey Quest");		
			return false;
		}
	}
	
	// Quest_����.txt ���� �κ�..
	const char* pszLanguageSuffix =	GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage);

	i3::stack_string textFilePath;
#if defined( MULTYLANGUAGE)
	i3::sprintf(textFilePath, "Locale/%s/String/%s_%s.txt", pszLanguageSuffix, scriptTextFileTitle, pszLanguageSuffix);
#else
	i3::sprintf(textFilePath, "Script/String/%s/%s_%s.txt", pszLanguageSuffix, scriptTextFileTitle, pszLanguageSuffix);
#endif

	if ( !ImportStringFile(pRootRegKey, textFilePath) )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Failed to load %s", textFilePath );		
		return false;
	}
			

	{
	// 1. ī��� ���ڿ� ���� Ȯ��
		i3RegKey* pKey = static_cast<i3RegKey*>(pMainKey->FindChildByName("CardSet"));
	
		if (pKey)
		{		
			i3::vector<MCardSetInfo*> vecCardSetInfoByID;
			const INT32 keyCount = pKey->getChildCount();

			for(INT32 i = 0; i < keyCount; i++)
			{
				i3RegKey* childKey = (i3RegKey*)pKey->getChild(i);

				const i3::rc_string& childKeyName = childKey->GetNameString();


				INT32 cardSetID = -1;
				GetKeyData(childKey, "CardSetId", &cardSetID);
				
				if (cardSetID < 1) continue;
				
				if (i3::vu::is_value_valid(vecCardSetInfoByID, cardSetID) == true)
				{
					MCardSetInfo* pAlreadySet = vecCardSetInfoByID[cardSetID];
					I3PRINTLOG(I3LOG_FATAL, "%s : Already Set exists (from SetID : %d)", childKeyName, pAlreadySet->setID);
					delete pAlreadySet;
					vecCardSetInfoByID[cardSetID] = nullptr;
				}
				
				
				MCardSetInfo* pNewSetInfo = new MCardSetInfo;
				InitMCardSetInfo(pNewSetInfo);
				i3::vu::set_value_force(vecCardSetInfoByID, cardSetID, pNewSetInfo);
				
				pNewSetInfo->setID = cardSetID;

				GetKeyData(childKey, "CardSetName", pNewSetInfo->wstrName);
				GetKeyData(childKey, "CardSetTitle", pNewSetInfo->wstrTitle);
				GetKeyData(childKey, "FileName", pNewSetInfo->wstrFileName);
				GetKeyData(childKey, "BonusBadgeMaster", &pNewSetInfo->bonusBadgeMaster);
				GetKeyData(childKey, "UITemplateName", pNewSetInfo->wstrUITemplateName);
				GetKeyData(childKey, "UITemplateIndex", &pNewSetInfo->UITemplateIndex);

				GetKeyData(childKey, "CardSetPeriod", pNewSetInfo->wstrEventPeriod);
			}

			m_vecCardSetInfoByID.assign(vecCardSetInfoByID.begin(), vecCardSetInfoByID.end());
		}
	}

	// 2. ���� ���ڿ� ���� Ȯ�� (4�� �ۿ� �Ⱦ��� ������, V1�� �޸� ���� ���� �����մϴ�...(pef/txt ��� )
	{
		i3RegKey* pKey = static_cast<i3RegKey*>(pMainKey->FindChildByName("Prize"));

		if (pKey)
		{	
			// ����, ����, ���� RegKey ���̺�
			i3RegKey* prizeTypeKey[BADGE_TYPE_COUNT];
			prizeTypeKey[BADGE_TYPE_RIBBON] = static_cast<i3RegKey*>(pKey->FindChildByName("Ribbon"));
			prizeTypeKey[BADGE_TYPE_ENSIGN] = static_cast<i3RegKey*>(pKey->FindChildByName("Ensign"));
			prizeTypeKey[BADGE_TYPE_MEDAL] = static_cast<i3RegKey*>(pKey->FindChildByName("Medal"));
			prizeTypeKey[BADGE_TYPE_MASTER] = static_cast<i3RegKey*>(pKey->FindChildByName("Master"));

			for (INT32 i = 0 ; i < BADGE_TYPE_COUNT ; ++i)
			{
				if (prizeTypeKey[i])
				{
					prizeTypeKey[i]->GetData("Description", m_awstrMedalDescription[i]);
				}
			}
		}
	}
	
	// 3. Quest������ ����Ǿ� �ִ� mqf���ϵ��� �а� MCardSetInfo�� ����..
	{
		
		QUEST_CTX	loadedQuest[MAX_QUEST_PER_CARDSET];		// 16*10�̶�� �Ѵ�... (�����δ� 4*10�����θ� ���� ����� ���� �� ����..)
		QUEST_ITEM_INFO_NEW2		itemInfoNew;

		i3::stack_string strQuestFileName;

		const size_t numCardSet = m_vecCardSetInfoByID.size();

		for (size_t i = 0 ; i < numCardSet ; ++i)
		{
			MCardSetInfo* pSet = m_vecCardSetInfoByID[i];
			if (pSet == nullptr)
				continue;

			const i3::rc_wstring& wstrFileName = pSet->wstrFileName;
			
			if (wstrFileName.empty())
				continue;

			i3::utf16_to_mb(wstrFileName, strQuestFileName);

			if ( QuestFileLoadEx(strQuestFileName.c_str(), loadedQuest, &itemInfoNew ) < 0)
			{
				I3PRINTLOG(I3LOG_FATAL, "not found file : %s\n", strQuestFileName);
				continue;
			}
				
			UINT32 count = 0;
			
			for (INT32 j = 0 ; j < MAX_CARD_PER_CARDSET ; ++j)		// 10
			{
				pSet->mCardInfo[j].reset(new MCardInfo);
				
				MCardInfo* pCardInfo = pSet->mCardInfo[j].get();

				InitMCardInfo(pCardInfo);

				for (INT32 k = 0; k < MAX_QUEST_PER_CARD_NEW ; ++k, ++count)	// 4
				{
					const QUEST_CTX& questCtx =	loadedQuest[count];
					
					MCardQuestInfo* new_quest_info = new MCardQuestInfo;

					bool bRes = FillMCardQuest(&questCtx, new_quest_info);

					if (bRes)
					{
						pCardInfo->aQuestInfo[k].reset(new_quest_info);
					}
					else
					{
						delete new_quest_info;
					}
				}
			}
			// ���� ������ �ƴ�..itemInfoNew�� ������ ��ƾ��Ѵ�...
			
			pSet->totPoint = 0;
			pSet->totExp = 0;	

			for (INT32 j = 0; j < MAX_CARD_PER_CARDSET ; ++j)
			{	
				MCardInfo* pCardInfo = pSet->mCardInfo[j].get();
				MCardPrizeInfo_Mqf& mCardPrize = pCardInfo->mCardPrize;
				QUEST_ITEM_INFO_NEW& itemInfo =	itemInfoNew.m_CardReward[j];

				FillMCardPrizeInfo_Mqf(itemInfo, &mCardPrize);
				pSet->totPoint	+= itemInfo.m_ui32Point;
				pSet->totExp	+= itemInfo.m_ui32Exp;
			}
			
		//	itemInfoNew.m_CompleteReward �� ���� �ݿ��Ǿ� ���� �ʾҴ� ���·� ���� �ݿ��ϴ°� ������ �ʹ�.
			{
				MCardPrizeInfo_Mqf& mCardPrize = pSet->prizeCompletion;
				QUEST_ITEM_INFO_NEW& itemInfo = itemInfoNew.m_CompleteReward;
				FillMCardPrizeInfo_Mqf(itemInfo, &mCardPrize);
			}

		}
	}
	
	// 4. Action String ����
	{
		i3RegKey* pKey = static_cast<i3RegKey*>(pMainKey->FindChildByName("Action"));

		if (pKey)
		{
			i3::vector<i3::rc_wstring> vecActionStrID;
			i3::vector<i3::rc_wstring> vecActionStrText;
			
			i3RegKey* pIDNameKey = static_cast<i3RegKey*>(pKey->FindChildByName("ID"));

			if (pIDNameKey)
			{
				const INT32 numData = pIDNameKey->GetDataCount();

				for (INT32 i = 0 ; i < numData ; ++i)
				{
					i3RegData* data = pIDNameKey->GetData(i);

					if (data->getDataType() == I3REG_TYPE_STRING)
					{
						const i3::rc_string& strID = data->GetNameString();
						int nID = i3::atoi(strID);
						const i3::rc_wstring& wstrText = static_cast<i3RegString*>(data)->getValue();
						i3::vu::set_value_force(vecActionStrID, nID, wstrText);
					}
				}
			}
			
			i3RegKey* pActionStringKey = static_cast<i3RegKey*>(pKey->FindChildByName("String"));

			if (pActionStringKey)
			{
				const INT32 numData = pActionStringKey->GetDataCount();
				for (INT32 i = 0 ; i < numData ; ++i)
				{
					i3RegData* data = pActionStringKey->GetData(i);		
					
					if (data->getDataType() == I3REG_TYPE_STRING)
					{
						const i3::rc_string& strID = data->GetNameString();
						int nID = i3::atoi(strID);
						const i3::rc_wstring& wstrText = static_cast<i3RegString*>(data)->getValue();

						i3::string strText;		i3::utf16_to_utf8(wstrText, strText);

						i3::vu::set_value_force(vecActionStrText, nID, GAME_RCSTRING(strText) );
					}
				}
			}

			const size_t numActionID = vecActionStrID.size();
			const size_t numActionString = vecActionStrText.size();

			const size_t numAction = ( numActionID < numActionString ) ? 
										numActionString : numActionID;

			m_vecActionByID.resize(numAction);
			
			for (size_t i = 0 ; i < numActionID ; ++i)
				m_vecActionByID[i].m_wstrActionID = vecActionStrID[i];
			for (size_t i = 0 ; i < numActionString ; ++i)
				m_vecActionByID[i].m_wstrActionName = vecActionStrText[i];
		}
	}

	return true;
}


const MCardActionInfo*	MCardMgr::GetMCardActionInfoByID( ACTION_ID actionID) const
{
	return &m_vecActionByID[actionID];
}

size_t		MCardMgr::GetMCardActionInfoCountByID() const
{
	return m_vecActionByID.size();	
}


const MCardSetInfo*	MCardMgr::FindMCardSetInfoBySetType( INT32 setType ) const
{ 
	if (setType < 0 || setType >= INT32(m_vecCardSetInfoByID.size()))
		return nullptr;
	return m_vecCardSetInfoByID[setType];
}

INT32		MCardMgr::GetMCardSetCountBySetType() const
{
	return INT32(m_vecCardSetInfoByID.size());
}

namespace
{

	void ActivateMCardSet(MCardSet* pSet, INT32 setType)
	{
		if (setType == QUEST_CARDSET_TYPE_NONE)
		{
			pSet->Deactivate();	return;
		}

		const MCardSetInfo* pSetInfo = MCardMgr::i()->FindMCardSetInfoBySetType(setType);
		if (pSetInfo == nullptr)
		{
			pSet->Deactivate();
			return;
		}

		pSet->Activate(pSetInfo);
	}

}

namespace 
{

	void UpdateSelectedMCardSetMCardMCardQuest(const QUESTING_INFO* questingInfo, INT32& outSelMCardSetIndex, MCardSet** inoutMCardSetArray )
	{

		outSelMCardSetIndex = questingInfo->m_ActiveIndexUse;

		if (outSelMCardSetIndex < 0 || outSelMCardSetIndex >= MAX_CARDSET_PER_USER)
			outSelMCardSetIndex = -1;
		
		if ( inoutMCardSetArray[outSelMCardSetIndex]->IsActive() == false )
			outSelMCardSetIndex = -1;

		for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
		{
			MCardSet* pSet = inoutMCardSetArray[i];

			if (pSet->IsActive() == false)
				continue;

			UINT8 cardIdx = questingInfo->m_pActiveIndexCardSet[i];

			pSet->SelectCardIndex( cardIdx);

			for (INT32 j = 0 ; j < MAX_CARD_PER_CARDSET ; ++j)
			{
				MCard* card = pSet->GetMCard( j );

				UINT16 questFlag = questingInfo->m_pActiveIndexCard[i][j];
				UINT16 bitMask = 0x000F;

				for (INT32 k = 0; k < MAX_QUEST_PER_CARD_NEW ; ++k , bitMask <<= 4)
				{
					bool bQuestComplete = ( (questFlag & bitMask) == bitMask );
					card->SetQuestCompletion_OffBattle(k, bQuestComplete);
				}
			}

		}

	}

}

void	MCardMgr::UpdateUserMCardInfoFromServer(const QUESTING_INFO* questingInfo, 	
													const UINT8 (&aQuestCtxActive)[ MAX_QUEST_ALL_USE_NEW ])	// ����Ʈ �Ϸ� ����
{
	// 1. 4���� �¾��̵� Ȱ��ȭī��� �迭�� �ű�..
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		UINT8 ui8CardType = questingInfo->m_pCardSetType[i];
		ActivateMCardSet(m_apMCardSet[i], ui8CardType);
	}
	
	// 2. �Ϸ� ���θ� �����Ѵ�..
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		MCardSet* pSet = m_apMCardSet[i];

		if (pSet->IsActive() == false)
			continue;

		for (INT32 j = 0 ; j < MAX_CARD_PER_CARDSET ; ++j)
		{
			MCard* pCard = pSet->GetMCard(j);

			for (INT32 k = 0 ; k < MAX_QUEST_PER_CARD_NEW ; ++k)
			{
				MCardQuest* pActiveQuest = pCard->GetMCardQuest(k);

				INT32 contextIdx =	i*MAX_CARD_PER_CARDSET*MAX_QUEST_PER_CARD_NEW + j*MAX_QUEST_PER_CARD_NEW + k;
				pActiveQuest->SetCompleteCount_OffBattle(aQuestCtxActive[contextIdx]);
			}
			
		}
	}

	// 3. ���� ���õ� �� /  �³��ο� ���õ� ī�� /  ī�� �ȿ� ���� ����Ʈ �ε��� ���� �ѱ��..
	{
		UpdateSelectedMCardSetMCardMCardQuest(questingInfo, m_selCardSetIndex, m_apMCardSet);
	}

}

void	MCardMgr::ActivateCardSetSlotFromServer(const QUESTING_INFO* questingInfo, INT32 setSlot)
{
	I3ASSERT( setSlot < MAX_CARDSET_PER_USER);

	UINT8 ui8CardType = questingInfo->m_pCardSetType[setSlot];
	ActivateMCardSet(m_apMCardSet[setSlot], ui8CardType);		// ��Ƽ��� ���� ����Ʈ������ ���� �Ϸ�ī��Ʈ�� 0���� ����.
	
	// ���� ���õ� �� /  �³��ο� ���õ� ī�� /  ī�� �ȿ� ���� ����Ʈ �ε��� ���� �ѱ��..
	{
		UpdateSelectedMCardSetMCardMCardQuest(questingInfo, m_selCardSetIndex, m_apMCardSet);
	}
}

void	MCardMgr::UpdateCardSetSlotsFromServer(const QUESTING_INFO* questingInfo)
{
	//
	// ������ ��� (�׸��� ���� �ִٸ� ���� �ƴ� ������ ���) ���� ���� ����ī��Ʈ�� �����Ǿ�� �Ǵ� ������ �ִ�.. ���� ���� ��Ÿ�԰� ���ؾߵɵ�??
	// ������ ���� �Ϸ��� m_apActiveMCardSet �迭�� ��°�� �ű�� ���� ���� �״�� ����..
	// �ٸ� ���� Activate��Ű�� ������ ó���Ѵ�...
	//

	i3::unordered_vector<MCardSet*> 
		vecMCardSet_Old( m_apMCardSet, m_apMCardSet+MAX_CARDSET_PER_USER);

	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
		m_apMCardSet[i] = nullptr;					// �ϴ� ��� nullptr�� ����..
		
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		UINT8 ui8CardType = questingInfo->m_pCardSetType[i];
		
		const size_t numOldSet = vecMCardSet_Old.size();

		for (size_t j = 0 ; j < numOldSet ; ++j)
		{
			MCardSet* pOldSet = vecMCardSet_Old[j];
			
			UINT8 ui8CardType_Old = QUEST_CARDSET_TYPE_NONE;

			if (pOldSet->GetMCardSetInfo())
				ui8CardType_Old = UINT8(pOldSet->GetMCardSetInfo()->setID);

			if (ui8CardType_Old == ui8CardType)
			{
				m_apMCardSet[i] = pOldSet;
				m_apMCardSet[i]->m_myIndex = i;			// myIndex ����ȭ�� ������ �ȵ�..
				vecMCardSet_Old.erase(j);
				break;
			}
		}
	}

	// �� �͸� ó��...
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		if ( m_apMCardSet[i] == nullptr )
		{
			I3ASSERT(!vecMCardSet_Old.empty());
			m_apMCardSet[i] = vecMCardSet_Old.back();
			m_apMCardSet[i]->m_myIndex = i;
			vecMCardSet_Old.pop_back();

			UINT8 ui8CardType = questingInfo->m_pCardSetType[i];	// 
			ActivateMCardSet(m_apMCardSet[i], ui8CardType);
		}
	}

	{
		UpdateSelectedMCardSetMCardMCardQuest(questingInfo, m_selCardSetIndex, m_apMCardSet);
	}
	
}

i3::rc_wstring MCardMgr::GetPrizeDescription(BADGE_TYPE t) const
{ 
	if (t == BADGE_TYPE_UNKOWN)
		return i3::rc_wstring();
	return m_awstrMedalDescription[t]; 
}

void		MCardMgr::ModifyQuestFromServer(INT32 activeSetIndex, INT32 cardIdx, INT32 questIdx, const QUEST_CTX& ctx)
{
	I3ASSERT(activeSetIndex < MAX_CARDSET_PER_USER);
	I3ASSERT(cardIdx < MAX_CARD_PER_CARDSET);
	I3ASSERT(questIdx < MAX_QUEST_PER_CARD_NEW);

	MCardSet* pSet = m_apMCardSet[activeSetIndex];
	MCard* pCard = pSet->GetMCard(cardIdx);
	MCardQuest* pActiveQuest = pCard->GetMCardQuest(questIdx);
	
	const MCardQuestInfo* pQuest = pActiveQuest->GetMCardQuestInfo();
	I3ASSERT(pQuest != nullptr);			// �ϴ� �����ؾ��Ѵٰ� �����Ѵ�..
	
	FillMCardQuest(&ctx, const_cast<MCardQuestInfo*>(pQuest));		// MCardQuest ���� ����..
	pActiveQuest->SetCompleteCount_OffBattle( ctx.m_Arg[MAX_QUEST_ARG_COMPLETE]);		// �Ϸ�ī��Ʈ�� ���� �������ش�..
}

void	MCardMgr::ModifyCardPrizeFromServer(INT32 activeSetIndex, INT32 cardIdx, const QUEST_ITEM_INFO_NEW& questItemInfo)
{
	I3ASSERT(activeSetIndex < MAX_CARDSET_PER_USER);
	I3ASSERT(cardIdx < MAX_CARD_PER_CARDSET);

	MCardSet* pSet = m_apMCardSet[activeSetIndex];
	MCard* pActiveCard = pSet->GetMCard(cardIdx);

	const MCardInfo* pCardInfo = pActiveCard->GetMCardInfo();
	I3ASSERT(pCardInfo != nullptr);			// �ϴ� �����ؾ��Ѵٰ� �����Ѵ�..

	FillMCardPrizeInfo_Mqf(questItemInfo, const_cast<MCardPrizeInfo_Mqf*>(&pCardInfo->mCardPrize) );		// �������� �ִ� ���, const�� ���� ����..
}

void	MCardMgr::UpdateSelectedCardQuestFromServer_OnBattle(INT32 questIdx, INT32 completeCountOfQuest, bool isQuestComplete)
{
	//I3ASSERT(m_selCardSetIndex != -1);
	if (m_selCardSetIndex < 0)
		return;

	MCardSet* pSet = m_apMCardSet[m_selCardSetIndex];

	MCard* pActiveCard = pSet->GetSelectedMCard();

	if (pActiveCard && pActiveCard->IsActive())
	{
		bool bCompleteQuest = false;
		bool bCompleteMissionCard = false;

		I3ASSERT(questIdx < MAX_QUEST_PER_CARD_NEW);
		
		MCardQuest* activeQuest =	pActiveCard->GetMCardQuest(questIdx);
		activeQuest->SetCompleteCount_OnBattle(completeCountOfQuest);		// ���� �� ���� ī�������� 
																	// �Ϸ���� ���� ä�� ������ ���߿� ��������쿡�� ���������ε� �����ؾ���..
																	//
		
		if (pActiveCard->IsQuestCompleted_OnBattle(questIdx) == false && 
			isQuestComplete == true)
		{
			pActiveCard->SetQuestCompletion_OnBattle(questIdx, true);	// �� �� ����Ʈ �Ϸ� ����..
			bCompleteQuest = true;

			// pActiveCard�� �������� ��ü ����Ʈ �Ϸ�Ǿ����� ���θ� �ݹ����� ����°� ���ڴ�..
			if ( pActiveCard->IsAllQuestCompleted_OnBattle() )
				bCompleteMissionCard = true;
		}

		m_pCallback->OnCurrQuestCompletionCountChanged_OnBattle(questIdx, bCompleteMissionCard, bCompleteQuest);

		if (bCompleteQuest)
		{
			GameEventSender::i()->SetEvent(EVENT_N_QUEST_END_NOTICE);	// ��Ʋ �� �Ϸ�� �̺�Ʈ ������ (ó���� �Ǵ����� �ǹ���)
			m_pCallback->OnCurrQuestCompleted_OnBattle(questIdx, bCompleteMissionCard);			// HUD�� AddChallengeClear() �� ����Ǿ�� �Ѵ�..
		}
		
		if (bCompleteMissionCard)
			m_pCallback->OnCurrMissionCardCompleted_OnBattle(questIdx);
	}

}

// ���� ������ ���̽��� Ŭ����̵� 3��, �������̵� 1����..
// 1. EVENT_GIVEUP_BATTLE�� Ŭ�󿡼� SEND�Ҷ� ���ÿ�..
// 2. ������ǥ ����� �����ڰ� �ڵ����� ���������� �Ǵ� ���..( PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK )
// 3. EVENT_M_TIMEOUTCS (��Ʋ�� �������� ��ġ�� ���) .. 
// 4. EVENT_M_TIMEOUTCN (��Ʋ�� �������� ��ġ�� ���) .. 

void	MCardMgr::ProcessGiveupBattle()
{
	// ���� ������ ��� ����Ʈ �Ϸ�� ��� ��ҵȴ�..
	if (m_selCardSetIndex == -1)
		return;
	
	MCardSet* pSet = m_apMCardSet[m_selCardSetIndex];
	MCard* pCard = pSet->GetSelectedMCard();

	if (pCard == nullptr)
		return;
	if (pCard->IsActive() == false)
		return;

	pCard->CancelCompletion();
		
}

void	MCardMgr::ReceiveSelectAndResultFromServer(bool isFinalCardCompleted, bool isEndProtocal )	// Ŭ��ۼ����� �������ε� ���̰�, ��Ʋ ���� �Ŀ��� �Ϲ������� �ޱ⵵ �ϴ� �� ����..
{
	// ��Ʋ �ƴ� �ٱ����� ȣ��Ǹ�, �̰����� Quest �� ī�� �ϷḦ �����ϰ� �������൵ �ǰڴ�... 
	if ( m_selCardSetIndex == -1)
		return;

	MCardSet* pSelectedSet = m_apMCardSet[m_selCardSetIndex];
	MCard*	pSelectedCard = pSelectedSet->GetSelectedMCard();
	
	if (pSelectedCard == nullptr || pSelectedCard->IsActive() == false )
		return;

	if( !isEndProtocal )
		m_bIsCardRewardFinal = isFinalCardCompleted;		// ENDBATTLE �������� ��ó���� �ϸ� �ȵ�.
	
	bool bAnyQuestComplete = pSelectedCard->IsDifferentComplete_OnOffBattle();

	if (bAnyQuestComplete == false)		// ���� ��Ʋ���� ���� ��찡 �ƴѰ� ���� ��� �ϴ� ����..
	{
		pSelectedCard->ConfirmCompletion();				// ��Ʋ ��������� �ƴϴ��� ���� ������ ���� ���� ���̴�.
		return;
	}
		
	m_pCardInfoForReward = pSelectedCard->GetMCardInfo();
	m_bIsAnyQuestCompleted = true;		// ����Ʈ ���఻���� ������ �ϴ� true... ���߿� ����ǥ�ôܰ迡�� �����ؾߵ�..

	bool bOnBattleAllComplete = pSelectedCard->IsAllQuestCompleted_OnBattle();
	bool bOffBattleAllComplete = pSelectedCard->IsAllQuestCompleted_OffBattle();
	bool bAllQuestComplete   = (bOffBattleAllComplete == false && bOnBattleAllComplete == true);
		
	pSelectedCard->ConfirmCompletion();				// ��Ʋ ��������� �ƴϴ��� ���� ������ ���� ���� ���̴�.
			
	// ��Ʋ ���� �� �����⵵ �ϴµ�, �̶� �����˾��� ���õ� ������ �׳� �ݹ�����.....
	INT32 prevCardIndex = pSelectedCard->GetMyIndex();
	INT32 nextCardIndex = prevCardIndex;

	if (bAllQuestComplete)
	{
		m_bIsCardRewardExist = true;

		m_pCallback->OnCurrMissionCardCompleted_AfterBattle(prevCardIndex, pSelectedCard->GetMCardInfo());
		// ī�� �Ϸ� ��������, ���� ��ī��� �ڵ����� �̵��ؾ��Ѵ�.....
		nextCardIndex = pSelectedSet->SelectNextAvailableCardIndex();
		//
	}
	else
	{
		m_bIsCardRewardExist = false;
		m_bIsCardRewardFinal = false;			// ������ ���� ���� ���̸� ���������÷��װ� �߸��Ȱ���..
	}

	// ������ �����ϴ� ���� ����..
	MCardMgr::SendClientSelectStateToServer();		
	//

	if ( bAllQuestComplete ) // ����ī�尡 �̵��� ��츦 ���� �ݹ�....
	{
		m_pCallback->OnNextMissionCardAutoSelecting_AfterBattle( prevCardIndex, nextCardIndex );	// SEND�� ��������...���� ���������� ���� ����....
	}																								// ���̻� ���õ� ī�尡 ���� ��쿡�� �������� �Ѵ�.

}

void	MCardMgr::SendClientSelectStateToServer()
{
	// 
	QUEST_ACTIVE_INDEX questActiveIdx;
	
	if (m_selCardSetIndex == -1)
	{
		questActiveIdx._idxUse = 0xFF;
		questActiveIdx._idxCard = 0xFF;
		questActiveIdx._idxQuest = 0xFFFF;
	}
	else
	{
		questActiveIdx._idxUse = UINT8(m_selCardSetIndex);

		MCardSet* pSet = m_apMCardSet[m_selCardSetIndex];
		MCard* pSelCard = pSet->GetSelectedMCard();
		
		if (pSelCard == nullptr)
		{
			questActiveIdx._idxCard = 0xFF;
			questActiveIdx._idxQuest = 0xFFFF;
		}
		else
		{
			questActiveIdx._idxCard = UINT8(pSelCard->GetMyIndex());
			
			UINT16 questFlag = 0x0000;

			for (INT32 i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; ++i)
			{
				UINT16 val = pSelCard->IsQuestCompleted_OffBattle(i) ? 0xF : 0x0;
				questFlag |= (val) << (i * 4);
			}

			questActiveIdx._idxQuest = questFlag;
		}	
	}
	
	GameEventSender::i()->SetEvent(EVENT_QUEST_CHANGE_ACTIVE_IDX, &questActiveIdx);
	
}

INT32	MCardMgr::FindCardSetSlotWithCardSetType(INT32 cardSetType) const
{
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		MCardSet* pSet = m_apMCardSet[i];

		if (pSet->IsActive() == false)
			continue;
		
		if (pSet->GetMCardSetInfo()->setID == cardSetType)
			return i;
	}

	return -1;	
}



void	MCardMgr::SelectActiveCardSetToServer(INT32 selSetIdx)	// ī���ε����� ���� ����� �ε��� �״�� ����..
{
	if (selSetIdx < 0 || selSetIdx >= MAX_CARDSET_PER_USER)
		selSetIdx = -1;
	else
	if ( m_apMCardSet[selSetIdx]->IsActive() == false)
		selSetIdx = -1;

	m_selCardSetIndex = selSetIdx;

	MCardMgr::SendClientSelectStateToServer();
}

void	MCardMgr::SelectActiveCardSetToServer(INT32 selSetIdx, INT32 selCardIdx)
{
	if (selSetIdx < 0 || selSetIdx >= MAX_CARDSET_PER_USER)
		selSetIdx = -1;
	else
	if ( m_apMCardSet[selSetIdx]->IsActive() == false)
		selSetIdx = -1;

	m_selCardSetIndex = selSetIdx;
	MCardMgr::SelectActiveCardToServer(selCardIdx);
}

void	MCardMgr::SelectActiveCardToServer(INT32 selCardIdx)
{
	if (m_selCardSetIndex == -1)
		return;
	
	MCardSet* pSet = m_apMCardSet[m_selCardSetIndex];
	pSet->SelectCardIndex(selCardIdx);
	MCardMgr::SendClientSelectStateToServer();
}


// �������� �ִ� ������� �ؾ��� �� ����..

INT32	MCardMgr::FindNextAvailableCardSetSlotIndex() const
{
	if (m_selCardSetIndex == -1)
	{
		// ������ ������� �ε��� ó������ ���� ������η� ó��..
		const size_t numServerList = m_vecCardSetTypeList_Server.size();

		for (size_t i = 0 ; i < numServerList ; ++i)
		{
			INT32 setType =	m_vecCardSetTypeList_Server[i];

			INT32 slotIdx = MCardMgr::FindCardSetSlotWithCardSetType(setType);

			if (slotIdx != -1)
				return slotIdx;
		}
		
		return -1;
	}

	// ���õ� ��Ʈ�� ������ �� �� �����ɷ� ó���ϵ��� �ؾ��Ѵ�..
	
	MCardSet* pCurrSet = m_apMCardSet[ m_selCardSetIndex ];
	INT32 selSetType = pCurrSet->GetMCardSetInfo()->setID;
	INT32 selServerIdx = m_vecServerListIdx_BySetType[selSetType];
	
	I3ASSERT(selServerIdx != -1);

	const INT32 numServerList = INT32(m_vecCardSetTypeList_Server.size());

	INT32 nextSel = selServerIdx + 1;
	if (nextSel >= numServerList)
		nextSel = 0;

	while(nextSel != selServerIdx)
	{
		INT32 nextSetType = m_vecCardSetTypeList_Server[nextSel];

		INT32 slotIdx = MCardMgr::FindCardSetSlotWithCardSetType(nextSetType);
		
		if (slotIdx != -1)
			return slotIdx;

		++nextSel;
		if (nextSel >= numServerList)
			nextSel = 0;
	}

	return -1;
}


namespace
{
	class MCardCallback_None : public MCardCallback
	{
		virtual void OnCurrQuestCompletionCountChanged_OnBattle(INT32 questIdx, bool bCompleteCard, bool bCompleteQuest) {}
		virtual void OnCurrQuestCompleted_OnBattle(INT32 questIdx, bool bCompleteCard) {}
		virtual void OnCurrMissionCardCompleted_OnBattle(INT32 lastQuestIdx) {}

		virtual void OnCurrMissionCardCompleted_AfterBattle(INT32 completeCardIndex, const MCardInfo* card ) {}
		virtual void OnNextMissionCardAutoSelecting_AfterBattle(INT32 prevCardIdx, INT32 currCardIdx) {}
		virtual void OnUpdateAllHUDOnBattle() {}
	} default_callback;
}

void	MCardMgr::SetCallback(MCardCallback* callback)
{
	m_pCallback = (callback) ? callback : &default_callback;
}

MCardSet*	MCardMgr::GetSelectedMCardSet() const
{
	if (m_selCardSetIndex == -1)
		return nullptr;
	return m_apMCardSet[m_selCardSetIndex];
}

INT32		MCardMgr::GetSelectedMCardSetIndex() const
{
	return m_selCardSetIndex;
}

MCard*	MCardMgr::GetSelectedMCard() const
{
	MCardSet*	pSet = MCardMgr::GetSelectedMCardSet();
	if (pSet == nullptr)
		return nullptr;
	return pSet->GetSelectedMCard();
}

void		MCardMgr::UpdateAllHUDCallbackOnBattle()
{
	m_pCallback->OnUpdateAllHUDOnBattle();	// pHUD->ChangeChallenge(); �� ��ü��..
}

bool		MCardMgr::RemoveSelectedCardSetAfterFinalPrize()
{
	if (m_selCardSetIndex == -1)
		return false;

	MCardSet* pSet = m_apMCardSet[ m_selCardSetIndex];
	if (!pSet->IsActive())
		return false;
	
	// �̺�Ʈ ī��� �������� �˾Ƽ� ������ �ʴ� �� ���������� ����̴�..
	if (pSet->GetMCardSetInfo()->setID == QUEST_CARDSET_TYPE_EVENT)
		return false;			
	
	pSet->Deactivate();
	m_selCardSetIndex = -1;
	return true;
}

//
// ī������� ������ �����°� ����... �Ƹ��� �������� �̹� ��������, Ŭ�󿡼� �̰� �ݿ��ϴ� ����̴�..
//

bool		MCardMgr::RemoveCardSetAfterFinalPrize(INT32 cardSetSlot)
{
	if (cardSetSlot < 0 || cardSetSlot >= MAX_CARDSET_PER_USER)
		return false;

	MCardSet* pSet = m_apMCardSet[ cardSetSlot];
	if (!pSet->IsActive())
		return false;
	
	// �̺�Ʈ ī��� �������� �˾Ƽ� ������ �ʴ� �� ���������� ����̴�..
	if (pSet->GetMCardSetInfo()->setID == QUEST_CARDSET_TYPE_EVENT)
		return false;			
	
	pSet->Deactivate();
	if (m_selCardSetIndex == cardSetSlot)
		m_selCardSetIndex = -1;

	return true;
}


i3::rc_wstring MCardMgr::GetCurrHUDDesc(INT32 idxQuest) const
{
	MCard* pCard = MCardMgr::GetSelectedMCard();
	
	if (pCard == nullptr)
		return i3::rc_wstring();
	
	MCardQuest* pQuest = pCard->GetMCardQuest(idxQuest);

	return pQuest->GetCurrHUDDesc();
}

//
// �ӹ�ī�� �� Ȱ��ȭ ���� ����. �̰� ������ �����Ǿ� ����..
//
void	MCardMgr::UpdateSystemInfoFromServer(UINT8 ui8ActiveQuestCount, UINT32* pui32ActiveQuest, const UINT32 (&ui32QuestCardGoodsID)[QUEST_CARDSET_TYPE_COUNT])
{
	
	// ������ ���� ���� ��� ũ���� ������ �Դϴ�. �α� �ϳ� �ھ��ּŵ� ������ �����ϴ�. -- 20110530 ������ --
	if( (((QUEST_CARDSET_TYPE_COUNT-1) / 32)+1) != ui8ActiveQuestCount )	return;

	m_vecCardSetTypeList_Server.clear();			// ������ ��Ÿ���� ������ ���̰�..
	m_vecServerListIdx_BySetType.clear();			// ������ ��Ÿ���� �ε����� ���翩�θ� Ȯ���ϴ� �����̳�..

	i3::vector<INT32> vecCardSetTypeList_Server;
	i3::vector<INT32> vecServerListIdx_BySetType;
	
	vecServerListIdx_BySetType.resize(QUEST_CARDSET_TYPE_COUNT, -1);

	for ( INT32 i = 0 ; i < QUEST_CARDSET_TYPE_COUNT ; ++i )
	{
		if( 0x1 & (pui32ActiveQuest[ i / 32 ] >> ( i % 32 )) )
		{
			vecCardSetTypeList_Server.push_back(i);
			
			vecServerListIdx_BySetType[i] = vecCardSetTypeList_Server.size() - 1;
		}
	}
	
	m_vecCardSetTypeList_Server.assign(vecCardSetTypeList_Server.begin(), vecCardSetTypeList_Server.end());
	m_vecServerListIdx_BySetType.assign(vecServerListIdx_BySetType.begin(), vecServerListIdx_BySetType.end());

	const INT32 numSizeSetType = (	QUEST_CARDSET_TYPE_COUNT < INT32(m_vecCardSetInfoByID.size() ) ) 
									? QUEST_CARDSET_TYPE_COUNT : INT32(m_vecCardSetInfoByID.size() );

	for (INT32 i = 0 ; i < numSizeSetType ; ++i)
	{
		if (m_vecCardSetInfoByID[i])
			m_vecCardSetInfoByID[i]->goodsID = ui32QuestCardGoodsID[i];	
	}

}

INT32		MCardMgr::GetMCardSetCount_Server() const
{
	return 	m_vecCardSetTypeList_Server.size();
}

INT32		MCardMgr::GetMCardSetType_Server(INT32 idx) const // �μ��� ��Ÿ���� �ƴϰ� ���ϰ��� ��Ÿ����..
{
	if ( size_t(idx) >= m_vecCardSetTypeList_Server.size() )
		return QUEST_CARDSET_TYPE_NONE;
	return m_vecCardSetTypeList_Server[idx];
}

bool		MCardMgr::IsExistMCardSet_Server(INT32 MCardSetType) const
{
	if ( size_t(MCardSetType) >= m_vecServerListIdx_BySetType.size() )
		return false;
	return m_vecServerListIdx_BySetType[MCardSetType] != -1;
}

INT32		MCardMgr::GetServerIndexWithMCardSetType(INT32 MCardSetType) const
{
	if ( size_t(MCardSetType) >= m_vecServerListIdx_BySetType.size() )
		return -1;
	return m_vecServerListIdx_BySetType[MCardSetType];
	
}




MCardSet*	MCardMgr::GetMCardSet(INT32 slotIdx) const
{
	return m_apMCardSet[slotIdx];
}

bool		MCardMgr::HasMCardSet(INT32 cardSetType) const
{
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		const MCardSet* pSet = GetMCardSet(i);
		
		if (pSet->IsActive() == false)
			continue;
		
		if ( pSet->GetMCardSetInfo()->setID == cardSetType )
			return true;
	}

	return false;
}


void	MCardMgr::PopRewardMark(const MCardInfo*& outInfo, bool& isAnyQuestCompleted, 
								bool& isCardRewardExist, bool& isFinalCard) 
{
	outInfo = m_pCardInfoForReward;
	isAnyQuestCompleted = m_bIsAnyQuestCompleted;
	isCardRewardExist = m_bIsCardRewardExist;
	isFinalCard = m_bIsCardRewardFinal;

	m_pCardInfoForReward = nullptr;
	m_bIsAnyQuestCompleted = false;
	m_bIsCardRewardExist = false;
	m_bIsCardRewardFinal = false;
}
