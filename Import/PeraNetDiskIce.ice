module Pera{
	module NetDisk{
		module Daemon{
			exception InvokeException
			{
				string reason;
			};

			interface PeraNetDiskDaemonService{
				string InvokeMethod(string strMethodName, string strJsonIn) throws InvokeException;
			};
		};
	};
};

