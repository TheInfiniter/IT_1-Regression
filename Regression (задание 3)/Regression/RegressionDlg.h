
// RegressionDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CRegressionDlg
class CRegressionDlg : public CDialogEx
{
// Создание
public:
	CRegressionDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGRESSION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	//Картинка
	//Обычный график
	CWnd *SignalWnd;
	CDC *SignalDC;
	CRect SignalRect;

	CWnd *SigmaWnd;
	CDC *SigmaDC;
	CRect SigmaRect;

	CPen Axis; //кисть для рисования осей
	CPen Grid; //сетки
	CPen Line; //графика
	CPen Filter; //фильтра
	CPen Level; //уровня

	double freq1, freq2, freq3, freq4, freq5; //частоты
	int amp1, amp2, amp3, amp4, amp5; //амплитуды
	int arraysize; //количество отсчетов
	int amount; //количество синусов
	int raz_m; //порядок авторегрессии
	double sigma_max; //максимальный элемент матрицы Sigma
	int rang; //ранг матрицы Sigma
	double summ_square; //сумма квадратов отклонений
	double edge; //порог
	bool Start;

	int element; //количество элементов в сумме шума
	double signal_en; //энергия сигнала
	double noise_en; //энергия шума
	double summ; //сумма элементов шумового сигнала
	double alpha; //нормировочный коэффициент
	double noise_percent; //процент шума

	//массивы
	double *record; //массив исходного сигнала
	double *auto_reg; //массив авторегрессии
	double ampl_array[5]; // массив амплитуд
	double freq_array[5]; // массив частот
	double *AKP; //автокорелляционная последовательность
	double *ARM; //авторегрессионная матрица
	double *kazMatrix; //матрица для решения методом Качмаржа
	double *Error; //ошибка
	//U V sigma
	double *U;
	double *V;
	double *Sigma;
	double *SigmaP;
	double *neizv;
	double *Stolb;

	//коэффициенты для рисования
	double xmin_graph, xmax_graph, ymin_graph, ymax_graph, xtrans_graph, ytrans_graph; //график
	double xmin_sigma, xmax_sigma, ymin_sigma, ymax_sigma, xtrans_sigma, ytrans_sigma; //сигма

	int svd_hestenes(int m_m, int n_n, double * a, double * u, double * v, double * sigma);
	void kazf(double* a, double* b, double* x, int nn, int ny);

	afx_msg void OnBnClickedGraph();
};
