// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "CoreCR95HF.h"
#include <cstdint>

#include "gtest/gtest.h"
#include "mocks/leka/CoreBufferedSerial.h"

using namespace leka;
using namespace interface;

using ::testing::Args;
using ::testing::DoAll;
using ::testing::ElementsAre;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

class CoreCR95HFSensorTest : public ::testing::Test
{
  protected:
	CoreCR95HFSensorTest() : corecr95hf(mockBufferedSerial) {};

	// void SetUp() override {}
	// void TearDown() override {}

	CoreCR95HF corecr95hf;
	CoreBufferedSerialMock mockBufferedSerial;

	template <size_t size>

	void setExpectedReveivedData(const std::array<uint8_t, size> &returned_values)
	{
		receiveCR95HFAnswer(returned_values);
		sendSetModeTagDetection();
	};

	void sendSetModeTagDetection()
	{
		const auto expected_values = ElementsAre(rfid::cr95hf::command::frame::set_mode_tag_detection[0],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[1],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[2],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[3],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[4],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[5],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[6],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[7],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[8],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[9],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[10],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[11],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[12],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[13],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[14],
												 rfid::cr95hf::command::frame::set_mode_tag_detection[15]

		);
		EXPECT_CALL(mockBufferedSerial, write).With(Args<0, 1>(expected_values));
	}

	void sendAskIdn()
	{
		const auto expected_values = ElementsAre(rfid::cr95hf::command::idn::id, rfid::cr95hf::command::idn::length);
		EXPECT_CALL(mockBufferedSerial, write).With(Args<0, 1>(expected_values));
	}

	void sendSetBaudrate(uint8_t baudrate)
	{
		const auto expected_values =
			ElementsAre(rfid::cr95hf::command::set_baudrate::id, rfid::cr95hf::command::set_baudrate::length, baudrate);
		EXPECT_CALL(mockBufferedSerial, write).With(Args<0, 1>(expected_values));
	}

	void sendSetProtocol()
	{
		const auto expected_values =
			ElementsAre(rfid::cr95hf::command::set_protocol::id, rfid::cr95hf::command::set_protocol::length,
						rfid::cr95hf::protocol::iso14443A.id, rfid::cr95hf::settings::default_rx_tx_speed);
		EXPECT_CALL(mockBufferedSerial, write).With(Args<0, 1>(expected_values));
	}

	void sendSetGainAndModulation()
	{
		const auto expected_values = ElementsAre(rfid::cr95hf::command::set_gain_and_modulation::id,
												 rfid::cr95hf::command::set_gain_and_modulation::length,
												 rfid::cr95hf::settings::arc_b, rfid::cr95hf::settings::flag_increment,
												 rfid::cr95hf::settings::acr_b_index_for_gain_and_modulation,
												 rfid::cr95hf::protocol::iso14443A.gain_modulation_values());
		EXPECT_CALL(mockBufferedSerial, write).With(Args<0, 1>(expected_values));
	}

	template <size_t size>
	void receiveCR95HFAnswer(const std::array<uint8_t, size> &returned_values)
	{
		EXPECT_CALL(mockBufferedSerial, readable).WillOnce(Return(true));
		EXPECT_CALL(mockBufferedSerial, read)
			.WillOnce(DoAll(SetArrayArgument<0>(begin(returned_values), begin(returned_values) + size), Return(size)));
	}

	void receiveSetBaudrate(uint8_t returned_value, int answer_size)
	{
		EXPECT_CALL(mockBufferedSerial, readable).WillOnce(Return(true));
		EXPECT_CALL(mockBufferedSerial, read).WillOnce(DoAll(SetArgPointee<0>(returned_value), Return(answer_size)));
	}
};

TEST_F(CoreCR95HFSensorTest, initialization)
{
	ASSERT_NE(&corecr95hf, nullptr);
}

TEST_F(CoreCR95HFSensorTest, enableTagDetection)
{
	EXPECT_CALL(mockBufferedSerial, sigio).Times(1);

	corecr95hf.init();
}

void callback() {};
TEST_F(CoreCR95HFSensorTest, registerTagAvailableCallback)
{
	corecr95hf.registerTagAvailableCallback(callback);
}

