#include "pch.h"
#include "InvenDB.h"

#include "ui/UIItemInfoUtil.h"

#include "UI/PurchaseMgr.h"

namespace db_util
{
	i3::vector<INVEN_BUFFER*> g_none_list;

	bool CompareItemID_ForInvenBuffer( const INVEN_BUFFER * pLeft, const INVEN_BUFFER * pRight)
	{
		if ( pLeft->_ui32ItemID != pRight->_ui32ItemID )
			return pLeft->_ui32ItemID < pRight->_ui32ItemID ;
		return false;
	}

	template<typename TBuffer>
	struct Fn
	{
		T_ItemDBIdx _TItemWareDBIdx;
		Fn(T_ItemDBIdx i) : _TItemWareDBIdx(i) {}

		bool operator()(TBuffer p)
		{
			return (p->_TItemWareDBIdx == _TItemWareDBIdx);
		}
	};

	template<typename TBuffer, typename TList>
	void DeleteInvenBuffer(TList & list, T_ItemDBIdx WareDBIdx)
	{
		TList::iterator it = std::find_if(list.begin(), list.end(), db_util::Fn<TBuffer>(WareDBIdx) );
		if( it != list.end() )
		{
			I3MEM_SAFE_FREE( *it );
			list.erase(it);
		}
	}

	template< typename TBuffer, typename TList >
	bool UpdateInvenBufferToList(const TList & list, INVEN_BUFFER* pBuffer)
	{
		if (nullptr == pBuffer) return false;
		if (list.empty()) return false;
		if( ITEM_ATTR_TYPE_AUTH < pBuffer->_ui8ItemType )	return false;

		TList::const_iterator it = std::find_if(list.begin(), list.end(), db_util::Fn<TBuffer>(pBuffer->_TItemWareDBIdx) );
		if( it == list.end() ) return false;

		i3mem::Copy( *it, pBuffer, sizeof(INVEN_BUFFER) );
		return true;
	}


	template< typename TBuffer, typename TList >
	INVEN_BUFFER* Find_InvenBuffer(const TList & list, INVEN_BUFFER* pBuffer)
	{
		if (nullptr == pBuffer) return 0;
		if (list.empty()) return 0;
		if( ITEM_ATTR_TYPE_AUTH < pBuffer->_ui8ItemType )	return 0;

		TList::const_iterator it = std::find_if(list.begin(), list.end(), db_util::Fn<TBuffer>(pBuffer->_TItemWareDBIdx) );
		if( it == list.end() ) return 0;

		return *it;
	}
}


/**************************/
//iInvenDB
iInvenDB::~iInvenDB()
{
	i3::cu::for_each_SafeFree_clear(m_Main_list);
}

void iInvenDB::AddInvenBufferToList(INVEN_BUFFER* pBuffer,INT32 nCount)
{
	enum { DINO_RAPTER_ITEM_ID = 1500511,	};		//Rapter Item Id

	// ���� ����� ����ִٸ� ���̻� ó������ �ʴ´�.
	if (nCount <= 0 || pBuffer == nullptr)	return;

	for ( INT32 i = 0 ; i < nCount ; ++i )
	{
		// �����Ϳ� �̻��� ������ �˻縦 �մϴ�.
		if ( 0 == pBuffer[i]._ui32ItemID)
		{
			db_util::DeleteInvenBuffer<INVEN_BUFFER*>(m_Main_list, pBuffer[i]._TItemWareDBIdx);
			continue;
		}

		// ���� WareDBIndex�� InvenBuffer�� �ִٸ� ���� �����, ���ٸ� ���� �߰� �մϴ�.
		INVEN_BUFFER* pIB = db_util::Find_InvenBuffer<INVEN_BUFFER*>(m_Main_list, &pBuffer[i]);
		if( pIB == 0 )
		{
			pIB = (INVEN_BUFFER*)i3MemAlloc(sizeof(INVEN_BUFFER));
			i3mem::Copy(pIB, &pBuffer[i], sizeof(INVEN_BUFFER));
			m_Main_list.push_back(pIB);
		}
		else
		{
			i3mem::Copy( pIB, &pBuffer[i], sizeof(INVEN_BUFFER) );
		}

		//�������� ��� �����Ŀ� ������ ���(Ư�� ����) ItemWareDBIdx�� �˰� �վ�� �ϱ� ������ �����Ѵ�.
		PurchaseMgr::i()->OnNotify_NewInvenBufferAdded(pIB);
	}

	i3::sort(m_Main_list.begin(), m_Main_list.end(), db_util::CompareItemID_ForInvenBuffer);

	ReReferenceUsage_impl();
}

