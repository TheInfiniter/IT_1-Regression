
// RegressionDlg.cpp: файл реализации
//
#define _USE_MATH_DEFINES
#include "stdafx.h"
#include <math.h>
#include "Regression.h"
#include "RegressionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//макрос для пересчёта координат
#define DOTS_GRAPH(x, y) (xtrans_graph * ((x) - xmin_graph)), (ytrans_graph * ((y) - ymax_graph)) //график
#define DOTS_SIGMA(x, y) (xtrans_sigma * ((x) - xmin_sigma)), (ytrans_sigma * ((y) - ymax_sigma)) //сигма

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CRegressionDlg



CRegressionDlg::CRegressionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGRESSION_DIALOG, pParent)
	, freq1(0.3)
	, freq2(0.1)
	, freq3(0.2)
	, freq4(0.5)
	, freq5(0.4)
	, amp1(1)
	, amp2(2)
	, amp3(3)
	, amp4(4)
	, amp5(5)
	, amount(5)
	, arraysize(256)
	, raz_m(10)
	, summ_square(0)
	, edge(30)
	, element(15)
	, noise_percent(0)
	, Start(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegressionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FREQ1, freq1);
	DDX_Text(pDX, IDC_FREQ2, freq2);
	DDX_Text(pDX, IDC_FREQ3, freq3);
	DDX_Text(pDX, IDC_FREQ4, freq4);
	DDX_Text(pDX, IDC_FREQ5, freq5);
	DDX_Text(pDX, IDC_AMP1, amp1);
	DDX_Text(pDX, IDC_AMP2, amp2);
	DDX_Text(pDX, IDC_AMP3, amp3);
	DDX_Text(pDX, IDC_AMP4, amp4);
	DDX_Text(pDX, IDC_AMP5, amp5);
	DDX_Text(pDX, IDC_ARRAYSIZE, arraysize);
	DDX_Text(pDX, IDC_AMOUNT, amount);
	DDX_Text(pDX, IDC_RAZ_M, raz_m);
	DDX_Text(pDX, IDC_SUMM, summ_square);
	DDX_Text(pDX, IDC_EDGE, edge);
	DDX_Text(pDX, IDC_RANG, rang);
	DDX_Text(pDX, IDC_NOISE, noise_percent);
}

BEGIN_MESSAGE_MAP(CRegressionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GRAPH, &CRegressionDlg::OnBnClickedGraph)
END_MESSAGE_MAP()


// Обработчики сообщений CRegressionDlg

