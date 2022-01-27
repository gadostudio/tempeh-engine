#include <gtest/gtest.h>

#include <boost/predef.h>
#include <filesystem/filetree.hpp>

TEST(FileSystem, FileTree_ObjectCreation)
{
    ASSERT_DEATH({
        TempehEditor::FileSystem::FileTree test_obj(std::string("/blah"));
    }, "Path did not exists");

#ifdef BOOST_OS_WINDOWS
    ASSERT_NO_FATAL_FAILURE({
        // Uhhh... is C:\\ is not a `standard`?
        TempehEditor::FileSystem::FileTree test_obj(std::string("C:\\Windows"));
    });
#elif BOOST_OS_LINUX
    // TODO linux
#endif

}
