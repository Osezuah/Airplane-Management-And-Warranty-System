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
			uint32_t expectedSequence = 1;
			uint32_t actualSequence = 0;
			std::string expectedTechnicianID = "1";
			std::string actualTechnicianID = "0";
			std::string expectedName = "mike";
			std::string actualName = "test";
			Packet packet = PacketFactory::Handshake(expectedSequence, expectedTechnicianID, expectedName);
			PacketType expectedType = PacketType::HANDSHAKE;
			PacketType actualType;
			uint32_t payloadLength;

			//ACT
			actualType = packet.getType();
			payloadLength = packet.header.payloadLength;
			actualSequence = packet.header.sequenceNumber;

			crow::json::rvalue body = crow::json::load(packet.payloadString());
			actualTechnicianID = std::string(body["technicianID"].s());
			actualName = std::string(body["name"].s());

			//ASSET
			Assert::AreEqual(static_cast<int>(expectedType), static_cast<int>(actualType), L"PacketFactory::Handshake should create a Packet of type HANDSHAKE.");
			Assert::IsTrue(payloadLength > 0, L"Handshake packet should have a non-empty payload.");
			Assert::IsTrue((bool)body, L"Handshake payload should be valid JSON.");
			Assert::IsTrue(body.has("technicianID"), L"Handshake payload should contain field 'technicianID'.");
			Assert::IsTrue(body.has("name"), L"Handshake payload should contain field 'name'.");
			Assert::AreEqual(expectedSequence, actualSequence, L"sequence should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedTechnicianID, actualTechnicianID, L"technicianID should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedName, actualName, L"name should be equal to the one passed into the factory.");
		}

		TEST_METHOD(IT_PCF_02_MaintenanceEventPacketConstruction)
		{
			// Arrange
			uint32_t expectedSequence = 1;
			uint32_t actualSequence = 0;
			int expectedAirplaneID = 1;
			int actualAirplaneID = 0;
			std::string expectedTechnicianID = "1";
			std::string actualTechnicianID = "0";
			std::string expectedEventType = "Unit Test";
			std::string actualEventType = "none";
			std::string expectedDescription = "Unit Test of Maintenance Event Packet Construction";
			std::string actualDescription = "none";
			Packet packet = PacketFactory::MaintenanceEvent(expectedSequence, expectedAirplaneID, expectedTechnicianID, expectedEventType, expectedDescription);
			PacketType expectedType = PacketType::MAINTENANCE_EVENT;
			PacketType actualType;
			uint32_t payloadLength;

			// Act
			actualType = packet.getType();
			payloadLength = packet.header.payloadLength;
			actualSequence = packet.header.sequenceNumber;

			crow::json::rvalue body = crow::json::load(packet.payloadString());
			actualAirplaneID = (int)body["airplaneID"].i();
			actualTechnicianID = std::string(body["technicianID"].s());
			actualEventType = std::string(body["eventType"].s());
			actualDescription = std::string(body["description"].s());

			// Assert
			Assert::AreEqual(static_cast<int>(expectedType), static_cast<int>(actualType), L"PacketFactory::MaintenanceEvent should create a Packet of type MAINTENANCE_EVENT.");
			Assert::IsTrue(payloadLength > 0, L"MaintenanceEvent packet should have a non-empty payload.");
			Assert::IsTrue((bool)body, L"MaintenanceEvent payload should be valid JSON.");
			Assert::IsTrue(body.has("airplaneID"), L"Payload should contain field 'airplaneID'.");
			Assert::IsTrue(body.has("technicianID"), L"Payload should contain field 'technicianID'.");
			Assert::IsTrue(body.has("eventType"), L"Payload should contain field 'eventType'.");
			Assert::IsTrue(body.has("description"), L"Payload should contain field 'description'.");
			Assert::AreEqual(expectedSequence, actualSequence, L"sequence should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedAirplaneID, actualAirplaneID, L"airplaneID should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedTechnicianID, actualTechnicianID, L"airplaneID should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedEventType, actualEventType, L"eventType should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedDescription, actualDescription, L"description should be equal to the one passed into the factory.");
		}

		TEST_METHOD(IT_PCF_03_WarrantyEventPacketConstruction)
		{
			// Arrange
			uint32_t expectedSequence = 1;
			uint32_t actualSequence = 0;
			int expectedAirplaneID = 1;
			int actualAirplaneID = 0;
			std::string expectedTechnicianID = "1";
			std::string actualTechnicianID = "0";
			int expectedWarrantyID = 1;
			int actualWarrantyID = 0;
			std::string expectedDescription = "Unit Test of Warranty Event Packet Construction";
			std::string actualDescription = "none";
			std::string expectedImageBytes = "insertrandombyteshere";
			std::string atualImageBytes = "insertrandombyteshere";
			Packet packet = PacketFactory::WarrantyEvent(expectedSequence, expectedAirplaneID, expectedTechnicianID, expectedWarrantyID, expectedDescription, expectedImageBytes);
			PacketType expectedType = PacketType::WARRANTY_EVENT;
			PacketType actualType;
			uint32_t payloadLength;

			// Act
			actualType = packet.getType();
			payloadLength = packet.header.payloadLength;
			actualSequence = packet.header.sequenceNumber;

			crow::json::rvalue body = crow::json::load(packet.payloadString());
			actualAirplaneID = (int)body["airplaneID"].i();
			actualTechnicianID = std::string(body["technicianID"].s());
			actualWarrantyID = (int)body["warrantyID"].i();
			actualDescription = std::string(body["description"].s());
			atualImageBytes = std::string(body["imageBytes"].s());

			// Assert
			Assert::AreEqual(static_cast<int>(expectedType), static_cast<int>(actualType), L"PacketFactory::WarrantyEvent should create a Packet of type WARRANTY_EVENT.");
			Assert::IsTrue(packet.header.payloadLength > 0, L"WarrantyEvent packet should have a non-empty payload.");
			Assert::IsTrue((bool)body, L"WarrantyEvent payload should be valid JSON.");
			Assert::IsTrue(body.has("airplaneID"), L"Payload should contain field 'airplaneID'.");
			Assert::IsTrue(body.has("technicianID"), L"Payload should contain field 'technicianID'.");
			Assert::IsTrue(body.has("warrantyID"), L"Payload should contain field 'warrantyID'.");
			Assert::IsTrue(body.has("description"), L"Payload should contain field 'description'.");
			Assert::IsTrue(body.has("imageBytes"), L"Payload should contain field 'imageBytes'.");
			Assert::AreEqual(expectedSequence, actualSequence, L"sequence should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedAirplaneID, actualAirplaneID, L"airplaneID should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedTechnicianID, actualTechnicianID, L"airplaneID should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedWarrantyID, actualWarrantyID, L"warrantyID should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedDescription, actualDescription, L"description should be equal to the one passed into the factory.");
			Assert::AreEqual(expectedImageBytes, atualImageBytes, L"imageBytes should be equal to the one passed into the factory.");
		}
	};

	TEST_CLASS(Logger_Tests)
	{
		//helper function for testing
		//returns true if inputString matches any line in the file.
		static bool FileContains(const std::string& filename, const std::string& inputString)
		{
			std::ifstream file(filename);
			std::string line;
			while (std::getline(file, line))
				if (line.find(inputString) != std::string::npos)
					return true;
			return false;
		}

	public:

		TEST_METHOD(IT_LOG_01_LogStateTransitions)
		{
			// Arrange
			const std::string testFileName = "IT_LOG_01_test.txt";
			std::remove(testFileName.c_str());
			::Logger logger(testFileName);
			StateMachine stateMachine;
			std::string expectedStartState;
			std::string expectedEndState;

			// Act
			expectedStartState = stateMachine.StateToString();
			logger.Log("Task received. State: " + stateMachine.StateToString());
			stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);
			expectedEndState = stateMachine.StateToString();
			logger.Log("Task received. State: " + stateMachine.StateToString());

			// Assert
			Assert::IsTrue(FileContains(testFileName, expectedStartState), L"Log entry should contain the staet state name.");
			Assert::IsTrue(FileContains(testFileName, expectedEndState), L"Log entry should contain the end state name.");
			Assert::IsTrue(FileContains(testFileName, "["), L"Log entry must contain a timestamp bracket.");

			std::remove(testFileName.c_str());
		}
	};
};