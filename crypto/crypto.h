#pragma once
#include "md5.h"
#include "RSA.HPP"

#pragma comment( lib, "crypto.lib" )

static inline public_key get_public_key()
{
	public_key key;
	#include "\vt5\crypto\public_key.h"
	return key;
}
