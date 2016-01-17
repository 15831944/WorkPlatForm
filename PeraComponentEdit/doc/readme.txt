20140704
前后表单是否失效的处理
以前返回IsBackFormParamsChanged和IsFrontFormParamsChanged两个属性分别标识前后表单是否失效
现在改为IsFormParamsChanged属性来标识表单是否失效，值为字符串的true或false


20140704
单个组件运行接口
以前的格式为
{
    "front": {
        "InstanceID": "c020ea08-3127-44cf-808a-60a03512b1e1",
        "Path": "C:\\WorkSpace\\PeraWorkSpace\\Data\\A9B61E18-D030-43C9-B79F-608D813B1172\\F8in1d3\\前表单组件.bot",
        "Title": "数据解析组件 前处理"
    },
    "nodeName": "数据解析组件",
    "component": {
        "Id": "F8in1d3",
        "InstanceID": "024df8ba-ea3a-4ec1-ab80-96b415a18c16",
        "Path": "C:\\WorkSpace\\PeraWorkSpace\\Data\\A9B61E18-D030-43C9-B79F-608D813B1172\\F8in1d3\\数据解析组件.bot"
    },
    "Parameters": [
        {
           "id": "89b67920-b2f6-4a7f-8a87-0a80735b5beb",
            "name": "a",
            "value": "",
            "type": "String",
            "version": ""
        },
        {
            "id": "907bedb9-f7f4-423d-9688-4ff76db2068e",
            "name": "c",
            "value": "",
            "type": "String",
            "version": ""
        }
    ]
}
需更改
Title，只要节点名即可
加入isPopForm属性，值为字符串的true或false，放在和nodeName属性在同一级


20140704
纪要：
CheckParamsChanged接口与CheckFormParamsChanged

CheckParamsChanged接口
时机：当组件或流程节点修改后，调用此方法，来得到有哪些参数是新增、删除、或修改了。之后通知结构树对应的变化，结构树只处理变化了的参数。
传入修改前后的JSON对象，对象中的Parameters为参数数组，接口中对修改前后的参数数组做比较得出变更结果
备注：
中间件提供比较参数的接口，CheckParamsChanged
传入两个参数，分别是旧的参数的串和新的参数的串
旧的：
{
    "Parameters": []
}
新的：
{
    "Parameters": [
        {
            "cnname": "",
            "desc": "",
            "direction": "In",
            "format": "",
            "group": "",
            "id": "07c9aeb1-3da6-4b7a-b8c1-918a7a84ba25",
            "isdynamicarray": "false",
            "listname": "",
            "name": "a",
            "type": "String",
            "units": "",
            "value": ""
        },
        {
            "cnname": "",
            "desc": "",
            "direction": "In",
            "format": "",
            "group": "",
            "id": "4756c5e5-03fe-44b6-a95e-4c8c640c3550",
            "isdynamicarray": "false",
            "listname": "",
            "name": "b",
            "type": "String",
            "units": "",
            "value": ""
        },
        {
            "cnname": "",
            "desc": "",
            "direction": "In",
            "format": "",
            "group": "",
            "id": "2a6167ad-a511-411a-ab7f-a002302b081c",
            "isdynamicarray": "false",
            "listname": "",
            "name": "c",
            "type": "String",
            "units": "",
            "value": ""
        },
        {
            "cnname": "",
            "desc": "",
            "direction": "In",
            "format": "",
            "group": "",
            "id": "27868f68-805e-4d2d-9a47-1e02f429bd02",
            "isdynamicarray": "false",
            "listname": "",
            "name": "d",
            "type": "String",
            "units": "",
            "value": ""
        },
        {
            "cnname": "",
            "desc": "",
            "direction": "In",
            "format": "",
            "group": "",
            "id": "9f5d92d6-e435-466e-8891-78fee86762dc",
            "isdynamicarray": "false",
            "listname": "",
            "name": "e",
            "type": "String",
            "units": "",
            "value": ""
        },
        {
            "cnname": "",
            "desc": "",
            "direction": "In",
            "format": "",
            "group": "",
            "id": "188ca7f1-e13d-4785-9c9b-948e5b74819a",
            "isdynamicarray": "false",
            "listname": "",
            "name": "f",
            "type": "String",
            "units": "",
            "value": ""
        },
        {
            "cnname": "",
            "desc": "",
            "direction": "In",
            "format": "",
            "group": "",
            "id": "748e3776-5955-4166-8320-7c052b2587db",
            "isdynamicarray": "false",
            "listname": "",
            "name": "g",
            "type": "String",
            "units": "",
            "value": ""
        },
        {
            "cnname": "",
            "desc": "",
            "direction": "In",
            "format": "",
            "group": "",
            "id": "b07787bd-ac56-42ec-9551-5b52947d6ce6",
            "isdynamicarray": "false",
            "listname": "",
            "name": "h",
            "type": "String",
            "units": "",
            "value": ""
        }
    ],
    "flowId": "wfprcsfre267njjap716o6"
}
中间件给SVG返回：//删除的参数ID在前面，后面增加和修改的参数按真实顺序排
[
    {
        "flag": "add",
        "id": "07c9aeb1-3da6-4b7a-b8c1-918a7a84ba25"
    },
    {
        "flag": "add",
        "id": "4756c5e5-03fe-44b6-a95e-4c8c640c3550"
    },
    {
        "flag": "add",
        "id": "2a6167ad-a511-411a-ab7f-a002302b081c"
    },
    {
        "flag": "add",
        "id": "27868f68-805e-4d2d-9a47-1e02f429bd02"
    },
    {
        "flag": "add",
        "id": "9f5d92d6-e435-466e-8891-78fee86762dc"
    },
    {
        "flag": "add",
        "id": "188ca7f1-e13d-4785-9c9b-948e5b74819a"
    },
    {
        "flag": "add",
        "id": "748e3776-5955-4166-8320-7c052b2587db"
    },
    {
        "flag": "add",
        "id": "b07787bd-ac56-42ec-9551-5b52947d6ce6"
    }
]

而对于CheckFormParamsChanged接口
传入和上个接口一致
返回值为
{
    "IsBackFormParamsChanged": "false",
    "IsFrontFormParamsChanged": "true"
}
用于标识前后表单中的参数是否过时
时机：用于流程节点修改后，调用此方法得到表单是否失效，是否应置为红色