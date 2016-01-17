mkdir output
..\bin\wsdl2h.exe ERMClientService.wsdl -oERMClientService.h -nERMClientService -qERMClientService
..\bin\soapcpp2.exe -j ERMClientService.h -I..\bin\import -x -C -pERMClientService -doutput -n