#pragma once

#include "includes.h"

const wchar_t APPLICATION_TITLE_W[] = L"DirectXTest";
const char    APPLICATION_TITLE_A[] =  "DirectXTest";
const uint16_t MAX_STRING_LENGTH = 512;

std::string& CreateStringOnFail (const char* text, ...);
void CleverMessageBox (const char* text, ...);

#include "Errors.h"

class ExceptionData_t;
extern ExceptionData_t* __EXPN__;

#include "Exception.h"

#include "Window.h"
