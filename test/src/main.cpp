#include <gtest/gtest.h>
#include "src/xml_parser_tests.h"
#include "src/xml_writer_tests.h"
#include "src/file_analysis_tests.h"

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	
	return 0;
}
