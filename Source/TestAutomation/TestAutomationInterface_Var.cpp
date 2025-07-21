#include "pch.h"
#include "TestAutomationInterface.h"

#if defined( TEST_AUTOMATION)

#include "UI/UIDefine.h"
#include "UI/UIMainFrame.h"

#include "UI/UIShopInfoMgr.h"

#include "IGShopContext.h"
#include "ShopContext.h"

#include "i3Base/string/ext/contain_string_head.h"

#include "ClientStageInfoUtil.h"
#include "ClientStageInfoContext.h"

vector<wstring *> *	TestAutomationInterface::GetStageList(STAGE_MODE stageType, int count)
{
	/*
	��� ������� �ش� �κ����δ� �������� ������ ���� �� ���� �Ʒ� �Լ��� ��ü �մϴ�.
	const STAGEID_INFO_LIST::StageIdInfoList& list =
		s_StageInfoTableList.GetStageIdInfoList();
	*/
	const i3::vector<CSI_STAGE>* stagelist = ClientStageInfoContext::i()->GetALLStageList();

	//char name[256]; �ȽἭ ���� �մϴ�.
	int cnt;
	
	//StageInfoList�� ��ϵ� ���븸 �����Ѵ�.
	//��帮����� ���� ���� �޴� �κ��� �ٲ�� �����մϴ�.
	//STAGEID_INFO_LIST::StageIdInfoList::const_iterator it = list.begin();
	//STAGEID_INFO_LIST::StageIdInfoList::const_iterator end = list.end();
	//it++;	//�� ó���� ������� �ʴ� �� (STAGE_ORD_NA) �̴�.

	i3::vector<CSI_STAGE>::const_iterator it	= stagelist->begin();
	i3::vector<CSI_STAGE>::const_iterator end	= stagelist->end();

	vector<wstring *> * pList = new vector<wstring *>;

	for ( cnt = 0;(it != end); it++)
	{
		//��帮����� ���� ���� �޴� �κ��� �ٲ�� �����մϴ�.
		//STAGEID_INFO & info = (STAGEID_INFO &)it->second;
		const CSI_STAGE & info = *it;

		//��Ƽ�� ��������� �������� �������� ���� ��� ������������ Ȱ��ȭ �Ǿ��ִٰ� �����մϴ�.
		//STAGE_ID id = info.GetStageID();
		//if (isActiveStage( id) == false)
		//	continue;

		//��尡 �������� ���� ������ ���� ������ϴ�.
		//�㳪 �� �Լ������� ���� �Ǻ��߾�����, ���� ��� �������� �̸�(���̸�)�� �ִ� ������ �ٲߴϴ�.
		/*
		if (stageType != STAGE_MODE_NA)
		{
			STAGE_MODE type = getStageType(id);

			if (type != stageType)
				continue;
		}

		info.GetStageDisplayName(name);

		wchar_t wszName[256];

		MultiByteToWideChar(CP_ACP, 0, name, -1, wszName, _countof(wszName) - 1);
		*/

		if ( StageID::GetStageMode(info.m_ui32StageID) != stageType)
			continue;

		wstring * pName = new wstring( CStageInfoUtil::GetMapData(info.m_ui32StageID)->getDisplayName().c_str() );

		pList->push_back(pName);

		cnt++;
		if((count > 0) && (cnt >= count))
			break;
	}

	return pList;
}

namespace
{
	item_def::Category to_item_category(SHOP_GOODS_TYPE type)
	{
		switch(type)
		{
		case SHOP_WEAPON_GOODS:		return item_def::CTG_WEAPON;
		case SHOP_CHARACTER_GOODS:	return item_def::CTG_CHARA;
		case SHOP_CASH_ITEM_GOODS:	return item_def::CTG_ITEM;
		case SHOP_PACKAGE_GOODS:	return item_def::CTG_ITEM;
		case SHOP_SKIN_GOODS :		return item_def::CTG_SKIN;
		}
		return item_def::CTG_NA;
	}
}

vector<wstring *> *		TestAutomationInterface::EnumStageValues(const wchar_t * pszValue, int count)
{
	if (wcsicmp(pszValue, L"��� ��������") == 0)
	{
		return GetStageList(STAGE_MODE_NA, count);
	}
	else if (wcsicmp(pszValue, L"��� ��ü��") == 0)
	{
		return GetStageList(STAGE_MODE_DEATHMATCH, count);
	}
	else if (wcsicmp(pszValue, L"��� ���Ĺ̼�") == 0)
	{
		return GetStageList(STAGE_MODE_BOMB, count);
	}
	else if (wcsicmp(pszValue, L"��� �ı��̼�") == 0)
	{
		return GetStageList(STAGE_MODE_DESTROY, count);
	}
	else if (wcsicmp(pszValue, L"��� ������") == 0)
	{
		return GetStageList(STAGE_MODE_ANNIHILATION, count);
	}
	else if (wcsicmp(pszValue, L"��� �����") == 0)
	{
		return GetStageList(STAGE_MODE_DEFENCE, count);
	}
	
	return nullptr;
}

