/************************************************************************
* C++ ����python�ű�����numpy array��ͨ�÷���
* 2019-10-24 
* Ԭ����
************************************************************************/

#include "python.h"
#include <map>
#include <string>
#include "config.h"

#pragma once

// Py_DECREF ���³����������ʱ����
#define My_DECREF(p) 

// ���� numpy �е�ͷ�ļ�arrayobject.h
#include "..\Lib\site-packages\numpy\core\include\numpy\arrayobject.h"

/************************************************************************
* @class pyCaller
* @brief python������: �����ڶ�ͼƬ����Ƶ����ʶ��
* @author Ԭ����, 2018-4-11
************************************************************************/
class pyCaller
{
private:
	static wchar_t pyHome[_MAX_PATH];			// python·��
	PyObject* pModule;							// pythonģ��
	std::map<std::string, PyObject*> pFunMap;	// �����б�
	bool bMultiThread;							// ���߳�

	// ��ʼ�� numpy ִ�л�������Ҫ�ǵ����
	// python2.7��void�������ͣ�python3.0������int��������
	inline int init_numpy()
	{
		import_array();
		return 1;
	}

	// ����python���
	void ParseResult(PyObject *pRetVal, tfOutput &tf);

public:
	// ����python��װĿ¼
	static bool SetPythonHome(const char *py)
	{
		char pyExe[_MAX_PATH] = { 0 };
		strcat_s(pyExe, py);
		strcat_s(pyExe, "\\python.exe");
		if (-1 != _access(pyExe, 0))
		{
			size_t s;
			mbstowcs_s(&s, pyHome, py, strlen(py));
			OUTPUT("======> SetPythonHome: %s\n", py);
			return true;
		}
		else
		{
			OUTPUT("======> SetPythonHome: \"%s\" don't include python.exe.\n", py);
			return false;
		}
	}

	/**
	* @brief ����һ��pyCaller����
	*/
	pyCaller(bool multi_thread = true)
	{
		pModule = NULL;
		bMultiThread = multi_thread;
	}

	/**
	* @brief ��ʼ��pyCaller���󣬽���py�ű�������Ϊ�������
	*/
	bool Init(const char * module_name)
	{
		if (pyHome[0] && NULL == pModule)
		{
			clock_t t = clock();
			Py_SetPythonHome(pyHome);
			Py_Initialize();
			if (0 == Py_IsInitialized())
			{
				OUTPUT("Py_IsInitialized = 0.\n");
				return false;
			}
			if (bMultiThread)
				PyEval_InitThreads(); // ���߳�֧��
			if (NUMPY_IMPORT_ARRAY_RETVAL == init_numpy())
			{
				OUTPUT("init_numpy failed.\n");
				return false;
			}
			PyObject *py = PyImport_ImportModule(module_name);
			if (bMultiThread && PyEval_ThreadsInitialized())
				PyEval_SaveThread();
			if (NULL == py)
				OUTPUT("PyImport_ImportModule[%s] failed.\n", module_name);
			t = clock() - t;
			char szOut[128];
			sprintf_s(szOut, "PyImport_ImportModule using %d ms.\n", t);
			printf(szOut);
			pModule = py;
		}
		else
		{
			OUTPUT("Py_SetPythonHome is not called.\n");
		}
		return pModule;
	}

	/**
	* @brief ����ʼ��python�������ͷ�python������ڴ�
	*/
	~pyCaller()
	{
		if (pModule)
			Py_DECREF(pModule);
		for (std::map<std::string, PyObject*>::iterator p = pFunMap.begin(); 
			p != pFunMap.end(); ++p)
			if (p->second) Py_DECREF(p->second);
		if (pyHome[0])
		{
			if (bMultiThread && !PyGILState_Check())
				PyGILState_Ensure();
			Py_Finalize();
		}
	}

	// �Ƿ������ָ��ģ��
	bool IsModuleLoaded() const { return pModule; }

	/**
	* @brief ʹ��ǰ����ָ�����Ƶĺ���
	*/
	bool ActivateFunc(const char * func_name)
	{
		bool bFind = false;
		std::string fun(func_name);
		for (std::map<std::string, PyObject*>::iterator p = pFunMap.begin(); 
			p != pFunMap.end(); ++p)
		{
			if (p->first == fun)
			{
				bFind = true;
				break;
			}
		}
		if (bFind)
			return true;
		PyObject *pFunc = NULL;

		PyGILState_STATE gstate;
		int nHold = bMultiThread ? PyGILState_Check() : TRUE;
		if (!nHold) gstate = PyGILState_Ensure();
		Py_BEGIN_ALLOW_THREADS;
		Py_BLOCK_THREADS;

		pFunc =  pModule ? PyObject_GetAttrString(pModule, func_name) : 0;

		Py_UNBLOCK_THREADS;
		Py_END_ALLOW_THREADS;
		if (!nHold) PyGILState_Release(gstate);

		pFunMap.insert(std::make_pair(func_name, pFunc));

		return pFunc;
	}

	/**
	* @brief ����python�ű��е�ָ������
	* @param func_name ��������
	* @param arg ��������
	* @param tf ���numpy array�Ľṹ��
	*/
	void CallFunction(const char * func_name, PyObject *arg, tfOutput &tf)
	{
#if STATIC_DETECTING
		static PyObject *pFunc = pFunMap[func_name];
#else 
		PyObject *pFunc = pFunMap[func_name];
#endif
		if (pFunc)
		{
			PyGILState_STATE gstate;
			int nHold = bMultiThread ? PyGILState_Check() : TRUE;
			if (!nHold)gstate = PyGILState_Ensure();
			Py_BEGIN_ALLOW_THREADS;
			Py_BLOCK_THREADS;

 			PyObject* pRetVal = PyEval_CallObject(pFunc, arg);
			if (pRetVal)
				ParseResult(pRetVal, tf);

			Py_UNBLOCK_THREADS;
			Py_END_ALLOW_THREADS;
			if (!nHold)PyGILState_Release(gstate);
		}
	}
};
