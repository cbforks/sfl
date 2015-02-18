#include "stf.h"
#include "sfl/Prelude.h"

using namespace sfl;

TEST(Prelude,span)
{
	std::vector<int> xs = {1,2,-5,7};
	auto spanned = span([](int x){return x > 0;}, xs);

	ASSERT_EQ(spanned.first.size(), 2);
	ASSERT_EQ(spanned.second.size(), 2);
	ASSERT_EQ(spanned.first.at(0), 1);
	ASSERT_EQ(spanned.first.at(1), 2);
	ASSERT_EQ(spanned.second.at(0), -5);
	ASSERT_EQ(spanned.second.at(1), 7);
}