void iInvenDB::UpdateInvenBufferToList(INVEN_BUFFER* pBuffer)
{
	db_util::UpdateInvenBufferToList<INVEN_BUFFER*>(m_Main_list, pBuffer);

	ReReferenceUsage_impl();
}

void iInvenDB::ReReferenceUsage()
{
	ReReferenceUsage_impl();
}

/**************************/
//WeaponDB
WeaponDB::~WeaponDB()
{
}

void WeaponDB::ReReferenceUsage_impl()
{
	for(size_t i=0; i<Max;++i)
		m_Ref_list[i].clear();

	size_t Count = m_Main_list.size();
	for(size_t i = 0 ; i < Count ; i++)
	{
		INVEN_BUFFER* pBuffer = m_Main_list[i];

		CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(pBuffer->_ui32ItemID);
		if (pInfo == nullptr) continue;

		switch(pInfo->GetTypeUsage())
		{
		case WEAPON_SLOT_PRIMARY:	m_Ref_list[WeaponDB::Primary].push_back(pBuffer);		break;
		case WEAPON_SLOT_SECONDARY:	m_Ref_list[WeaponDB::Secondary].push_back(pBuffer);	break;
		case WEAPON_SLOT_MELEE:		m_Ref_list[WeaponDB::Melee].push_back(pBuffer);		break;
		case WEAPON_SLOT_THROWING1:	m_Ref_list[WeaponDB::Throw].push_back(pBuffer);		break;
		case WEAPON_SLOT_THROWING2:	m_Ref_list[WeaponDB::Item].push_back(pBuffer);	break;
		}
	}
}

const i3::vector<INVEN_BUFFER*>* WeaponDB::get_list(WEAPON_SLOT_TYPE nUsageType) const
{
	switch(nUsageType)
	{
	case WEAPON_SLOT_ALL:	return &m_Main_list;
	case WEAPON_SLOT_PRIMARY:	return &m_Ref_list[WeaponDB::Primary];		break;
	case WEAPON_SLOT_SECONDARY:	return &m_Ref_list[WeaponDB::Secondary];	break;
	case WEAPON_SLOT_MELEE:		return &m_Ref_list[WeaponDB::Melee];		break;
	case WEAPON_SLOT_THROWING1:	return &m_Ref_list[WeaponDB::Throw];		break;
	case WEAPON_SLOT_THROWING2:	return &m_Ref_list[WeaponDB::Item];	break;
	}
	return &db_util::g_none_list;
}



/**************************/
//CharacterDB
CharacterDB::~CharacterDB()
{
}

void CharacterDB::ReReferenceUsage_impl()
{
	for(size_t i=0; i<Max;++i)
		m_Ref_list[i].clear();

	//	ĳ������ �������� ������ ����. (���� ���ܵ� ����)
	//	�������� �޴� ����Ʈ(default) ĳ���ʹ� �ϳ��� �ε������� ����/��� 2������ ����� �κ��� �������� �ϹǷ� �̰��� �ݿ��ؾ��մϴ�
	m_nCharacterSlotCount = 0;

	size_t Count = m_Main_list.size();
	for(size_t i = 0 ; i < Count ; i++)
	{
		INVEN_BUFFER* pBuffer = m_Main_list[i];

		switch( CHARA::ItemID2Class(pBuffer->_ui32ItemID) )
		{
		case CHARACTER_CLASS_CHARA_TYPE:	
			m_Ref_list[CharacterDB::Chara].push_back(pBuffer);

			if( WEAPON::ItemID2Class( pBuffer->_ui32ItemID) == CHARACTER_TEAM_RB)
				++m_nCharacterSlotCount;	//	����/��� 2���� ��밡���� �ε����� ������ �ѷ� ������.
			break;
		case CHARACTER_CLASS_CHARA_DINO:	m_Ref_list[CharacterDB::Dino].push_back(pBuffer);	break;
		}
	}
}


