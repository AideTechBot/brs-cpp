#include "brs_test.hpp"
#define BRS_BRICKADIA_IMPLEMENTATION_H
#include "../brs/brs.hpp"
#include <iostream>

// So we can chain the calls
void testBRSReaderHeader1(BRS::Reader&);
void testBRSReaderHeader2(BRS::Reader&);
void testBRSReaderBricks(BRS::Reader&);

void testBRSReaderConstructor(std::string path) {
	std::cout << path << std::endl;
	BRS_TEST_BEGIN("BRS Reader Constructor");

	BRS::Reader test(path);
	BRS_ASSERT_THROW(test.version == BRS::Version::AddedDateTime);
	BRS_ASSERT_THROW(test.gameVersion == 3642);

	BRS_TEST_END();

	testBRSReaderHeader1(test);
}


void testBRSReaderHeader1(BRS::Reader& test) {
	BRS_TEST_BEGIN("BRS Reader Header1 Reader");

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

	// This test is depended on by the following tests
	testBRSReaderHeader2(test);
}

void testBRSReaderHeader2(BRS::Reader& test) {
	BRS_TEST_BEGIN("BRS Reader Header2 Reader");

	test.readHeader2();

	BRS_ASSERT_THROW(test.loadedHeader2());

	BRS::Header2 header2 = test.getHeader2();

	BRS_ASSERT_THROW(header2.mods.empty());
	BRS_ASSERT_THROW(header2.brickAssets.size() == 1);
	BRS_ASSERT_THROW(header2.brickAssets[0] == u"PB_DefaultBrick");
	BRS_ASSERT_THROW(header2.colors.size() == 56);
	// TODO: Add check for all the colors, I'm too lazy to do this.
	BRS_ASSERT_THROW(header2.materials.size() == 6);
	BRS_ASSERT_THROW(header2.materials[0] == u"BMC_Ghost");
	BRS_ASSERT_THROW(header2.materials[1] == u"BMC_Ghost_Fail");
	BRS_ASSERT_THROW(header2.materials[2] == u"BMC_Plastic");
	BRS_ASSERT_THROW(header2.materials[3] == u"BMC_Glow");
	BRS_ASSERT_THROW(header2.materials[4] == u"BMC_Metallic");
	BRS_ASSERT_THROW(header2.materials[5] == u"BMC_Hologram");
	BRS_ASSERT_THROW(header2.brickOwners.size() == 1);
	BRS_ASSERT_THROW(header2.brickOwners[0].name == u"Aide33");
	BRS_ASSERT_THROW(header2.brickOwners[0].uuid.toString() == "e4f5c60b-1407-42ff-ad1e-98a67b25c2da");
	
	BRS_TEST_END();

	// This test is depended on by the following tests
	testBRSReaderBricks(test);
}

void testBRSReaderBricks(BRS::Reader& test) {
	BRS_TEST_BEGIN("BRS Reader Brick Reader");

	test.readBricks();

	BRS_ASSERT_THROW(test.loadedBricks());

	std::vector<BRS::Brick> bricks = test.getBricks();

	BRS_TEST_END();
}

int main(int argc, char** argv)
{
	// for(int i = 0; i < argc; i++)
	// {
	// 	std::cout << argv[i] << std::endl;
	// }
	if(argc > 1 && std::string(argv[1]) == "WORKING_DIRECTORY")
	{
		// Start the chain of tests
		std::string test_path = std::string(argv[2]);
		std::string file_name = "/test_save.brs";
		testBRSReaderConstructor(test_path + file_name);
	}
	else {
		// Start the chain of tests
		testBRSReaderConstructor("test2.brs");
	}
}
