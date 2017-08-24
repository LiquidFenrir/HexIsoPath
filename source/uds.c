#include "uds.h"

#define APP_DATA_HEADER "HexIsoPath-"

static u32 wlancommID = 0xFF150848;
static u8 appdata[0x14] = {0x69, 0x8a, 0x05, 0x5c};
static char * appdataStr = (char*)appdata+4;

static u8 data_channel = 1;
static udsNetworkStruct networkstruct;
static udsBindContext bindctx;

static u32 recv_buffer_size = UDS_DEFAULT_RECVBUFSIZE;

static Result startServer(void)
{
	Result ret = 0;
	
	DEBUG("Starting the server...\n");
	
	//only allow 2 nodes (players): the server and 1 client
	udsGenerateDefaultNetworkStruct(&networkstruct, wlancommID, 0, 2);
	
	char passphrase[sizeof(appdata)] = {0};
	strncpy(passphrase, appdataStr, strlen(appdataStr));
	DEBUG("passphrase: '%s'\n", passphrase);
	
	ret = udsCreateNetwork(&networkstruct, passphrase, strlen(passphrase)+1, &bindctx, data_channel, recv_buffer_size);
	if(R_FAILED(ret))
		return ret;
	
	ret = udsSetApplicationData(appdata, sizeof(appdata));
	if(R_FAILED(ret)) {
		exitComm(true);
		return ret;
	}
	
	//dont allow spectators
	ret = udsEjectSpectator();
	if(R_FAILED(ret)) {
		exitComm(true);
		return ret;
	}
	
	return 0;
}

static Result connectToServer(udsNetworkStruct network)
{
	Result ret = 0;
	
	DEBUG("Connecting to the server as a client...\n");
	
	char passphrase[sizeof(appdata)] = {0};
	char * networkAppdataStr = (char*)network.appdata+4;
	strncpy(passphrase, networkAppdataStr, strlen(networkAppdataStr));
	DEBUG("passphrase: '%s'\n", passphrase);
	
	for (int pos = 0; pos < 10; pos++) { // 10 tries
		ret = udsConnectNetwork(&network, passphrase, strlen(passphrase)+1, &bindctx, UDS_BROADCAST_NETWORKNODEID, UDSCONTYPE_Client, data_channel, recv_buffer_size);
		if (R_FAILED(ret))
			DEBUG("udsConnectNetwork() returned 0x%.8lx\n", ret);
		else {
			DEBUG("Succesfully connected to the server!\n");
			break;
		}
		ret = -3;
	}
	
	return ret;
}

static Result startClient(void)
{
	Result ret = 0;
	
	DEBUG("Starting client...\n");
	
	u32 tmpbuf_size = 0x4000;
	u32 * tmpbuf = malloc(tmpbuf_size);
	if (tmpbuf == NULL) {
		DEBUG("Failed to allocate tmpbuf for beacon data.\n");
		return -1;
	}
	
	memset(tmpbuf, 0, tmpbuf_size);
	
	size_t total_networks = 0;
	udsNetworkScanInfo * networks = NULL;
	
	ret = udsScanBeacons(tmpbuf, tmpbuf_size, &networks, &total_networks, wlancommID, 0, NULL, false); //always returns -1 (failed to malloc networks) if there's a network to scan, 0 otherwise, but total_networks stays 0
	if (R_FAILED(ret))
		DEBUG("udsScanBeacons() returned 0x%.8lx\n", ret);
	
	DEBUG("servers found: %u\n", total_networks);
	
	if (total_networks) {
		for (size_t selected_network = 0; selected_network < total_networks; selected_network++) {
			udsNetworkStruct network = networks[selected_network].network;
			DEBUG("check.\n");
			if (memcmp(network.appdata, appdata, 4+strlen(APP_DATA_HEADER)) == 0 && network.max_nodes == 2 && network.total_nodes < 2) {
				ret = connectToServer(network);
				if (R_FAILED(ret)) {
					DEBUG("Couldn't connect to the server.\n");
				}
				else {
					DEBUG("Connected to server %u.\n", selected_network);
					break;
				}
			}
		}
	}
	else
		ret = -2;
	
	free(networks);
	free(tmpbuf);
	
	return ret;
}

static void initAppData(void)
{
	DEBUG("Init AppData...\n");
	srand(time(NULL));
	strncpy(appdataStr, APP_DATA_HEADER, strlen(APP_DATA_HEADER));
	u8 tempbuf[sizeof(appdata)-(strlen(APP_DATA_HEADER)+4)];
	for (unsigned int i = 0; i < sizeof(tempbuf); i++) {
		tempbuf[i] = rand() % 0xFF;
	}
	memcpy(appdata+4+strlen(APP_DATA_HEADER), tempbuf, sizeof(tempbuf));
	DEBUG("AppData: '%.*s'\n", sizeof(appdata)-4, appdataStr);
}

Result initComm(bool server)
{
	initAppData();
	return (server ? startServer() : startClient());
}

void exitComm(bool server)
{
	if (server)
		udsDestroyNetwork();
	else
		udsDisconnectNetwork();
	
	udsUnbind(&bindctx);
}

Result receiveData(void * data, size_t bufSize, size_t * receivedSize)
{
	u16 src_NetworkNodeID;
	Result ret = udsPullPacket(&bindctx, data, bufSize, receivedSize, &src_NetworkNodeID);
	if (R_FAILED(ret))
		DEBUG("udsPullPacket() returned 0x%.8lx\n", ret);
	return ret;
}

Result sendData(void * data, size_t size)
{
	Result ret = udsSendTo(UDS_BROADCAST_NETWORKNODEID, data_channel, UDS_SENDFLAG_Default, (u8 *)data, size);
	if (R_FAILED(ret))
		DEBUG("udsSendTo() returned 0x%.8lx\n", ret);
	return ret;
}
