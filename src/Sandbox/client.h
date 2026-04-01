#pragma once

#include <stop_token>
#include "ServiceBus.h"

class Client
{

public:

	~Client() = default;

	static int start(std::stop_token st, ServiceBus& bus);

};
