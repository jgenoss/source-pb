#ifndef __FILTERNODE_H__
#define __FILTERNODE_H__

#include "i3Base/smart_ptr/pscoped_ptr_fwd.h"

typedef i3::pscoped_ptr<class CStrFilterRoot>	CStrFilterRootPtr;
typedef i3::shared_ptr<class CStrFilterNode>	CStrFilterNodePtr;

// ��Ʈ ��� ��� �׳� CStrFilter�� ����..
class CStrFilterRoot
{
public:
	CStrFilterRoot();
	~CStrFilterRoot();

	CStrFilterNode*		InsertChild(wchar_t child_val, bool is_end);
	CStrFilterNode*		FindChild(wchar_t child_val) const ;
	
private:
	i3::vector_set<CStrFilterNodePtr>	m_ChildList;	
};


class CStrFilterNode
{
public:
	CStrFilterNode(wchar_t c, bool bEnd);									// �����ڴ� �̰͸� �д�..
	~CStrFilterNode();

	CStrFilterNode*						InsertChild(wchar_t child_val, bool is_end);
	CStrFilterNode*						FindChild(wchar_t child_val) const;

	bool								IsEnd() const { return m_bEnd; }		// �ڽĳ�尡 �ִ��� ����� ��ŷ�ɼ� ����..
	wchar_t								GetChar() const { return m_wcText; }
	void								SetEnd(bool bEnd) { m_bEnd = bEnd; }

private:
	wchar_t								m_wcText;
	bool								m_bEnd;
	i3::vector_set<CStrFilterNodePtr>	m_ChildList;
};

#endif