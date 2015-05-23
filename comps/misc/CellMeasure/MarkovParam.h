#pragma once

const int nEqualColors=16; // количество уровней после эквализации
// (для расчета матрицы совместной встречаемости)

class CMarkovParamCalculator
{
public:
	CMarkovParamCalculator(void);
	~CMarkovParamCalculator(void);
	void CalcLevels(color **ppc, byte **ppm, int cx, int cy, int phase=0x7F80); // Рассчитать уровни для equalize
	void CalcMatrix(color **ppc, byte **ppm, int cx, int cy, int phase=0x7F80); // Расчет матрицы смежности
	// параметр phase: младший байт - номер требуемой фазы
	// старший байт - сколько фаз, начиная c указанной, учитывать, минус 1
	// т.е. подходящими считаются фазы от low до low+high
	// если phase<256 - то это просто номер фазы
	// 0x7F80 - все фазы, принадлежащие объекту (>=128)
	void CalcParams(); // посчитать параметры по уже готовой матрице

	// Рабочие данные
	int m_Hist[256]; // гистограмма - для эквалайза
	int m_LevelNum[256]; // номера уровней для 256 цветов
	int m_Matrix[nEqualColors*nEqualColors]; // собственно матрица

	int m_nLevelsPhases;

	// Параметры
	double m_fInertia, m_fEnergy, m_fEntropy,
		m_fCorrelation, m_fHomogenity;
};
