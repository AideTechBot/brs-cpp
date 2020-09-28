#include "brs_test.hpp"
#define BRS_BRICKADIA_IMPLEMENTATION_H
#include "../brs/brs.hpp"
#include <iostream>

void testReaduint32BitReader()
{
	BRS_TEST_BEGIN("Color Inequality");

	BRS::Color one(255, 255, 255, 255);

	BRS::Color two(255, 255, 255, 0);

	BRS_ASSERT_THROW(one != two);

	BRS_TEST_END();
}

void testEqColor()
{
	BRS_TEST_BEGIN("Color Equality");

	BRS::Color one(255, 255, 255, 255);

	BRS_ASSERT_THROW(one == one);

	BRS_TEST_END();
}

void testSetColor()
{
	BRS_TEST_BEGIN("Color Set RGBA");

	BRS::Color one(255, 255, 255, 255);

	BRS::Color two(255, 255, 255, 0);

	two.set_RGBA(255, 255, 255, 255);

	BRS_ASSERT_THROW(one == two);

	BRS_TEST_END();
}

int main(int, char**)
{
	testReaduint32BitReader();
	testEqColor();
	testSetColor();
}
