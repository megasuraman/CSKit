#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

//à¯êîÇ≈éwíËÇµÇΩ .bat Çé¿çsÇ∑ÇÈ
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: RunBat <batfile> [args...]" << std::endl;
		return 1;
	}

	std::string commandLine = "cmd.exe /c \"";
	for (int i = 1; i < argc; ++i)
	{
		if (i > 1) commandLine += " ";
		commandLine += "\"";
		commandLine += argv[i];
		commandLine += "\"";
	}
	commandLine += "\"";

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// CreateProcess requires a non-const char* for the command line
	std::vector<char> cmdBuffer(commandLine.begin(), commandLine.end());
	cmdBuffer.push_back('\0');

	if (!CreateProcessA(
		NULL,           // No module name (use command line)
		cmdBuffer.data(), // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW, // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		std::cerr << "CreateProcess failed (" << GetLastError() << ")." << std::endl;
		return 1;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode = 0;
	GetExitCodeProcess(pi.hProcess, &exitCode);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return static_cast<int>(exitCode);
}
