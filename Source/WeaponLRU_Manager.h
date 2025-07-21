#pragma once

class ItemTypeCharaClass;

enum LRU_TYPE
{
	LRU_DEFAULT = 0x00000001,
	LRU_TIME	= 0x00000002,

};

#pragma pack(push,4)
struct LRUData
{
	UINT32	_ItemID;
	UINT32	_count;
	UINT32	_dayScore;
	LRUData(){};
};
#pragma pack(pop)

typedef i3::vector<LRUData>::iterator LRUListIt;
#define MAX_LRU_BUFFER 300
#define UNUSED_LRU_DELETE 5
#define MAX_LRU_LOADING_COUNT 10		// 10������ common res �켱����
#define WEAPON_PRIORITY_COUNT 20		// 20������ �ǽð� �켱����
#define WEAPON_PRIORITY_RESPAWN_COUNT	5	// 5ȸ ������ �̻��ϸ� Common���� ���

class _finder
{
public:
	_finder(T_ItemID itemID):_id(itemID){}
	bool operator() (const LRUData& item) const
	{
		return (item._ItemID == _id);
	}
private:
	const T_ItemID _id;
};

class fn_sort_LRU // Sort(����� ī��Ʈ * t
{
public:
	bool operator() (const LRUData& LItem, const LRUData& RItem) const
	{
		return LItem._count*(LItem._dayScore+1) > RItem._count*(RItem._dayScore+1);
	}
};

struct _finder1	// ���� ItemID���� üũ
{
	_finder1(T_ItemID id){ _id = id;}
	bool operator() (const LRUData& item) const
	{
		return (item._ItemID == _id);
	}
	T_ItemID _id;
};
typedef i3::pair<i3GameRes*, bool> tSharedResPair;
typedef i3::unordered_map<i3GameRes*, i3::vector<tSharedResPair> > tConSharedRes;

class WeaponLRU_Manager : public i3::shared_ginst< WeaponLRU_Manager >
{
public:
	WeaponLRU_Manager();
	virtual ~WeaponLRU_Manager();

	void	Init();
	bool	saveData();
	bool	loadData();
	void	addCount(T_ItemID _ID);

	void	getCurretDateToValue(UINT32 * day);
	const i3::vector<LRUData>&	getItemList( void)	{ return m_ItemList; }
	bool	cbFindFilePath(char *, bool isCommon);

	void	BackingDataFromAddList( void);
	void	SortWeaponByPriority( void);
	/** \brief Common load
		\note common load time�� ȣ���Ͽ� ���� ���Ǵ� ���⸦ ���ε��Ѵ�. */
	void	PreLoad( void);

	/** \brief InGame load
		\note stage jump time�� ȣ��Ǹ� �뷫 30�� �̻� ���� ���⸦common res�� �����Ѵ�. */
	void	AfterLoad( void);

	/** \brief �߿� ���� ����
		\note ���� ����� ������ ��� true ��ȯ */
	bool	IsPriorityWeapon( T_ItemID itemID);

private:
	i3::vector<LRUData>	m_ItemList;
	i3::unordered_map< T_ItemID, LRUData>	m_PreLoadItemMap;
	tConSharedRes							m_CommonResList;

	i3::vector<T_ItemID>					m_TempAddList;		// �ӽ� �����
};