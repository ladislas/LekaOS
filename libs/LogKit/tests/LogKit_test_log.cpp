// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include <string>

#include "LogKit.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "stubs/mbed/Kernel.h"

using namespace leka;

using ::testing::HasSubstr;
using ::testing::MatchesRegex;

class LogKitTest : public ::testing::Test
{
  protected:
	void SetUp() override
	{
		spy_sink_output = "";
		logger::set_sink_function(spy_sink_function);
	}

	void TearDown() override { logger::set_sink_function(logger::default_sink_function); }

	static void spy_sink_function(const char *str, size_t size)
	{
		spy_sink_output = std::string {str};
		std::cout << spy_sink_output;
	}

	static inline auto spy_sink_output = std::string {};
};

TEST_F(LogKitTest, init)
{
	logger::init();
}

TEST_F(LogKitTest, logDebug)
{
	log_debug("Hello, World");

	ASSERT_THAT(spy_sink_output, HasSubstr("[DBUG]"));
	ASSERT_THAT(spy_sink_output, HasSubstr("Hello, World"));
}

TEST_F(LogKitTest, logInfo)
{
	log_info("Hello, World");

	ASSERT_THAT(spy_sink_output, HasSubstr("[INFO]"));
	ASSERT_THAT(spy_sink_output, HasSubstr("Hello, World"));
}

TEST_F(LogKitTest, logError)
{
	log_error("Hello, World");

	ASSERT_THAT(spy_sink_output, HasSubstr("[ERR ]"));
	ASSERT_THAT(spy_sink_output, HasSubstr("Hello, World"));
}

TEST_F(LogKitTest, correctLineNumber)
{
	auto line = std::to_string(__LINE__ + 1);	// + 1 to count the next line
	log_info("Line number: %s", line.c_str());

	ASSERT_THAT(spy_sink_output, MatchesRegex(".+ \\[.+[.h|.cpp]:" + line + "\\] .*"));
}

TEST_F(LogKitTest, variadicArguments)
{
	log_info("%s, %s with %i %s", "Hello", "World", 4, "variadic arguments");

	ASSERT_THAT(spy_sink_output, MatchesRegex(".+ \\[INFO\\] \\[.*\\] .* > .* with 4 variadic arguments\n"));
}

TEST_F(LogKitTest, formatFullContentStringOnly)
{
	log_debug("Hello, World");

	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+() > .+"));

	log_info("Hello, World");

	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+() > .+"));

	log_error("Hello, World");

	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+() > .+"));
}

TEST_F(LogKitTest, formatFullContentStringAdditionalArguments)

{
	log_debug("Hello, World. %i %s!", 42, "FTW");

	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+() > .+"));

	log_info("Hello, World. %i %s!", 42, "FTW");

	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+() > .+"));

	log_error("Hello, World. %i %s!", 42, "FTW");

	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+() > .+"));
}

TEST_F(LogKitTest, formatFullContentStringEmpty)
{
	log_debug("");

	ASSERT_THAT(spy_sink_output, Not(HasSubstr(" > ")));
	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+()"));

	log_info("");

	ASSERT_THAT(spy_sink_output, Not(HasSubstr(" > ")));
	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+()"));

	log_error("");

	ASSERT_THAT(spy_sink_output, Not(HasSubstr(" > ")));
	ASSERT_THAT(spy_sink_output, MatchesRegex("[0-9:]+ \\[[A-Z ]+\\] \\[.+:[0-9]+\\] .+()"));
}
