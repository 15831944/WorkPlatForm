module com{
	module pera{
		module base{
			module runtime{
				module remote{
					exception InvokeException
					{
					    string reason;
					};
					
					sequence<byte> ByteSeq;

					interface WsICEService{
						["ami"] string runRobotFinished(string taskId, string outPutSvnPath, string runType) throws InvokeException;
						["ami"] void runRobotLog(string logInfo) throws InvokeException;
						["ami"] void runRobotRate(string rateInfo) throws InvokeException;
						string invokeWsMethod(string methodName, string paramJsonArray) throws InvokeException;
						["ami"] string invokeLocalWsMethod(string methodName, string paramJsonObject) throws InvokeException;
						["ami"] string runLocalRobotFinished(string taskId, string outPutSvnPath, string runType) throws InvokeException;
						["ami"] void runLocalRobotLog(string logInfo) throws InvokeException;
						["ami"] void runLocalRobotRate(string rateInfo) throws InvokeException;
						string savePlanByByteXpdl(string jsonInfo, ByteSeq xpdlBytes) throws InvokeException;
					};
				};
			};
		};
	};
};

