
// Regression.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CRegressionApp:
// Сведения о реализации этого класса: Regression.cpp
//

class CRegressionApp : public CWinApp
{
public:
	CRegressionApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CRegressionApp theApp;
