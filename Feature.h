#pragma once

#include "pyCaller.h"

/************************************************************************
* @class Feature
* @brief ��ͼ�����������ȡ����
************************************************************************/
class Feature
{
private:
	int w, h; // ͼƬ�ߴ�
	std::string module; // python�ű���
	std::string func; // python������
	pyCaller m_py; // python������

public:
	bool Init(const char *python, const char *mod, const char *fun);
	void Uninit();

	Feature();
	~Feature();

	tfOutput features(const cv::Mat &m, int x, int y, int z);
};

extern Feature F;
