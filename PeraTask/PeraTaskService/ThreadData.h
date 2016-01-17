#include <string>
class PrepareComponentDataThreadData
{
public:
	std::string strInfo;
	std::string strId;
	//运行类型，当发起失败时，直接传空结果给后台，运行类型为空（组件），或022（子流程）
	std::string strRunType;
protected:
private:
};
