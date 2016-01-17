module pera{
	module demo601Client{
			exception InvokeException
			{
				string reason;
			};

			interface PeraDemo601ClientService{
				string InvokeMethod(string strMethodName, string strJsonIn) throws InvokeException;
		};
	};
};

