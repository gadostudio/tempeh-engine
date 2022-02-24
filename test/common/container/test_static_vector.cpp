#include <gtest/gtest.h>
#include <tempeh/common/typedefs.hpp>
#include <tempeh/container/static_vector.hpp>

struct TestClassPOD
{
	int x;
	int y;
	u8 z;
};

struct TestClass
{
	int x;
	int y;
	std::string str = "Hello";
};

TEST(Container_StaticVector, ConstructEmpty)
{
	{
		Tempeh::Container::StaticVector<int, 4> v;
		EXPECT_TRUE(v.empty());
	}

	{
		Tempeh::Container::StaticVector<TestClassPOD, 4> v;
		EXPECT_TRUE(v.empty());
	}

	{
		Tempeh::Container::StaticVector<TestClass, 4> v;
		EXPECT_TRUE(v.empty());
	}
}

TEST(Container_StaticVector, ConstructElements)
{
	{
		Tempeh::Container::StaticVector<int, 4> v(3);

		EXPECT_TRUE(!v.empty());

		for (const auto& i : v) {
			EXPECT_EQ(i, 0);
		}
	}

	{
		Tempeh::Container::StaticVector<TestClassPOD, 4> v(3);

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 0);
			EXPECT_EQ(elem.y, 0);
			EXPECT_EQ(elem.z, 0);
		}
	}

	{
		std::vector<TestClass> vt(3);
		Tempeh::Container::StaticVector<TestClass, 4> v(3);

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.str, "Hello");
		}
	}
}

TEST(Container_StaticVector, ConstructElementsWithValue)
{
	{
		Tempeh::Container::StaticVector<int, 4> v(3, -1);

		EXPECT_TRUE(!v.empty());

		for (const auto& i : v) {
			EXPECT_EQ(i, -1);
		}
	}

	{
		Tempeh::Container::StaticVector<TestClassPOD, 4> v(3, TestClassPOD{ -1, -1, 'a' });

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, -1);
			EXPECT_EQ(elem.y, -1);
			EXPECT_EQ(elem.z, 'a');
		}
	}

	{
		Tempeh::Container::StaticVector<TestClass, 4> v(3, TestClass{ -1, -1, "test" });

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, -1);
			EXPECT_EQ(elem.y, -1);
			EXPECT_EQ(elem.str, "test");
		}
	}
}

TEST(Container_StaticVector, CopyConstructor)
{
	{
		Tempeh::Container::StaticVector<int, 4> v{ 1, 2, 3 };
		Tempeh::Container::StaticVector<int, 4> v2(v);

		EXPECT_TRUE(!v2.empty());
		EXPECT_EQ(v2[0], 1);
		EXPECT_EQ(v2[1], 2);
		EXPECT_EQ(v2[2], 3);
	}

	{
		Tempeh::Container::StaticVector<TestClassPOD, 4> v{
			{ 1, 2, 'a'},
			{ 1, 2, 'a'},
		};

		Tempeh::Container::StaticVector<TestClassPOD, 4> v2(v);

		EXPECT_TRUE(!v2.empty());

		for (const auto& elem : v2) {
			EXPECT_EQ(elem.x, 1);
			EXPECT_EQ(elem.y, 2);
			EXPECT_EQ(elem.z, 'a');
		}
	}

	{
		Tempeh::Container::StaticVector<TestClass, 4> v{
			{ 1, 2, "test"},
			{ 1, 2, "test"},
		};

		Tempeh::Container::StaticVector<TestClass, 4> v2(v);

		EXPECT_TRUE(!v2.empty());

		for (const auto& elem : v2) {
			EXPECT_EQ(elem.x, 1);
			EXPECT_EQ(elem.y, 2);
			EXPECT_EQ(elem.str, "test");
		}
	}
}

