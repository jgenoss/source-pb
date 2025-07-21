#pragma once
#if ENABLE_UNIT_TEST

enum	UIPOPUPWIN;

namespace GameUnitTest
{
	namespace Popup
	{
		class TesterBase;
	}
}

namespace GameUnitTest
{
	class PopupList
	{
	private:
		i3::vector<GameUnitTest::Popup::TesterBase*> m_list;

	public:
		PopupList() {}
		~PopupList() {}

		//test �� popup type ���
		bool AddPopup( UIPOPUPWIN _type);

		//delete
		void Clear();
			
		//test �� popup�� �������� �����Ͽ� open
		//return ���� 0�� ���� popup�� open ���� �������.
		bool OpenPopup();

		//�ƹ� ��ư ������..
		void ProcessPopup();

		//��� �Ǿ� �ִ� popup���� �����ִ� popup�� ���� ��� 1.
		bool IsOpenPopup();

		//��� �Ǿ� �ִ� popup�� ����.
		size_t GetSize() const	{ return m_list.size();}
	};
};

#endif //end of ENABLE_UNIT_TEST