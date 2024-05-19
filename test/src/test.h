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

TEST(PARAMETERS_COMPILER, Dummy)
{
	MockWindow mockWindow("aaaaaaaaaaa");
	ASSERT_EQ(1, 1);
}

#endif // TEST_H_
