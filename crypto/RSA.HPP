// RSA public key encryption
#pragma once

#include "vlong.hpp"
#include "stdio.h"

class public_key
{
  public:
  vlong m,e;
  vlong encrypt( const vlong& plain ); // Requires 0 <= plain < m
};

class private_key : public public_key
{
  public:
  vlong p,q;
  vlong decrypt( const vlong& cipher );

  void create( const char * r1, const char * r2 );
  // r1 and r2 should be null terminated random strings
  // each of length around 35 characters (for a 500 bit modulus)
};

void fprintf_vlong(FILE *f, char *name, vlong v); // записать vlong в *.h-файл - как процедуру чтения
int vlong_to_string(vlong v, char *sz_buf, int buf_size);  // преобразование в строку; возвращает количество байт в буфере (если sz_buf==0 - то требуемое)
vlong string_to_vlong(char *sz_buf, int buf_size=512); // получить vlong из строки; строка - не более buf_size байт

void write_public_key( const char *filename, public_key key ); // запись ключа в *.h
void write_private_key( const char *filename, private_key key );
private_key read_key( const char *filename ); // чтение ключа из *.h
