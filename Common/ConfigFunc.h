#pragma once
//统用取配置的调用接口
//内部维护一个MAP对象，缓存已取过的配置，第一次取配置或强制读配置时会调用取配置的通用COM接口
//

//增加配置时，可以config.ini中创建一个新的章节section，建议以模块划分
//在新章节或已有章节下创建键值对，如下格式
/*
[新模块名section]
键=值
key=value
*/
//修改后的config.ini提交到解决方案目录WorkPlatForm\PeraClientConfig\bin下，构建版本时以此目录下醒置文件为准

extern CString GetConfig(CString sSection, CString sKey, BOOL bReRead = FALSE);
extern BOOL SetConfig(CString sSection, CString sKey, CString sValue);