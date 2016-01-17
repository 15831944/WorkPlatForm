#pragma once

//使用COM来调用引擎的ICE接口

//调用网络版引擎接口，传入参数为多字节编码，内部处理时转为utf-8再调用服务器引擎
extern CString InvokeWsMethod(CString sMethod, CString sJson);

//调用网络版引擎接口，传入参数不做编码转换，直接调用服务器引擎
extern CString InvokeWsMethodOrignal(CString sMethod, CString sJson);

//调用单机版引擎接口，传入参数为多字节编码，内部处理时转为utf-8再调用本地引擎
extern CString InvokeLocalWsMethod(CString sMethod, CString sJson);

//调用单机版引擎接口，传入参数不做编码转换，直接调用本地引擎
extern CString InvokeLocalWsMethodOrignal(CString sMethod, CString sJson);