#include <string>
class PostComponentOutDataThreadData
{
public:
	std::string strTaskId;
	std::string strOutxmlContent;
	//运行类型，由封装传过来，直接传递给后台，可能的值有021 ，流程执行 022 流程确定 ，为空则为组件提交
	std::string strRunType;
protected:
private:
};
