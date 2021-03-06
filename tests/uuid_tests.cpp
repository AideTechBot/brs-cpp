#include "brs_test.hpp"
#define BRS_BRICKADIA_IMPLEMENTATION_H
#include "../brs/brs.hpp"
#include <iostream>

void testUUIDConstructor() {
	BRS_TEST_BEGIN("UUID Constructor");

	BRS::UUID test(LLONG_MAX,LLONG_MAX);

	BRS_TEST_END();
}

void testUUIDEquality() {
	BRS_TEST_BEGIN("UUID Equality");

	BRS::UUID test(LLONG_MAX,LLONG_MAX);
	BRS::UUID test2(LLONG_MAX,LLONG_MAX);

	BRS_ASSERT_THROW(test == test2);
	BRS_TEST_END();
}

void testUUIDInequality() {
	BRS_TEST_BEGIN("UUID Inequality");

	BRS::UUID test(LLONG_MAX,LLONG_MAX);
	BRS::UUID test2(0,0);

	BRS_ASSERT_THROW(test != test2);
	BRS_TEST_END();
}

void testUUIDString() {
	BRS_TEST_BEGIN("UUID String");

	BRS::UUID test(1443195872420939275, 8873712885266552998);

	BRS_ASSERT_THROW(test.toString() == "e4f5c60b-1407-42ff-ad1e-98a67b25c2da");
	BRS_TEST_END();
}

void testUUIDString2() {
	BRS_TEST_BEGIN("UUID String 2");

	BRS::UUID test(0, 0);

	BRS_ASSERT_THROW(test.toString() == "00000000-0000-0000-0000-000000000000");
	BRS_TEST_END();
}

void testUUIDString3() {
	BRS_TEST_BEGIN("UUID String 3");

	BRS::UUID test(UINT64_MAX, UINT64_MAX);

	BRS_ASSERT_THROW(test.toString() == "ffffffff-ffff-ffff-ffff-ffffffffffff");
	BRS_TEST_END();
}

int main(int, char**)
{
	testUUIDConstructor();
	testUUIDEquality();
	testUUIDInequality();
	testUUIDString();
	testUUIDString2();
	testUUIDString3();
}