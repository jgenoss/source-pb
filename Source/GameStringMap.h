#ifndef __GAME_STRING_MAP_H__
#define __GAME_STRING_MAP_H__


//////////////////////////////////////////////////////////////////////////
// CGameStringMap
//////////////////////////////////////////////////////////////////////////

// Ű�� i3::string / ���� i3::rc_string�� �ǵ��� ����..

class CGameStringMap
{
public:

public:
	enum GameStringIndex
	{
		GSI_STRING = 0,
		GSI_AICHARACTER,	GSI_BASIC,	GSI_CHARACTER,	GSI_EQUIPMENT,
		GSI_GOODS,		GSI_QUEST,		GSI_STAGE,		GSI_WEAPON,		GSI_WORLD,
		GSI_MAX,
	};
	typedef i3::vector< i3::rc_wstring > StringVector;
	typedef i3::unordered_map< i3::string, size_t > StringHashMap;		
	
private:
	struct StringTableContext
	{
		StringVector string_list;
		StringHashMap label_map;
	};
	StringTableContext m_StringTable[GSI_MAX];
	
private:
	enum { MAX_CACHE_SIZE = 256, };

private:
	bool _Create( const char* local_path, const char* local_name);			// �̷� �Լ� �߰�... 
	bool _AddFromTextFilesImpl(StringTableContext& table, const i3::string& strLocalStringPath, bool case_sensitive );
	const i3::rc_wstring& _GetStringImpl(StringTableContext& table, const i3::string& strLabel, bool case_sensitive);
	const i3::rc_wstring& _GetStringImpl(StringTableContext& table, const char* strLabel, bool case_sensitive);
	int _GetStringHandleImpl( StringTableContext& table,  const i3::string& strLabel, bool case_sensitive);
	const i3::rc_wstring& _GetStringByHandleImpl(StringTableContext& table, int nHandle);
	static bool _IsCaseSensitive(GameStringIndex idx );

public:
	static		CGameStringMap* Get();

	CGameStringMap();
	virtual ~CGameStringMap();

	bool CreateForPointBlank();
	void	 Destroy();


	// �󺧷� ����ȭ�� ���ڿ��� ��´�.(���������� ���� ���� ��ȯ. map�� �˻��ϹǷ�)
	// ����, �������� �ʴ� ���̶�� ���̸� ��ü�� ���ڿ��� ����� ��ȯ�Ѵ�.
	const i3::rc_wstring&	GetString(  const i3::string& strLabel, GameStringIndex idx );
	const i3::rc_wstring&	GetString( const char* strLabel, GameStringIndex idx);


	// �󺧷� ����ȭ�� ���ڿ��� �ڵ��� ��´�.(���������� ���� �˻��� ���� ��ġ��)
	// ����, �������� �ʴ� ���̶�� -1�� ��ȯ�Ѵ�.
	int				GetStringHandle( const i3::string& strLabel, GameStringIndex idx );

	// �ڵ�� ����ȭ�� ���ڿ��� ��´�.(���� �˻�)
	// ����, �������� �ʴ� �ڵ��̶�� �ڵ��� ���ڰ��� ���ڿ��� ����� ��ȯ�Ѵ�.
	const i3::rc_wstring&	GetStringByHandle( int nHandle, GameStringIndex idx );

	// ������ ���� ��ȯ
	int GetCount(GameStringIndex idx);

	// �ڷᱸ�� ��ȯ
	const StringVector&	GetStringList(GameStringIndex idx) const;
	const StringHashMap&		GetLabelMap(GameStringIndex idx) const;

	bool LoadingKoreanLanguage();
	bool LoadingEnglishLanguage();
};

//////////////////////////////////////////////////////////////////////////
// �ӵ��� ũ�� ��������� �̰� ����. (STL map)
// �̰� �ַ� ���� �ֿ�.
inline const wchar_t* GAME_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STRING).c_str();}
inline const wchar_t* GAME_PEF_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_AICHARACTER).c_str();}
inline const wchar_t* GAME_BASIC_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_BASIC).c_str();}
inline const wchar_t* GAME_CHARACTER_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_CHARACTER).c_str();}
inline const wchar_t* GAME_EQUIPMENT_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_EQUIPMENT).c_str();}
inline const wchar_t* GAME_GOODS_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_GOODS).c_str();}
inline const wchar_t* GAME_QUEST_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_QUEST).c_str();}
inline const wchar_t* GAME_STAGE_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STAGE).c_str();}
inline const wchar_t* GAME_WEAPON_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WEAPON).c_str();}
inline const wchar_t* GAME_WORLD_STRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WORLD).c_str();}

