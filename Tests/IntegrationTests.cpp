#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTests
{
	TEST_CLASS(PacketFactory_Tests)
	{
	public:

		TEST_METHOD(IT_PCF_01_HandshakePacketConstruction)
		{


		}

		TEST_METHOD(IT_PCF_02_MaintenanceEventPacketConstruction)
		{


		}

		TEST_METHOD(IT_PCF_03_WarrantyEventPacketConstruction)
		{


		}
	};

	TEST_CLASS(Logger_Tests)
	{
	public:

		TEST_METHOD(IT_LOG_01_LogMaintenanceHistoryToClient)
		{


		}

		TEST_METHOD(IT_LOG_02_LogReceivedEvent)
		{


		}

		TEST_METHOD(IT_LOG_03_LogWarrantyEventToClient)
		{


		}

		TEST_METHOD(IT_LOG_04_LogStateTransitions)
		{


		}

		TEST_METHOD(IT_LOG_05_LogMaintenanceEventToServer)
		{


		}

		TEST_METHOD(IT_LOG_06_LogMaintenanceEventFromServer)
		{


		}

		TEST_METHOD(IT_LOG_07_LogWarrantyEventFromServer)
		{


		}
	};
}
