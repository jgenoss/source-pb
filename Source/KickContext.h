#pragma once

//
// ����� ���õ� �͵鸸 ���� �и�..  (������ �ʾƺ��δ�..)
//

class KickContext : public i3::shared_ginst<KickContext>
{
public:
	void		SetKickedByVote()		{ m_KickedByVote = true; }
	void		ClearKickedByVote()		{ m_KickedByVote = false; }
	bool		IsKickedByVote() const	{ return m_KickedByVote; }
	
private:
	
	bool		m_KickedByVote = false;

};
