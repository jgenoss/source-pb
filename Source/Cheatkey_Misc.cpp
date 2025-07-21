#include "pch.h"
#include "Cheatkey_Misc.h"
#include <functional>

namespace Cheatkey
{
	void TrimEmptySpace(i3::string& str)
	{
		i3::string::iterator it = str.begin();
		while (it != str.end())
		{
			if (*it == ' ' || *it == '\t')
				it = str.erase(it);
			else
				++it;
		}
	}

	// ������ ��ȿ ����.
	// ���� ��� Ű���尡 ������ ���� ��� /warp slot[0,1], nick[player1]
	// �Ķ���� �� �������� '[]'���� ','�� �����ڷ� �������� �ʾƾ� slot[0,1]�� nick[player1]�� �Ķ���ͷ� �̾Ƴ� �� �ִ�.
	// ','�� �����ڷ� �����Ǳ� ���ؼ��� ���� ��ġ�� �Ķ���� �� ������(���⼭�� [])�� ¦�� �̷������ �Ǵ�.
	bool IsSepValid(const i3::string& text, size_t pos, const i3::string& sep)
	{
		if (pos == i3::string::npos) return false;

		struct fn : std::binary_function<char, i3::string, bool>
		{
			bool operator()(const char ch, const i3::string& sp) const
			{
				return sp.find(ch) != i3::string::npos;
			}
		};

		INT32 seps = std::count_if(text.begin(), text.begin()+pos, std::bind2nd(fn(), sep));

		return (seps % 2) ? false : true; 
	}

	Tokenizer::Tokenizer(const i3::string& tgt, const i3::string& sep, const i3::string& subsep) 
		: m_target(tgt), m_sep(sep), m_subsep(subsep), m_pos(0), m_idx(0) {}

	bool Tokenizer::HasToken() 
	{ 
		return (m_pos != i3::string::npos && m_pos != m_target.size()-1); 
	}

	i3::string Tokenizer::NextToken()
	{
		m_pos = m_target.find_first_of(m_sep, m_idx);

		if (!m_subsep.empty())
		{
			while (m_pos != i3::string::npos && !IsSepValid(m_target, m_pos, m_subsep))
			{
				m_pos = m_target.find_first_of(m_sep, m_pos+1);
			}
		}

		i3::string ret = m_target.substr(m_idx, m_pos - m_idx);
		m_idx = m_pos + 1;

		return ret;
	}

	void Tokenize(const i3::string& text, i3::vector<i3::string>& outTokens, const i3::string& sep, const i3::string& subsep)
	{
		Tokenizer tok(text, sep, subsep);

		outTokens.push_back(tok.NextToken());

		while (tok.HasToken())
		{
			outTokens.push_back(tok.NextToken());
		}
	}
}