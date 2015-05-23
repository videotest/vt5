#include "stdafx.h"
#include "rsa.hpp"
#include <stdlib.h>

class prime_factory
{
  unsigned np;
  unsigned *pl;
  public:
  prime_factory();
  ~prime_factory();
  vlong find_prime( vlong & start );
};

// prime factory implementation

static int is_probable_prime( const vlong &p )
{
  // Test based on Fermats theorem a**(p-1) = 1 mod p for prime p
  // For 1000 bit numbers this can take quite a while
  const int rep = 4;
  const unsigned any[rep] = { 2,3,5,7 };
  for ( unsigned i=0; i<rep; i+=1 )
    if ( modexp( any[i], p-1, p ) != 1 )
      return 0;
  return 1;
}

prime_factory::prime_factory()
{
  np = 0;
  unsigned NP = 200;
  pl = new unsigned[NP];

  // Initialise pl
  unsigned SS = 8*NP; // Rough estimate to fill pl
  char * b = new char[SS+1]; // one extra to stop search
  for (unsigned i=0;i<=SS;i+=1) b[i] = 1;
  unsigned p = 2;
  while (1)
  {
    // skip composites
    while ( b[p] == 0 ) p += 1;
    if ( p == SS ) break;
    pl[np] = p;
    np += 1;
    if ( np == NP ) break;
    // cross off multiples
    unsigned c = p*2;
    while ( c < SS )
    {
      b[c] = 0;
      c += p;
    }
    p += 1;
  }
  delete [] b;
}

prime_factory::~prime_factory()
{
  delete [] pl;
}

vlong prime_factory::find_prime( vlong & start )
{
  unsigned SS = 1000; // should be enough unless we are unlucky
  char * b = new char[SS]; // bitset of candidate primes
  unsigned tested = 0;
  while (1)
  {
    unsigned i;
    for (i=0;i<SS;i+=1)
      b[i] = 1;
    for (i=0;i<np;i+=1)
    {
      unsigned p = pl[i];
      unsigned r = (start % p).get_unsigned(); // not as fast as it should be - could do with special routine
      if (r) r = p - r;
      // cross off multiples of p
      while ( r < SS )
      {
        b[r] = 0;
        r += p;
      }
    }
    // now test candidates
    for (i=0;i<SS;i+=1)
    {
      if ( b[i] )
      {
        tested += 1;
        if ( is_probable_prime(start) )
          return start;
      }
      start += 1;
    }
  }
  delete [] b;
}

static vlong from_str( const char * s )
{
  vlong x = 0;
  while (*s)
  {
    x = x * 256 + (unsigned char)*s;
    s += 1;
  }
  return x;
}

void private_key::create( const char * r1, const char * r2 )
{
  // Choose primes
  {
    prime_factory pf;
    p = pf.find_prime( from_str(r1) );
    q = pf.find_prime( from_str(r2) );
    if ( p > q ) { vlong tmp = p; p = q; q = tmp; }
  }
  // Calculate public key
  {
    m = p*q;
    e = 50001; // must be odd since p-1 and q-1 are even
    while ( gcd(p-1,e) != 1 || gcd(q-1,e) != 1 ) e += 2;
  }
}

vlong public_key::encrypt( const vlong& plain )
{
  return modexp( plain, e, m );
}

vlong private_key::decrypt( const vlong& cipher )
{
  // Calculate values for performing decryption
  // These could be cached, but the calculation is quite fast
  vlong d = modinv( e, (p-1)*(q-1) );
  vlong u = modinv( p, q );
  vlong dp = d % (p-1);
  vlong dq = d % (q-1);

  // Apply chinese remainder theorem
  vlong a = modexp( cipher % p, dp, p );
  vlong b = modexp( cipher % q, dq, q );
  if ( b < a ) b += q;
  return a + p * ( ((b-a)*u) % q );
}

/* ENC is the basic 1-character encoding function to make a char printing */
#define ENC(c) ((c) ? ((c) & 077) + ' ': '`')
/* single-character decode */
#define DEC(c)	(((c) - ' ') & 077)

int vlong_to_string(vlong v, char *sz_buf, int buf_size)
{ // возвращает количество байт в буфере (если sz_buf==0 - то требуемое)
	long n=v.get_data_size();
	const int usize = (sizeof(unsigned)*8 + 5) / 6; // число байт для записи одного unsigned
	if( !sz_buf ) return n*usize+1;
	if( buf_size < n*usize+1 ) return 0;

	for(int i=0; i<n; i++)
	{
		unsigned u = v.get_data(i);
		for(int j=0; j<usize; j++)
		{
			*(sz_buf++) = ENC(u & 63);
			u >>= 6;
		}
	}
	*sz_buf=0;
	return n*usize+1;
}

vlong string_to_vlong(char *sz_buf, int buf_size)
{
	vlong v=0;
	const int usize = (sizeof(unsigned)*8 + 5) / 6; // число байт для записи одного unsigned
	int i=0;
	while(*sz_buf)
	{
		unsigned u=0;
		for(int j=0; j<usize; j++)
		{
			if(buf_size==0) return v; // подстраховка - не читать за пределами строки
			if(*sz_buf==0) break;
			u |= DEC(*sz_buf) << (j*6);
			sz_buf++;
			buf_size--;
		}
		v.set_data(i,u);
		i++;
	}
	return v;
}

void fprintf_vlong(FILE *f, char *name, vlong v)
{
	int n = v.get_data_size();
	for(int i=0; i<n; i++)
	{
		fprintf(f, "%s . set_data ( %u , 0x%X );\n", name, i, v.get_data(i) );
	}
	fprintf(f, "\n");
}

void write_private_key( const char *filename, private_key key )
{
	FILE *f = fopen(filename, "w");
	fprintf_vlong(f, "key.m", key.m);
	fprintf_vlong(f, "key.e", key.e);
	fprintf_vlong(f, "key.p", key.p);
	fprintf_vlong(f, "key.q", key.q);
	fclose(f);
}

void write_public_key( const char *filename, public_key key )
{
	FILE *f = fopen(filename, "w");
	fprintf_vlong(f, "key.m", key.m);
	fprintf_vlong(f, "key.e", key.e);
	fclose(f);
}

private_key read_key( const char *filename )
{
	private_key key2;
	FILE *f = fopen(filename, "r");
	if(0==f) return key2;
	while(1)
	{
		unsigned i,x;
		char name;
		int res = fscanf(f, " key . %c . set_data ( %u , %X ) ;\n", &name, &i, &x );
		if(res<=0) break;
		switch(tolower(name))
		{
		case 'm': key2.m.set_data(i,x); break;
		case 'e': key2.e.set_data(i,x); break;
		case 'p': key2.p.set_data(i,x); break;
		case 'q': key2.q.set_data(i,x); break;
		}
	}
	fclose(f);

	return key2;
}
