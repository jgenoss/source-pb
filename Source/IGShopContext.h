#ifndef __IGSHOP_CONTEXT_H
#define __IGSHOP_CONTEXT_H

//MAX_FIELDSHOP_CATEGORYCOUNT 
//MAX_FIELDSHOP_GOODSLISTCOUNT
//MAX_FIELDSHOP_GOODSPERIODCOUNT
class IGShopContext_Mode;

class IGShopContext : public i3GameNode
{
	I3_CLASS_DEFINE(IGShopContext, i3GameNode);
public:
	
	IGShopContext();
	virtual ~IGShopContext();						// �⺻Ŭ������ �����ٸ� ����Ҹ��ڴ� �����ʾ�������..

	void				SetEvent_GetInfo();			// �κ����Խÿ��� ������ �˴ϴ�..
	void				SetEventOnce_Open();
	void				SetEvent_EquipItem(const ITEM_INFO & item_info);
	void				LeaveServer();
		
	bool				IsIGShopOpenning() const {  return m_bRequestOpen; }
	bool				IsEnteredServer() const { return m_bEntered; }		// �����δ� ��Ż�� ���� ��Ŷ�� ���⶧���� ������ ��ƾ�̴�...
//
	INT32				GetNumCategory() const;
	const TTCHAR*		GetCategoryString(INT32 idx) const ;

	INT32				GetNumShopGoodsPack(INT32 cat) const ;
	SHOP_GOODS_PACK*	GetShopGoodsPack( INT32 cat, INT32 idx) const;

private:
	void				Allocate_StageID(UINT32 ui32FieldShopDataFlag);		// 32���̸��� ����������Ʈ������...

	void				EnteredServer();
	void				SetMainShopChange(); 
	void				ReceiveAndUpdateGoodsList(const FieldShopInfo& fsgi);	
	bool				IsMainShopChange() const;
	void				UpdateGoodsList();

private:
	i3::vector<IGShopContext_Mode*>	m_vecStorage;						// ���Ŵ��..
	IGShopContext_Mode*	m_apContextMode[STAGE_MODE_MAX];				// �����ʹ� ������������ ���� �����־�� �Ѵ�..

	bool				m_bRequestOpen;									// ���½õ� ��ü�� �Ұ��� �ʿ�...
	bool				m_bEntered;										// ���� ���Կ� ���� ���θ� ����...
	STAGE_MODE			m_curr_stage_type;

	friend class CShop;
	friend class ClientTCPSocket;
};

// ��庰�� �и��ǹǷ� ���� �и����ش�..
class IGShopContext_Mode
{
public:
	IGShopContext_Mode();
	~IGShopContext_Mode();		// ����ó���� ���� �ʴ´�..
//
	INT32				GetNumCategory() const { return m_num_category; }
	const TTCHAR*		GetCategoryString(INT32 idx) const { return m_pstrCategoryName[idx]; }

	INT32				GetNumShopGoodsPack(INT32 cat) const { return m_num_items[cat]; }
	SHOP_GOODS_PACK*	GetShopGoodsPack( INT32 cat, INT32 idx) const { return m_aapGoodsPack[cat][idx]; }
//

private:
	void				ReceiveAndUpdateGoodsList(const FieldShopInfo& fsgi);		// �������� �ش� �����͵��� ��ȿ�� ������ ����..
	// ������ �迭�� Ŭ���̾�Ʈ�� �迭�� �� ��ġ�� ��ġ���� ������ �ִٴ� �� ����...
	void				UpdateGoodsList();

	bool				IsMainShopChange() const { return m_bChangeMainShopContext; }
	void				SetMainShopChange() { m_bChangeMainShopContext = true; } 

private:

	UINT32				GetVersion() const { return m_info_from_server.m_ui32Version; }
	void				ClearInfo();

	void				BuildShopGoodsPack(UINT32* goodsIDs);		// ������ ShopContext���� ã�ƾ��ϴµ�..�� ã�� ��ƾ�� �� ������..
	void				BuildShopGoodsPack(SHOP_GOODS_TYPE type, UINT32 AddGoodsID, bool bHint);
	bool				IsExistedPack( SHOP_GOODS_TYPE type,	SHOP_GOODS_PACK* test_pack,	UINT32 AddGoodsID);


	FieldShopInfo		m_info_from_server;							// ���� ���ϰ� ������, �ʿ信 ���� ����.(SHOP_GOODS_PACK ������ ��ȿ���� ������)

	INT32				m_num_category;								// 
	INT32				m_num_items[MAX_FIELDSHOP_CATEGORYCOUNT];	// 
	const TTCHAR*		m_pstrCategoryName[MAX_FIELDSHOP_CATEGORYCOUNT];		
	
	// �����Ҵ�/�ҰŸ� ����Ʈ�� �����ϰ�, ��ġ�� �迭�� �����͸� ��´�...(����Ʈ �����ÿ� Ȱ��)
	// SHOP_GOODS_PACK�� ���� �����Ҵ�ǰ� ��� �������� �ʰ� ��� �޸� �Ұ� �䱸��...
	SHOP_GOODS_PACK*	m_aapGoodsPack[MAX_FIELDSHOP_CATEGORYCOUNT][MAX_FIELDSHOP_GOODSLISTCOUNT];

	bool				m_bChangeMainShopContext;					// ���μ� ���ؽ�Ʈ�� �ٲ�ٸ� �װ��� �����ؾ��Ѵ�...(��庰�� ����ó�� �ʿ�)

	friend class IGShopContext;
};


#endif