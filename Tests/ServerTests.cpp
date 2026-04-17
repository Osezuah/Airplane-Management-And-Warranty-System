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
			StateMachine stateMachine;

			//ACT
			// code has already acted from declaration of stateMachine variable.

			//ASSET
			Assert::AreEqual(static_cast<int>(stateMachine.GetState()), static_cast<int>(ServerState::IDLE));
		}

		TEST_METHOD(UT_SVM_02_ValidStateTransitions)
		{
			//ARRANGE
			StateMachine stateMachine;
			bool idleToWaiting = false;
			bool waitingToConnected = false;
			bool connectedToProcessing = false;
			bool processingToTransfer = false;
			bool transferToConnected = false;
			bool processingToConnected = false;
			bool connectedToIdle = false;

			//ACT
			idleToWaiting = stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);

			waitingToConnected = stateMachine.TransitionStateTo(ServerState::CONNECTED);

			connectedToProcessing = stateMachine.TransitionStateTo(ServerState::PROCESSING_REQUEST);

			processingToTransfer = stateMachine.TransitionStateTo(ServerState::TRANSFERRING_DATA);

			transferToConnected = stateMachine.TransitionStateTo(ServerState::CONNECTED);

			stateMachine.TransitionStateTo(ServerState::PROCESSING_REQUEST);
			processingToConnected = stateMachine.TransitionStateTo(ServerState::CONNECTED);

			connectedToIdle = stateMachine.TransitionStateTo(ServerState::CONNECTED);

			//ASSET
			Assert::IsTrue(idleToWaiting, L"Must be able to go from IDLE to WAITING");
			Assert::IsTrue(waitingToConnected, L"Must be able to go from WAITING to CONNECTED");
			Assert::IsTrue(connectedToProcessing, L"Must be able to go from CONNECTED to PROCESSIN");
			Assert::IsTrue(processingToTransfer, L"Must be able to go from PROCESSING to TRANSFER");
			Assert::IsTrue(transferToConnected, L"Must be able to go from TRANSFER to CONNECTED");
			Assert::IsTrue(processingToConnected, L"Must be able to go from PROCESSONG to CONNECTED");
			Assert::IsTrue(connectedToIdle, L"Must be able to go from CONNECTED to IDLE");

		}

		TEST_METHOD(UT_SVM_03_InvalidStateTransitions)
		{
			//ARRANGE
			StateMachine stateMachine; //starts IDLE

			bool idleResult = true;
			ServerState badIdleTransitions[] = { ServerState::IDLE, ServerState::CONNECTED, ServerState::PROCESSING_REQUEST, ServerState::TRANSFERRING_DATA };

			bool waitingResult = true;
			ServerState badWaitingTransitions[] = { ServerState::WAITING_FOR_VERIFICATION, ServerState::IDLE, ServerState::PROCESSING_REQUEST, ServerState::TRANSFERRING_DATA };

			bool connectedResult = true;
			ServerState badConnectedTransitions[] = { ServerState::CONNECTED, ServerState::TRANSFERRING_DATA, ServerState::WAITING_FOR_VERIFICATION };

			bool processingResult = true;
			ServerState badProcessingTransitions[] = { ServerState::PROCESSING_REQUEST, ServerState::IDLE, ServerState::WAITING_FOR_VERIFICATION };

			bool transferResult = true;
			ServerState badTransferTransitions[] = { ServerState::TRANSFERRING_DATA, ServerState::PROCESSING_REQUEST, ServerState::IDLE, ServerState::WAITING_FOR_VERIFICATION};

			//ACT + ASSERT
			for each(ServerState state in badIdleTransitions)
			{
				idleResult = stateMachine.TransitionStateTo(state);
				Assert::IsFalse(idleResult);
			}

			stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);
			for each(ServerState state in badWaitingTransitions)
			{
				waitingResult = stateMachine.TransitionStateTo(state);
				Assert::IsFalse(waitingResult);
			}
			
			stateMachine.TransitionStateTo(ServerState::CONNECTED);
			for each(ServerState state in badConnectedTransitions)
			{
				connectedResult = stateMachine.TransitionStateTo(state);
				Assert::IsFalse(connectedResult);
			}

			stateMachine.TransitionStateTo(ServerState::PROCESSING_REQUEST);
			for each(ServerState state in badProcessingTransitions)
			{
				processingResult = stateMachine.TransitionStateTo(state);
				Assert::IsFalse(processingResult);
			}

			stateMachine.TransitionStateTo(ServerState::TRANSFERRING_DATA);
			for each(ServerState state in badTransferTransitions)
			{
				transferResult = stateMachine.TransitionStateTo(state);
				Assert::IsFalse(transferResult);
			}
		}

		TEST_METHOD(UT_SVM_04_AnyStateCanReachErrorState)
		{
			//ARRANGE
			StateMachine stateMachine;
			bool idleResult = false;
			bool waitingResult = false;
			bool connectedResult = false;
			bool processingResult = false;
			bool transferResult = false;

			//ACT
			idleResult = stateMachine.TransitionStateTo(ServerState::ERROR_STATE);

			stateMachine.TransitionStateTo(ServerState::IDLE);
			stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);
			waitingResult = stateMachine.TransitionStateTo(ServerState::ERROR_STATE);

			stateMachine.TransitionStateTo(ServerState::IDLE);
			stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);
			stateMachine.TransitionStateTo(ServerState::CONNECTED);
			connectedResult = stateMachine.TransitionStateTo(ServerState::ERROR_STATE);

			stateMachine.TransitionStateTo(ServerState::IDLE);
			stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);
			stateMachine.TransitionStateTo(ServerState::CONNECTED);
			stateMachine.TransitionStateTo(ServerState::PROCESSING_REQUEST);
			processingResult = stateMachine.TransitionStateTo(ServerState::ERROR_STATE);

			stateMachine.TransitionStateTo(ServerState::IDLE);
			stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);
			stateMachine.TransitionStateTo(ServerState::CONNECTED);
			stateMachine.TransitionStateTo(ServerState::PROCESSING_REQUEST);
			stateMachine.TransitionStateTo(ServerState::TRANSFERRING_DATA);
			transferResult = stateMachine.TransitionStateTo(ServerState::ERROR_STATE);

			//ASSET
			Assert::IsTrue(idleResult, L"IDLE must be able to transition to ERROR");
			Assert::IsTrue(waitingResult, L"WAITING must be able to transition to ERROR");
			Assert::IsTrue(connectedResult, L"CONNECTED must be able to transition to ERROR");
			Assert::IsTrue(processingResult, L"PROCESSING must be able to transition to ERROR");
			Assert::IsTrue(transferResult, L"TRANSFER must be able to transition to ERROR");
		}
	};
}
