#include "pch.h"
#include "CppUnitTest.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "StateMachine.h"
#include "ErrorCode.h"
#include <libpq-fe.h>
#include <vector>
#include <stdexcept>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerTests
{
	TEST_CLASS(Server_Tests)
	{
	public:

		TEST_METHOD(UT_SRV_01_ServerSurvivesBadPacket)
		{


		}

		TEST_METHOD(UT_SRV_02_ServerRejectsUnauthenticatedRequest)
		{


		}

		TEST_METHOD(UT_SRV_03_ServerStoresMaintenanceEvent)
		{


		}
	};

	TEST_CLASS(StateMachine_Tests)
	{
	public:

		TEST_METHOD(UT_SVM_01_StateMachineStartsIdleState)
		{


		}

		TEST_METHOD(UT_SVM_02_ValidStateTransitions)
		{


		}

		TEST_METHOD(UT_SVM_03_InvalidStateTransitions)
		{


		}

		TEST_METHOD(UT_SVM_04_AnyStateCanReachErrorState)
		{


		}
	};
}
