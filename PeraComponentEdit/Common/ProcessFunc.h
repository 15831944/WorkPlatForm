#pragma once

//extern HANDLE CreateJobKillOnClose(void);
extern BOOL AddProcessToJobKillOnClose(DWORD dwPid);
extern BOOL AddProcessToJobKillOnClose(HANDLE hProcess);