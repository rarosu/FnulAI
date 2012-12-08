/**
	File: Logger.cpp
	Author: Lars Woxberg | Rarosu
*/

#include <Utils\Logger.h>
#include <r2-exception.hpp>
#include <cstdarg>

const int Logger::C_BUFFER_SIZE = 2048;

Logger::Logger(const std::string& filename)
{
	m_file.open(filename.c_str(), std::fstream::out | std::fstream::trunc);

	if (!m_file.is_open())
		throw r2ExceptionIOM("Failed to open log file");
}

Logger::~Logger() throw()
{
	m_file.close();
}

void Logger::printf(const char* format, ...)
{
	va_list args;
	char buffer[C_BUFFER_SIZE];

	va_start(args, format);
	vsnprintf_s(buffer, C_BUFFER_SIZE, C_BUFFER_SIZE, format, args);
	va_end(args);

	m_file << buffer << std::flush;
}

void Logger::printfln(const char* format, ...)
{
	va_list args;
	char buffer[C_BUFFER_SIZE];

	va_start(args, format);
	vsnprintf_s(buffer, C_BUFFER_SIZE, C_BUFFER_SIZE, format, args);
	va_end(args);

	m_file << buffer << std::endl << std::flush;
}