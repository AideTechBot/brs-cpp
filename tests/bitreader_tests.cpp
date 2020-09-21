#include "brs_test.hpp"
#define BRS_BRICKADIA_IMPLEMENTATION_H
#include "../brs/brs.hpp"
#include <iostream>

void testReaduint32BitReader() {
	BRS_TEST_BEGIN("BitReader uint32");

	std::vector<uint8_t> buf = { 0b00000100, 0b00000000, 0b00000000, 0b00000000 };

	BitReader r(buf);
	uint32_t val = r.read_uint32(0xffffffff);

	BRS_ASSERT_THROW(val == 4);

	BRS_TEST_END();
}

void testReaduint32MaxBitReader() {
	BRS_TEST_BEGIN("BitReader uint32 max");

	std::vector<uint8_t> buf = { 0b00000100, 0b00000000, 0b00000000, 0b00000000 };

	BitReader r(buf);
	uint32_t val = r.read_uint32(0);

	BRS_ASSERT_THROW(val == 0);

	BRS_TEST_END();
}

void testReadBitBitReader() {
	BRS_TEST_BEGIN("BitReader bit");

	std::vector<uint8_t> buf = { 0b00000100, 0b00000000, 0b00000000, 0b10000000 };

	BitReader r(buf);

	BRS_ASSERT_THROW(!r.read_bit());
	BRS_ASSERT_THROW(!r.read_bit());
	BRS_ASSERT_THROW(r.read_bit());
	for(int i = 0; i < 28; i++)
	{
		BRS_ASSERT_THROW(!r.read_bit());
	}
	BRS_ASSERT_THROW(r.read_bit());

	BRS_TEST_END();
}

void testAlignBitReader() {
	BRS_TEST_BEGIN("BitReader align");

	std::vector<uint8_t> buf = { 0b00000100, 0b00001111 };

	BitReader r(buf);

	BRS_ASSERT_THROW(!r.read_bit());
	BRS_ASSERT_THROW(!r.read_bit());
	BRS_ASSERT_THROW(r.read_bit());
	for(int i = 0; i < 5; i++)
	{
		r.read_bit();
	}
	r.align();
	for(int i = 0; i < 4; i++)
	{
		BRS_ASSERT_THROW(r.read_bit());
	}
	for(int i = 0; i < 4; i++)
	{
		BRS_ASSERT_THROW(!r.read_bit());
	}

	BRS_TEST_END();
}

int main(int, char**)
{
	testReaduint32BitReader();
	testReaduint32MaxBitReader();
	testReadBitBitReader();
	testAlignBitReader();
}
