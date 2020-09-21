#include "brs_test.hpp"
#define BRS_BRICKADIA_IMPLEMENTATION_H
#include "../brs/brs.hpp"
#include <iostream>

void testUUIDConstructor() {
	BRS_TEST_BEGIN("UUID Constructor");

	UUID test(LLONG_MAX,LLONG_MAX);

	BRS_TEST_END();
}

void testUUIDEquality() {
	BRS_TEST_BEGIN("UUID Equality");

	UUID test(LLONG_MAX,LLONG_MAX);
	UUID test2(LLONG_MAX,LLONG_MAX);

	BRS_ASSERT_THROW(test == test2);
	BRS_TEST_END();
}

void testUUIDInequality() {
	BRS_TEST_BEGIN("UUID Equality");

	UUID test(LLONG_MAX,LLONG_MAX);
	UUID test2(0,0);

	BRS_ASSERT_THROW(test != test2);
	BRS_TEST_END();
}

int main(int, char**)
{
	testUUIDConstructor();
	testUUIDEquality();
	testUUIDInequality();
}