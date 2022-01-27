#include <gtest/gtest.h>

#include "/home/rahman/Projects/tempeh-engine/editor/src/filesystem/filetree.hpp"

TEST(HelloTest, BasicAssertions) {
    ASSERT_NO_FATAL_FAILURE({
        Tempeh::File::FileTree test_obj("/home/rahman/Projects/tempeh-engine");
    });
}
