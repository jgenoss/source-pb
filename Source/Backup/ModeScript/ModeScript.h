#pragma once

class ModeScriptMgr;
struct lua_State;


enum lua_userdata_type
{
	lua_userdata_type_error = -1,
	lua_userdata_type_owner,
};

template<class T>
T	get_lua_owner(lua_State* L)
{
	lua_pushnumber( L, (lua_Number)lua_userdata_type_owner );
	lua_gettable(L, LUA_REGISTRYINDEX );
	return T(lua_touserdata(L, -1));
}

//
//  ���������ϰ���������, �̰͵� �ݹ��� ������������ ��������ü�� ����ϴ�..
//
class IModeScript;

struct ModeDataCB
{
	virtual void operator()( IModeScript* s, PROTOCOL p, const char* data_name, INT32 data_idx, INT32 data_value ) = 0;
	virtual bool is_object_same(const type_info& ti, const void* ptr) const = 0;
	virtual ~ModeDataCB() {}
};

template<class C>
struct TModeDataCB : ModeDataCB
{
	typedef void (C::*fun_type)(IModeScript*, PROTOCOL, const char*, INT32, INT32);

	C*		m_o;	
	fun_type m_f; 
	INT32 m_offset; 
	TModeDataCB(C* o, fun_type f, INT32 offset = 0) : m_o(o), m_f(f), m_offset(offset) {}	
	virtual void operator()(IModeScript* s, PROTOCOL p, const char* n, INT32 i, INT32 v) {  (m_o->*m_f)(s, p, n, i - m_offset, v);  }
	virtual bool is_object_same(const type_info& ti, const void* ptr) const 
	{
		if ( ti != typeid(m_o) ) return false;
		return ptr == reinterpret_cast<const void*>(m_o);
	}
};

//std::endl


struct mode_script_arg
{
	struct process_ret {	virtual void	operator()(mode_script_arg& a, INT32 mstack, INT32 i) = 0;	virtual ~process_ret() {}	};
	lua_State*	l;	INT32		narg;		std::vector<process_ret*>	rlist;		const char*		fun_name;
};

mode_script_arg&	operator<<(mode_script_arg& a, REAL64 r);
mode_script_arg&	operator<<(mode_script_arg& a, REAL32 r);
mode_script_arg&	operator<<(mode_script_arg& a, bool  b);
mode_script_arg&	operator<<(mode_script_arg& a, INT32 i);
mode_script_arg&	operator<<(mode_script_arg& a, const char* str);			// �� �κ��� Ȯ���� ������ �ִ�...(�̻��¿����� ������ ������)


mode_script_arg&	operator>>(mode_script_arg& a, mode_script_arg& (*f)(mode_script_arg& )  );	// ������ �κ� ó��..

mode_script_arg&	operator>>(mode_script_arg& a, REAL64& r);
mode_script_arg&	operator>>(mode_script_arg& a, REAL32& r);
mode_script_arg&	operator>>(mode_script_arg& a, bool& b);
mode_script_arg&	operator>>(mode_script_arg& a, INT32& i);
mode_script_arg&	operator>>(mode_script_arg& a, char*& str);

mode_script_arg&	end_call(mode_script_arg& a);

#define MODE_DATA_MAX 150

class IModeScript
{
public:
	IModeScript(i3Stage* pStage);
	virtual ~IModeScript();

	template<class C>
	bool		add_modedata_cb(const char* mode_index_name, C* o, void (C::*f)(IModeScript*, PROTOCOL , const char*, INT32, INT32));

	// �Ʒ� �Լ��� ��� �������μ��� ���� begin���� 0���� ���������Ͽ� �ɼ�ó������...
	template<class C>
	bool		add_modedata_cb(const char* begin_idx_name, const char* end_idx_name, C* o, 
		void (C::*f)(IModeScript*, PROTOCOL, const char*, INT32, INT32), bool offset_enable);

	// �ε����� �̿��ؼ� �ݹ��Լ��� ����Ѵ�.
	template<class C>
	bool		add_modedata_cb(INT32 mode_idx, C* o, void (C::*f)(IModeScript*, PROTOCOL, const char*, INT32, INT32));

	template<class C>
	void		remove_modedata_cb(C* o);

	INT32		find_modedata_index(const char* mode_index_name);	// ����ε����̸�, ���ε��� ��������..�̸� ã�� �����صδ°� ����..
	INT32		get_modedata(INT32 mode_index) const { return (mode_index != -1 ) ? m_i32RoomModeData[mode_index] : -1; }
	INT32		calc_modedata(const char* mode_index_name) {  return get_modedata( find_modedata_index(mode_index_name)); }
	
	void		set_modedata(INT32 mode_idx, INT32 mode_value) { m_i32RoomModeData[mode_idx] = mode_value; }
	
	virtual void OnOpen() = 0;
	void		Reset( i3Stage* pStage, bool bDataReset );

protected:
	INT32				get_global_int(const char* g);
	REAL32				get_global_real(const char* g);
	INT32				get_table_enum_int( const char* table_name, const char* enum_name);
	INT32				get_table_array_2d_int(const char* table_name, INT32 i, INT32 j);	// �̰ɷ� ������ ������ �Ʊ���.�˻��뵵.

