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
	E (24, DESTROY)
	E (25, NOT_RUNNING)
	E (26, CREATE_WINDOW)
	E (27, NULL_HANDLE)
	E (28, NULL_WND)
	E (29, DEVICE_SWAP_CHAIN)
	E (30, NULL_SWAP_CHAIN)
	E (31, NULL_DEVICE)
	E (32, NULL_DEVICE_CONTEXT)
	E (33, PROCESS_DRAWING)
	E (34, DEVICE_SWAP_CHAIN_GET_BUFFER)
	E (35, CREATE_RENDER_TARGET)
	E (36, PRESENT)
	E (37, SWAP_PRESENT)
};

const wchar_t  APPLICATION_TITLE_W[] = L"DirectXTest";
const char     APPLICATION_TITLE_A[] = "DirectXTest";
const uint16_t MAX_STRING_LENGTH = 512;
const UINT     CONST_WINDOW_STYLE = WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION | WS_SYSMENU;

std::string& CreateStringOnFail (const char* text, ...);
void CleverMessageBox (const char* text, ...);

const UINT SCREEN_WIDTH  = GetSystemMetrics (SM_CXSCREEN);
const UINT SCREEN_HEIGHT = GetSystemMetrics (SM_CYSCREEN);


class ExceptionData_t;
extern ExceptionData_t* __EXPN__;

const DXGI_SAMPLE_DESC SAMPLE_DESC = { 4, 0 };

#undef E