BOOL CRegressionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	//График
	SignalWnd = GetDlgItem(IDC_DRAW);
	SignalDC = SignalWnd->GetDC();
	SignalWnd->GetClientRect(&SignalRect);
	//сигма
	SigmaWnd = GetDlgItem(IDC_SIGMA);
	SigmaDC = SigmaWnd->GetDC();
	SigmaWnd->GetClientRect(&SigmaRect);

	//График
	xmin_graph = -1.0;
	xmax_graph = arraysize;
	ymin_graph = -1.0;
	ymax_graph = 1.0;
	xtrans_graph = ((double)(SignalRect.Width()) / (xmax_graph - xmin_graph));
	ytrans_graph = -((double)(SignalRect.Height()) / (ymax_graph - ymin_graph));
	//сигма
	xmin_sigma = -1.0;
	xmax_sigma = raz_m;
	ymin_sigma = -1.0;
	ymax_sigma = 1.0;
	xtrans_sigma = ((double)(SigmaRect.Width()) / (xmax_sigma - xmin_sigma));
	ytrans_sigma = -((double)(SigmaRect.Height()) / (ymax_sigma - ymin_sigma));

	Axis.CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); //оси: прямая линия, толщина 2 пикселя, черный цвет
	Grid.CreatePen(PS_DOT, 1, RGB(0, 0, 0)); //сетка: пунктирная линия, 1 пиксель, черный
	Line.CreatePen(PS_SOLID, 2, RGB(0, 0, 255)); //график: прямая линия, 1 пиксель, синий
	Filter.CreatePen(PS_SOLID, 2, RGB(255, 0, 0)); //фильтрованная ошибка: прямая линия, 1 пиксель, красный
	Level.CreatePen(PS_SOLID, 2, RGB(255, 0, 255)); //уровень: прямая линия, 1 пиксель, фиолетовый

	//выделяем место под массивы
	record = new double[arraysize];
	auto_reg = new double[arraysize];
	Error = new double[arraysize];
	AKP = new double[raz_m];
	ARM = new double[raz_m * raz_m];
	U = new double[raz_m * raz_m];
	V = new double[raz_m * raz_m];
	Sigma = new double[raz_m];
	SigmaP = new double[raz_m];

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CRegressionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CRegressionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		ampl_array[0] = amp1;
		ampl_array[1] = amp2;
		ampl_array[2] = amp3;
		ampl_array[3] = amp4;
		ampl_array[4] = amp5;

		freq_array[0] = freq1;
		freq_array[1] = freq2;
		freq_array[2] = freq3;
		freq_array[3] = freq4;
		freq_array[4] = freq5;

		SignalDC->FillSolidRect(SignalRect, RGB(242, 225, 241));
		SigmaDC->FillSolidRect(SigmaRect, RGB(242, 225, 241));

		xtrans_graph = ((double)(SignalRect.Width()) / (xmax_graph - xmin_graph));
		ytrans_graph = -((double)(SignalRect.Height()) / (ymax_graph - ymin_graph));

		xtrans_sigma = ((double)(SigmaRect.Width()) / (xmax_sigma - xmin_sigma));
		ytrans_sigma = -((double)(SigmaRect.Height()) / (ymax_sigma - ymin_sigma));

		//надписи
		//график
		SignalDC->SelectObject(&Axis); //выбор кисти для осей
		for (int i = 0; i <= xmax_graph; i = i + (arraysize / 8)) //засечки
		{
			SignalDC->MoveTo(DOTS_GRAPH(i, 0.02 * (ymax_graph - ymin_graph)));
			SignalDC->LineTo(DOTS_GRAPH(i, -0.02 * (ymax_graph - ymin_graph)));
		}
		for (double i = 0; i <= xmax_graph; i = i + (arraysize / 8))
		{
			CString str;
			str.Format(_T("%.1f"), i); //вытаскивание числа из i!
			SignalDC->TextOutW(DOTS_GRAPH(i - 0.02 * (ymax_graph - ymin_graph), 0 - 0.02 * (ymax_graph - ymin_graph)), str);
		}
		CString str;
		str.Format(_T("%.1f"), ymin_graph); //вытаскивание числа из i!
		SignalDC->TextOutW(DOTS_GRAPH(0.02 * (xmax_graph - xmin_graph), ymin_graph + 0.1 * (ymax_graph - ymin_graph)), str);
		str.Format(_T("%.1f"), ymax_graph); //вытаскивание числа из i!
		SignalDC->TextOutW(DOTS_GRAPH(0.02 * (xmax_graph - xmin_graph), ymax_graph), str);

		//сигма
		SigmaDC->SelectObject(&Axis); //выбор кисти для осей
		for (int i = 0; i <= xmax_sigma; i = i + (raz_m / 8)) //засечки
		{
			SigmaDC->MoveTo(DOTS_SIGMA(i, 0.02 * (ymax_sigma - ymin_sigma)));
			SigmaDC->LineTo(DOTS_SIGMA(i, -0.02 * (ymax_sigma - ymin_sigma)));
		}
		for (double i = 0; i <= xmax_sigma; i = i + (raz_m / 8))
		{
			CString str;
			str.Format(_T("%.1f"), i); //вытаскивание числа из i!
			SigmaDC->TextOutW(DOTS_SIGMA(i - 0.02 * (ymax_sigma - ymin_sigma), 0 - 0.02 * (ymax_sigma - ymin_sigma)), str);
		}
		//CString str;
		str.Format(_T("%.1f"), ymin_sigma); //вытаскивание числа из i!
		SigmaDC->TextOutW(DOTS_SIGMA(0.02 * (xmax_sigma - xmin_sigma), ymin_sigma + 0.1 * (ymax_sigma - ymin_sigma)), str);
		str.Format(_T("%.1f"), ymax_sigma); //вытаскивание числа из i!
		SigmaDC->TextOutW(DOTS_SIGMA(0.02 * (xmax_sigma - xmin_sigma), ymax_sigma), str);

		//прорисовка осей
		//График
		//ось Х
		SignalDC->MoveTo(DOTS_GRAPH(xmin_graph, 0));
		SignalDC->LineTo(DOTS_GRAPH(xmax_graph, 0));
		//ось Y
		SignalDC->MoveTo(DOTS_GRAPH(0, ymin_graph));
		SignalDC->LineTo(DOTS_GRAPH(0, ymax_graph));

		//ось Х
		SigmaDC->MoveTo(DOTS_SIGMA(xmin_sigma, 0));
		SigmaDC->LineTo(DOTS_SIGMA(xmax_sigma, 0));
		//ось Y
		SigmaDC->MoveTo(DOTS_SIGMA(0, ymin_sigma));
		SigmaDC->LineTo(DOTS_SIGMA(0, ymax_sigma));

		if (Start == true)
		{
			double Pi = 3.1415926535897932;


			for (int i = 0; i < arraysize; i++)
			{
				record[i] = 0;
				auto_reg[i] = 0;
				Error[i] = 0;
			}
			for (int i = 0; i < raz_m; i++)
			{
				AKP[i] = 0;
				Sigma[i] = 0;
				SigmaP[i] = 0;
			}
			for (int i = 0; i < raz_m * raz_m; i++)
			{
				ARM[i] = 0;
				U[i] = 0;
				V[i] = 0;
			}

			//заполняем массив сигнала
			for (int i = 0; i < amount; i++)
			{
				for (int j = 0; j < arraysize; j++)
				{
					record[j] += ampl_array[i] * sin(j * freq_array[i]);
				}
			}
			if (noise_percent != 0)
			{
				//начинаем добавлять шум
				double *gauss_noise = new double[element];
				double *signal_array = new double[arraysize];

				for (int i = 0; i < element; i++)
				{
					gauss_noise[i] = 0;
				}

				for (int i = 0; i < arraysize; i++)
				{
					signal_array[i] = 0;
					for (int j = 0; j < element; j++)
					{
						gauss_noise[j] = -1 + 2 * rand()*1. / RAND_MAX;
						signal_array[i] += gauss_noise[j];
					}
					signal_array[i] /= element;
				}

				for (int i = 0; i < arraysize; i++)
				{
					signal_en += (record[i] * record[i]);
					summ += (signal_array[i] * signal_array[i]);
				}

				noise_en = noise_percent * signal_en / 100; //энергия шума
				alpha = sqrt(noise_en / summ);

				//прибавляем шум к основному сигналу
				for (int i = 0; i < arraysize; i++)
				{
					record[i] = record[i] + alpha * signal_array[i];
				}

				delete[] gauss_noise;
				delete[] signal_array;
			}

			auto AKP_fill = [](double *S, double M, double N) {
				double ARR = 0;
				int step;
				for (int i = 0; i < N - M - 1; i++)
				{
					step = i + M;
					ARR += 1 / (N - M) * S[i] * S[step];
				}
				return ARR;
			};

			for (int i = 0; i < raz_m; i++)
			{
				AKP[i] = AKP_fill(record, i, arraysize);
			}

			double *buff = new double[raz_m];
			for (int i = 1; i < raz_m; i++)
			{
				buff[i] = AKP[raz_m - i];
			}
			buff[0] = AKP[0];
			int NM = raz_m * raz_m;
			double bufer_var(0);
			int counter(0), step(0);

			while (counter < NM)
			{
				for (int i = 0; i < raz_m; i++)
				{
					ARM[step] = buff[i];
					step++;
				}
				bufer_var = buff[raz_m - 1];
				for (int j = raz_m - 1; j > 0; j--)
				{
					buff[j] = buff[j - 1];
				}
				buff[0] = bufer_var;
				counter += raz_m;
			}

			svd_hestenes(raz_m, raz_m, ARM, U, V, Sigma);

			for (int i = 0; i < raz_m; i++)
			{
				SigmaP[i] = Sigma[i];
			}

			double porog = edge;
			porog /= 100;
			sigma_max = Sigma[0];

			rang = 0;
			for (int i = 0; i < raz_m; i++)
			{
				if (Sigma[i] > porog * sigma_max)
				{
					rang++;
				}
			}

			int stop = rang + 1;
			neizv = new double[stop];
			Stolb = new double[stop];
			for (int i = 0; i < stop; i++)
			{
				neizv[i] = 0;
				Stolb[i] = 0;
			}
			neizv[0] = 1;
			Stolb[0] = porog * ARM[0];

			kazMatrix = new double[stop * stop];
			int st(0), count(0);
			for (int j = 0; j < stop; j++)
			{
				for (int i = 0; i < stop; i++)
				{
					kazMatrix[i + count] = ARM[i + st];
				}
				st += raz_m;
				count += stop;
			}

			kazf(kazMatrix, Stolb, neizv, stop, stop);

			for (int i = 0; i < rang; i++)
			{
				auto_reg[i] = record[i];
			}
			for (int j = rang; j < arraysize; j++)
			{
				for (int i = 1; i < stop; i++)
				{
					auto_reg[j] -= neizv[i] * record[j - i];
				}
			}

			for (int j = 1; j < stop; j++)
			{
				for (int i = rang; i < arraysize; i++)
				{
					auto_reg[i] -= neizv[j] * record[i - j];
				}
			}

			summ_square = 0;
			for (int i = 0; i < arraysize; i++)
			{
				Error[i] = record[i] - auto_reg[i];
				summ_square += Error[i];
			}
			//работаем с масштабом окна ошибки, чтобы графики уместились в окне
			xmax_graph = arraysize;
			xmax_sigma = raz_m;
			ymin_graph = 0;
			ymax_graph = 0;
			ymin_sigma = -1;
			ymax_sigma = 0;
			for (int i = 0; i < arraysize; i++)
			{
				if (record[i] > ymax_graph) ymax_graph = record[i];
				if (record[i] < ymin_graph) ymin_graph = record[i];
				if (auto_reg[i] > ymax_graph) ymax_graph = auto_reg[i];
				if (auto_reg[i] < ymin_graph) ymin_graph = auto_reg[i];
			}
			for (int i = 0; i < raz_m; i++)
			{
				if (SigmaP[i] > ymax_sigma) ymax_sigma = SigmaP[i];
				if (SigmaP[i] < ymin_sigma) ymin_sigma = SigmaP[i];
			}
			SignalDC->FillSolidRect(SignalRect, RGB(242, 225, 241));
			SigmaDC->FillSolidRect(SignalRect, RGB(242, 225, 241));
			xtrans_graph = ((double)(SignalRect.Width()) / (xmax_graph - xmin_graph));
			ytrans_graph = -((double)(SignalRect.Height()) / (ymax_graph - ymin_graph));
			xtrans_sigma = ((double)(SigmaRect.Width()) / (xmax_sigma - xmin_sigma));
			ytrans_sigma = -((double)(SigmaRect.Height()) / (ymax_sigma - ymin_sigma));

			//надписи
			//график
			SignalDC->SelectObject(&Axis); //выбор кисти для осей
			for (int i = 0; i <= xmax_graph; i = i + (arraysize / 8)) //засечки
			{
				SignalDC->MoveTo(DOTS_GRAPH(i, 0.02 * (ymax_graph - ymin_graph)));
				SignalDC->LineTo(DOTS_GRAPH(i, -0.02 * (ymax_graph - ymin_graph)));
			}
			for (double i = 0; i <= xmax_graph; i = i + (arraysize / 8))
			{
				CString str;
				str.Format(_T("%.1f"), i); //вытаскивание числа из i!
				SignalDC->TextOutW(DOTS_GRAPH(i - 0.02 * (ymax_graph - ymin_graph), 0 - 0.02 * (ymax_graph - ymin_graph)), str);
			}
			CString str;
			str.Format(_T("%.1f"), ymin_graph); //вытаскивание числа из i!
			SignalDC->TextOutW(DOTS_GRAPH(0.02 * (xmax_graph - xmin_graph), ymin_graph + 0.1 * (ymax_graph - ymin_graph)), str);
			str.Format(_T("%.1f"), ymax_graph); //вытаскивание числа из i!
			SignalDC->TextOutW(DOTS_GRAPH(0.02 * (xmax_graph - xmin_graph), ymax_graph), str);

			//сигма
			SigmaDC->SelectObject(&Axis); //выбор кисти для осей
			for (int i = 0; i <= xmax_sigma; i = i + (raz_m / 8)) //засечки
			{
				SigmaDC->MoveTo(DOTS_SIGMA(i, 0.02 * (ymax_sigma - ymin_sigma)));
				SigmaDC->LineTo(DOTS_SIGMA(i, -0.02 * (ymax_sigma - ymin_sigma)));
			}
			for (double i = 0; i <= xmax_sigma; i = i + (raz_m / 8))
			{
				CString str;
				str.Format(_T("%.1f"), i); //вытаскивание числа из i!
				SigmaDC->TextOutW(DOTS_SIGMA(i - 0.02 * (ymax_sigma - ymin_sigma), 0 - 0.02 * (ymax_sigma - ymin_sigma)), str);
			}
			//CString str;
			str.Format(_T("%.1f"), ymin_sigma); //вытаскивание числа из i!
			SigmaDC->TextOutW(DOTS_SIGMA(0.02 * (xmax_sigma - xmin_sigma), ymin_sigma + 0.1 * (ymax_sigma - ymin_sigma)), str);
			str.Format(_T("%.1f"), ymax_sigma); //вытаскивание числа из i!
			SigmaDC->TextOutW(DOTS_SIGMA(0.02 * (xmax_sigma - xmin_sigma), ymax_sigma), str);

			//прорисовка осей
			//График
			//ось Х
			SignalDC->MoveTo(DOTS_GRAPH(xmin_graph, 0));
			SignalDC->LineTo(DOTS_GRAPH(xmax_graph, 0));
			//ось Y
			SignalDC->MoveTo(DOTS_GRAPH(0, ymin_graph));
			SignalDC->LineTo(DOTS_GRAPH(0, ymax_graph));

			//ось Х
			SigmaDC->MoveTo(DOTS_SIGMA(xmin_sigma, 0));
			SigmaDC->LineTo(DOTS_SIGMA(xmax_sigma, 0));
			//ось Y
			SigmaDC->MoveTo(DOTS_SIGMA(0, ymin_sigma));
			SigmaDC->LineTo(DOTS_SIGMA(0, ymax_sigma));

			SigmaDC->SelectObject(&Level);
			SigmaDC->MoveTo(DOTS_SIGMA(xmin_sigma, sigma_max * porog));
			SigmaDC->LineTo(DOTS_SIGMA(xmax_sigma, sigma_max * porog));

			//рисуем графики
			SignalDC->SelectObject(&Line);
			SignalDC->MoveTo(DOTS_GRAPH(0, record[0]));
			for (int i = 0; i < arraysize; i++)
			{
				SignalDC->LineTo(DOTS_GRAPH(i, record[i]));
			}
			SignalDC->SelectObject(&Filter);
			SignalDC->MoveTo(DOTS_GRAPH(0, auto_reg[0]));
			for (int i = 0; i < arraysize; i++)
			{
				SignalDC->LineTo(DOTS_GRAPH(i, auto_reg[i]));
			}

			SigmaDC->SelectObject(&Filter);
			SigmaDC->MoveTo(DOTS_SIGMA(0, SigmaP[0]));
			for (int i = 0; i < raz_m; i++)
			{
				SigmaDC->LineTo(DOTS_SIGMA(i, SigmaP[i]));
			}

			delete[] record;
			delete[] auto_reg;
			delete[] Error;
			delete[] AKP;
			delete[] ARM;
			delete[] U;
			delete[] V;
			delete[] Sigma;
			delete[] SigmaP;
			delete[] buff;
		}
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CRegressionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRegressionDlg::OnBnClickedGraph()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData(TRUE);
	Start = true;
	OnPaint();	
	UpdateData(FALSE);
}

