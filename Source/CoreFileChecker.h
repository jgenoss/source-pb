#pragma once


class CFileList;

class CCoreFileChecker
{
public:
	CCoreFileChecker(void);

public:
	bool	Open();
	int		Check(const tstring & FilePath);

	// cpp�� ����� ������ �˻��Ѵ�.
	int		CoreFilesCheck();

	const UINT32* GetMd5(const tstring & FilePath);

private:
	std::tr1::shared_ptr<CFileList>	m_FileList;
	bool					m_IsNTFS;
};
