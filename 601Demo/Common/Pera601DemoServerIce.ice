module pera{
	module Demo601Server{
		exception InvokeException
		{
		    string reason;
		};
		
		interface PeraDemo601ServerService{
			string InvokeMethod(string methodName, string strJsonIn) throws InvokeException;
		};
	};
};