const i3::vector<INVEN_BUFFER*>* CharacterDB::get_list(CHARACTER_CLASS_TYPE nUsageType) const
{
	switch(nUsageType)
	{
		case CHARACTER_CLASS_ALL: return &m_Main_list;
		case CHARACTER_CLASS_CHARA_TYPE:	return &m_Ref_list[CharacterDB::Chara];
		case CHARACTER_CLASS_CHARA_DINO:	return &m_Ref_list[CharacterDB::Dino]; break;
		default:
		{
			return &m_Main_list;
		}
	}
	
	//return &db_util::g_none_list;
}


/**************************/
//PartsDB
PartsDB::~PartsDB()
{
}

void PartsDB::ReReferenceUsage_impl()
{
	for(size_t i=0; i<Max;++i)
		m_Ref_list[i].clear();

	size_t Count = m_Main_list.size();
	for(size_t i = 0 ; i < Count ; i++)
	{
		INVEN_BUFFER* pBuffer = m_Main_list[i];

		switch( CHARA::ItemID2Class(pBuffer->_ui32ItemID) )
		{
		case CHARACTER_CLASS_HEADGEAR	:	m_Ref_list[PartsDB::Headgear].push_back(pBuffer);	break;
		case CHARACTER_CLASS_FACEGEAR	:	m_Ref_list[PartsDB::Facegear].push_back(pBuffer);	break;
		case CHARACTER_CLASS_UPPER		:	m_Ref_list[PartsDB::Upper].push_back(pBuffer);		break;
		case CHARACTER_CLASS_LOWER		:	m_Ref_list[PartsDB::Lower].push_back(pBuffer);		break;
		case CHARACTER_CLASS_GLOVE		:	m_Ref_list[PartsDB::Glove].push_back(pBuffer);		break;
		case CHARACTER_CLASS_BELT		:	m_Ref_list[PartsDB::Belt].push_back(pBuffer);		break;
		case CHARACTER_CLASS_HOLSTER	:	m_Ref_list[PartsDB::Holster].push_back(pBuffer);	break;
		case CHARACTER_CLASS_BERET		:	m_Ref_list[PartsDB::Beret].push_back(pBuffer);	break;
		default :
			I3PRINTLOG(I3LOG_FATAL,  "no parts item!");
			break;
		}
	}
}


const i3::vector<INVEN_BUFFER*>* PartsDB::get_list(CHARACTER_CLASS_TYPE nUsageType) const
{
	switch(nUsageType)
	{
	case CHARACTER_CLASS_ALL:		return &m_Main_list;
	case CHARACTER_CLASS_HEADGEAR:	return &m_Ref_list[PartsDB::Headgear];
	case CHARACTER_CLASS_FACEGEAR:	return &m_Ref_list[PartsDB::Facegear];
	case CHARACTER_CLASS_UPPER:		return &m_Ref_list[PartsDB::Upper];
	case CHARACTER_CLASS_LOWER:		return &m_Ref_list[PartsDB::Lower];
	case CHARACTER_CLASS_GLOVE:		return &m_Ref_list[PartsDB::Glove];
	case CHARACTER_CLASS_BELT:		return &m_Ref_list[PartsDB::Belt];
	case CHARACTER_CLASS_HOLSTER:	return &m_Ref_list[PartsDB::Holster];
	case CHARACTER_CLASS_BERET:		return &m_Ref_list[PartsDB::Beret];
	case CHARACTER_CLASS_UNKNOWN:	break;
	default :
		I3PRINTLOG(I3LOG_FATAL,  "no parts item!!");
	}
	return &db_util::g_none_list;
}

/**************************/
const i3::vector<INVEN_BUFFER*>* SkinDB::get_list( UINT32 usageType) const
{
	return &m_Main_list;
	//return &m_Ref_list[ usageType];
}

