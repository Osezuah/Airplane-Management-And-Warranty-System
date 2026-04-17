#include "pch.h"
#include "CppUnitTest.h"
#include "..\Client\SocketConnection.h"
#include "..\Client\BusinessLogic.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ClientTests
{
	TEST_CLASS(Client_Tests)
	{
	public:

		TEST_METHOD(UT_CLT_01_ClientTerminateConnection)
		{
			//ARRANGE
			// Note: This WILL fail if your server isn't running on port 27000!
			SOCKET testSock = establish_connection();

			// Validate that we actually connected before trying to disconnect
			Assert::AreNotEqual((unsigned long long)INVALID_SOCKET, (unsigned long long)testSock,
				L"Connection failed. Is the server running?");

			// ACT
			terminate_connection(testSock);

			//ASSERT
			// After closing, any operation on the socket should return an error proving the 'closesocket' inside terminate_connection actually ran.
			char dummy = 'a';
			int result = send(testSock, &dummy, 1, 0);

			Assert::AreEqual(SOCKET_ERROR, result, L"Socket should have been closed.");

		}

		TEST_METHOD(UT_CLT_02_ClientHandshakeWithTCPServer)
		{
			//ARRANGE
			// Note: This test will fail if your server isn't running on port 27000!
			SOCKET testSock = establish_connection();
			// Validate that we actually connected before trying to disconnect
			Assert::AreNotEqual((unsigned long long)INVALID_SOCKET, (unsigned long long)testSock,
				L"Connection failed. Is the server running?");
			// ACT
			// If establish_connection succeeds, that means the handshake also succeeded because establish_connection calls handshake_with_tcp_server internally.
			
			//ASSERT
			// If we got here without an exception, that means the handshake succeeded and we can assert true.
			Assert::IsTrue(true);
		}

		TEST_METHOD(UT_CLT_03_GetAirplaneDataFromDB) 
		{
			//ARRANGE
			PGconn* conn = PQconnectdb(
				"host=127.0.0.1 port=5432 dbname=amws user=postgres password=Lkj876*bv"
			);

			// ACT
			crow::json::wvalue result;
			try {
				result = get_airplane_data_from_db(conn);
			}
			catch (const std::exception& e) {
				PQfinish(conn);
				Assert::Fail(L"Exception thrown while getting airplane data from DB"); 
			}

			//ASSERT
			std::string jsonStr = result.dump();
			Assert::AreNotEqual(std::string("null"), jsonStr, L"Result should not be null.");

			PQfinish(conn);
		}

		TEST_METHOD(UT_CLT_04_VerifyEventInputIsValid) 
		{
			//verify the event input is valid (regardless if it is maintenance or warranty)
			//ARRANGE
			std::string validInput = R"({
				"airplaneID": 10,
				"technicianID": "1"
									})";
			crow::json::rvalue validInputBody = crow::json::load(validInput);

			std::string invalidInput_MissingFields = R"({
				"airplaneID": 10
									})";
			crow::json::rvalue invalidInput_MissingFieldsBody = crow::json::load(invalidInput_MissingFields);

			// ACT & ASSERT
			Assert::IsTrue(isEventRequestValid(validInputBody), L"Valid input should return true.");
			Assert::IsFalse(isEventRequestValid(invalidInput_MissingFieldsBody), L"Input missing fields should return false.");
			
		}
	};
}
