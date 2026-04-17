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
			// Arrange
			Packet testPacket(PacketType::ACK, 1, std::vector<uint8_t>{});
			const size_t expectedSize = PACKETHEADER_BYTE_SIZE;
			size_t actualSize = 0;

			// Act
			actualSize = sizeof(testPacket.header);

			// Assert
			Assert::AreEqual(expectedSize, actualSize, L"PacketHeader should be exactly 17 bytes. It is possible the compiler added padding.");
		}

		TEST_METHOD(UT_PCK_02_CorrectlySetPackageType)
		{
			// Arrange
			PacketType expectedType = PacketType::MAINTENANCE_EVENT;
			std::vector<uint8_t> payload = { 'a', 'b', 'c' };
			Packet testPacket(expectedType, 1, payload);

			// Act
			PacketType actualType = testPacket.getType();

			// Assert
			Assert::AreEqual(static_cast<int>(expectedType), static_cast<int>(actualType), L"Packet type should be the same as the one passed to the constructor.");
		}

		TEST_METHOD(UT_PCK_03_CorrectPayloadLength)
		{
			// Arrange
			std::vector<uint8_t> payload = { 1, 2, 3, 4, 5 };
			uint32_t expectedLength = static_cast<uint32_t>(payload.size());
			Packet packet(PacketType::ACK, 1, payload);

			// Act
			uint32_t actualLength = packet.header.payloadLength;

			// Assert
			Assert::AreEqual(expectedLength, actualLength, L"header.payloadLength should be the same as the number of bytes as in the payload.");
		}

		TEST_METHOD(UT_PCK_04_SerializeThenDesirialize)
		{
			// Arrange
			std::string expectedPayloadText = "round trip test";
			std::string actualPayloadText = "none";
			PacketType expectedType = PacketType::MAINTENANCE_EVENT;
			PacketType actualType = PacketType::ACK;
			uint32_t expectedSequence = 1;
			uint32_t actualSequence = 0;
			std::vector<uint8_t> payloadBytes(expectedPayloadText.begin(), expectedPayloadText.end());
			Packet testPacket(expectedType, expectedSequence, payloadBytes);
			uint32_t expectedPayloadLength = testPacket.payloadSize();
			uint32_t actualPayloadLength = 0;

			// Act
			std::vector<uint8_t> serialized = testPacket.Serialize();
			Packet desirializedPacket = Packet::Deserialize(serialized.data(), serialized.size(), false);
			actualPayloadText = desirializedPacket.payloadString();
			actualType = desirializedPacket.getType();
			actualSequence = desirializedPacket.getSequence();
			actualPayloadLength = desirializedPacket.payloadSize();


			// Assert
			Assert::AreEqual (static_cast<int>(expectedType), static_cast<int>(actualType), L"Packet type should be the same before and after serialization and desirialization.");
			Assert::AreEqual(expectedSequence, actualSequence, L"Sequence number should be the same before and after serialization and desirialization.");
			Assert::AreEqual(expectedPayloadText, actualPayloadText, L"Payload contents should be the same before and after serialization and desirialization.");
			Assert::AreEqual(expectedPayloadLength, actualPayloadLength, L"payloadLength field should be the same before and after serialization and desirialization.");
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
			bool result = false;

			{ // scope needed to destroy logger so file can be accessed later
				::Logger logger(testFileName);
				// Act
				logger.Log(expectedMessage);
			}

			std::ifstream file(testFileName);
			Assert::IsTrue(file.is_open(), L"Logger should create the log file on construction.");

			std::ifstream testFile(testFileName);
			std::string line;
			while (std::getline(testFile, line))
				if (line.find(expectedMessage) != std::string::npos)
					result = true;

			// Assert
			Assert::IsTrue(result, L"Logger should write the logged message to the file.");

			std::remove(testFileName.c_str());
		}
	};
}
