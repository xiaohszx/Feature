#pragma once

#include "config.h"

#ifdef _MSC_VER
	#ifdef BUILD_LIB
		#define FBC_API
	#else
		#if BUILD_DLL
			#define  SDK  __declspec(dllimport)
		#else
			#define  SDK  __declspec(dllexport)
		#endif
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/************************************************************************
	* @func Init
	* @brief ��ʼ����Դ
	* @param[in] python python����λ��
	* @param[in] mod python�ű���
	* @param[in] func python������
	* @return �Ƿ�ɹ�
	************************************************************************/
	SDK bool  Init(const char *python, const char *mod, const char *fun);

	/************************************************************************
	* @func Uninit
	* @brief �ͷ���Դ
	************************************************************************/
	SDK void  Uninit();

	/************************************************************************
	* @func features
	* @brief ��ȡͼ������
	* @param[in] m ԭʼͼ��
	* @param[in] x numpyά��x
	* @param[in] y numpyά��y
	* @param[in] z numpyά��z
	* @return numpy array
	************************************************************************/
	SDK tfOutput features(const cv::Mat &m, int x, int y, int z);


#ifdef __cplusplus
}
#endif
