rem DSNode Startup Script
rem If you need proxy server support
rem You can specify a proxy server by adding --proxyserver=IP_ADDRESS --proxyport=8080
rem Something like DSNode-Host.exe --proxyserver=10.1.2.153 --proxyport=8080
cd %~dp0
DSNode-Host.exe --nodeserver=http://bmc.downstreamsandbox.com/dsnode
