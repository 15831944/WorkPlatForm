module pera
{
	module runner
	{
		module assist
		{
				exception InvokeException
				{
					string reason;
				};

				interface PeraWsAstICE
				{
					string GetRunId() throws InvokeException;
					string Stop(string sJsonString) throws InvokeException;
					["ami","amd"] string CreateCacheByWS(string sJsonString) throws InvokeException;
					["ami","amd"] string CreateWSByCache(string sJsonString) throws InvokeException;
					["ami","amd"] string CopyCache(string sJsonString) throws InvokeException;
					["ami","amd"] string DeleteCache(string sJsonString) throws InvokeException;
					["ami","amd"] string CreateCacheByXpdlStream(string sJsonString) throws InvokeException;
					["ami","amd"] string CreateWSByXpdlStream(string sJsonString) throws InvokeException;
				};
		};
	};
};