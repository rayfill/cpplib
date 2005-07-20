#include <winsock2.h>
#include <process.h>
#include <memory.h>
#include <iostream>

unsigned __stdcall threadFunc(void* ReciverHandle) {
  char buffer[2];
  while (recv(*(SOCKET*)(ReciverHandle), buffer, 1, 0)) {
    if (buffer[0] == 'q')
      break;
    std::cerr <<  buffer[0];
  }
}

//int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, const char* cmdline, int cmdShow) 
int main()
{
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

 
  SOCKET sockHandle;
  //  WSAPROTOCOL_INFO wsaProtocolInfo;
  //  memset(&wsaProtocolInfo, 0, sizeof(wsaProtocolInfo));
  WSADATA wsaData;
  WSAStartup(MAKEWORD(1, 1), &wsaData);

  addr.sin_family = AF_INET;
  hostent* hostEntry = gethostbyname("localhost");
  memcpy(&addr.sin_addr, hostEntry->h_addr, hostEntry->h_length);
  addr.sin_port = htons(6543);

  if ((sockHandle = socket(AF_INET, SOCK_STREAM, 0)) &&
      (bind(sockHandle, (sockaddr*)&addr, sizeof(addr)) == 0) &&
      (listen(sockHandle, 0) == 0)) {

    SOCKET ReciverHandle = 0;
    ReciverHandle = accept(sockHandle, NULL, NULL);
    unsigned threadId = 0;
    _beginthreadex(NULL,
		   0,
		   threadFunc,
		   (void*)&ReciverHandle,
		   0,
		   &threadId);
    Sleep(100000);
  }
  return 0;
}
