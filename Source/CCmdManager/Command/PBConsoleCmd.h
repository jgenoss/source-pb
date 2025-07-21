#pragma once

#include "i3Framework/i3UI/i3UIConsoleCmd.h"

enum CMD_RESULT_FLAG
{
	CMD_RESULT_FAIL = 0,
	CMD_RESULT_OK,
	CMD_RESULT_LOOP
};

class PBConsoleCmd
{
public:
	UINT32 m_LoopCount = 0;
	string m_LogContext;
public:
	PBConsoleCmd(void) {}
	~PBConsoleCmd(void) {}
public:
	
	/* SAMPLE - .\\OutGameCmd\\PBCMD_UseItem
	--
		ó�� ��ɾ ������ ExecuteCommand() ���� ���� ó���� �����ϰ� �Ǵµ� ���⼭ �ʿ��� ���ڵ���
		���ͼ� ����ü���� ��� �����鿡 �����մϴ�.
	--
	*/
	virtual CMD_RESULT_FLAG ExecuteCommand( i3::vector<i3::string> &params) { return CMD_RESULT_FAIL; }

	/*
	--
		RunCommand() �� ���� �Ǵ°�� ���� Ŀ�ǵ��Է½� �ݺ�Ƚ���� ���ڷ� ���������� �⺻ LoopCount�� 1�� �����˴ϴ�
		return CMD_RESULT_OK; �� ��ȯ �ϰ� �Ǹ� PBConsoleManager ��ü���� LoopCount �� �ڵ����� 1�� �Ҹ�����ָ�
		LoopCount �� 0 �� �Ǹ� ���� ��ɾ PBConsoleManager ��ü���� ��ɼ��ฮ��Ʈ ���� ���ŵ˴ϴ�
		return CMD_RESULT_LOOP; �� ��ȯ �ϰԵǸ� LoopCount �� �ڵ����� �Ҹ�������� ������ ��Ȳ������ ó���Ͻø� �˴ϴ�.
	--
	*/
	virtual CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec) = 0;

public:
	void setLoopCount(UINT32 Count);
	UINT32 getLoopCount();

	void setLogContext(string str);
	void addLogContext(string str);
	string getLogContext( );
	void clearLogContext( );
	bool SaveLogFile( char Path[] , bool AddWrite = false );

	void PrintLog(string str) { }
	void PrintLog(wstring wstr) { }

};
