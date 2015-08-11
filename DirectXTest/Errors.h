#pragma once

#define E(num, name) ERROR_##name = num,

enum ERRORS
{
	E (14, DTOR)
	E (15, CTOR)
	E (16, NULL_THIS)
	E (17, BAD_ALLOC)
	E (18, UNKNOWN)
	E (19, REGISTER_APP)
	E (20, REGISTER_CLASS)
	E (21, FATAL_NO_EXCEPTION_DATA)
	E (22, DESTROYED)
	E (23, WIN_CALLBACK)
};