int CRegressionDlg::svd_hestenes(int m_m, int n_n, double * a, double * u, double * v, double * sigma)
{
	double thr = 1.E-4f, nul = 1.E-16f;
	int n, m, i, j, l, k, lort, iter, in, ll, kk;
	double alfa, betta, hamma, eta, t, cos0, sin0, buf, s;
	n = n_n;
	m = m_m;
	for (i = 0; i < n; i++)
	{
		in = i * n;
		for (j = 0; j < n; j++)
			if (i == j) v[in + j] = 1.;
			else v[in + j] = 0.;
	}
	for (i = 0; i < m; i++)
	{
		in = i * n;
		for (j = 0; j < n; j++)
		{
			u[in + j] = a[in + j];
		}
	}

	iter = 0;
	while (1)
	{
		lort = 0;
		iter++;
		for (l = 0; l < n - 1; l++)
			for (k = l + 1; k < n; k++)
			{
				alfa = 0.; betta = 0.; hamma = 0.;
				for (i = 0; i < m; i++)
				{
					in = i * n;
					ll = in + l;
					kk = in + k;
					alfa += u[ll] * u[ll];
					betta += u[kk] * u[kk];
					hamma += u[ll] * u[kk];
				}

				if (sqrt(alfa*betta) < nul)	continue;
				if (fabs(hamma) / sqrt(alfa*betta) < thr) continue;

				lort = 1;
				eta = (betta - alfa) / (2.f*hamma);
				t = (double)((eta / fabs(eta)) / (fabs(eta) + sqrt(1. + eta * eta)));
				cos0 = (double)(1. / sqrt(1. + t * t));
				sin0 = t * cos0;

				for (i = 0; i < m; i++)
				{
					in = i * n;
					buf = u[in + l] * cos0 - u[in + k] * sin0;
					u[in + k] = u[in + l] * sin0 + u[in + k] * cos0;
					u[in + l] = buf;

					if (i >= n) continue;
					buf = v[in + l] * cos0 - v[in + k] * sin0;
					v[in + k] = v[in + l] * sin0 + v[in + k] * cos0;
					v[in + l] = buf;
				}
			}

		if (!lort) break;
	}

	for (i = 0; i < n; i++)
	{
		s = 0.;
		for (j = 0; j < m; j++)	s += u[j*n + i] * u[j*n + i];
		s = (double)sqrt(s);
		sigma[i] = s;
		if (s < nul)	continue;
		for (j = 0; j < m; j++)	u[j*n + i] /= s;
	}
	//======= Sortirovka ==============
	for (i = 0; i < n - 1; i++)
		for (j = i; j < n; j++)
			if (sigma[i] < sigma[j])
			{
				s = sigma[i]; sigma[i] = sigma[j]; sigma[j] = s;
				for (k = 0; k < m; k++)
				{
					s = u[i + k * n]; u[i + k * n] = u[j + k * n]; u[j + k * n] = s;
				}
				for (k = 0; k < n; k++)
				{
					s = v[i + k * n]; v[i + k * n] = v[j + k * n]; v[j + k * n] = s;
				}
			}

	return iter;
}

