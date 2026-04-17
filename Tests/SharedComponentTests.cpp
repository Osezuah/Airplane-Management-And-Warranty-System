#include "pch.h"
#include "CppUnitTest.h"
#include "Logger.h"
#include "StateMachine.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Logger.h"
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
			// Arrange
			const std::string testFileName = "UT_LOG_01_test.txt";
			std::remove(testFileName.c_str());
			const std::string expectedMessage = "UT-LOG-01 unit test";
			::Logger logger(testFileName);
			bool result = false;

			// Act
			logger.Log(expectedMessage);
			logger.~Logger();

			std::ifstream file(testFileName);
			Assert::IsTrue(file.is_open(),
				L"Logger should create the log file on construction.");

			std::ifstream testFile(testFileName);
			std::string line;
			while (std::getline(testFile, line))
				if (line.find(expectedMessage) != std::string::npos)
					result = true;

			// Assert
			Assert::IsTrue(result,
				L"Logger should write the logged message to the file.");

			std::remove(testFileName.c_str());
		}
	};
}