static void _addList(vector<wstring *> & list, const wchar_t * pszStr)
{
	size_t count = list.size();

	// ������ �׸��� �ִ��� ���� Ȯ���Ѵ�.
	for (size_t i = 0; i < count; i++)
	{
		wstring * pTemp = list[i];

		if (*pTemp == pszStr)
		{
			// �̹� �ִ�. �߰����� �ʴ´�.
			return;
		}
	}

	wstring * pItem = new wstring;

	*pItem = pszStr;

	list.push_back(pItem);
}

vector<wstring *> *		TestAutomationInterface::GetGoodsList( item_def::Category cate, int count, WEAPON_CLASS_TYPE weaponType)
{
	vector<wstring *> * pList = new vector<wstring *>;
	i3::rc_wstring name;
	int cnt = 0;

	CShopContext * pCtx = CShopContext::i();

	for( size_t ctg = SHOP_WEAPON_GOODS; ctg < SHOP_GOODS_MAX; ++ctg )
	{
		SHOP_GOODS_TYPE shop_type = (SHOP_GOODS_TYPE)ctg;
		item_def::Category temp = to_item_category(shop_type);

		if((cate != item_def::CTG_NA) && (cate != temp))
			continue;

		INT32 cntPack = pCtx->GetGoodsPackCount( shop_type );
		for(INT32 i = 0; i < cntPack; i++)
		{
			const SHOP_GOODS_PACK * pPack = pCtx->GetGoodsPack(shop_type, i);
			if( pPack == nullptr) continue;

			SHOP_GOODS * pGoods = pPack->GetGoods(0);
			if( pGoods == nullptr) continue;

			// �̱�������� �̺�Ʈ �������� ���� �����մϴ�.
			// EVENT ������ ��ü�� GoodsPack�� ���ԵǾ� �ֽ��ϴ�.
			// �ٸ� ��� �������� EVENT�̸� �������� ���� �մϴ�.
			for( INT32 j = 0; j < pPack->GetGoodsCount(); j++)
			{
				pGoods = pPack->GetGoods( j);				

				switch( cate)
				{
					case item_def::CTG_WEAPON:
						{
							WEAPON_CLASS_TYPE weapon_class = WEAPON::ItemID2Class( pGoods->_ItemID);

							if((weaponType != WEAPON_CLASS_UNKNOWN) && (weaponType != weapon_class))
								continue;

							CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(pGoods->_ItemID);

							if( pInfo != nullptr)
							{
								if((pGoods->_PriceInfo->_SaleMark == 0) && (pGoods->_SaleDesignation == 0))
								{
									pCtx->GetGoodsName(name, pGoods->_GoodsID, pGoods->_ItemID);

									if (wcsicmp(name.c_str(), L"") == 0)
										continue;

									_addList(*pList, name.c_str());

									cnt++;
								}
							}
							else
							{
								wchar_t conv[256];

								swprintf_s( conv, _countof(conv), L"Invalid ItemID : %d", pGoods->_ItemID);

								//_addList(*pList, conv);
							}
						}
						break;

					default:
						{
							pCtx->GetGoodsName( name, pGoods->_GoodsID, pGoods->_ItemID);

							_addList(*pList, name.c_str());

							cnt++;
						}
						break;
				}

				if((count > 0) && (cnt >= count))
				{
					return pList;
				}
			}
		}
	}

	return pList;
}


vector<wstring *> *		TestAutomationInterface::EnumGoodsValues(const wchar_t * pszValue, int count)
{
	if (wcsicmp(pszValue, L"��� ��ǰ") == 0)
	{
		return GetGoodsList( item_def::CTG_NA, count);
	}
	else if (wcsicmp(pszValue, L"��� ����") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count);
	}
	else if (wcsicmp(pszValue, L"��� ���ݼ���") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count, WEAPON_CLASS_ASSAULT);
	}
	else if (wcsicmp(pszValue, L"��� �������") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count, WEAPON_CLASS_SMG);
	}
	else if (wcsicmp(pszValue, L"��� ��ź��") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count, WEAPON_CLASS_SHOTGUN);
	}
	else if (wcsicmp(pszValue, L"��� ������") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count, WEAPON_CLASS_SNIPER);
	}
	else if (wcsicmp(pszValue, L"��� ĳ����") == 0)
	{
		return GetGoodsList( item_def::CTG_CHARA, count);
	}
	else if (wcsicmp(pszValue, L"��� ����") == 0)
	{
		return GetGoodsList( item_def::CTG_PARTS, count);
	}
	else if (wcsicmp(pszValue, L"��� ��Ų") == 0)
	{
		return GetGoodsList( item_def::CTG_SKIN, count);
	}
	else if (wcsicmp(pszValue, L"��� ������") == 0)
	{
		return GetGoodsList( item_def::CTG_ITEM, count);
	}

	
	return nullptr;
}

vector<wstring *> *		TestAutomationInterface::EnumInvenValues(const wchar_t * pszValue, int count)
{
	return nullptr;
}

vector<std::wstring *> * TestAutomationInterface::EnumVariableValue( const wchar_t * pszVar, const wchar_t * pszValue, int count)
{
	if (wcsicmp(pszVar, L"Stage") == 0)
	{
		return EnumStageValues(pszValue, count);
	}
	else if (wcsicmp(pszVar, L"Goods") == 0)
	{
		return EnumGoodsValues(pszValue, count);
	}
	else if (wcsicmp(pszVar, L"Inven") == 0)
	{
		return EnumInvenValues(pszValue, count);
	}

	return nullptr;
}

#endif