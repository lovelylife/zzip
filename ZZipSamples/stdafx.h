// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifdef _STATIC_LINK
#define _EXT_CLASS
#else
#define _EXT_CLASS _declspec(dllimport)
#endif

#ifndef USING_TestCode
#define USING_TestCode
#endif

#include "ZZipFile.h"

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
