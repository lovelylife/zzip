
#ifndef __COLOR_H__
#define __COLOR_H__

namespace QUI {

// Color Helper Class
// provide transition between value and string RRGGBB
class Color {
public:
	static Color* FromStringRRGGBB(const tstring& sRRGGBB);
	static Color* FromStringRGBVVV(const tstring& sRGBVVV);

// construct
public:
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	Color(const Color&);
	Color operator =(const Color&);
	operator COLORREF() const;
protected:
	Color();
// methods
public:
	// ����ɫת���ַ��� #rrggbb
	tstring Format2RRGGBB();
	// ����ɫת��RGB(0xrr,0xgg,0xbb); ����alphaֵ
	tstring Format2RGBVVV();

private:
	// ARGB
// 	unsigned char A;
// 	unsigned char R;
// 	unsigned char G;
// 	unsigned char B;
	unsigned long ARGBData;
};
	
}; // end namespace QUI


#endif