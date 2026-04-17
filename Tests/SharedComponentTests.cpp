#include "pch.h"
#include "CppUnitTest.h"
#include "Logger.h"
#include "StateMachine.h"
#include "Packet.h"
#include "PacketFactory.h"
#include <fstream>
#include <string>
#include <cstdio>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SharedComponentTests
{
	TEST_CLASS(Packet_Tests)
	{
	public:

		TEST_METHOD(UT_PCK_01_FixedHeaderPacketLength)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(UT_PCK_02_CorrectlySetPackageType)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(UT_PCK_03_CorrectPayloadLength)
		{
			//ARRANGE


			//ACT


			//ASSET

		}

		TEST_METHOD(UT_PCK_04_SerializeThenDesirialize)
		{
			//ARRANGE


			//ACT


			//ASSET

		}
	};

	TEST_CLASS(Logger_Tests)
	{
	public:

		TEST_METHOD(UT_Log_01_LoggerCreatesAndWritesToFile)
		{
			//ARRANGE


			//ACT


			//ASSET

		}
	};
}
