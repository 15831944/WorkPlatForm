#pragma once

//����ǳ�
//// ����json��������������,errorΪ0ʱ��½�ɹ���errorΪ1ʱ��ʧ��
// "error"
// "userName"
// "password"
// "ticket"
// "realName"
// "roles"
extern CString Login(CString sUser, CString sPassword);
extern CString Logout(CString sTicket);
extern CString KeepTicket(CString sTicket);