#include "pch.h"
#include "CppUnitTest.h"
#include "..\Client\SocketConnection.h"

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

			//ASSET
			// After closing, any operation on the socket should return an error proving the 'closesocket' inside terminate_connection actually ran.
			char dummy = 'a';
			int result = send(testSock, &dummy, 1, 0);

			Assert::AreEqual(SOCKET_ERROR, result, L"Socket should have been closed.");

		}
	};
}