void SkinDB::ReReferenceUsage_impl()
{
	// ���� ������� �ʴ� ��ƾ���� �ּ� ó���մϴ�. (���� ����Ʈ�� m_Main_list�� Ȱ��)
	// ���� ������ ���� ����Ʈ ������ �ʿ��� ��� ���ۼ� �Ǿ�� �մϴ�. 2015.12.04 ���õ
	/*m_Ref_list.clear();

	const CHARA_PARTS_INFO * pRedParts = CCharaInfoContext::i()->GetMainCharaPartsInfo(TEAM_RED);
	const CHARA_PARTS_INFO * pBlueParts  = CCharaInfoContext::i()->GetMainCharaPartsInfo(TEAM_BLUE);

	if (pRedParts == nullptr || pBlueParts == nullptr)
		return;

	// vv3 - check
	const CCharaInfo * pRedChara = g_pCharaInfoDataBase->GetCharaDBInfo( pRedParts->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));
	const CCharaInfo * pBlueChara = g_pCharaInfoDataBase->GetCharaDBInfo( pBlueParts->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));

	UINT32 Count = m_Main_list.size();
	for( UINT32 i = 0 ; i < Count ; i++)
	{
		INVEN_BUFFER* pBuffer = m_Main_list[i];

		switch( EQUIP::ItemID2SkinClass(pBuffer->_ui32ItemID) )
		{
		case SKIN_CLASS_COMMON :	m_Ref_list.push_back(pBuffer);	break;
		case SKIN_CLASS_MAN :
			{
				if( pRedChara->GetCharaSexType() == CHARA::eSEX_TYPE_MALE || pBlueChara->GetCharaSexType() == CHARA::eSEX_TYPE_MALE )
				{
					m_Ref_list.push_back(pBuffer);
				}
			}
			break;
		case SKIN_CLASS_WOMAN :
			{
				if( pRedChara->GetCharaSexType() == CHARA::eSEX_TYPE_FEMALE || pBlueChara->GetCharaSexType() == CHARA::eSEX_TYPE_FEMALE )
				{
					m_Ref_list.push_back(pBuffer);
				}
			}
			break;
		}
	}*/	
}


/**************************/
//CashItemDB
CashItemDB::~CashItemDB()
{
	i3::cu::for_each_SafeFree_clear(m_Spreaded_Main_list);
}

void CashItemDB::ReReferenceUsage_impl()
{
	i3::cu::for_each_SafeFree_clear(m_Spreaded_Main_list);

	for(size_t i=0; i<Max;++i)
		m_Ref_list[i].clear();

	size_t Count = m_Main_list.size();
	for(size_t i = 0 ; i < Count ; i++)
	{
		INVEN_BUFFER* pBuffer = m_Main_list[i];

		if( is_spread_item( pBuffer ) )
		{
			UINT32 MaxCount = pBuffer->_ui32ItemArg;

			for( UINT32 j = 0; j < MaxCount; ++j )
			{
				INVEN_BUFFER* pTemp = (INVEN_BUFFER*)i3MemAlloc(sizeof(INVEN_BUFFER) );
				memcpy(pTemp, pBuffer, sizeof(INVEN_BUFFER));

				pTemp->_ui32ItemArg = 1;
				m_Spreaded_Main_list.push_back(pTemp);
			}
		}
		else
		{
			INVEN_BUFFER* pTemp = (INVEN_BUFFER*)i3MemAlloc(sizeof(INVEN_BUFFER) );
			memcpy(pTemp, pBuffer, sizeof(INVEN_BUFFER));

			m_Spreaded_Main_list.push_back(pTemp);
		}
	}

	Count = m_Spreaded_Main_list.size();
	for(size_t i = 0 ; i < Count ; i++)
	{
		INVEN_BUFFER* pBuffer = m_Spreaded_Main_list[i];

		INT32 Type = GET_ITEM_TYPE( pBuffer->_ui32ItemID );
		switch(Type)
		{
		case ITEM_TYPE_WRAP_PERIOD: m_Ref_list[CashItemDB::Maintenance].push_back(pBuffer); break;
		case ITEM_TYPE_MAINTENANCE: 
		case ITEM_TYPE_WRAP_COUNT:
		case ITEM_TYPE_GACHA_TICKET:
			m_Ref_list[CashItemDB::Expendable].push_back(pBuffer);		
			break;
		case ITEM_TYPE_DINO :
			m_Ref_list[CashItemDB::CashDino].push_back(pBuffer);		
			break;
		}
	}
}


