#pragma once

#include "game.h"

Result initComm(bool server);
void exitComm(bool server);

Result receiveData(void * data, size_t bufSize, size_t * receivedSize);
Result sendData(void * data, size_t size);