	// �������� �Ʒ��� ���� ������ ����..
	void				access_global_table(const char* table_name); 
	void				access_table(const char* table_name);
	void				access_index_from_table(INT32 idx);
	void				pop_access(INT32 n) {  lua_pop(m_hL, n); }		// �＼���� �������� �ϳ���..
	//

	INT32				get_stack_int() { return luaL_optint(  m_hL, -1, -1); }
	REAL32				get_stack_real() { return lua_tonumber(  m_hL, -1 ); }
	const char*			get_stack_string() { return luaL_optstring( m_hL, -1, NULL); }


	void				push_global(const char* g) { lua_getglobal(m_hL, g); }
	bool				convertible_numeric()		{ return lua_isnumber( m_hL, -1 ) != FALSE; }		// �ʿ��Ҷ��� �ᵵ ����..
	bool				convertible_table()			{ return lua_istable(m_hL, -1) != FALSE; }

	void				access_table_idx(INT32 idx) { lua_rawgeti(m_hL, -1, idx + 1); }		// +1�� ��ư� �ε��� 1���� �����ؼ���.
	void				access_table_key(const char* k) { lua_getfield(m_hL, -1, k); }
	void				clear() { lua_settop(m_hL, 0); }
private:
	void				_remove_modedata_cb(const type_info& ti, const void* vo); 
	void				_clear_modedata_cb();
public:

	bool		Open(const char* szFilename);		// �񰡻�ó��..���ο� TP�����Լ�����ó��..
	void		set_modedata(PROTOCOL p, P_BATTLE_SERVER_SYNC_MODEDATA_ACK* data, const INT32 num);		
	//	call("LuaFun") << 32 << a << str >> ret1 >> ret 2 >> end_call;
	mode_script_arg&	call(const char* str_fun);

public:
	// lua register ���
	virtual void RegFunc(const luaL_Reg* pLuaReg);

	// ������ - SOW_������ 
	static int on_error( lua_State *L );
	static void print_error( lua_State *L, const char* fmt, ... );
	static void call_stack( lua_State* L, int n );
	static void print_stack( lua_State *L );						// ���� Ȯ��

protected:
	lua_State*			m_hL;
	i3Stage*			m_pStage;
	mode_script_arg		m_msa;
protected:
	std::string				m_strModeDataString[MODE_DATA_MAX];
	std::vector<ModeDataCB*> m_modeDataCtn[MODE_DATA_MAX];
	INT32					m_i32RoomModeData[MODE_DATA_MAX];	// ��嵥����
};

template<class C>
bool	IModeScript::add_modedata_cb(const char* mode_index_name, C* o, void (C::*f)(IModeScript*, PROTOCOL, const char*, INT32, INT32))
{
	INT32 mode_idx = find_modedata_index(mode_index_name);
	if (mode_idx < 0 || mode_idx >= MODE_DATA_MAX) return false;
	
	std::string& str = m_strModeDataString[mode_idx];
	if (str.empty()) str = mode_index_name;

	std::vector<ModeDataCB*>& v = m_modeDataCtn[mode_idx];
	v.push_back( new TModeDataCB<C>(o, f) );

	return true;
}

template<class C>
bool	IModeScript::add_modedata_cb(const char* begin_idx_name, const char* end_idx_name, C* o, 
									 void (C::*f)(IModeScript*, PROTOCOL, const char*, INT32, INT32), bool offset_enable)
{
	INT32 begin_idx = find_modedata_index(begin_idx_name);
	if (begin_idx < 0 || begin_idx >= MODE_DATA_MAX) return false;

	INT32 end_idx = find_modedata_index(end_idx_name);
	if (end_idx < begin_idx || end_idx >= MODE_DATA_MAX) return false;

	std::string& str_begin = m_strModeDataString[begin_idx];
	if (str_begin.empty()) str_begin = begin_idx_name;

	std::string& str_end = m_strModeDataString[end_idx];
	if (str_end.empty()) str_end = end_idx_name;
	
	INT32 offset = INT32(offset_enable)*begin_idx;
	
	for (INT32 i = begin_idx ; i <= end_idx ; ++i)
	{
//		std::string& str = m_strModeDataString[i];
		std::vector<ModeDataCB*>& v = m_modeDataCtn[i];
		v.push_back(new TModeDataCB<C>(o, f, offset));
	}

	return true;
}

template<class C>
bool	IModeScript::add_modedata_cb(INT32 mode_idx, C* o, void (C::*f)(IModeScript*, PROTOCOL, const char*, INT32, INT32))
{
	if (mode_idx < 0 || mode_idx >= MODE_DATA_MAX) return false;
	
	std::string& str = m_strModeDataString[mode_idx];
	if (str.empty())
	{
		str = StringUtil::ToString( mode_idx );
	}

	std::vector<ModeDataCB*>& v = m_modeDataCtn[mode_idx];
	v.push_back( new TModeDataCB<C>(o, f) );

	return true;
}


//�Ϲ������� ��ü�� �������� ���� ������ �ɰ��̹Ƿ�, �װ����� �Ǵ��ؼ� �����..
template<class C>
void	IModeScript::remove_modedata_cb(C* o)
{
	_remove_modedata_cb(typeid(o),  reinterpret_cast<const void*>(o));
}