inline const i3::rc_wstring& GAME_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STRING);}
inline const i3::rc_wstring& GAME_PEF_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_AICHARACTER);}
inline const i3::rc_wstring& GAME_BASIC_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_BASIC);}
inline const i3::rc_wstring& GAME_CHARACTER_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_CHARACTER);}
inline const i3::rc_wstring& GAME_EQUIPMENT_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_EQUIPMENT);}
inline const i3::rc_wstring& GAME_GOODS_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_GOODS);}
inline const i3::rc_wstring& GAME_QUEST_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_QUEST);}
inline const i3::rc_wstring& GAME_STAGE_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STAGE);}
inline const i3::rc_wstring& GAME_WEAPON_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WEAPON);}
inline const i3::rc_wstring& GAME_WORLD_RCSTRING(const i3::string& s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WORLD);}


inline const wchar_t* GAME_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STRING).c_str();}
inline const wchar_t* GAME_PEF_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_AICHARACTER).c_str();}
inline const wchar_t* GAME_BASIC_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_BASIC).c_str();}
inline const wchar_t* GAME_CHARACTER_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_CHARACTER).c_str();}
inline const wchar_t* GAME_EQUIPMENT_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_EQUIPMENT).c_str();}
inline const wchar_t* GAME_GOODS_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_GOODS).c_str();}
inline const wchar_t* GAME_QUEST_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_QUEST).c_str();}
inline const wchar_t* GAME_STAGE_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STAGE).c_str();}
inline const wchar_t* GAME_WEAPON_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WEAPON).c_str();}
inline const wchar_t* GAME_WORLD_STRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WORLD).c_str();}

inline const i3::rc_wstring& GAME_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STRING);}
inline const i3::rc_wstring& GAME_PEF_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_AICHARACTER);}
inline const i3::rc_wstring& GAME_BASIC_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_BASIC);}
inline const i3::rc_wstring& GAME_CHARACTER_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_CHARACTER);}
inline const i3::rc_wstring& GAME_EQUIPMENT_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_EQUIPMENT);}
inline const i3::rc_wstring& GAME_GOODS_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_GOODS);}
inline const i3::rc_wstring& GAME_QUEST_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_QUEST);}
inline const i3::rc_wstring& GAME_STAGE_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_STAGE);}
inline const i3::rc_wstring& GAME_WEAPON_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WEAPON);}
inline const i3::rc_wstring& GAME_WORLD_RCSTRING(const char* s){	return CGameStringMap::Get()->GetString(s, CGameStringMap::GSI_WORLD);}




//////////////////////////////////////////////////////////////////////////
// �ӵ��� ���� �߿��� ���̸� GET_GAME_STRING_HANDLE �� �̿��ؼ� �ڵ��� �ѹ� �����ߴٰ�,
// GAME_STRING_BY_HANDLE�� �̿��ϸ� �ȴ�. (STL vector)
inline int GET_GAME_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_STRING);}
inline int GET_GAME_PEF_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_AICHARACTER);}
inline int GET_GAME_BASIC_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_BASIC);}
inline int GET_GAME_CHARACTER_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_CHARACTER);}
inline int GET_GAME_EQUIPMENT_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_EQUIPMENT);}
inline int GET_GAME_GOODS_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_GOODS);}
inline int GET_GAME_QUEST_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_QUEST);}
inline int GET_GAME_STAGE_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_STAGE);}
inline int GET_GAME_WEAPON_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_WEAPON);}
inline int GET_GAME_WORLD_STRING_HANDLE(const i3::string& s){	return CGameStringMap::Get()->GetStringHandle(s, CGameStringMap::GSI_WORLD);}

inline const wchar_t* GAME_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_STRING).c_str();}
inline const wchar_t* GAME_PEF_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_AICHARACTER).c_str();}
inline const wchar_t* GAME_BASIC_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_BASIC).c_str();}
inline const wchar_t* GAME_CHARACTER_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_CHARACTER).c_str();}
inline const wchar_t* GAME_EQUIPMENT_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_EQUIPMENT).c_str();}
inline const wchar_t* GAME_GOODS_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_GOODS).c_str();}
inline const wchar_t* GAME_QUEST_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_QUEST).c_str();}
inline const wchar_t* GAME_STAGE_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_STAGE).c_str();}
inline const wchar_t* GAME_WEAPON_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_WEAPON).c_str();}
inline const wchar_t* GAME_WORLD_STRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_WORLD).c_str();}

inline const i3::rc_wstring& GAME_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_STRING);}
inline const i3::rc_wstring& GAME_PEF_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_AICHARACTER);}
inline const i3::rc_wstring& GAME_BASIC_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_BASIC);}
inline const i3::rc_wstring& GAME_CHARACTER_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_CHARACTER);}
inline const i3::rc_wstring& GAME_EQUIPMENT_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_EQUIPMENT);}
inline const i3::rc_wstring& GAME_GOODS_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_GOODS);}
inline const i3::rc_wstring& GAME_QUEST_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_QUEST);}
inline const i3::rc_wstring& GAME_STAGE_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_STAGE);}
inline const i3::rc_wstring& GAME_WEAPON_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_WEAPON);}
inline const i3::rc_wstring& GAME_WORLD_RCSTRING_BY_HANDLE(int h){	return CGameStringMap::Get()->GetStringByHandle(h, CGameStringMap::GSI_WORLD);}



#endif //__GAME_STRING_MAP_H__

