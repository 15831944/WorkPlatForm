module com{
	module pera{
		module base{
			module runtime{
				module remote{
					exception RemoteException
					{
					    string reason;
					};

					interface RobotRemoteService{
						string runRobotStart(string robotInfo) throws RemoteException;
						void runRobotStop(string taskId ) throws RemoteException;
					};
				};
			};
		};
	};
};

