#if !BUILD_DLL

#include "FeatureSDK.h"
#include "Classify.h"
#include <direct.h>

// ��ȡĿ¼����.
std::string getDirName(const char *dir) {
	if (*dir)
	{
		char buf[_MAX_PATH], *p = buf;
		strcpy_s(buf, dir);
		while (*p) ++p;
		if (*(p - 1) == '\\' || *(p - 1) == '/') { --p; *p = 0; }
		while (p != buf && *p != '\\' && *p != '/') --p;
		std::string name = p+1;
		return name;
	}
	return "src";
}

// ע�����SetPythonHome�޸ĵ�ǰʹ�õ�python.
// main: ��srcĿ¼�µ�ͼƬ����������ȡ������Ϊ"result.bin"
// ����ЩͼƬ����CW�㷨���з��࣬������������classifyĿ¼.
// ����1�� ����2�� ����3������1ΪͼƬĿ¼������2Ϊ�ָ���ֵ.
// ����3Ϊpython����Ŀ¼.
// �˳�������resultĿ¼�����Ѿ�����Ľ����������.
int main(int argc, const char *argv[])
{
	const char *path = argc > 1 ? argv[1] : "./src";
	const float thresh = argc > 2 ? atof(argv[2]) : 0.6f;
	const char *python = argc > 3 ? argv[3] : 
		"D:\\ProgramData\\Anaconda3\\envs\\tfcpu";
	printf("Processing \"%s\".\n", path);

	_mkdir("./result");
	std::string bin = "./result/" + getDirName(path) + ".bin";
	Results m = LoadFile(bin);
	if (m.size() == 0)
	{
		if (!Init(python, "index", "Feature")) {
			system("PAUSE");
			return -1;
		}
		printf("Getting features - powered by VGG-16.\n");
		m = getFeature(path);
		SaveFile(m, bin);
	}
	
	// ��ֵԽС��������Խ�࣬��ֵԽ�󣬷�����Խ��.
	if (m.size())
	{
		std::string classify = "./classify/";
		std::string targetDir = getFileName(path);
		removeDir(classify + targetDir);
		Classify(m, thresh);
	}
#ifdef _DEBUG
	system("PAUSE");
#endif
	return 0;
}

#endif
