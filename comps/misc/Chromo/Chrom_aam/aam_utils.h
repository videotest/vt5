#include <limits.h>

const int NC=256;
const double hist_smooth_level=0.5;

void CalcHist(color **src, int cx, int cy, int *hist);
//в hist должно быть NC свободных ячеек

void CalcHistMasked(color **src, byte **bin, byte bin_val, int cx, int cy, int *hist);
//в hist должно быть NC свободных ячеек

void CalcHistMaskedSqr(color **src, byte **bin, byte bin_val, int cx, int cy, int *hist, int x0, int y0, int x1, int y1);
//в hist должно быть NC свободных ячеек
//(x0,y0)-(x1,y1) - рабочий прямоугольник

int OrderValue(int *x, int n, int order, int xmin=INT_MIN, int xmax=INT_MAX);
//вернуть order'th значение из неупорядоченного массива
//если задать xmin, xmax - быстрее работает