bool CashItemDB::is_spread_item(const INVEN_BUFFER* pInvenBuffer)
{
	// ���޻��ڸ� �ɰ��� ���� �ֱ� ���� ������ ����Ʈ
	static UINT32 READYBOX_ITEMID_LIST[] = 
	{
		1301111000,	//�߳��� �ǹ� ���� ���� : CASHITEM_GROUP_READYBOX_SILVER_SET_LATIN
		1301112000,		//�߳��� ��� ���� ���� : CASHITEM_GROUP_READYBOX_GOLD_SET_LATIN
		1301113000,		//�߳��� �Ϲ� ���� ���� : CASHITEM_GROUP_READYBOX_NORMAL_SET_LATIN

		1301122000,		//�߳��� �ǹ� �� ���� ���� : CASHITEM_GROUP_READYBOX_SILVER_PRIZE_SET_LATIN
		1301123000,		//�߳��� ��� �� ���� ���� : CASHITEM_GROUP_READYBOX_GOLD_PRIZE_SET_LATIN
		1301124000,		//�߳��� �Ϲ� �� ���� ���� : CASHITEM_GROUP_READYBOX_NORMAL_PRIZE_SET_LATIN

		1301125000,		//�ǹ� ���� ���� : CASHITEM_GROUP_READYBOX_SILVER_SET
		1301126000,		//��� ���� ���� : CASHITEM_GROUP_READYBOX_GOLD_SET
		1301127000		//�Ϲ� ���� ���� : CASHITEM_GROUP_READYBOX_NORMAL_SET
	};

	size_t size = sizeof(READYBOX_ITEMID_LIST) / sizeof( READYBOX_ITEMID_LIST[0]);
	for(size_t i=0;i<size;++i)
	{
		if( pInvenBuffer->_ui32ItemID == READYBOX_ITEMID_LIST[i] )
			return true;
	}

	return false;
}

const i3::vector<INVEN_BUFFER*>* CashItemDB::get_list(ITEM_TYPE nUsageType) const
{
	switch(nUsageType)
	{
	case ITEM_TYPE_POINT:
	case ITEM_TYPE_UNKNOWN:	
		return &m_Spreaded_Main_list;	break;
	case ITEM_TYPE_WRAP_PERIOD:	return &m_Ref_list[CashItemDB::Maintenance];		break;
	case ITEM_TYPE_WRAP_COUNT:
	case ITEM_TYPE_MAINTENANCE: 
			return &m_Ref_list[CashItemDB::Expendable];	break;
	case ITEM_TYPE_DINO:		return &m_Ref_list[CashItemDB::CashDino];				break;
	}
	return &db_util::g_none_list;
}





/**************************/
//CouponBufferDB
CouponBufferDB::~CouponBufferDB()
{
	i3::cu::for_each_SafeFree_clear(m_Main_list);
}


void CouponBufferDB::AddInvenBufferToList(INVEN_BUFFER* pBuffer,INT32 nCount)
{
	// ���� ����� ����ִٸ� ���̻� ó������ �ʴ´�.
	if (nCount <= 0 || pBuffer == nullptr)	return;

	for ( INT32 i = 0 ; i < nCount ; ++i )
	{
		// �����Ϳ� �̻��� ������ �˻縦 �մϴ�.
		if ( 0 == pBuffer[i]._ui32ItemID)
		{
			db_util::DeleteInvenBuffer<COUPON_BUFFER*>(m_Main_list, pBuffer[i]._TItemWareDBIdx);
			continue;
		}

		// ���� WareDBIndex�� InvenBuffer�� �ִٸ� ���� �����, ���ٸ� ���� �߰� �մϴ�.
		bool is_updated = db_util::UpdateInvenBufferToList<COUPON_BUFFER*>(m_Main_list, &pBuffer[i]);
		if( is_updated == false )
		{
			COUPON_BUFFER* pCouponBuffer = (COUPON_BUFFER*)i3MemAlloc(sizeof(COUPON_BUFFER));

			i3mem::Copy(pCouponBuffer, &pBuffer[i], sizeof(INVEN_BUFFER));
			pCouponBuffer->m_bUsed = false;

			m_Main_list.push_back(pCouponBuffer);
		}
	}

	i3::sort(m_Main_list.begin(), m_Main_list.end(), db_util::CompareItemID_ForInvenBuffer);
}

void CouponBufferDB::UpdateInvenBufferToList(INVEN_BUFFER* pBuffer)
{
	db_util::UpdateInvenBufferToList<COUPON_BUFFER*>(m_Main_list, pBuffer);
}

