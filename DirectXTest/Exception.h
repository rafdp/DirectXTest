#pragma once

#include <exception>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <conio.h>
#include <new>


class NonCopiable_t
{
	NonCopiable_t (const NonCopiable_t&);
	void operator = (const NonCopiable_t&);
	public:
	NonCopiable_t () {}
};

#define DISABLE_CLASS_COPY(type) \
type (const type&); \
type& operator = (const type&);


#define E_NAT(message, code) (message), (code), __LINE__, __FILE__
#define E_CONS(message, code, pt) (message), (code), (pt), __LINE__, __FILE__

#define NAT_EXCEPTION(data, message, code) \
{ \
    ExceptionHandler* e_nat = new (data) ExceptionHandler (E_NAT ((message), (code))); \
    throw *e_nat; \
}

#define CONS_EXCEPTION(data, message, code, old) \
{ \
    ExceptionHandler* ec = new (data) ExceptionHandler (E_CONS ((message), (code), (old).pt_)); \
    throw *ec; \
}

#define DETAILED_CONSECUTIVE_CATCH(message, code, expn) \
catch (ExceptionHandler& catchedException00172456) \
{ \
    static std::string completeMessage00172456 (__PRETTY_FUNCTION__); \
    completeMessage00172456 += ": "; \
    completeMessage00172456 += message; \
    CONS_EXCEPTION (expn, completeMessage00172456.c_str (), code, catchedException00172456); \
}

#define DETAILED_CATCH(message, code, catching, expn) \
catch (catching) \
{ \
    static std::string completeMessage43215876 (__PRETTY_FUNCTION__); \
    completeMessage43215876 += ": ."; \
    completeMessage43215876 += #catching; \
    completeMessage43215876 += ". "; \
    NAT_EXCEPTION (expn, completeMessage43215876.c_str (), code); \
}

#define DETAILED_UNKNOWN_CATCH(message, code, expn) \
catch (...) \
{ \
    static std::string completeMessage67416279 (__PRETTY_FUNCTION__); \
    completeMessage67416279 += ": .unknown."; \
    NAT_EXCEPTION (expn, completeMessage67416279.c_str (), code); \
}

class ExceptionData_t;

class ExceptionHandler
{
	private:
	const char* message_;
	int error_code_;
	int line_;
	const char* file_;

	const ExceptionHandler* cause_;


	public:
	const ExceptionHandler* pt_;
	ExceptionHandler ();
	ExceptionHandler (const char*     message,
		int             error_code_,
		int             line,
		const char*     file);

	ExceptionHandler (const char*             message,
		int                     error_code_,
		const ExceptionHandler* cause,
		int                     line,
		const char*             file);

	ExceptionHandler (const ExceptionHandler& that);
	ExceptionHandler& operator = (const ExceptionHandler& that);

	~ExceptionHandler ();
	void WriteLog (ExceptionData_t* data) const;
	void* operator new(size_t s, ExceptionData_t* data);
};

class ExceptionData_t : NonCopiable_t
{
	DISABLE_CLASS_COPY (ExceptionData_t)
	public:
	ExceptionHandler* allocatedMem_;
	int64_t usedMem_;
	int64_t availableMem_;
	std::string filename_;
	FILE* log_;

	void* mem_alloc (size_t size = 0);

	ExceptionData_t (size_t size, const char* filename);

	~ExceptionData_t ();

	void OpenLog ();

	void CloseLog ();

};