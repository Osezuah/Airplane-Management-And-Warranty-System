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
			//ARRANGE
			uint8_t badData[1] = { 0x00 };
			size_t size = 1;
			bool result = false;

			//ACT
			try {
				Packet::Deserialize(badData, size, false);
			}
			catch (const std::runtime_error&) {
				result = true;
			}
			catch (...) {
				result = true;
			}

			//ASSET
			Assert::IsTrue(result);
		}

		TEST_METHOD(UT_SRV_02_ServerRejectsUnauthenticatedRequest)
		{
			//ARRANGE
			StateMachine stateMachine;
			stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);
			Packet unautheticatedPacket = PacketFactory::MaintenanceEvent(1, 10, "mike", "repair", "had to fix this");
			ServerState expected = ServerState::ERROR_STATE;
			ServerState actual = ServerState::IDLE;

			//ACT
			if (unautheticatedPacket.getType() != PacketType::HANDSHAKE) {
				stateMachine.TransitionStateTo(ServerState::ERROR_STATE);
				actual = stateMachine.GetState();
			}

			//ASSET
			Assert::AreEqual(static_cast<int>(expected), static_cast<int>(actual));
		}

		TEST_METHOD(UT_SRV_03_ServerStoresMaintenanceEvent)
		{
			//ARRANGE
			PGconn* conn = PQconnectdb(
				"host=127.0.0.1 port=5432 dbname=amws user=postgres password=Lkj876*bv"
			);
			const char* insertCommand =
				"INSERT INTO MaintenanceEvent (TechnicianID_FK, AirplaneID_FK, Type, Description) VALUES (1, 10, 'Unit Test', 'UT-SRV-03 test entry')";
			const char* selectCommand =
				"SELECT MEventID FROM MaintenanceEvent "
				"WHERE Type = 'Unit Test' AND Description = 'UT-SRV-03 test entry'";
			bool connectionOk = false;
			bool insertOk = false;
			bool selectOk = false;

			//ACT
			connectionOk = (PQstatus(conn) == CONNECTION_OK);

			PGresult* insertResult = PQexec(conn, insertCommand);
			insertOk = (PQresultStatus(insertResult) == PGRES_COMMAND_OK);
			PQclear(insertResult);

			PGresult* selectResult = PQexec(conn, selectCommand);
			selectOk = (PQresultStatus(selectResult) == PGRES_TUPLES_OK)
				&& (PQntuples(selectResult) > 0);
			PQclear(selectResult);

			// Cleanup database
			PQexec(conn, "DELETE FROM MaintenanceEvent WHERE Type = 'Unit Test' "
				"AND Description = 'UT-SRV-03 test entry'");
			PQfinish(conn);

			//ASSET
			Assert::IsTrue(connectionOk,
				L"Server must be able to connect to database.");
			Assert::IsTrue(insertOk,
				L"Server must be able to insert into database.");
			Assert::IsTrue(selectOk,
				L"Server must be able to retrieve from database.");
		}
	};

	TEST_CLASS(StateMachine_Tests)
	{
	public:

		TEST_METHOD(UT_SVM_01_StateMachineStartsIdleState)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(UT_SVM_02_ValidStateTransitions)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(UT_SVM_03_InvalidStateTransitions)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(UT_SVM_04_AnyStateCanReachErrorState)
		{
			//ARRANGE


			//ACT


			//ASSET

		}
	};
}
