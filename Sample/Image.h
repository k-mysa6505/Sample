#pragma once
class Image
{
public:
	/*Image.h�́upublic:�v�ɒǉ�*/
	//�����o�ϐ�
	int Height; //���� 
	int Width; //��
	unsigned char** R, ** G, ** B; //RGB�f�[�^�i�[�p

	//�������R���X�g���N�^
	Image(int h, int w);
	//�f�X�g���N�^(���z�֐�)
	virtual ~Image();
};

