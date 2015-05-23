#pragma once

void DilateColor(BYTE **sel, int cx, int cy, int mask, byte cFore, byte cForeNew, byte cBack);
int DeleteSmallSegments(byte **buf, int cx, int cy, int del_size=10, int fill_color=2);
