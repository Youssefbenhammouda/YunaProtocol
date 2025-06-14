//
// Created by youss on 6/13/2025.
//

#include "Transport.h"
void  YunaProtocol::YunaTransport::registerDataReceivedCallback(const DataReceivedCallback& callback) {
    this->callback = callback;

 }