TEST(Container_StaticVector, AssignWithIterator)
{
	{
		Tempeh::Container::StaticVector<int, 4> v{ 1, 2, 3 };

		// expand
		Tempeh::Container::StaticVector<int, 4> tmp{ 2, 4, 5, 6 };
		v.assign(tmp.begin(), tmp.end());

		EXPECT_TRUE(!v.empty());
		EXPECT_EQ(v[0], 2);
		EXPECT_EQ(v[1], 4);
		EXPECT_EQ(v[2], 5);
		EXPECT_EQ(v[3], 6);

		// shrink
		Tempeh::Container::StaticVector<int, 4> tmp2{ 7, 8 };
		v.assign(tmp2.begin(), tmp2.end());

		EXPECT_TRUE(!v.empty());
		EXPECT_EQ(v[0], 7);
		EXPECT_EQ(v[1], 8);
	}

	{
		Tempeh::Container::StaticVector<TestClassPOD, 4> v{
			{ 1, 2, 'a'},
			{ 1, 2, 'a'},
		};

		// expand
		Tempeh::Container::StaticVector<TestClassPOD, 4> tmp{
			{ 2, 3, 'b'},
			{ 2, 3, 'b'},
			{ 2, 3, 'b'},
			{ 2, 3, 'b'},
		};

		v.assign(tmp.begin(), tmp.end());

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 2);
			EXPECT_EQ(elem.y, 3);
			EXPECT_EQ(elem.z, 'b');
		}

		// shrink
		Tempeh::Container::StaticVector<TestClassPOD, 4> tmp2{
			{ 2, 3, 'c'}
		};

		v.assign(tmp2.begin(), tmp2.end());

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 2);
			EXPECT_EQ(elem.y, 3);
			EXPECT_EQ(elem.z, 'c');
		}
	}

	{
		Tempeh::Container::StaticVector<TestClass, 4> v{
			{ 1, 2, "test"},
			{ 1, 2, "test"},
		};

		// expand
		Tempeh::Container::StaticVector<TestClass, 4> tmp{
			{ 1, 2, "bruh"},
			{ 1, 2, "bruh"},
			{ 1, 2, "bruh"},
		};

		v.assign(tmp.begin(), tmp.end());

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 1);
			EXPECT_EQ(elem.y, 2);
			EXPECT_EQ(elem.str, "bruh");
		}

		// shrink
		Tempeh::Container::StaticVector<TestClass, 4> tmp2{
			{ 3, 4, "brah"},
		};

		v.assign(tmp2.begin(), tmp2.end());

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 3);
			EXPECT_EQ(elem.y, 4);
			EXPECT_EQ(elem.str, "brah");
		}
	}
}

TEST(Container_StaticVector, AssignWithInitializerList)
{
	{
		Tempeh::Container::StaticVector<int, 4> v{ 1, 2, 3 };

		v.assign({ 2, 4, 5, 6 });

		EXPECT_TRUE(!v.empty());
		EXPECT_EQ(v[0], 2);
		EXPECT_EQ(v[1], 4);
		EXPECT_EQ(v[2], 5);
		EXPECT_EQ(v[3], 6);

		v.assign({ 7, 8 });

		EXPECT_TRUE(!v.empty());
		EXPECT_EQ(v[0], 7);
		EXPECT_EQ(v[1], 8);
	}

	{
		Tempeh::Container::StaticVector<TestClassPOD, 4> v{
			{ 1, 2, 'a'},
			{ 1, 2, 'a'},
		};

		v.assign({
			{ 2, 3, 'b'},
			{ 2, 3, 'b'},
			{ 2, 3, 'b'},
		});

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 2);
			EXPECT_EQ(elem.y, 3);
			EXPECT_EQ(elem.z, 'b');
		}

		v.assign({
			{ 2, 3, 'c'}
		});

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 2);
			EXPECT_EQ(elem.y, 3);
			EXPECT_EQ(elem.z, 'c');
		}
	}

	{
		Tempeh::Container::StaticVector<TestClass, 4> v{
			{ 1, 2, "test"},
			{ 1, 2, "test"},
		};

		v.assign({
			{ 1, 2, "bruh"},
			{ 1, 2, "bruh"},
			{ 1, 2, "bruh"},
		});

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 1);
			EXPECT_EQ(elem.y, 2);
			EXPECT_EQ(elem.str, "bruh");
		}

		v.assign({
			{ 3, 4, "brah"},
		});

		EXPECT_TRUE(!v.empty());

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 3);
			EXPECT_EQ(elem.y, 4);
			EXPECT_EQ(elem.str, "brah");
		}
	}
}

