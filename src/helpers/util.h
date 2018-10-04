#pragma once

#include <../libs/uws/uWS.h>

void sendSocketText(uWS::WebSocket<uWS::CLIENT> *ws, std::string textToSend) {
	ws->send(textToSend, textToSend.size(), uWS::OpCode::TEXT);
}
