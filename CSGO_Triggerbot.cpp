#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
using namespace std;


uintptr_t GetModuleBaseAddress(DWORD dwProcID, TCHAR *szModuleName)
{
	uintptr_t ModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry32;
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32))
		{
			do
			{
				if (_tcsicmp(ModuleEntry32.szModule, szModuleName) == 0)
				{
					ModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
	return ModuleBaseAddress;
}

int main() {
	//playerbase *currently* is
	//"client.dll"+0x00AA9AB4
	DWORD playerBase;
	DWORD playerBaseOffset = 0x00AA9AB4;
	DWORD crosshairId;
	DWORD crosshairIdOffset = 0xB2A4;
	DWORD teamNum;
	DWORD teamNumOffset = 0xF0;
	DWORD entitiylist;
	DWORD entitiylistOfset = 0x0022FF74;
	DWORD entitiylistOfset1 = 0x2C;


	HWND hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");

	if (hwnd == NULL) {
		cout << "ERR: COULD NOT FIND WINDOW" << endl;

	}
	else
	{
		DWORD procID;
		GetWindowThreadProcessId(hwnd, &procID);
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

		if (procID == NULL) {
			cout << "ERR: COULD NOT OBTAIN PID" << endl;
		}

		cout << "CLIENT.DLL: " << GetModuleBaseAddress(procID, (TCHAR*)"client.dll") <<endl;

		
		//Caclulate READ(client.dll + offset) to get playerbaseADDR
		ReadProcessMemory(handle, LPVOID(GetModuleBaseAddress(procID, (TCHAR*)"client.dll") + playerBaseOffset),&playerBase,4,NULL);
		ReadProcessMemory(handle, LPVOID(GetModuleBaseAddress(procID, (TCHAR*)"client.dll") + entitiylistOfset), &entitiylist, 4, NULL);

		crosshairId = playerBase + crosshairIdOffset;
		teamNum = playerBase + teamNumOffset;
		entitiylist = entitiylist + entitiylistOfset1;


		cout << "PLAYER BASE: " << (LPVOID)playerBase << endl;
		cout << "CROSSHAIRID: " << (LPVOID)crosshairId << endl;
		cout << "TEAMNUMBER : " << (LPVOID)teamNum << endl;
		cout << "ELIST      : " << (LPVOID)entitiylist << endl;



		/*
		this code block will show all player info :P
		while (true){
			for (int of = 0; of <= 10; of++) {
				DWORD health;
				DWORD team;
				DWORD entitynum;
				DWORD entity;
				//cout << of <<" : " <<LPVOID(entitiylist + of * 16) << endl;
				ReadProcessMemory(handle, LPVOID(entitiylist + of * 8), &entity, 4, NULL);

				ReadProcessMemory(handle, LPVOID(entity + 0xF4), &health, 4, NULL);
				ReadProcessMemory(handle, LPVOID(entity + 0xEC), &team, 4, NULL);
				ReadProcessMemory(handle, LPVOID(entity + 0x5C), &entitynum, 4, NULL);

				cout << "Health: " << health << endl;
				cout << "Number: " << entitynum << endl;
				cout << "Team: " << team << endl;
				cout << "--------" << endl;
			}
			system("cls");
		}
		*/
		

		system("pause");

		bool active = false;
		int read_crosshairId;
		int read_teamNum;

		while (true) {

			active = true;//false;

			/*
			if (GetAsyncKeyState(VK_MENU) & 0x8000)
			{
				active = true;

			}
			*/

			ReadProcessMemory(handle, LPVOID(crosshairId), &read_crosshairId, 4, NULL);
			ReadProcessMemory(handle, LPVOID(teamNum), &read_teamNum, 4, NULL);


			if (active) {
				//iterate through all players 
				for (int of = 0; of <= 32; of++) {
					DWORD team;
					DWORD entitynum;
					DWORD entity;
		
					ReadProcessMemory(handle, LPVOID(entitiylist + of * 8), &entity, 4, NULL);

					ReadProcessMemory(handle, LPVOID(entity + 0xEC), &team, 4, NULL);
					ReadProcessMemory(handle, LPVOID(entity + 0x5C), &entitynum, 4, NULL);

					if (entitynum == read_crosshairId && team != read_teamNum) {
						INPUT ip;
						ip.type = INPUT_KEYBOARD;
						ip.ki.time = 0;
						ip.ki.wVk = 0;
						ip.ki.dwExtraInfo = 0;
						ip.ki.dwFlags = KEYEVENTF_SCANCODE;
						ip.ki.wScan = 0x19;
						SendInput(1, &ip, sizeof(INPUT));
						Sleep(25);
						ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
						SendInput(1, &ip, sizeof(INPUT));

					}
				}


			}

			Sleep(1);
			cout << "CURRENTLY AIMING AT:" << read_crosshairId << endl;
			cout << "MY TEAM            :" << read_teamNum;
			system("cls");
		}


		return 0;

	}
}