TEST(Container_StaticVector, PushBack)
{
	{
		int i = 2;
		Tempeh::Container::StaticVector<int, 6> v;

		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		v.push_back(i);
		v.push_back(i);
		v.push_back(i);

		EXPECT_EQ(v.size(), 6);
		EXPECT_EQ(v[0], 1);
		EXPECT_EQ(v[1], 2);
		EXPECT_EQ(v[2], 3);
		EXPECT_EQ(v[3], i);
		EXPECT_EQ(v[4], i);
		EXPECT_EQ(v[5], i);
	}

	{
		TestClassPOD test_class = {
			1, 2, 'a'
		};

		Tempeh::Container::StaticVector<TestClassPOD, 4> v;

		v.push_back(TestClassPOD{ 1, 2, 'a' });
		v.push_back(TestClassPOD{ 1, 2, 'a' });
		v.push_back(TestClassPOD{ 1, 2, 'a' });
		v.push_back(test_class);

		EXPECT_EQ(v.size(), 4);

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 1);
			EXPECT_EQ(elem.y, 2);
			EXPECT_EQ(elem.z, 'a');
		}
	}

	{
		TestClass test_class = {
			1, 2, "hello world"
		};

		Tempeh::Container::StaticVector<TestClass, 4> v;

		v.push_back(TestClass{ 1, 2, "hello world" });
		v.push_back(TestClass{ 1, 2, "hello world" });
		v.push_back(TestClass{ 1, 2, "hello world" });
		v.push_back(test_class);

		EXPECT_EQ(v.size(), 4);

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 1);
			EXPECT_EQ(elem.y, 2);
			EXPECT_EQ(elem.str, "hello world");
		}
	}
}

TEST(Container_StaticVector, Emplace)
{
	{
		Tempeh::Container::StaticVector<int, 6> v;

		v.emplace_back(1);
		v.emplace_back(2);
		v.emplace_back(3);
		v.emplace_back(4);
		v.emplace_back(5);
		v.emplace_back(6);

		EXPECT_EQ(v.size(), 6);
		EXPECT_EQ(v[0], 1);
		EXPECT_EQ(v[1], 2);
		EXPECT_EQ(v[2], 3);
		EXPECT_EQ(v[3], 4);
		EXPECT_EQ(v[4], 5);
		EXPECT_EQ(v[5], 6);
	}
}

TEST(Container_StaticVector, PopBack)
{
	{
		int i = 2;
		Tempeh::Container::StaticVector<int, 6> v;

		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		v.push_back(i);
		v.push_back(i);
		v.push_back(i);
		v.pop_back();
		v.pop_back();
		v.pop_back();

		EXPECT_EQ(v.size(), 3);
		EXPECT_EQ(v[0], 1);
		EXPECT_EQ(v[1], 2);
		EXPECT_EQ(v[2], 3);
	}

	{
		TestClassPOD test_class = {
			1, 2, 'a'
		};

		Tempeh::Container::StaticVector<TestClassPOD, 4> v;

		v.push_back(TestClassPOD{ 1, 2, 'a' });
		v.push_back(TestClassPOD{ 1, 2, 'a' });
		v.push_back(TestClassPOD{ 1, 2, 'a' });
		v.push_back(test_class);
		v.pop_back();
		v.pop_back();

		EXPECT_EQ(v.size(), 2);

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 1);
			EXPECT_EQ(elem.y, 2);
			EXPECT_EQ(elem.z, 'a');
		}
	}

	{
		TestClass test_class = {
			1, 2, "hello world"
		};

		Tempeh::Container::StaticVector<TestClass, 4> v;

		v.push_back(TestClass{ 1, 2, "hello world" });
		v.push_back(TestClass{ 1, 2, "hello world" });
		v.push_back(TestClass{ 1, 2, "hello world" });
		v.push_back(test_class);
		v.pop_back();
		v.pop_back();

		EXPECT_EQ(v.size(), 2);

		for (const auto& elem : v) {
			EXPECT_EQ(elem.x, 1);
			EXPECT_EQ(elem.y, 2);
			EXPECT_EQ(elem.str, "hello world");
		}
	}
}
