#pragma once
#include <vector>
#include <string>
#include "config.h"

// @see https://github.com/yuanyuanxiang/public
// �뽫����Ŀ��¡
#include "../public/fileop.h"

// ͼƬ��������
struct Result
{
	std::string name;
	tfOutput feature;
	Result(const std::string &n, const tfOutput &f):name(n), feature(f) { }
};

// ����-ͼƬ��������
typedef std::vector<Result> Results;

// ����������ȡ
Results getFeature(const std::string &folder_path);

// �ֱ���б���-�������ļ��б�Ϊ�ļ�����������Ϊ��һ���ļ�
void SaveFile(const Results &m, const std::string &name, const std::string &feats);

// ���������ļ�
void SaveFile(const Results & m, const std::string &file);

// ����
void Classify(const Results &m, double threshold);

// ���ļ����ؽ��
const Results LoadFile(const std::string &file);
