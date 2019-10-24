#if !BUILD_DLL

#include "FeatureSDK.h"
#include "Classify.h"

// ע�����SetPythonHome�޸ĵ�ǰʹ�õ�python.
// main: ��srcĿ¼�µ�ͼƬ����������ȡ������Ϊ"result.bin"
// ����ЩͼƬ����CW�㷨���з��࣬������������classifyĿ¼.
int main(int argc, const char *argv[])
{
	const char *path = argc > 1 ? argv[1] : "./src";
	if (!Init("D:\\ProgramData\\Anaconda3\\envs\\tfcpu", 
		"index", "Feature"))
		return -1;

	Results m = LoadFile("./result.bin");
	if (m.size() == 0)
	{
		m = getFeature(path);
		SaveFile(m, "./result.bin");
	}
	
	// ��ֵԽС��������Խ�࣬��ֵԽ�󣬷�����Խ��.
	Classify(m, 0.6);
	system("PAUSE");
	return 0;
}

#endif
