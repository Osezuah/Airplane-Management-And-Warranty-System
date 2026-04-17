#include "pch.h"
#include "CppUnitTest.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Logger.h"
#include "StateMachine.h"
#include <crow/json.h>
#include <fstream>
#include <string>
#include <cstdio>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTests
{
	TEST_CLASS(PacketFactory_Tests)
	{
	public:

		TEST_METHOD(IT_PCF_01_HandshakePacketConstruction)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(IT_PCF_02_MaintenanceEventPacketConstruction)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(IT_PCF_03_WarrantyEventPacketConstruction)
		{
			//ARRANGE


			//ACT


			//ASSET

		}
	};

	TEST_CLASS(Logger_Tests)
	{
	public:

		TEST_METHOD(IT_LOG_01_LogMaintenanceHistoryToClient)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(IT_LOG_02_LogReceivedEvent)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(IT_LOG_03_LogWarrantyEventToClient)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(IT_LOG_04_LogStateTransitions)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(IT_LOG_05_LogMaintenanceEventToServer)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(IT_LOG_06_LogMaintenanceEventFromServer)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(IT_LOG_07_LogWarrantyEventFromServer)
		{
			//ARRANGE


			//ACT


			//ASSET

		}
	};
}
