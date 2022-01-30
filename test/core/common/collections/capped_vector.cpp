#include <gtest/gtest.h>
#include <cstdint>
#include <cmath>
#include <tempeh/common/collections/capped_vector.hpp>

TEST(CollectionsTest, CappedVectorSizeAndContent)
{
    int16_t max_size = 20;
    sbstd::collections::CappedVector<int16_t> capped_vector(max_size, 20);
    for (int16_t i = 1; i <= 50; ++i)
    {
        capped_vector.push_back(i);
        EXPECT_EQ(capped_vector.size(), std::min(i, max_size));
    }

    int16_t j = 31;
    for (auto& i: capped_vector)
    {
        EXPECT_EQ(i, j);
        j++;
    }
}

TEST(CollectionsTest, CappedVectorRawContent) {
    int16_t max_size = 20;
    sbstd::collections::CappedVector<int16_t> capped_vector(max_size, 20);
    for (int16_t i = 1; i <= 39; ++i)
    {
        capped_vector.push_back(i);
    }

    auto& inner_capped_vector = capped_vector.raw_data();

    int16_t j = 1;
    for (auto& i: inner_capped_vector)
    {
        EXPECT_EQ(i, j);
        j++;
    }

    for (int16_t i = 40; i <= 50; ++i)
    {
        capped_vector.push_back(i);
    }

    j = 21;
    for (auto& i: inner_capped_vector)
    {
        EXPECT_EQ(i, j);
        j++;
    }
}
