#pragma once

namespace WinMagnet
{
	/*	������ â �ڼ� ȿ�� �Լ�(http://paulownia.egloos.com/4443241)
	*/
	void CreateProc(INT32 offset = 60); //�ڼ� ȿ�� �ʱ�ȭ �Լ� offset ��ŭ ��������� ������� �ٴ´�. WM_CREATE ������ �޼������� ȣ���ϸ� �ȴ�.
	void WindowPosChangingProc(LPARAM lParam); //WM_WINDOWPOSCHANGING ������ �޼������� ȣ���ϸ� �ȴ�.
	void SetOffset(INT32 offset);
}