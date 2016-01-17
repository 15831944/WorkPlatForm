#pragma once

//extern HANDLE CreateJobKillOnClose(void);
extern BOOL AddProcessToJobKillOnClose(DWORD dwPid);
extern BOOL AddProcessToJobKillOnClose(HANDLE hProcess);

extern DWORD CreateProcessInJob(LPCTSTR lpExePath, LPCTSTR lpParam, WORD wShowFlag, LPCTSTR lpAppTital = NULL);