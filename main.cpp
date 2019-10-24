#if !BUILD_DLL

#include "FeatureSDK.h"
#include "opencv2/opencv.hpp" 

// ע�����SetPythonHome�޸ĵ�ǰʹ�õ�python.
int main(int argc, const char *argv[])
{
	const char *path = argc == 1 ? "./image.jpg" : argv[1];

	if (!Init("D:\\ProgramData\\Anaconda3\\envs\\tfcpu", "index", "Feature"))
		return -1;

	cv::Mat m = cv::imread(path);
	tfOutput f = features(m, 512, 1, 1);
	system("PAUSE");
	return 0;
}

#endif
