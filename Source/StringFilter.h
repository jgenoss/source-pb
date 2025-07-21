#ifndef _StringFilter_h
#define _StringFilter_h

#include "FilterNode.h"

enum STR_FILTER
{
	STR_FILTER_INCLUDE_NICK = 0,	//
	STR_FILTER_MATCH_NICK,			//
	STR_FILTER_INCLUDE_CHAT_NICK,	//

#if defined( LOCALE_NORTHAMERICA )
	STR_FILTER_INCLUDE_CHAT_NICK2,
#endif

	STR_FILTER_URL,
	STR_FILTER_MAX
};

bool IsValidNicknameLetterSize(const i3::wliteral_range& nickname);

// �ݺ��Է�,�����Է� ó���� ��� ���͸��� �ƹ� ������� ������ Ŭ������ �и��Ѵ�...

namespace detail
{
	template<class T>
	struct is_wchar_str_if_iterator : i3::integral_constant<bool, sizeof(typename i3::iterator_value<T>::type) == sizeof(wchar_t)> {};
	template<class T>
	struct is_wchar_str_if_seq : i3::integral_constant<bool, sizeof(typename i3::range_value<T>::type) == sizeof(wchar_t)> {};
	template<class T>
	struct is_wchar_str : mpl::eval_if<i3::is_iterator<T>, is_wchar_str_if_iterator<T>, is_wchar_str_if_seq<T> > {};

	template<class T, class R>
	struct enable_if_wchar_str : i3::enable_if< is_wchar_str<T>, R> {};



}

#include "i3Base/smart_ptr/pscoped_ptr.h"

typedef i3::pscoped_ptr<class CChattingInputChecker>		CChattingInputCheckerPtr;

class CChattingInputChecker
{
public:
	
	CChattingInputChecker();
	~CChattingInputChecker();

	void					UpdateText(const i3::wliteral_range& wTextRng);
	void					UpdateText(const i3::rc_wstring& wstrText);
	
	template<class T>		typename detail::enable_if_wchar_str<T,
	void>::type				UpdateText(const T& wstr) { UpdateText( i3::wliteral_range(wstr) );   }

	bool					IsInputBlock() const { return m_bInputBlocked;  }
	bool					IsRepeatBlock() const { return m_bRepeatBlocked; }
	
private:
	bool					InputCheckImp(DWORD dwCurrTime);
	bool					RepeatCheckImp(const i3::wliteral_range& wTextRng, DWORD dwCurrTime);
	bool 					UpdateCheckImp(const i3::wliteral_range& wTextRng);

	DWORD					m_dwInputStamp;
	DWORD					m_dwRepeatStamp;
	INT32					m_nRepeatCount;
	i3::rc_wstring			m_wstrOldString;
	
	bool					m_bInputBlocked;
	bool					m_bRepeatBlocked;
};


typedef i3::pscoped_ptr<class CStrFilter>		CStrFilterPtr;

// IsFilterNick�� IsFilterOK�� �����ƾ�� ���� ���� �ִ°��� �����ϰ�� IsFilterOK�� ����� ���� �Լ��̴�..

// IsFilterMatchOK�� �׽�Ʈ���ڿ��� �߰� ~ ������ Ȯ���� ���͹��ڿ��� ��ġ�� ��� ���͸���Ų��....
//                    ����, �׽�Ʈ���ڿ��� ���͹��ڿ��� ������ �� �ٸ� ���ڿ��� �̾����ٸ� ����
//                    ���͸��� �̷������ �ʴ´�..

class CStrFilter
{
public:
	CStrFilter();
	~CStrFilter();
	void					CreateFilter(STR_FILTER mode);
	bool					IsFilterOK( const i3::wliteral_range& wTextRng ) const;			// ä�ù��ڿ� ���ڿ��� ���ԵǾ��ִ���
	bool					IsFilterNickOK( const i3::wliteral_range& wTextRng ) const;		// �г��ӿ� ���ڿ��� ���ԵǾ��ִ���
	bool					IsFilterMatchOK( const i3::wliteral_range& wTextRng ) const;		// ���ڿ��� ������ �������� ��
	
	bool					ChangeStringOK( i3::wstring& wstrText ) const;			// ��ȣ������ ��Ʈ�� �Լ� ����
	bool					ChangeStringOK( i3::rc_wstring& wstrText) const;
	bool					ChangeStringOK( i3::stack_wstring& wstrText) const;


private:
	CStrFilterRoot			m_root;
};


#endif //_StringFilter_h
