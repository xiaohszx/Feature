#include <windows.h>
#include "Classify.h"
#include <io.h>
#include "FeatureSDK.h"
#include <fstream>
#include "chinese_whispers.h"
#include <direct.h>

#define SAVE_MODE 2

// �Ը���Ŀ¼�ڵ�ͼƬ����������ȡ
Results getFeature(const std::string &folder_path)
{
	clock_t t = clock();
	//�ļ����
	intptr_t hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	std::string p;
	Results ret;
	try
	{
		if ((hFile = _findfirst(p.assign(folder_path).append("\\*.*").c_str(), &fileinfo)) != -1)
		{
			do {
				if (0 == strcmp(fileinfo.name, ".") || 0 == strcmp(fileinfo.name, ".."))
					continue;
				std::string child = folder_path + "\\" + fileinfo.name;
				if (FILE_ATTRIBUTE_DIRECTORY & fileinfo.attrib) //��Ŀ¼
				{
					continue;
				}
				else
				{
					cv::Mat m = imread(child.c_str());
					if (m.rows > 32 && m.cols > 32)
					{
						tfOutput f = features(m, 512, 1, 1);
						ret.push_back(Result(child, f));
					}
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}
	catch (std::exception e) { if (hFile) _findclose(hFile); }
	t = clock() - t;
	if (!ret.empty())
		printf("Total %d, Avg using time %fms.\n", (int)ret.size(), float(t)/ret.size());
	return ret;
}

// ������ֱ���б���
void SaveFile(const Results & m, const std::string &name, const std::string &feats)
{
	std::ofstream  f1, f2;
	f1.open(name.c_str());
	f2.open(feats.c_str());
	if (f1.is_open() && f2.is_open())
	{
		for (Results::const_iterator p = m.begin(); p != m.end(); ++p)
		{
			f1 << p->name << "\n";
			tfOutput f = p->feature;
			for (int i = 0; i < f.nx-1; ++i)
				f2 << f.feature[i] << " ";
			f2 << f.feature[f.nx - 1] << "\n";
		}
		f1.close();
		f2.close();
	}
}

// ���������Ϊ�������ļ�
void SaveFile(const Results & m, const std::string &file)
{
	if (m.size() == 0)
		return;
	FILE *fid = fopen(file.c_str(), "wb");
	if (fid)
	{
		// �ļ�ͷ
		const char HEADER[8] = "FEATURE";
		fwrite(HEADER, 1, 8, fid);
		// ����
		intptr_t num = m.size();
		fwrite(&num, sizeof(intptr_t), 1, fid);
		// �ߴ�
		int dims[3] = { m[0].feature.nx, m[0].feature.ny, m[0].feature.nz };
		int total = dims[0] * dims[1] * dims[2];
		fwrite(dims, sizeof(int), 3, fid);
		// ��������
		for (Results::const_iterator p = m.begin(); p != m.end(); ++p)
		{
			std::string name = p->name;
			char buf[_MAX_PATH] = { 0 };
			strcpy_s(buf, name.c_str());
			fwrite(buf, _MAX_PATH, 1, fid);
			tfOutput feature = p->feature;
			fwrite(feature.feature, sizeof(float), total, fid);
		}
		fclose(fid);
	}
}

// �Ӷ������ļ����ؽ��
const Results LoadFile(const std::string &file) {
	Results m;
	FILE *fid = fopen(file.c_str(), "rb");
	if (fid)
	{
		printf("Loading features - powered by VGG-16.\n");
		// �ļ�ͷ
		char HEADER[8] = { 0 };
		fread(HEADER, 1, 8, fid);
		if (strcmp("FEATURE", HEADER))
			return m;
		// ����
		intptr_t num = -1;
		fread(&num, sizeof(intptr_t), 1, fid);
		m.reserve(num);
		// �ߴ�
		int dims[3] = { 0,0,0 };
		fread(dims, sizeof(int), 3, fid);
		int total = dims[0] * dims[1] * dims[2];
		// ��������
		for (int i=0; i < num; ++i)
		{
			char name[_MAX_PATH];
			fread(name, _MAX_PATH, 1, fid);
			tfOutput feature(dims[0], dims[1], dims[2]);
			fread(feature.feature, sizeof(float), total, fid);
			m.push_back(Result(name, feature));
		}
		fclose(fid);
	}
	return m;
}

// �ݹ�ɾ��Ŀ¼
bool DeleteFolder(const std::string &folder_path)
{
	bool ret = true;
	//�ļ����
	intptr_t hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	std::string p;
	try
	{
		if ((hFile = _findfirst(p.assign(folder_path).append("\\*.*").c_str(), &fileinfo)) != -1)
		{
			do {
				if (0 == strcmp(fileinfo.name, ".") || 0 == strcmp(fileinfo.name, ".."))
					continue;
				std::string child = folder_path + "\\" + fileinfo.name;
				if (FILE_ATTRIBUTE_DIRECTORY & fileinfo.attrib) //��Ŀ¼
				{
					if (DeleteFolder(child)) {
						BOOL b = RemoveDirectory(child.c_str());
						if (FALSE == b) ret = false;
						printf("ɾ��\"%s\"%s.\n", child.c_str(), b ? "�ɹ�" : "ʧ��");
					}
					else ret = false;
				}
				else // �ǿ�Ŀ¼
				{
					if (!DeleteFileA(child.c_str())) {
						ret = false;
						printf("ɾ��\"%s\"ʧ��.\n", child.c_str());
					}
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}
	catch (std::exception e) { if (hFile) _findclose(hFile); }

	return ret;
}

// ��path��ȡ�ļ�����
std::string getFileName(const std::string &path) {
	const char *h = path.c_str(), *p = h + path.length();
	while (p != h && *p != '\\' && *p != '/') --p;
	return p + 1;
}

// ��path��ȡ�ļ�Ŀ¼
std::string getFileDir(const std::string &path) {
	char buf[_MAX_PATH], *p = buf + path.length();;
	strcpy_s(buf, path.c_str());
	while (p != buf && *p != '\\' && *p != '/') --p; *p = 0;
	while (p != buf && *p != '\\' && *p != '/') --p;
	return p + 1;
}

// ��path�������ļ����浽ָ�������Ŀ¼
// λ��classifyĿ¼����Ŀ¼���ļ�ȡ�������ͷ
void save_mode1(const std::string &path, int class_id) {
	static bool made = false;
	char dir[_MAX_PATH], src[_MAX_PATH], dst[_MAX_PATH];
	std::string targetDir = getFileDir(path);
	std::string targetName = getFileName(path);
	sprintf_s(dir, "./classify/%s", targetDir.c_str());
	if (!made)
	{
		_mkdir("./classify");
		_mkdir(dir);
		made = true;
	}
	sprintf_s(src, "%s", path.c_str());
	sprintf_s(dst, "%s/%03d_%s", dir, class_id, targetName.c_str());
	if (FALSE == CopyFile(src, dst, TRUE))
		printf("CopyFile %s failed.\n", path.c_str());
}

// ��path�������ļ����浽ָ�������Ŀ¼
// λ��classifyĿ¼����Ŀ¼����Ŀ¼Ϊ���
void save_mode2(const std::string &path, int class_id) {
	static bool made = false;
	static int i = 0;
	char sub_dir[_MAX_PATH], src[_MAX_PATH], dst[_MAX_PATH];
	std::string targetDir = getFileDir(path);
	std::string targetName = getFileName(path);
	std::string dir = "./classify/" + targetDir;
	sprintf_s(sub_dir, "%s/%03d", dir.c_str(), class_id);
	if (!made)
	{
		_mkdir("./classify");
		_mkdir(dir.c_str());
		_mkdir(sub_dir);
	}
	sprintf_s(src, "%s", path.c_str());
	sprintf_s(dst, "%s/%04d_%s", sub_dir, ++i, targetName.c_str());
	if (FALSE == CopyFile(src, dst, TRUE))
		printf("CopyFile %s failed.\n", path.c_str());
}

void save(const std::string &path, int class_id, int mode=1)
{
	switch (mode)
	{
	case 1: return save_mode1(path, class_id);
	case 2: return save_mode2(path, class_id);
	default:
		assert(0);
	}
}

// CW����
void Classify(const Results & m, double threshold)
{
	int num = m.size();
	if (num == 0)
		return;
	ARRAY<dlib::sample_pair> edges(USING_STL ? 0 : num * num / 16);
	unsigned long x = 0;
	for (Results::const_iterator i = m.begin(); i != m.end(); ++i, ++x)
	{
		unsigned long y = x;
		for (Results::const_iterator j = i; j != m.end(); ++j, ++y)
		{
			double d = i == j ? 0 : distance(j->feature, i->feature);
			if (d < threshold) edges.push_back(dlib::sample_pair(x, y));
		}
	}
	ARRAY<unsigned long> labels;
	const int num_clusters = chinese_whispers(edges, labels);
	int *count = new int[num_clusters]();
	std::cout << "element num: " << num << ", number of clusters: " << num_clusters << ".\n";
	for (int cluster_id = 0; cluster_id < num_clusters; ++cluster_id)
	{
		std::cout << "cluster " << cluster_id << ": ";
		for (int j = 0; j < labels.size(); ++j)
		{
			if (cluster_id == labels[j])
			{
				++count[cluster_id];
				save(m[j].name, cluster_id, SAVE_MODE);
			}
		}
		std::cout << count[cluster_id] << "��.\n";
	}
	delete[] count;
}
