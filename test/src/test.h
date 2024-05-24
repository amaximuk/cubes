#ifndef TEST_H_
#define TEST_H_

#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include "mock_window.h"

TEST(CUBES, XmlReaderTest1)
{
	MockWindow mockWindow("../../test/resources/test1/units");
	ASSERT_FALSE(mockWindow.OpenFile("../../test/resources/test1/config1.xmlx"));
}

#endif // TEST_H_
