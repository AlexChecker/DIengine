#include "DILogger.h"

namespace DILog {
	DI_LOG_MODE LOG_MODE = DI_LOG_CONSOLE;

	void DILog::log(DILogMessage message)
	{
		{
			//TODO: Сделать мультиплатформенный вывод ошибок. Хотя бы для мака и линукса
			if (LOG_MODE != DI_NO_LOG) {
				//Вывод чисто в консольку
				if (LOG_MODE == DI_LOG_CONSOLE) {
					//Дальше бога нет. Только платформозависимый код
#ifdef _WIN32
					HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
					switch (message.level)
					{
					case DI_LOG_LEVEL_MESSAGE:
						SetConsoleTextAttribute(hConsole, 0x07);
						break;
					case DI_LOG_LEVEL_INFO:
						SetConsoleTextAttribute(hConsole, 0x02);
						break;
					case DI_LOG_LEVEL_WARN:
						SetConsoleTextAttribute(hConsole, 0x06);
						break;
					case DI_LOG_LEVEL_ERROR:
						SetConsoleTextAttribute(hConsole, 0x04);
						break;
					case DI_LOG_LEVEL_FATAL:
						SetConsoleTextAttribute(hConsole, 0x40);
						break;
					}

					std::cout << std::format("{0} [{1}]: {2}", message.source, message.line, message.message) << std::endl;
#endif
				}
			}
		}
	}
}