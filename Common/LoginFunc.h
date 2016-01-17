#pragma once

//登入登出
//// 返回json串，有如下属性,error为0时登陆成功，error为1时，失败
// "error"
// "userName"
// "password"
// "ticket"
// "realName"
// "roles"
extern CString Login(CString sUser, CString sPassword);
extern CString Logout(CString sTicket);
extern CString KeepTicket(CString sTicket);