TEST_F(CoreCR95HFSensorTest, onTagAvailableSuccess)
{
	std::array<uint8_t, 3> expected_tag_detection_callback = {0x00, 0x01, 0x02};

	corecr95hf.registerTagAvailableCallback(callback);
	{
		InSequence seq;
		receiveCR95HFAnswer(expected_tag_detection_callback);
		sendSetModeTagDetection();
	}

	corecr95hf.onTagAvailable();
}
TEST_F(CoreCR95HFSensorTest, onTagAvailableWrongCallback)
{
	std::array<uint8_t, 3> expected_tag_detection_callback = {0x00, 0x01, 0xff};

	corecr95hf.registerTagAvailableCallback(callback);

	receiveCR95HFAnswer(expected_tag_detection_callback);

	corecr95hf.onTagAvailable();
}

TEST_F(CoreCR95HFSensorTest, getIDNSuccess)
{
	std::array<uint8_t, 17> expected_idn = {0x00, 0x0F, 0x4E, 0x46, 0x43, 0x20, 0x46, 0x53, 0x32,
											0x4A, 0x41, 0x53, 0x54, 0x34, 0x00, 0x2A, 0xCE};

	{
		InSequence seq;
		setExpectedReveivedData(expected_idn);

		sendAskIdn();
	}

	corecr95hf.onTagAvailable();
	auto idn = corecr95hf.getIDN();
	ASSERT_EQ(idn, expected_idn);
}

TEST_F(CoreCR95HFSensorTest, getIDNFailedOnSize)
{
	std::array<uint8_t, 15> wrong_idn = {0x00, 0x0F, 0x4E, 0x46, 0x43, 0x20, 0x46, 0x53,
										 0x32, 0x4A, 0x41, 0x53, 0x54, 0x34, 0x00};

	std::array<uint8_t, 17> expected_idn = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	{
		InSequence seq;
		setExpectedReveivedData(wrong_idn);
		sendAskIdn();
	}

	corecr95hf.onTagAvailable();
	auto idn = corecr95hf.getIDN();
	ASSERT_EQ(idn, expected_idn);
}

TEST_F(CoreCR95HFSensorTest, getIDNFailedOnValues)
{
	std::array<uint8_t, 17> expected_idn = {0xFF, 0xFF, 0x4E, 0x46, 0x43, 0x20, 0x46, 0x53, 0x32,
											0x4A, 0x41, 0x53, 0x54, 0x34, 0x00, 0x2A, 0xCE};

	{
		InSequence seq;
		setExpectedReveivedData(expected_idn);
		sendAskIdn();
	}

	corecr95hf.onTagAvailable();
	auto idn = corecr95hf.getIDN();
	ASSERT_NE(idn, expected_idn);
}

TEST_F(CoreCR95HFSensorTest, setBaudrateSuccess)
{
	uint8_t expected_baudrate = 0x55;

	{
		InSequence seq;
		receiveSetBaudrate(expected_baudrate, 1);
		sendSetModeTagDetection();
		sendSetBaudrate(expected_baudrate);
	}

	corecr95hf.onTagAvailable();
	auto baudrate = corecr95hf.setBaudrate(expected_baudrate);
	ASSERT_EQ(baudrate, true);
}

TEST_F(CoreCR95HFSensorTest, setBaudrateFailedOnSize)
{
	uint8_t expected_baudrate = 0x55;

	{
		InSequence seq;
		receiveSetBaudrate(expected_baudrate, 2);
		sendSetModeTagDetection();
		sendSetBaudrate(expected_baudrate);
	}

	corecr95hf.onTagAvailable();
	auto baudrate = corecr95hf.setBaudrate(expected_baudrate);
	ASSERT_EQ(baudrate, false);
}

TEST_F(CoreCR95HFSensorTest, setBaudrateFailedOnValue)
{
	uint8_t expected_baudrate = 0x55;
	uint8_t wrong_baudrate	  = 0xff;

	{
		InSequence seq;
		receiveSetBaudrate(wrong_baudrate, 1);
		sendSetModeTagDetection();
		sendSetBaudrate(expected_baudrate);
	}

	corecr95hf.onTagAvailable();
	auto baudrate = corecr95hf.setBaudrate(expected_baudrate);
	ASSERT_EQ(baudrate, false);
}

