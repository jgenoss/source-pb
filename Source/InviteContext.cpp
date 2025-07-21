#include "pch.h"
#include "InviteContext.h"

void InviteContext::CheckInviteUserInfo(UINT32 idx)
{
//	idx = MINMAX(0, idx, MAX_INVITE_COUNT);
//	I3_BOUNDCHK(idx, MAX_INVITE_COUNT);				//  ������ �ϰ� �Ʒ��Ŵ� �� �ϴ°ǵ�... MINMAX�� ��..���δ�..

	if (idx < MAX_INVITE_COUNT)		// ���� ���ڵ忡 ������ �ִٰ� ����ģ �ʰ���??  �̰� �Լ��� ���°� ���۰� �ƴ�����?
		m_bInviteUserIdx[idx] = true;
	else
		I3ASSERT(idx < MAX_INVITE_COUNT);
}

void InviteContext::ClearInviteCheck(void)
{
	INT32 i;
	for( i = 0; i < MAX_INVITE_COUNT; i++)
	{
		m_bInviteUserIdx[i] = false;
	}
}

UINT32 InviteContext::GetCheckInviteCount(void) const
{
	INT32 i;
	UINT32 nCount = 0;

	for( i = 0; i < MAX_INVITE_COUNT; i++)
	{
		if (m_bInviteUserIdx[i])
		{
			nCount++;
		}
	}

	return nCount;
}

bool	InviteContext::PopInviteInfo(INVITE_INFO* out)
{
	if (m_stackInviteInfo.empty())
		return false;

	*out = m_stackInviteInfo.back();
	m_stackInviteInfo.pop_back();
	return true;
}

void	InviteContext::PushInviteInfo(const INVITE_INFO& in)
{
	m_stackInviteInfo.push_back(in);
}

// Ư�� ������ �г������� ���� �ε��� �������� 
INT32 	InviteContext::GetUserSessionIdxByUserNickName(const i3::wliteral_range& wNickName ) const
{
	INT32 i32SessionIndex = -1;

	for ( UINT32 i = 0 ; i < m_nUserInfoInviteCount ; i++ )
	{
		if( i3::generic_is_iequal( wNickName, GetUserInfoInvite(i)->_wstr_nick ) ) 
		{	
			i32SessionIndex = GetUserInfoInvite(i)->_idxSession;
		}
	}

	return i32SessionIndex;
}