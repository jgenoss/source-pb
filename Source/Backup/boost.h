#ifndef		__USING_BOOST_H__
#define		__USING_BOOST_H__

// ver boost_1_45_0
// boost library (�߰��� ����� boost ���̺귯���� ���� ��� ������� include�ϸ� �ش� boost �Լ��� ����� �� �ִ�. (komet)
//#include "boost/format.hpp"		// format



//-------------------------------------------------------------------------------
// boost::format()
//
// web: http://www.boost.org/doc/libs/1_45_0/libs/format/index.html
// syntax: http://www.boost.org/doc/libs/1_45_0/libs/format/doc/format.html
//-------------------------------------------------------------------------------
// boost ���̺귯���� format �Լ��� type safe string format �̸� ������ ���Ծ��� ���ڸ� �־ �����ϴ�. 
// sprintf �迭�� ������ Ÿ���� �ٸ��� ������ �Ǹ� ���� ��� "%s" ��Ʈ���� <- int�� ���� ���� ������ API ���ο��� ũ�����ϰԵȴ�.
// ������ ��Ʈ���� �����Ҷ� ���� ����� ����� ���� ������ ������ �ݴ�� ���� ��찡 ���� ��� �ɰ��� ũ���ø� �����Ͽ� �� ������ �ذ��ϱ� ���� boost ���̺귯���� ����Ѵ�.
// �Լ��� ����Ϸ��� #include "boost.h" �� �����ϼ���.
//
// ��Ʈ�� ���� ������ ���� ���� ���� ���ʷ� ���ԵǸ� ���� Ÿ���� �����Ӵ�.
// ���� ��Ʈ���� %d %s �� ��밡���ϳ� �������� ���ؼ� ���ڰ� Ʋ���� ũ�����ϹǷ� �����ؼ� ����� ��.
//
//	����:
//
//	%��ȣ%		: �������� %���۷����͸� ����ϸ� ��Ʈ�� ���信�� ������ ������ ���Ѵ�.
//	%��ȣ$03d	: %03d�� ������ 000 �������� ��ġ ���� ��Ÿ����. �������� ���� 0���� ������ �ڸ��� ä�����Ƿ� �����ϴ�.
//
//  ���:	�Ʒ� ���� ��ȯ���� std::string �̹Ƿ� char * �� ����Ϸ��� �ѹ� ��ȯ�� �ʿ��ϴ�.
//
//	��) std::string str = BOOST_FORMAT3( "%3% %2% %1%", "1��", "2��", "3��");		//	std::string���� �޾ƿ´�.
//	    const char * temp = str.c_str();		// std::string���� ��ȯ
//
//	���)	s == "3�� 2�� 1��";
//
//#define BOOST_FORMAT1( fmt, arg1)						((boost::format(fmt) % (arg1)).str())
//#define BOOST_FORMAT2( fmt, arg1, arg2)					((boost::format(fmt) % (arg1) % (arg2)).str())
//#define BOOST_FORMAT3( fmt, arg1, arg2, arg3)			((boost::format(fmt) % (arg1) % (arg2) % (arg3)).str())
//#define BOOST_FORMAT4( fmt, arg1, arg2, arg3, arg4)		((boost::format(fmt) % (arg1) % (arg2) % (arg3) % (arg4)).str())


#endif // __USING_BOOST_H__