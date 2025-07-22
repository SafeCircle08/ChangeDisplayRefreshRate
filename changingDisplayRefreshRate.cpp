#include <Windows.h>
#include <iostream>
#include <set>
#include <atomic>

DEVMODE currentMode = {};
DEVMODE originalMode = {};
std::atomic<bool> running = true;
const int optimalFrequence = 60;

bool resetDisplayFrequency()
{
	originalMode.dmFields = DM_DISPLAYFREQUENCY;
	LONG result = ChangeDisplaySettings(&originalMode, CDS_UPDATEREGISTRY);
	if (result == DISP_CHANGE_SUCCESSFUL)
	{
		std::cout << "Display frequence has been repristinated!" << std::endl;
		return true;
	}
	else
	{
		std::cout << "Could not set the original display frequence..." << std::endl;
		return false;
	}
}

BOOL WINAPI ConsoleHandler(DWORD signal)
{
	switch (signal)
	{
		case CTRL_LOGOFF_EVENT:
		case CTRL_C_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_BREAK_EVENT:
			std::cout << "Close event detected!" << std::endl;

			resetDisplayFrequency();
			running = false;
			Sleep(2000);

			return TRUE;
	}
}

int main()
{
	//Set up
	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE))
	{
		std::cerr << "Error while trying to set the ConsoleHandler..." << std::endl;
		return 1;
	}

	//Getting start info
	currentMode.dmSize = sizeof(DEVMODE);
	if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &currentMode))
	{
		std::cerr << "Failed to get frequency values...\n	";
		return 1;
	}
	originalMode = currentMode;


	//Searching to optimal frequence
	int modeNum = 0;
	bool found = false;

	while (EnumDisplaySettings(NULL, modeNum++, &currentMode))
	{
		int hertz = currentMode.dmDisplayFrequency;
		//Optimal frequence found
		if (hertz == optimalFrequence) 
		{ 
			found = true;
			break; 
		}
	}

	if (!found)
	{
		std::cerr << "Could not find optimal frequence. Did not change any display setting";
	}

	//Setting to the current mode the optimal frequences
	currentMode.dmDisplayFrequency = optimalFrequence;
	currentMode.dmFields = DM_DISPLAYFREQUENCY;
	//Output messages
	LONG result = ChangeDisplaySettings(&currentMode, CDS_UPDATEREGISTRY);
	if (result == DISP_CHANGE_SUCCESSFUL) { std::cout << "Optimale display refresh rate (60Hz) setted correctly" << std::endl; }
	else { std::cout << "Could not set the optimal refresh rate..." << std::endl; }

	//Stay in background
	while (running)
	{
		Sleep(500);
	}
	std::cout << "Program finished" << std::endl;
	return 0;
}