#include <gtest/gtest.h>
#include "../mocks/Stream.h" 
#include "../mocks/Arduino.h"
#include "../../src/VEDirect.h"
#include <map>
#include <string>

// raw data - correct frame
const char correctFrame[] = 
	"\r\nPID\t0x203\r\n"
	"V\t26201\r\n"
	"I\t0\r\n"
	"P\t0\r\n"
	"CE\t0\r\n"
	"SOC\t1000\r\n"
	"TTG\t-1\r\n"
	"Alarm\tOFF\r\n"
	"Relay\tOFF\r\n"
	"AR\t0\r\n"
	"BMV\t700\r\n"
	"FW\t0307\r\n"
	"Checksum\t\xD8\r\n";
	
// raw data - faulty frame
const char faultyFrame[] = 
	"\r\nPID\t0x203\r\n"
	"V\t26201\r\n"
	"I\t0\r\n"
	"P\t0\r\n"
	"CE\t0\r\n"
	"SOC\t1000\r\n"
	"TTG\t-1\r\n"
	"Alarm\tON\r\n"  // OFF => ON
	"Relay\tOFF\r\n"
	"AR\t0\r\n"
	"BMV\t700\r\n"
	"FW\t0307\r\n"
	"Checksum\t\xD8\r\n";

// parsed data - correct frame
const std::map<std::string, std::string> correctFrameParsed = {
	{"PID", "0x203"},
	{"V", "26201"},
	{"I", "0"},
	{"P", "0"},
	{"CE", "0"},
	{"SOC", "1000"},
	{"TTG", "-1"},
	{"Alarm", "OFF"},
	{"Relay", "OFF"},
	{"AR", "0"},
	{"BMV", "700"},
	{"FW", "0307"},
	{"Checksum", "\xD8"},
};

// parsed data - faulty frame
const std::map<std::string, std::string> faultyFrameParsed = {
	{"PID", "0x203"},
	{"V", "26201"},
	{"I", "0"},
	{"P", "0"},
	{"CE", "0"},
	{"SOC", "1000"},
	{"TTG", "-1"},
	{"Alarm", "ON"},
	{"Relay", "OFF"},
	{"AR", "0"},
	{"BMV", "700"},
	{"FW", "0307"},
	{"Checksum", "\xD8"},
};


/**
* Ensures the whole frame, parsed as (label, value) pairs is passed to the consumer
* if the checksum was correct.
*/
TEST(VEDirectTest, ParsesFullFrameCorrectly) {
	
	// assert
	auto handler = [](const char* label, const char* value) {
		std::cout << "LABEL: " << label << " VALUE: " << correctFrameParsed.at(label) << std::endl; 
        ASSERT_TRUE(correctFrameParsed.at(label) == value) << "Mismatch for label: " << label;
    };
	auto logger = [](const char* message) {
		ASSERT_NE(std::string(message), "Invalid Checksum");
    };
	
	// prepare
    MockStream stream;
    VEDirect veDirect(&stream, handler, logger);
    stream.write(correctFrame);

    // rest
    veDirect.read();
}


/**
* Ensures the frame is not passed to the consumer if the checksum was incorrect.
*/
TEST(VEDirectTest, FiltersFaultyFrame) {
	
	// assert
	auto handler = [](const char* label, const char* value) {
		FAIL() << "Unexpected call to handler with label: " << label << " and value: " << value;
    };
	auto logger = [](const char* message) {
		ASSERT_NE(std::string(message), "Valid Checksum");
    };
	
	// prepare
    MockStream stream;
    VEDirect veDirect(&stream, handler, logger);
    stream.write(faultyFrame);

    // rest
    veDirect.read();
}

/**
* Ensures the checksum calculation can be skipped. This will result in faulty frames being sent to the consumer.
*/
TEST(VEDirectTest, ChecksumCalculationCanBeDisabled) {
	
	// assert
	auto handler = [](const char* label, const char* value) {
		std::cout << "LABEL: " << label << " VALUE: " << faultyFrameParsed.at(label) << std::endl; 
        ASSERT_TRUE(faultyFrameParsed.at(label) == value) << "Mismatch for label: " << label;
    };
	auto logger = [](const char* message) {
		ASSERT_NE(std::string(message), "Valid Checksum");
    };
	
	// prepare
    MockStream stream;
    VEDirect veDirect(&stream, handler, logger, 256, false);
    stream.write(faultyFrame);

    // rest
    veDirect.read();
}


// gtest entry point
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}