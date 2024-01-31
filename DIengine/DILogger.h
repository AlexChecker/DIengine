#pragma once
#include <iostream>
#include <vector>
#include <format>
#ifdef _WIN32
#include <windows.h>
#endif


namespace DILog
{
	enum DI_LOG_LEVEL 
	{
		DI_LOG_LEVEL_MESSAGE,
		DI_LOG_LEVEL_INFO,
		DI_LOG_LEVEL_WARN,
		DI_LOG_LEVEL_ERROR,
		DI_LOG_LEVEL_FATAL
	};


	enum DI_LOG_MODE 
	{
		DI_NO_LOG,
		DI_LOG_CONSOLE,
		DI_LOG_FILE,
		DI_LOG_ONSCREEN
	};

	struct DILogMessage 
	{
	public:
		DI_LOG_LEVEL level;
		int line;
		std::string source;
		std::string message;
		/// <summary>
		/// Log message constructor
		/// </summary>
		/// <param name="message">Message to display</param>
		/// <param name="line">Line number in source file. Use __LINE__ for it</param>
		/// <param name="source">Source code file. Use __FILE__ for it</param>
		/// <param name="level">Message level. Changes color pallete for log output</param>
		DILogMessage(std::string message,int line = 0 , std::string source = "Unknown", DI_LOG_LEVEL level = DI_LOG_LEVEL_MESSAGE)
		{
			this->line = line;
			this->level = level;
			this->message = message;
			this->source = source;
		}

	};
	void log(DILogMessage message);
	void logVector(DILogMessage header, std::vector<std::string> data);

}