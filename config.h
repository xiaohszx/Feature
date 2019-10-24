// ��ע�⡿�뽫OpenCV�İ���Ŀ¼��python����Ŀ¼��ӵ���Ŀ���Ӱ���Ŀ¼
// ��python�Ŀ�Ŀ¼��ӵ���Ŀ�ĸ��ӿ�Ŀ¼
// Ȼ�����ʵ������޸ı��ļ���

#pragma once

// ����ڴ�й©���粻��Ҫ����ע������
//#include "vld.h"

//////////////////////////////////////////////////////////////////////////
// OpenCV ����
#define CV_VER "320"

#define USING_STL 1

#if _MSC_VER < 1900 // Visual Studio 2015���°汾��ʹ�������궨��������ӿ�

#ifdef _DEBUG
#define CV_LIB_PATH "D:/opencv/opencv32/x64/lib/Debug/"
#define CV_LIB_X(LIB, VER) CV_LIB_PATH##"opencv_"##LIB##VER##"d.lib"
#else
#define CV_LIB_PATH "D:/opencv/opencv32/x64/lib/Release/"
#define CV_LIB_X(LIB, VER) CV_LIB_PATH##"opencv_"##LIB##VER##".lib"
#endif

#define USING_CV_LIB(LIB) CV_LIB_X(LIB, CV_VER) // ʹ��CV�Ŀ�


//////////////////////////////////////////////////////////////////////////
#include "opencv2/opencv.hpp" 
#include "opencv/cv.h"
using namespace cv;

#pragma comment(lib, USING_CV_LIB("core"))
#pragma comment(lib, USING_CV_LIB("highgui"))
#pragma comment(lib, USING_CV_LIB("imgproc"))
#pragma comment(lib, USING_CV_LIB("video"))
#pragma comment(lib, USING_CV_LIB("videoio"))
#pragma comment(lib, USING_CV_LIB("imgcodecs"))
#pragma comment(lib, USING_CV_LIB("photo"))

#else

#include "opencv2/opencv.hpp" 
#include "opencv/cv.h"
using namespace cv;

#ifdef _DEBUG
#pragma comment(lib, "opencv_core320d.lib")
#pragma comment(lib, "opencv_highgui320d.lib")
#pragma comment(lib, "opencv_imgproc320d.lib")
#pragma comment(lib, "opencv_video320d.lib")
#pragma comment(lib, "opencv_videoio320d.lib")
#pragma comment(lib, "opencv_imgcodecs320d.lib")
#pragma comment(lib, "opencv_photo320d.lib")
#else
#pragma comment(lib, "opencv_core320.lib")
#pragma comment(lib, "opencv_highgui320.lib")
#pragma comment(lib, "opencv_imgproc320.lib")
#pragma comment(lib, "opencv_video320.lib")
#pragma comment(lib, "opencv_videoio320.lib")
#pragma comment(lib, "opencv_imgcodecs320.lib")
#pragma comment(lib, "opencv_photo320.lib")
#endif

#endif

#ifdef _AFX
#define OUTPUT TRACE
#else
#define OUTPUT printf
#endif


// �Ƿ�֧��IPC
#define SUPPORT_IPC 1

// �Ƿ�ʹ��⺯��Ϊstatic(�ܱ���Ƶ������map)
#define STATIC_DETECTING 1

// ������ʶ������ͼ�����������С�ߴ�
#define MIN_FACESIZE 64

#define SAFE_DELETE_ARRAY(p) if(p) { delete [] (p); (p) = NULL; }

#define TRUE 1

#define FALSE 0

/************************************************************************
* @class tfOutput
* @brief tensorflowģ������Ĳ����ṹ, ���numpy array
* @note ���ڸýṹ��Ҫ��Ƶ��ʹ��, �ʸ���ͨ�����ü���ref�����ڴ�, \n
�����ڴ�й©, ����ϵ����
************************************************************************/
class tfOutput
{
public:
	int nx, ny, nz;			// numpy array��ά����Ϣ
protected:
	int *ref;				// ���ü���
	int addref() const { return ++(*ref); }
	int removeref() const { return --(*ref); }
	void destroy()			// ����
	{
		if (0 == removeref())
		{
			SAFE_DELETE_ARRAY(ref);
			SAFE_DELETE_ARRAY(feature);
		}
	}
	void assign(const tfOutput &o) {
		nx = o.nx; ny = o.ny; nz = o.nz;
		ref = o.ref;
		feature = o.feature;
		addref();
	}
public:
	float *feature;			// 512ά����������

	tfOutput(int x, int y = 1, int z = 1)
	{
		memset(this, 0, sizeof(tfOutput));
		ref = new int(1);
		nx = x; ny = y; nz = z;
		feature = new float[nx * ny * nz];
	}
	~tfOutput()
	{
		destroy();
	}
	tfOutput(const tfOutput &o)
	{
		assign(o);
	}
	tfOutput operator = (const tfOutput &o)
	{
		if (this != &o)// ��ֹ�Լ���ֵ���Լ�
		{
			destroy();// ����������
			assign(o);// this��o����
		}
		return *this;
	}
	friend double distance(const tfOutput &v1, const tfOutput &v2) {
		float sum = 0;
		for (int i=0; i<v1.nx; ++i)
		{
			float t = v1.feature[i] - v2.feature[i];
			sum += t*t;
		}
		return sqrt(sum);
	}
};