//метод Качмаржа
void CRegressionDlg::kazf(double* a, double* b, double* x, int nn, int ny)
/* матрица А, столбец свободных членов, массив неизвестных,
nn - количество неизвестных;  ny - количество уравнений*/
{

	double eps = 1.e-6f;
	//float s;
	int i, j, k;
	double s1, s2, fa1, t;
	double *x1;

	x1 = new double[nn];

	x[0] = 0.5f;
	for (i = 1; i < nn; i++)  x[i] = 0.f;

	s1 = s2 = 1.f;
	while (s1 > eps*s2)
	{
		for (i = 0; i < nn; i++) x1[i] = x[i];

		for (i = 0; i < ny; i++)
		{
			s1 = 0.0;
			s2 = 0.0;
			for (j = 0; j < nn; j++)
			{
				fa1 = a[i*nn + j];
				s1 += fa1 * x[j];
				s2 += fa1 * fa1;
			}
			t = (b[i] - s1) / s2;
			for (k = 0; k < nn; k++)    x[k] += a[i*nn + k] * t;
		}

		x[0] = 1.0; //первый элемент массива неизвестных всегда равен единице
		s1 = 0.0;
		s2 = 0.0;
		for (i = 0; i < nn; i++)
		{
			s1 += (x[i] - x1[i]) * (x[i] - x1[i]);
			s2 += x[i] * x[i];
		}
		s1 = (double)sqrt(s1);
		s2 = (double)sqrt(s2);
	}
	delete[] x1;
}
