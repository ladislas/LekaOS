// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "RFIDKit.h"
#include <cstddef>
#include <cstdint>
#include <lstd_span>

#include "CoreCR95HF.h"
#include "gtest/gtest.h"
#include "mocks/leka/CoreBufferedSerial.h"
#include "mocks/leka/CoreRFID.h"

using namespace leka;
using namespace interface;

using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SetArgPointee;

class CoreRFIDKitTest : public ::testing::Test
{
  protected:
	CoreRFIDKitTest() : coreRfid(mockCR95HF) {};

	// void SetUp() override {}
	// void TearDown() override {}

	RFIDKit coreRfid;
	CoreRFIDMock mockCR95HF;
	CoreBufferedSerialMock mockBufferedSerial;

	template <size_t size>
	void receive(std::array<uint8_t, size> &data)
	{
		lstd::span<uint8_t> span {data};
		EXPECT_CALL(mockCR95HF, receiveTagData).WillRepeatedly(DoAll(SetArgPointee<0>(span), Return(size)));
	}

	std::array<uint8_t, 2> REQA = {0x26, 0x07};
	lstd::span<uint8_t> REQA_span {REQA};
	std::array<uint8_t, 2> ATQA			   = {0x44, 0x00};
	std::array<uint8_t, 3> read_register_8 = {0x30, 0x08, 0x28};
	lstd::span<uint8_t> read_register_8_span {read_register_8};
	std::array<uint8_t, 16> actual_values {0};
};

TEST_F(CoreRFIDKitTest, initialization)
{
	ASSERT_NE(&coreRfid, nullptr);
}

void interruptFunction() {};
TEST_F(CoreRFIDKitTest, setInterruptSuccess)
{
	EXPECT_CALL(mockCR95HF, getSerial).WillOnce(ReturnRef(mockBufferedSerial));
	EXPECT_CALL(mockBufferedSerial, sigio).Times(1);

	coreRfid.setInterrupt(interruptFunction);
}

TEST_F(CoreRFIDKitTest, waitForTagDetectionTEST)
{
	EXPECT_CALL(mockCR95HF, enableTagDetection).Times(1);

	coreRfid.setReaderForTagDetection();
}

MATCHER_P(compareSpan, expected_span, "")
{
	auto actual_span_size	= arg.size();
	auto expected_span_size = expected_span.size();

	if (actual_span_size != expected_span_size) {
		return false;
	}

	auto same_span = bool {true};

	for (auto i = 0; i < actual_span_size; i++) {
		same_span = same_span && (arg[i] == expected_span[i]);
	}

	return same_span;
}

TEST_F(CoreRFIDKitTest, getTagDataSuccess)
{
	{
		InSequence seq;

		EXPECT_CALL(mockCR95HF, receiveCallback).WillOnce(Return(true));
		EXPECT_CALL(mockCR95HF, setup).WillOnce(Return(true));

		EXPECT_CALL(mockCR95HF, send(compareSpan(REQA_span))).Times(1);

		receive(ATQA);

		EXPECT_CALL(mockCR95HF, send(compareSpan(read_register_8_span))).Times(1);

		std::array<uint8_t, 18> tagData = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01,
										   0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x24, 0xF5};
		receive(tagData);
	}

	std::array<uint8_t, 16> expected_values = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
											   0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

	bool is_communication_succeed = coreRfid.getTagData(actual_values);

	ASSERT_EQ(is_communication_succeed, true);
	ASSERT_EQ(actual_values, expected_values);
}

TEST_F(CoreRFIDKitTest, getTagDataFailedOnWrongCallbackValues)
{
	EXPECT_CALL(mockCR95HF, receiveCallback).WillOnce(Return(false));

	auto is_initialized = coreRfid.getTagData(actual_values);
	ASSERT_EQ(is_initialized, false);
}

TEST_F(CoreRFIDKitTest, getTagDataFailedOnWrongCallback)
{
	EXPECT_CALL(mockCR95HF, receiveCallback).WillOnce(Return(false));

	auto is_initialized = coreRfid.getTagData(actual_values);
	ASSERT_EQ(is_initialized, false);
}

TEST_F(CoreRFIDKitTest, getTagDataFailedOnWrongSetProtocol)
{
	EXPECT_CALL(mockCR95HF, receiveCallback).WillOnce(Return(true));
	EXPECT_CALL(mockCR95HF, setup).WillOnce(Return(false));

	auto is_initialized = coreRfid.getTagData(actual_values);
	ASSERT_EQ(is_initialized, false);
}

TEST_F(CoreRFIDKitTest, getTagDataFailedOnWrongATQA)
{
	EXPECT_CALL(mockCR95HF, receiveCallback).WillOnce(Return(true));
	EXPECT_CALL(mockCR95HF, setup).WillOnce(Return(true));

	EXPECT_CALL(mockCR95HF, send(compareSpan(REQA_span))).Times(1);

	ATQA = {0xFF, 0xFF};
	receive(ATQA);

	auto is_initialized = coreRfid.getTagData(actual_values);
	ASSERT_EQ(is_initialized, false);
}

TEST_F(CoreRFIDKitTest, getTagDataFailedOnWrongCRC)
{
	{
		InSequence seq;

		EXPECT_CALL(mockCR95HF, receiveCallback).WillOnce(Return(true));
		EXPECT_CALL(mockCR95HF, setup).WillOnce(Return(true));

		EXPECT_CALL(mockCR95HF, send(compareSpan(REQA_span))).Times(1);

		receive(ATQA);

		EXPECT_CALL(mockCR95HF, send(compareSpan(read_register_8_span))).Times(1);

		std::array<uint8_t, 18> tagData = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01,
										   0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0xFF, 0xFF};
		receive(tagData);
	}

	auto is_initialized = coreRfid.getTagData(actual_values);
	ASSERT_EQ(is_initialized, false);
}
