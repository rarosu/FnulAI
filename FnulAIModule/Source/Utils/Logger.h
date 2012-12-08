/**
	File: Logger.h
	Author: Lars Woxberg | Rarosu

	A simple logger class
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <r2-singleton.hpp>

class Logger : public r2::Singleton<Logger>
{
public:
	Logger(const std::string& filename);
	~Logger() throw();

	void printf(const char* format, ...);
	void printfln(const char* format, ...);
private:
	std::fstream m_file;

	static const int C_BUFFER_SIZE;
};

#endif