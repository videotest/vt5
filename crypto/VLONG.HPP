#pragma once

class vlong // very long integer - can be used like long
{
public:
  // Standard arithmetic operators
  friend vlong operator +( const vlong& x, const vlong& y );
  friend vlong operator -( const vlong& x, const vlong& y );
  friend vlong operator *( const vlong& x, const vlong& y );
  friend vlong operator /( const vlong& x, const vlong& y );
  friend vlong operator %( const vlong& x, const vlong& y );
  vlong& operator +=( const vlong& x );
  vlong& operator -=( const vlong& x );

  // Standard comparison operators
  friend inline bool operator !=( const vlong& x, const vlong& y ){ return x.cf( y ) != 0; }
  friend inline bool operator ==( const vlong& x, const vlong& y ){ return x.cf( y ) == 0; }
  friend inline bool operator >=( const vlong& x, const vlong& y ){ return x.cf( y ) >= 0; }
  friend inline bool operator <=( const vlong& x, const vlong& y ){ return x.cf( y ) <= 0; }
  friend inline bool operator > ( const vlong& x, const vlong& y ){ return x.cf( y ) > 0; }
  friend inline bool operator < ( const vlong& x, const vlong& y ){ return x.cf( y ) < 0; }

  // Construction and conversion operations
  vlong ( unsigned x=0 );
  vlong ( const vlong& x );
  ~vlong();
  // operator unsigned ();
  unsigned get_unsigned();
  vlong& operator =(const vlong& x);

  unsigned	get_data_size(); // возвращает размер (чтобы перейти к байтам - умножить на sizeof(unsigned))
  unsigned*	get_data_buf(); // возвращает указатель на данные
  unsigned	get_data(unsigned i); // возвращает данные - i элемент
  void		set_data(unsigned i, unsigned x);

  class vlong_value * value;
  int negative;
private:
  int cf( const vlong x ) const;
  void docopy();
  friend class monty;
};

vlong modexp( const vlong & x, const vlong & e, const vlong & m ); // m must be odd
vlong gcd( const vlong &X, const vlong &Y ); // greatest common denominator
vlong modinv( const vlong &a, const vlong &m ); // modular inverse
