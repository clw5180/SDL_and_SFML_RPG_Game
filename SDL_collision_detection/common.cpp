#include "common.h"
#include <iostream>
#include <iomanip>
#include <iconv.h> //clw note：这里需要在VS添加iconv.h的包含目录
/************************************************************************/
/* 作者：clwclw
/* 参考：https ://blog.csdn.net/lf426/article/details/2243032                                                                    */
/************************************************************************/
std::vector<Uint16> getUnicode(const std::string& str)
{
	//string size
	const int STR_SIZE = 256;
	//string to be changed
	const unsigned char* src = (const unsigned char*)(str.c_str());
	size_t src_len = strlen((char*)src);

	//Unicode dst to get
	//wchar_t dst[STR_SIZE] = { 0 };  //clw modify 20190125
	unsigned char dst[STR_SIZE] = { 0 };
	size_t dst_len = sizeof(dst);

	//iconv's arg
	const unsigned char* in = src;
	//wchar_t* out = dst; //clw modify 20190125
	unsigned char* out = dst;

#ifdef _DEBUG
	std::cout << "src: " << src << std::endl;
#endif

	//GB2312 to UCS-2 (Unicode)
	iconv_t cd;
	cd = iconv_open("UCS-2", "GB2312");
	if ((iconv_t)-1 == cd) {
		exit(-1);
	}
	//conversion
	iconv(cd, (const char**)&in, &src_len, (char**)&out, &dst_len);
	iconv_close(cd);


#ifdef _DEBUG
	//Unicode dst
	std::cout << "dst: ";
#endif

	/************************************************************************/
	//修改原语句：
	//    int unicode_len = strlen((char*)dst);
	//修改之后：
	int unicode_len = wcslen((wchar_t*)dst); //clw modify 20190125：
	/************************************************************************
	问题描述：
		iconv不能完全正确的获得Unicode，也就是我们刚才遇到的纯汉字转换没问题，而有英文就不行了；
		GB2312转换unicode时，例如英文字符w，gb2312编码是单字节，unicode编码是两个字节，低位为0x00,
		转换完后w后面的其他字符都没了....即，在使用C库函数strlen（）的时候就出现了问题，
		比如ANSI字符的编码如'a'=97，就使用高位补0，也就是dst[0]=0或者说'\0'
		dst[1]=97或者说'\a'，那么在使用strlen（）的时候就出现了问题。

	原因：
		因为unicode中包含了字符串结束符'\0'，所以不能用strlen这样的函数去取长度

	解决：
		方法1：使用wcslen。
		方法2：也可以考虑自己实现wcslen：
		以步长为2进行统计，当第一个字符为0x00时就是字符串结束
		这个函数自己写没什么难的吧？strlen是单字节比较，你就每次双字节比较不就行了？
		不能用wchar的话就每次指针+2，比较两个字节不就行了？
		char* p = str;
		while(!(*p == 0 &&*(p+1) == 0)) p += 2;
		return (int)(p-str)/2;
	/************************************************************************/

	std::vector<Uint16> unicodeVectorArray;
	for (int i = 0; i < unicode_len; i++)
	{
		int a = 256 * dst[2 * i] + dst[2 * i + 1]; //clw note：iconv的参数是dst，char类型，8位，但是
		                                           //这里需要直接取dst[2*i]和dst[2*i+1]构成的16位数
		unicodeVectorArray.push_back(a);
#ifdef _DEBUG
		std::cout << std::hex;  //clw note：可以用来进行十六进制流输出
		std::cout << "0x" << std::setw(4) << std::setfill('0') << a << " ";
		std::cout << std::dec;  //clw note：恢复十进制流输出
#endif
	}
#ifdef _DEBUG
	std::cout << std::endl;
	std::cout << "len: " << unicode_len << std::endl;
#endif
	return unicodeVectorArray;
}