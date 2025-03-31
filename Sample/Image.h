#pragma once
class Image
{
public:
	/*Image.hの「public:」に追加*/
	//メンバ変数
	int Height; //高さ 
	int Width; //幅
	unsigned char** R, ** G, ** B; //RGBデータ格納用

	//引数つきコンストラクタ
	Image(int h, int w);
	//デストラクタ(仮想関数)
	virtual ~Image();
};