TEST_F(CoreCR95HFSensorTest, setCommunicationProtocolSuccess)
{
	std::array<uint8_t, 2> set_protocol_success_answer			  = {0x00, 0x00};
	std::array<uint8_t, 2> set_gain_and_modulation_success_answer = {0x00, 0x00};

	{
		InSequence seq;

		setExpectedReveivedData(set_protocol_success_answer);
		sendSetProtocol();
		sendSetGainAndModulation();
	}

	corecr95hf.onTagAvailable();
	auto is_initialized = corecr95hf.setCommunicationProtocol(rfid::Protocol::ISO14443A);
	ASSERT_EQ(is_initialized, true);
}

TEST_F(CoreCR95HFSensorTest, setCommunicationProtocolFailedOnAnswerTooBig)
{
	std::array<uint8_t, 3> set_protocol_failed_answer = {0x00, 0x00, 0x00};
	{
		InSequence seq;
		setExpectedReveivedData(set_protocol_failed_answer);
		sendSetProtocol();
	}

	corecr95hf.onTagAvailable();
	auto is_initialized = corecr95hf.setCommunicationProtocol(rfid::Protocol::ISO14443A);
	ASSERT_EQ(is_initialized, false);
}

TEST_F(CoreCR95HFSensorTest, setCommunicationProtocolFailedOnWrongFirstValue)
{
	std::array<uint8_t, 2> set_protocol_failed_answer = {0x82, 0x00};
	{
		InSequence seq;

		setExpectedReveivedData(set_protocol_failed_answer);
		sendSetProtocol();
	}

	corecr95hf.onTagAvailable();
	auto is_initialized = corecr95hf.setCommunicationProtocol(rfid::Protocol::ISO14443A);
	ASSERT_EQ(is_initialized, false);
}

TEST_F(CoreCR95HFSensorTest, sendCommandSuccess)
{
	std::array<uint8_t, 2> command = {0x26, 0x07};
	const auto expected_values =
		ElementsAre(rfid::cr95hf::command::send_receive, command.size(), command[0], command[1]);

	EXPECT_CALL(mockBufferedSerial, write).With(Args<0, 1>(expected_values));

	corecr95hf.sendCommandToTag(command);
}

TEST_F(CoreCR95HFSensorTest, receiveDataSuccess)
{
	std::array<uint8_t, 23> read_values = {0x80, 0x15, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02,
										   0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0xDA, 0x48, 0x28, 0x00, 0x00};

	std::array<uint8_t, 18> expected_values = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01,
											   0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0xDA, 0x48};

	std::array<uint8_t, 18> actual_values {0};

	setExpectedReveivedData(read_values);

	corecr95hf.onTagAvailable();
	uint8_t is_communication_succeed = corecr95hf.receiveDataFromTag(actual_values);

	ASSERT_EQ(is_communication_succeed, true);
	ASSERT_EQ(actual_values, expected_values);
}

TEST_F(CoreCR95HFSensorTest, receiveDataFailedWrongAnswerFlag)
{
	std::array<uint8_t, 7> read_values = {0xff, 0x05, 0x44, 0x00, 0x28, 0x00, 0x00};
	std::array<uint8_t, 7> actual_values {0};

	setExpectedReveivedData(read_values);

	corecr95hf.onTagAvailable();

	uint8_t is_communication_succeed = corecr95hf.receiveDataFromTag(actual_values);

	ASSERT_EQ(is_communication_succeed, false);
	ASSERT_NE(actual_values, read_values);
}

TEST_F(CoreCR95HFSensorTest, receiveDataFailedWrongLength)
{
	std::array<uint8_t, 7> read_values = {0x80, 0x02, 0x44, 0x00, 0x28, 0x00, 0x00};
	std::array<uint8_t, 7> actual_values {0};

	setExpectedReveivedData(read_values);

	corecr95hf.onTagAvailable();

	uint8_t is_communication_succeed = corecr95hf.receiveDataFromTag(actual_values);

	ASSERT_EQ(is_communication_succeed, false);
	ASSERT_NE(actual_values, read_values);
}
