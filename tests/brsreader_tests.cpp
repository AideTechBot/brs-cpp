#include "brs_test.hpp"
#define BRS_BRICKADIA_IMPLEMENTATION_H
#include "../brs/brs.hpp"
#include <iostream>

void testBRSReaderConstructor() {
	BRS_TEST_BEGIN("BRS Reader Constructor");

	BRS::Reader test("test.brs");
	BRS_ASSERT_THROW(test.version == BRS::Version::AddedDateTime);
	BRS_ASSERT_THROW(test.gameVersion == 3642);

	BRS_TEST_END();
}

void testBRSReaderHeader1() {
	BRS_TEST_BEGIN("BRS Reader Header1 Reader");

	BRS::Reader test("test2.brs");
	test.readHeader1();

	BRS_ASSERT_THROW(test.loadedHeader1());

	BRS::Header1 header1 = test.getHeader1();
	
	BRS_ASSERT_THROW(header1.map == u"Plate");
	BRS_ASSERT_THROW(header1.author.name == u"Aide33");
	BRS_ASSERT_THROW(header1.author.uuid.toString() == "e4f5c60b-1407-42ff-ad1e-98a67b25c2da");
	BRS_ASSERT_THROW(header1.saveTime.has_value());
	BRS_ASSERT_THROW(header1.saveTime.value() == 637360765302670000);
	BRS_ASSERT_THROW(header1.brickCount == 1);

	BRS_TEST_END();
}

int main(int, char**)
{
	testBRSReaderConstructor();
	testBRSReaderHeader1();
}
