#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <compare>
#include <concepts>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "vectorofunique.h"

using namespace containerofunique;

TEST(VectorOfUniqueTest, DefaultConstructor) {
  vector_of_unique<int> vou;
  std::vector<int> emptyvec;
  std::unordered_set<int> emptyset;

  EXPECT_EQ(vou.vector(), emptyvec);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAreArray(emptyset));
}

TEST(VectorOfUniqueTest, ConstructorFromEmptyRange) {
  std::vector<int> empty;
  vector_of_unique<int> vou(empty.begin(), empty.end());
  EXPECT_TRUE(vou.empty());
  EXPECT_TRUE(vou.set().empty());
}

TEST(VectorOfUniqueTest, ConstructorInitializesFromIterators) {
  std::vector<int> vec1 = {3, 1, 2, 3, 4, 5};
  std::vector<int> vec2 = {3, 1, 2, 4, 5};
  vector_of_unique<int> vou1(vec1.begin(), vec1.end());
  vector_of_unique<int> vou2(vec2.begin(), vec2.end());

  EXPECT_EQ(vou1.vector(), vec2);
  EXPECT_THAT(std::vector<int>(vou1.set().begin(), vou1.set().end()),
              ::testing::UnorderedElementsAreArray(vec2));
  EXPECT_EQ(vou2.vector(), vec2);
  EXPECT_THAT(std::vector<int>(vou2.set().begin(), vou2.set().end()),
              ::testing::UnorderedElementsAreArray(vec2));
}

TEST(VectorOfUniqueTest, ConstructorWithInitializerListChecksVectorAndSet) {
  vector_of_unique<int> vou1 = {1};
  vector_of_unique<int> vou2 = {1, 2};
  vector_of_unique<int> vou3 = {1, 2, 3, 3};  // duplicate elements

  std::vector<int> vec1 = {1};
  std::vector<int> vec2 = {1, 2};
  std::vector<int> vec3 = {1, 2, 3};

  EXPECT_EQ(vou1.vector(), vec1);
  EXPECT_EQ(vou2.vector(), vec2);
  EXPECT_EQ(vou3.vector(), vec3);

  EXPECT_THAT(std::vector<int>(vou1.set().begin(), vou1.set().end()),
              ::testing::UnorderedElementsAreArray(vec1));
  EXPECT_THAT(std::vector<int>(vou2.set().begin(), vou2.set().end()),
              ::testing::UnorderedElementsAreArray(vec2));
  EXPECT_THAT(std::vector<int>(vou3.set().begin(), vou3.set().end()),
              ::testing::UnorderedElementsAreArray(vec3));
}

TEST(VectorOfUniqueTest, CopyConstructor_EmptyVector) {
  vector_of_unique<int> vou1;
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  vector_of_unique<int> vou2(vou1);
  EXPECT_TRUE(vou2.vector().empty());
  EXPECT_TRUE(vou2.set().empty());
}

TEST(VectorOfUniqueTest, CopyConstructor_SingleElement) {
  vector_of_unique<int> vou1 = {42};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  vector_of_unique<int> vou2(vou1);
  std::vector<int> vec = {42};
  EXPECT_EQ(vou2.vector(), vec);
  EXPECT_THAT(vou2.set(), ::testing::UnorderedElementsAreArray(vec));
}

TEST(VectorOfUniqueTest, CopyConstructor_Independence) {
  vector_of_unique<int> vou1 = {1, 2, 3};
  vector_of_unique<int> vou2(vou1);

  vou1.push_back(4);  // Modify the original
  EXPECT_EQ(vou1.vector(), std::vector<int>({1, 2, 3, 4}));
  EXPECT_EQ(vou2.vector(), std::vector<int>({1, 2, 3}));
}

TEST(VectorOfUniqueTest, CopyConstructor_LargeData) {
  std::vector<int> large_data(1000);
  // NOLINTNEXTLINE(modernize-use-std-ranges, modernize-use-algorith)
  std::iota(large_data.begin(), large_data.end(), 0);

  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  vector_of_unique<int> vou1(large_data.begin(), large_data.end());
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  vector_of_unique<int> vou2(vou1);

  EXPECT_EQ(vou1.vector(), vou2.vector());
  EXPECT_THAT(vou2.set(), ::testing::UnorderedElementsAreArray(large_data));
}

TEST(VectorOfUniqueTest, MoveConstructor) {
  vector_of_unique<int> vou1 = {1, 2, 3, 4};
  vector_of_unique<int> vou2(std::move(vou1));
  std::vector<int> vec = {1, 2, 3, 4};
  EXPECT_EQ(vou2.vector(), vec);
}

TEST(VectorOfUniqueTest, MoveConstructor_SourceIsEmpty) {
  vector_of_unique<int> vou1 = {1, 2, 3, 4};
  vector_of_unique<int> vou2(std::move(vou1));
  EXPECT_EQ(vou2.vector(), std::vector<int>({1, 2, 3, 4}));
  // NOLINTNEXTLINE(bugprone-use-after-move,clang-analyzer-cplusplus.Move,-warnings-as-errors)
  EXPECT_TRUE(vou1.empty());
  // NOLINTNEXTLINE(bugprone-use-after-move,clang-analyzer-cplusplus.Move,-warnings-as-errors)
  EXPECT_TRUE(vou1.set().empty());
}

TEST(VectorOfUniqueTest, CopyAssignmentOperator) {
  vector_of_unique<int> vou1 = {1, 2, 3, 4};
  vector_of_unique<int> vou2 = vou1;
  std::vector<int> vec = {1, 2, 3, 4};

  EXPECT_EQ(vou2.vector(), vou1.vector());
  EXPECT_THAT(std::vector<int>(vou2.set().begin(), vou2.set().end()),
              ::testing::UnorderedElementsAreArray(vec));
  vou1.push_back(
      5);  // This is used to suppress warning of
           // [performance-unnecessary-copy-initialization,-warnings-as-errors]
}

TEST(VectorOfUniqueTest, MoveAssignmentOperator) {
  vector_of_unique<int> vou1 = {1, 2, 3, 4};
  vector_of_unique<int> vou2 = std::move(vou1);
  std::vector<int> vec = {1, 2, 3, 4};

  EXPECT_EQ(vou2.vector(), vec);
  EXPECT_THAT(std::vector<int>(vou2.set().begin(), vou2.set().end()),
              ::testing::UnorderedElementsAreArray(vec));
}

// Conditionally define a test for C++17 or later
#if __cplusplus >= 201703L
TEST(VectorOfUniqueTest, MoveAssignmentIsNoexcept) {
  vector_of_unique<std::string> vou1;
  vector_of_unique<std::string> vou2;
  vector_of_unique<std::string> vou3;

  // Static assertion to check if the move assignment operator is noexcept
  static_assert(noexcept(std::declval<vector_of_unique<std::string>&>() =
                             std::declval<vector_of_unique<std::string>&&>()),
                "Move assignment operator should be noexcept.");

  // Test empty vous
  EXPECT_NO_THROW(vou1 = std::move(vou2));

  // Test non-empty vous
  vou3.push_back("Hello, world!");
  EXPECT_NO_THROW(vou1 = std::move(vou3));
  // NOLINTNEXTLINE(bugprone-use-after-move,-warnings-as-errors)
  EXPECT_TRUE(vou3.empty());

  // Self-assignment test (optional, but good for robustness)
  vou1 = std::move(vou1);
  EXPECT_NO_THROW(vou1 = std::move(vou1));
}
#endif

TEST(VectorOfUniqueTest, InitializerListAssignmentOperator) {
  vector_of_unique<int> vou = {1, 2, 3, 4};
  std::vector<int> vec = {1, 2, 3, 4};
  EXPECT_EQ(vou.vector(), vec);
  EXPECT_THAT(std::vector<int>(vou.set().begin(), vou.set().end()),
              ::testing::UnorderedElementsAreArray(vec));
}

TEST(VectorOfUniqueTest, InitializerListAssignment_OverwritesNonEmpty) {
  vector_of_unique<int> vou = {1, 2, 3};
  vou = {4, 5, 6};
  EXPECT_EQ(vou.vector(), std::vector<int>({4, 5, 6}));
  EXPECT_EQ(vou.size(), 3);
  EXPECT_FALSE(vou.find(1) != vou.cend());
  EXPECT_FALSE(vou.find(2) != vou.cend());
  EXPECT_FALSE(vou.find(3) != vou.cend());
}

TEST(VectorOfUniqueTest, AssignEmptyRange) {
  vector_of_unique<int> vou;
  std::vector<int> empty_range;

  vou.assign(empty_range.begin(), empty_range.end());
  EXPECT_EQ(vou.size(), 0);
}

TEST(VectorOfUniqueTest, AssignEmptyInitializerList) {
  vector_of_unique<int> vou;

  vou.assign({});

  EXPECT_EQ(vou.size(), 0);
}

TEST(VectorOfUniqueTest, AssignSingleElement) {
  vector_of_unique<int> vou;
  std::vector<int> single_element = {42};

  vou.assign(single_element.begin(), single_element.end());
  EXPECT_EQ(vou.size(), 1);
  EXPECT_TRUE(vou.find(42) != vou.cend());
}

TEST(VectorOfUniqueTest, AssignSingleElementInitializerList) {
  vector_of_unique<int> vou;

  vou.assign({42});

  EXPECT_EQ(vou.size(), 1);
  EXPECT_TRUE(vou.find(42) != vou.cend());
}

TEST(VectorOfUniqueTest, AssignMultipleUniqueElements) {
  vector_of_unique<int> vou;
  std::vector<int> unique_elements = {1, 2, 3, 4, 5};

  vou.assign(unique_elements.begin(), unique_elements.end());

  // Check vector_ contains the correct elements in order
  EXPECT_EQ(vou.vector().size(), 5);
  for (size_t i = 0; i < unique_elements.size(); ++i) {
    EXPECT_EQ(vou.vector()[i], unique_elements[i]);
  }

  // Check set_ contains the correct elements (no duplicates)
  EXPECT_EQ(vou.set().size(), 5);
  for (int elem : unique_elements) {
    EXPECT_TRUE(vou.set().find(elem) != vou.set().end());
  }
}

TEST(VectorOfUniqueTest, AssignMultipleUniqueElementsInitializerList) {
  vector_of_unique<int> vou;
  std::vector<int> unique_elements = {1, 2, 3, 4, 5};

  vou.assign({1, 2, 3, 4, 5});

  // Check vector_ contains the correct elements in order
  EXPECT_EQ(vou.vector().size(), 5);
  for (size_t i = 0; i < unique_elements.size(); ++i) {
    EXPECT_EQ(vou.vector()[i], unique_elements[i]);
  }

  // Check set_ contains the correct elements (no duplicates)
  EXPECT_EQ(vou.set().size(), 5);
  for (int elem : unique_elements) {
    EXPECT_TRUE(vou.set().find(elem) != vou.set().end());
  }
}

TEST(VectorOfUniqueTest, AssignWithDuplicates) {
  vector_of_unique<int> vou;
  std::vector<int> elements_with_duplicates = {1, 2, 2, 3, 3, 4};

  vou.assign(elements_with_duplicates.begin(), elements_with_duplicates.end());

  // Check vector_ contains only unique elements in order
  std::vector<int> expected = {1, 2, 3, 4};
  EXPECT_EQ(vou.vector().size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(vou.vector()[i], expected[i]);
  }

  // Check set_ contains the same unique elements
  EXPECT_EQ(vou.set().size(), expected.size());
  for (int elem : expected) {
    EXPECT_TRUE(vou.set().find(elem) != vou.set().end());
  }
}

TEST(VectorOfUniqueTest, AssignWithDuplicatesInitializerList) {
  vector_of_unique<int> vou;

  vou.assign({1, 2, 2, 3, 3, 4});

  // Check vector_ contains only unique elements in order
  std::vector<int> expected = {1, 2, 3, 4};
  EXPECT_EQ(vou.vector().size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(vou.vector()[i], expected[i]);
  }

  // Check set_ contains the same unique elements
  EXPECT_EQ(vou.set().size(), expected.size());
  for (int elem : expected) {
    EXPECT_TRUE(vou.set().find(elem) != vou.set().end());
  }
}

TEST(VectorOfUniqueTest, ReassignWithDifferentElements) {
  vector_of_unique<int> vou;
  std::vector<int> initial_elements = {1, 2, 3};
  vou.assign(initial_elements.begin(), initial_elements.end());

  std::vector<int> new_elements = {4, 5, 6};
  vou.assign(new_elements.begin(), new_elements.end());

  // Check vector_ has new elements
  EXPECT_EQ(vou.vector().size(), new_elements.size());
  for (size_t i = 0; i < new_elements.size(); ++i) {
    EXPECT_EQ(vou.vector()[i], new_elements[i]);
  }

  // Check set_ has the same new elements
  EXPECT_EQ(vou.set().size(), new_elements.size());
  for (int elem : new_elements) {
    EXPECT_TRUE(vou.set().find(elem) != vou.set().end());
  }
}

TEST(VectorOfUniqueTest, ReassignWithDifferentElementsInitializerList) {
  vector_of_unique<int> vou;
  std::vector<int> initial_elements = {1, 2, 3};
  std::vector<int> new_elements = {4, 5, 6};

  vou.assign({1, 2, 3});

  vou.assign({4, 5, 6});

  // Check vector_ has new elements
  EXPECT_EQ(vou.vector().size(), new_elements.size());
  for (size_t i = 0; i < new_elements.size(); ++i) {
    EXPECT_EQ(vou.vector()[i], new_elements[i]);
  }

  // Check set_ has the same new elements
  EXPECT_EQ(vou.set().size(), new_elements.size());
  for (int elem : new_elements) {
    EXPECT_TRUE(vou.set().find(elem) != vou.set().end());
  }
}

TEST(VectorOfUniqueTest, MixedInsertions) {
  vector_of_unique<int> vou;
  std::vector<int> initial_elements = {1, 2, 3};
  vou.assign(initial_elements.begin(), initial_elements.end());

  std::vector<int> new_elements = {3, 4, 5};
  vou.assign(new_elements.begin(), new_elements.end());

  // Check vector_ contains only the new unique elements
  std::vector<int> expected = {3, 4, 5};
  ASSERT_EQ(vou.vector().size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    ASSERT_EQ(vou.vector()[i], expected[i]);
  }

  // Check set_ contains the new unique elements
  ASSERT_EQ(vou.set().size(), expected.size());
  for (int elem : expected) {
    ASSERT_TRUE(vou.set().find(elem) != vou.set().end());
  }
}

TEST(VectorOfUniqueTest, AssignEmptyRangeAfterClearing) {
  vector_of_unique<int> vou;
  std::vector<int> initial_elements = {1, 2, 3};
  vou.assign(initial_elements.begin(), initial_elements.end());

  // Now clear and reassign an empty range
  std::vector<int> empty_range;
  vou.assign(empty_range.begin(), empty_range.end());

  // Check that vector_ and set_ are empty after clearing and assigning empty
  // range
  ASSERT_EQ(vou.vector().size(), 0);
  ASSERT_EQ(vou.set().size(), 0);
}

TEST(VectorOfUniqueTest, ElementAccess) {
  vector_of_unique<int> vou = {1, 2, 3, 4};
  EXPECT_EQ(vou.front(), 1);
  EXPECT_EQ(vou.at(1), 2);
  EXPECT_EQ(vou[2], 3);
  EXPECT_EQ(vou.back(), 4);
}

TEST(VectorOfUniqueTest, At_OutOfRange) {
  vector_of_unique<int> vou = {1, 2, 3, 4};
  const vector_of_unique<std::string> vou_const = {"hello", "world"};
  EXPECT_THROW(vou.at(4), std::out_of_range);
  EXPECT_THROW(vou_const.at(2), std::out_of_range);
}

TEST(VectorOfUniqueTest, ElementAccess_ConstVector) {
  const vector_of_unique<std::string> vou = {"hello", "world"};
  EXPECT_EQ(vou.front(), "hello");
  EXPECT_EQ(vou.at(0), "hello");
  EXPECT_EQ(vou.at(1), "world");
  EXPECT_EQ(vou[0], "hello");
  EXPECT_EQ(vou[1], "world");
  EXPECT_EQ(vou.back(), "world");
}

// Test for normal iteration using cbegin() and cend()
TEST(VectorOfUniqueTest, CbeginCend_Iteration) {
  vector_of_unique<int> vou = {1, 2, 3, 4};

  auto it = vou.cbegin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(it, vou.cend());
}

TEST(VectorOfUniqueTest, CrbeginCrend_Iteration) {
  vector_of_unique<int> vou = {1, 2, 3, 4};

  auto rit = vou.crbegin();
  EXPECT_EQ(*rit, 4);
  ++rit;
  EXPECT_EQ(*rit, 3);
  ++rit;
  EXPECT_EQ(*rit, 2);
  ++rit;
  EXPECT_EQ(*rit, 1);
  ++rit;
  EXPECT_EQ(rit, vou.crend());
}

TEST(VectorOfUniqueTest, IteratorsAreNoexcept) {
  // Test with empty vou
  vector_of_unique<int> vou1;
  static_assert(noexcept(vou1.cbegin()), "cbegin() should be noexcept.");
  EXPECT_NO_THROW(vou1.cbegin());
  static_assert(noexcept(vou1.cend()), "cend() should be noexcept.");
  EXPECT_NO_THROW(vou1.cend());
  static_assert(noexcept(vou1.crbegin()), "crbegin() should be noexcept.");
  EXPECT_NO_THROW(vou1.crbegin());
  static_assert(noexcept(vou1.crend()), "crend() should be noexcept.");
  EXPECT_NO_THROW(vou1.crend());

  // Test with non-empty vou
  vector_of_unique<int> vou2 = {1, 2, 3, 4};
  static_assert(noexcept(vou2.cbegin()), "cbegin() should be noexcept.");
  EXPECT_NO_THROW(vou2.cbegin());
  static_assert(noexcept(vou2.cend()), "cend() should be noexcept.");
  EXPECT_NO_THROW(vou2.cend());
  static_assert(noexcept(vou2.crbegin()), "crbegin() should be noexcept.");
  EXPECT_NO_THROW(vou2.crbegin());
  static_assert(noexcept(vou2.crend()), "crend() should be noexcept.");
  EXPECT_NO_THROW(vou2.crend());

  // Test with complex data (std::string)
  vector_of_unique<std::string> vou3 = {"apple", "banana", "cherry"};
  static_assert(noexcept(vou3.cbegin()), "cbegin() should be noexcept.");
  EXPECT_NO_THROW(vou3.cbegin());
  static_assert(noexcept(vou3.cend()), "cend() should be noexcept.");
  EXPECT_NO_THROW(vou3.cend());
  static_assert(noexcept(vou3.crbegin()), "crbegin() should be noexcept.");
  EXPECT_NO_THROW(vou3.crbegin());
  static_assert(noexcept(vou3.crend()), "crend() should be noexcept.");
  EXPECT_NO_THROW(vou3.crend());

  // Test with self-move assignment
  vector_of_unique<int> vou5 = {1, 2, 3};
  static_assert(noexcept(vou5.cbegin()), "cbegin() should be noexcept.");
  EXPECT_NO_THROW(vou5 = std::move(vou5));
  EXPECT_NO_THROW(vou5.cbegin());
  static_assert(noexcept(vou5.cend()), "cend() should be noexcept.");
  EXPECT_NO_THROW(vou5.cend());
  static_assert(noexcept(vou5.crbegin()), "crbegin() should be noexcept.");
  EXPECT_NO_THROW(vou5.crbegin());
  static_assert(noexcept(vou5.crend()), "crend() should be noexcept.");
  EXPECT_NO_THROW(vou5.crend());
}

TEST(VectorOfUniqueTest, EmptyContainer_Iterators) {
  vector_of_unique<int> empty_vou;

  EXPECT_EQ(empty_vou.cbegin(), empty_vou.cend());
  EXPECT_EQ(empty_vou.crbegin(), empty_vou.crend());
}

TEST(VectorOfUniqueTest, ConstCorrectness_Iterators) {
  vector_of_unique<int> vou = {1, 2, 3, 4};
#if __cplusplus >= 202002L
  EXPECT_TRUE((std::same_as<decltype(*vou.cbegin()), const int&>));
  EXPECT_TRUE((std::same_as<decltype(*vou.cend()), const int&>));
  EXPECT_TRUE((std::same_as<decltype(*vou.crbegin()), const int&>));
  EXPECT_TRUE((std::same_as<decltype(*vou.crend()), const int&>));
#else
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE((std::is_same<decltype(*vou.cbegin()), const int&>::value));
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE((std::is_same<decltype(*vou.cend()), const int&>::value));
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE((std::is_same<decltype(*vou.crbegin()), const int&>::value));
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE((std::is_same<decltype(*vou.crend()), const int&>::value));
#endif
}

// Test that iterators do not modify elements (compile-time check)
TEST(VectorOfUniqueTest, Iterator_ModificationNotAllowed) {
  vector_of_unique<int> vou = {1, 2, 3, 4};
  auto const_it = vou.cbegin();
  ASSERT_EQ(*const_it, 1);
  // NOLINTNEXTLINE(modernize-type-traits)
  ASSERT_TRUE(
      std::is_const<std::remove_reference_t<decltype(*const_it)>>::value);
}

TEST(VectorOfUniqueTest, BeginEnd_Iteration) {
  vector_of_unique<int> vou = {1, 2, 3, 4};

  auto it = vou.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(it, vou.end());
}

TEST(VectorOfUniqueTest, BeginEnd_RangeBasedFor) {
  vector_of_unique<int> vou = {1, 2, 3, 4};
  std::vector<int> result;
  for (const auto& x : vou) {
    result.push_back(x);
  }
  EXPECT_EQ(result, (std::vector<int>{1, 2, 3, 4}));
}

TEST(VectorOfUniqueTest, BeginEnd_ConstCorrectness) {
  vector_of_unique<int> vou = {1, 2, 3, 4};
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE(
      std::is_const<std::remove_reference_t<decltype(*vou.begin())>>::value);
}

TEST(VectorOfUniqueTest, Clear_EmptyContainer) {
  vector_of_unique<int> vou;
  EXPECT_NO_THROW(vou.clear());
  EXPECT_TRUE(vou.empty());
  EXPECT_TRUE(vou.set().empty());
}

TEST(VectorOfUniqueTest, Clear) {
  vector_of_unique<int> vou = {1, 2, 3, 4, 5};
  vou.clear();

  EXPECT_EQ(vou.vector().size(), 0);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAre());
}

TEST(VectorOfUniqueTest, Erase_ReturnsNextIterator) {
  vector_of_unique<int> vou = {1, 2, 3, 4, 5};
  auto it = vou.erase(vou.cbegin());
  EXPECT_EQ(*it, 2);
}

TEST(VectorOfUniqueTest, Erase_LastElement_ReturnsEnd) {
  vector_of_unique<int> vou = {1, 2, 3};
  auto it = vou.erase(vou.cend() - 1);
  EXPECT_EQ(it, vou.cend());
  EXPECT_EQ(vou.size(), 2);
  EXPECT_EQ(vou.back(), 2);
}

TEST(VectorOfUniqueTest, Erase_SingleElement) {
  vector_of_unique<int> vou = {1, 2, 3, 4, 5};
  std::vector<int> expected_vector = {2, 3, 4, 5};
  std::unordered_set<int> expected_set = {2, 3, 4, 5};

  vou.erase(vou.cbegin());
  EXPECT_EQ(vou.vector(), expected_vector);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAreArray(expected_set));
}

TEST(VectorOfUniqueTest, EraseEmptyRange) {
  vector_of_unique<int> vou1 = {1, 2, 3, 4, 5, 6};
  std::vector<int> vec2 = {1, 2, 3, 4, 5, 6};
  std::unordered_set<int> set2 = {1, 2, 3, 4, 5, 6};

  auto result = vou1.erase(vou1.cbegin(), vou1.cbegin());
  EXPECT_EQ(result, vou1.cbegin());
  EXPECT_EQ(vou1.vector(), vec2);
  EXPECT_THAT(vou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(VectorOfUniqueTest, EraseRangeOfElements) {
  vector_of_unique<int> vou1 = {1, 2, 3, 4, 5, 6};
  std::vector<int> vec2 = {4, 5, 6};
  std::unordered_set<int> set2 = {4, 5, 6};

  vou1.erase(vou1.cbegin(), vou1.cbegin() + 3);
  EXPECT_EQ(vou1.vector(), vec2);
  EXPECT_THAT(vou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(VectorOfUniqueTest, EraseAllElements) {
  vector_of_unique<int> vou1 = {1, 2, 3, 4, 5, 6};
  std::vector<int> vec2 = {};
  std::unordered_set<int> set2 = {};

  auto result = vou1.erase(vou1.cbegin(), vou1.cend());
  EXPECT_EQ(result, vou1.cend());
  EXPECT_EQ(vou1.vector(), vec2);
  EXPECT_THAT(vou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(VectorOfUniqueTest, InsertLvalueRvalue) {
  std::cout << "Test inserting a unique element" << '\n';
  vector_of_unique<int> vou1 = {1};
  std::vector<int> vec1 = {1};
  auto result1 = vou1.insert(vou1.cbegin(), 2);
  auto expected_result1 = vec1.insert(vec1.cbegin(), 2);
  EXPECT_EQ(*result1.first, *expected_result1);
  EXPECT_TRUE(result1.second);

  std::cout << "Test inserting a duplicate element" << '\n';
  vector_of_unique<int> vou2 = {1};
  std::vector<int> vec2 = {1, 2};
  auto result2 = vou2.insert(vou2.cbegin(), 1);
  EXPECT_EQ(*result2.first, *vou2.cbegin());
  EXPECT_FALSE(result2.second);

  std::cout << "Test inserting a unique rvalue string element" << '\n';
  vector_of_unique<std::string> vou3 = {"hello", "world"};
  std::vector<std::string> vec3 = {"hello", "world"};
  std::string str1 = "good";
  auto expected_result3 = vec3.insert(vec3.cbegin(), std::move("good"));
  auto result3 = vou3.insert(vou3.cbegin(), std::move(str1));
  EXPECT_EQ(vou3.vector(),
            (std::vector<std::string>{"good", "hello", "world"}));
  EXPECT_EQ(*result3.first, *expected_result3);
  EXPECT_TRUE(result3.second);

  std::cout << "Test inserting a duplicate rvalue string element" << '\n';
  vector_of_unique<std::string> vou4 = {"hello", "world"};
  std::vector<std::string> vec4 = {"hello", "world"};
  std::string str2 = "hello";
  auto result4 = vou4.insert(vou4.cbegin(), std::move(str2));
  EXPECT_EQ(vou4.vector(), vec4);
  EXPECT_EQ(*result4.first, *vou4.cbegin());
  EXPECT_FALSE(result4.second);
}

TEST(VectorOfUniqueTest, InsertRangeTest) {
  vector_of_unique<std::string> vou5_1 = {"hello", "world"};
  vector_of_unique<std::string> vou5_2 = {"good", "morning"};
  vector_of_unique<std::string> vou5_3 = {"hello", "world"};
  std::vector<std::string> vec5 = {"good", "morning", "hello", "world"};
  auto result5_1 =
      vou5_1.insert(vou5_1.cbegin(), vou5_2.cbegin(), vou5_2.cbegin() + 2);
  EXPECT_EQ(vou5_1.vector(), vec5);
  EXPECT_EQ(*result5_1, *vou5_1.cbegin());
  auto result5_2 =
      vou5_1.insert(vou5_1.cbegin(), vou5_3.cbegin(), vou5_3.cbegin() + 2);
  EXPECT_EQ(vou5_1.vector(), vec5);
  EXPECT_EQ(*result5_2, *(vou5_1.cbegin()));

  vector_of_unique<std::string> vou6 = {"hello", "world"};
  std::vector<std::string> vec6 = {"good", "morning", "hello", "world"};
  auto result6_1 = vou6.insert(vou6.cbegin(), {"good", "morning"});
  EXPECT_EQ(vou6.vector(), vec6);
  EXPECT_EQ(*result6_1, *vou6.cbegin());
  auto result6_2 = vou6.insert(vou6.cbegin(), {"good", "morning"});
  EXPECT_EQ(vou6.vector(), vec6);
  EXPECT_EQ(*result6_2, *vou6.cbegin());
}

TEST(VectorOfUniqueTest, InsertEmptyRange) {
  vector_of_unique<std::string> vou1 = {"existing"};
  std::vector<std::string> vec1 = {"existing"};
  std::vector<std::string> vec2 = {"hello", "world", "apple", "fruit"};

  auto result1 = vou1.insert(vou1.cbegin(), vec2.begin(), vec2.begin());
  EXPECT_EQ(result1, vou1.cbegin());
  EXPECT_EQ(vou1.vector(), vec1);

  auto result2 = vou1.insert(vou1.cbegin(), {});
  EXPECT_EQ(result2, vou1.cbegin());
  EXPECT_EQ(vou1.vector(), vec1);
}

TEST(VectorOfUniqueTest, InsertAtEnd) {
  vector_of_unique<std::string> vou = {"hello"};
  auto result = vou.insert(vou.cend(), "world");
  EXPECT_EQ(*result.first, vou.back());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(vou.vector(), (std::vector<std::string>{"hello", "world"}));
}

TEST(VectorOfUniqueTest, InsertAtBeginning) {
  vector_of_unique<std::string> vou = {"world"};
  auto result = vou.insert(vou.cbegin(), "hello");
  EXPECT_EQ(*result.first, vou.front());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(vou.vector(), (std::vector<std::string>{"hello", "world"}));
}

TEST(VectorOfUniqueTest, InsertDuplicateElement) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  auto result = vou.insert(vou.cend(), "hello");
  EXPECT_EQ(result.first, vou.cend());
  EXPECT_EQ(result.second, false);
  EXPECT_EQ(vou.vector(), (std::vector<std::string>{"hello", "world"}));
}

TEST(VectorOfUniqueTest, InsertIntoEmptyVector) {
  vector_of_unique<std::string> vou;
  auto result = vou.insert(vou.cend(), "first");
  EXPECT_EQ(*result.first, vou.front());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(vou.vector(), (std::vector<std::string>{"first"}));
}

TEST(VectorOfUniqueTest, InsertAtSpecificPosition) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  auto it = vou.cbegin();
  auto result = vou.insert(it + 1, "goodbye");

  EXPECT_EQ(*result.first, *(vou.cbegin() + 1));
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(vou.vector(),
            (std::vector<std::string>{"hello", "goodbye", "world"}));
}

TEST(VectorOfUniqueTest, Insert_DuplicateRvalue_SourceNotMoved) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::string str = "hello";
  auto result = vou.insert(vou.cbegin(), std::move(str));
  EXPECT_FALSE(result.second);
  // NOLINTNEXTLINE(bugprone-use-after-move,-warnings-as-errors)
  EXPECT_EQ(str, "hello");
  EXPECT_EQ(vou.vector(), (std::vector<std::string>{"hello", "world"}));
}

TEST(VectorOfUniqueTest, EmplaceIntoEmpty) {
  vector_of_unique<std::string> vou;
  std::vector<std::string> vec;

  auto result = vou.emplace(vou.cbegin(), "hello");
  vec.emplace(vec.begin(), "hello");

  EXPECT_EQ(vou.vector(), vec);
  EXPECT_EQ(*result.first, *vou.cbegin());
  EXPECT_TRUE(result.second);
}

TEST(VectorOfUniqueTest, EmplaceAtEnd) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::vector<std::string> vec = {"hello", "world"};

  auto result = vou.emplace(vou.cend(), "goodbye");
  vec.emplace(vec.end(), "goodbye");

  EXPECT_EQ(vou.vector(), vec);
  EXPECT_EQ(*result.first, *(vou.cend() - 1));
  EXPECT_TRUE(result.second);
}

TEST(VectorOfUniqueTest, EmplaceNonString) {
  vector_of_unique<int> vou = {1, 2, 3};
  std::vector<int> vec = {1, 2, 3};

  auto result = vou.emplace(vou.cbegin(), 4);
  vec.emplace(vec.begin(), 4);

  EXPECT_EQ(vou.vector(), vec);
  EXPECT_EQ(*result.first, *vou.cbegin());
  EXPECT_TRUE(result.second);

  // Attempt to emplace a duplicate
  result = vou.emplace(vou.cbegin(), 4);
  EXPECT_EQ(vou.vector(), vec);  // No change
  EXPECT_FALSE(result.second);
}

#if __cplusplus < 201703L  // Before C++20
TEST(VectorOfUniqueTest, EmplaceBackSingleElement) {
  vector_of_unique<int> vou;
  vou.emplace_back(42);
  EXPECT_EQ(vou.size(), 1);
  EXPECT_EQ(vou.back(), 42);
}

TEST(VectorOfUniqueTest, EmplaceBackDuplicateElement) {
  vector_of_unique<int> vou;
  vou.emplace_back(42);
  vou.emplace_back(42);
  EXPECT_EQ(vou.size(), 1);
  EXPECT_EQ(vou.back(), 42);
}

TEST(VectorOfUniqueTest, EmplaceBackMultipleUniqueElements) {
  vector_of_unique<int> vou;
  vou.emplace_back(1);
  vou.emplace_back(2);
  vou.emplace_back(3);
  EXPECT_EQ(vou.size(), 3);
  EXPECT_EQ(vou[2], 3);
  EXPECT_EQ(vou[1], 2);
  EXPECT_EQ(vou[0], 1);
}

TEST(VectorOfUniqueTest, EmplaceBackExistingElement) {
  vector_of_unique<int> vou;
  vou.emplace_back(42);
  vou.emplace_back(42);
  EXPECT_EQ(vou.size(), 1);
  EXPECT_EQ(vou.back(), 42);
}
#else
TEST(VectorOfUniqueTest, EmplaceBack_NewElement) {
  // Test 1: Emplace a new element "good" to the end of vou
  // Emplace_back "good" to vou1
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::vector<std::string> vec = {"hello", "world"};
  auto result = vou.emplace_back("good");
  vec.emplace_back("good");
  EXPECT_EQ(*(vou.cend() - 1), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(vou.vector(), vec);
}

TEST(VectorOfUniqueTest, EmplaceBack_DuplicateElement) {
  // Test 2: Try emplacing "good" twice (duplicate value)
  // Expected: No insertion, vector remains unchanged
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::vector<std::string> vec = {"hello", "world"};
  vou.emplace_back("good");
  vec.emplace_back("good");
  auto result = vou.emplace_back("good");
  EXPECT_EQ(*(vou.cend() - 1), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(vou.vector(), vec);
}

TEST(VectorOfUniqueTest, EmplaceBack_Rvalue) {
  // Test 3: Emplace an rvalue "good" to the back of vou
  // Expected: "good" should be inserted at the end of the vector
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::string str = "good";
  auto result = vou.emplace_back(std::move(str));
  std::vector<std::string> vec = {"hello", "world", "good"};
  EXPECT_EQ(*(vou.cend() - 1), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(vou.vector(), vec);
}

TEST(VectorOfUniqueTest, EmplaceBack_DuplicateRvalue) {
  // Test 4: Try emplacing the rvalues "good" twice (duplicate value)
  // Expected: No insertion, vector remains unchanged
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::string str1 = "good";
  std::string str2 = "good";
  vou.emplace_back(std::move(str1));
  auto result = vou.emplace_back(std::move(str2));
  std::vector<std::string> vec = {"hello", "world", "good"};
  EXPECT_EQ(*(vou.cend() - 1), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(vou.vector(), vec);
}

TEST(VectorOfUniqueTest, EmplaceBack_EmptyVector) {
  // Test 5: Emplace a new element "first" to an empty vector
  // Expected: "first" should be at the back of the vector
  vector_of_unique<std::string> vou_empty;
  auto result_empty = vou_empty.emplace_back("first");
  std::vector<std::string> vec = {"first"};
  EXPECT_EQ(*vou_empty.cbegin(), "first");
  ASSERT_TRUE(result_empty.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result_empty.value().get(), "first");
  EXPECT_EQ(vou_empty.vector(), vec);
}

TEST(VectorOfUniqueTest, EmplaceBack_MultipleElements) {
  // Test 6: Emplace multiple distinct elements to the back of the vector
  // Expected: Elements should be inserted at the back in the order of
  // emplace_back calls
  vector_of_unique<std::string> vou = {"hello"};
  vou.emplace_back("world");
  vou.emplace_back("good");
  vou.emplace_back("morning");

  std::vector<std::string> vec = {"hello", "world", "good", "morning"};

  EXPECT_EQ(*(vou.cend() - 1), "morning");
  EXPECT_EQ(*(vou.cend() - 2), "good");
  EXPECT_EQ(*(vou.cend() - 3), "world");
  EXPECT_EQ(*(vou.cend() - 4), "hello");
  EXPECT_EQ(vou.vector(), vec);
}

TEST(VectorOfUniqueTest, EmplaceBack_NonStringType) {
  // Test 7: Emplace an integer to the back of an integer vector (non-string
  // type) Expected: The integer 4 should be at the back of the vector
  vector_of_unique<int> vou = {1, 2, 3};
  auto result = vou.emplace_back(4);
  std::vector<int> vec = {1, 2, 3, 4};
  EXPECT_EQ(*(vou.cend() - 1), 4);
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access,-warnings-as-errors)
  EXPECT_EQ(result.value().get(), 4);
  EXPECT_EQ(vou.vector(), vec);
}
#endif

TEST(VectorOfUniqueTest, PopBack_EmptyVector) {
  vector_of_unique<std::string> vou;
  EXPECT_NO_THROW(vou.pop_back());
  EXPECT_TRUE(vou.vector().empty());
  EXPECT_TRUE(vou.set().empty());
}

TEST(VectorOfUniqueTest, PopBack_SingleElement) {
  vector_of_unique<std::string> vou = {"hello"};
  vou.pop_back();
  EXPECT_TRUE(vou.vector().empty());
  EXPECT_TRUE(vou.set().empty());
}

TEST(VectorOfUniqueTest, PopBack_MultipleSequential) {
  vector_of_unique<std::string> vou = {"hello", "world", "goodbye"};
  vou.pop_back();
  EXPECT_EQ(vou.vector(), (std::vector<std::string>{"hello", "world"}));
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAre("hello", "world"));

  vou.pop_back();
  EXPECT_EQ(vou.vector(), (std::vector<std::string>{"hello"}));
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAre("hello"));

  vou.pop_back();
  EXPECT_TRUE(vou.vector().empty());
  EXPECT_TRUE(vou.set().empty());
}

TEST(VectorOfUniqueTest, PushBack_NewElement) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::vector<std::string> expected = {"hello", "world", "good"};

  // Test pushing a new element to the back
  bool result = vou.push_back("good");
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(vou.vector(), expected);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(VectorOfUniqueTest, PushBack_DuplicateElement) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::vector<std::string> expected = {"hello", "world"};

  // Test pushing a duplicate element
  bool result = vou.push_back("hello");
  EXPECT_FALSE(result);  // Should return false
  EXPECT_EQ(vou.size(), 2);
  EXPECT_EQ(vou.vector(), expected);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(VectorOfUniqueTest, PushBack_Rvalue) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::vector<std::string> expected = {"hello", "world", "good"};

  // Test pushing an rvalue to the back
  std::string str = "good";
  bool result = vou.push_back(std::move(str));
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(vou.vector(), expected);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(VectorOfUniqueTest, PushBack_DuplicateRvalue) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::vector<std::string> expected = {"hello", "world"};

  // Duplicate rvalue: push_back should fail and leave str intact
  std::string str = "hello";
  bool result = vou.push_back(std::move(str));
  EXPECT_FALSE(result);
  // NOLINTNEXTLINE(bugprone-use-after-move,-warnings-as-errors)
  EXPECT_EQ(str, "hello");
  EXPECT_EQ(vou.vector(), expected);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(VectorOfUniqueTest, PushBack_EmptyRvalue) {
  vector_of_unique<std::string> vou = {"hello", "world"};
  std::vector<std::string> expected = {"hello", "world", ""};

  // Test pushing an empty string as an rvalue
  std::string str = "";
  bool result = vou.push_back(std::move(str));
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(vou.vector(), expected);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(VectorOfUniqueTest, PushBack_EmptyContainer) {
  vector_of_unique<std::string> vou;
  std::vector<std::string> expected = {"hello"};

  // Test pushing to an initially empty container
  bool result = vou.push_back("hello");
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(vou.vector(), expected);
  EXPECT_THAT(vou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(VectorOfUniqueTest, MemberFunctionSwap) {
  vector_of_unique<std::string> vou1 = {"hello", "world"};
  vector_of_unique<std::string> vou2 = {"good", "morning"};
  std::vector<std::string> vec1 = {"hello", "world"};
  std::vector<std::string> vec2 = {"good", "morning"};

  vou1.swap(vou2);
  EXPECT_EQ(vou1.vector(), vec2);
  EXPECT_THAT(vou1.set(), ::testing::UnorderedElementsAreArray(vec2));
  EXPECT_EQ(vou2.vector(), vec1);
  EXPECT_THAT(vou2.set(), ::testing::UnorderedElementsAreArray(vec1));
}

#if __cplusplus >= 201703L
TEST(VectorOfUniqueTest, SwapIsNoexcept) {
  vector_of_unique<std::string> vou1;
  vector_of_unique<std::string> vou2;
  vector_of_unique<std::string> vou3;

  // Test empty vous
  EXPECT_NO_THROW(vou1.swap(vou2));

  // Test non-empty vous
  vou1.push_back("hello");
  vou3.push_back("world");
  EXPECT_NO_THROW(vou1.swap(vou3));
  EXPECT_TRUE(vou1.front() == "world");
  EXPECT_TRUE(vou3.front() == "hello");

  // Self-swap test (optional but good for robustness)
  EXPECT_NO_THROW(vou1.swap(vou1));  // Self-swap should not throw an exception
}
#endif

TEST(VectorOfUniqueTest, StdSwap) {
  vector_of_unique<std::string> vou1 = {"hello", "world"};
  vector_of_unique<std::string> vou2 = {"good", "morning"};
  std::vector<std::string> vec1 = {"hello", "world"};
  std::vector<std::string> vec2 = {"good", "morning"};

  std::swap(vou1, vou2);
  EXPECT_EQ(vou1.vector(), vec2);
  EXPECT_THAT(vou1.set(), ::testing::UnorderedElementsAreArray(vec2));
  EXPECT_EQ(vou2.vector(), vec1);
  EXPECT_THAT(vou2.set(), ::testing::UnorderedElementsAreArray(vec1));
}

TEST(VectorOfUniqueTest, Empty) {
  // Test for empty vou
  vector_of_unique<std::string> vou1;
  static_assert(noexcept(vou1.empty()), "empty() should be noexcept.");
  EXPECT_NO_THROW({
    bool result = vou1.empty();
    EXPECT_TRUE(result);
  });
  EXPECT_TRUE(vou1.empty());

  // Test for non-empty vous
  vector_of_unique<std::string> vou2 = {"good"};
  EXPECT_NO_THROW({
    bool result = vou2.empty();
    EXPECT_FALSE(result);
  });
  EXPECT_FALSE(vou2.empty());

  vector_of_unique<std::string> vou3 = {"good", "morning", "hello", "world"};
  EXPECT_NO_THROW({
    bool result = vou3.empty();
    EXPECT_FALSE(result);
  });
  EXPECT_FALSE(vou3.empty());
}

TEST(VectorOfUniqueTest, Size) {
  // Test 1: Vector with a single element
  vector_of_unique<std::string> vou1 = {"good"};
  EXPECT_EQ(vou1.size(), 1);

  // Test 2: Vector with multiple unique elements
  vector_of_unique<std::string> vou2 = {"good", "morning", "hello", "world"};
  EXPECT_EQ(vou2.size(), 4);

  // Adding a new unique element increases the size
  vou2.push_back("new");
  EXPECT_EQ(vou2.size(), 5);

  // Attempting to add a duplicate element does not change the size
  vou2.push_back("morning");  // "morning" is already in the vector
  EXPECT_EQ(vou2.size(), 5);

  // Test 3: Empty vector
  vector_of_unique<std::string> vou3;
  EXPECT_EQ(vou3.size(), 0);  // Corrected to check vou3
}

#if __cplusplus < 202002L  // Before C++20
TEST(VectorOfUniqueTest, ComparisonOperatorsWithString) {
  vector_of_unique<std::string> vou1;
  vector_of_unique<std::string> vou2;
  vector_of_unique<std::string> vou3;

  // Test equality operator (operator==)
  EXPECT_TRUE(vou1 == vou2);
  vou1.push_back("Hello");
  EXPECT_FALSE(vou1 == vou2);

  // Test inequality operator (operator!=)
  vou2.push_back("World");
  EXPECT_TRUE(vou1 != vou2);

  // Test less-than operator (operator<)
  vou3.push_back("Apple");
  EXPECT_TRUE(vou3 < vou1);

  // Test less-than-or-equal operator (operator<=)
  EXPECT_TRUE(vou3 <= vou1);
  EXPECT_TRUE(vou1 <= vou2);

  // Test greater-than operator (operator>)
  EXPECT_TRUE(vou1 > vou3);

  // Test greater-than-or-equal operator (operator>=)
  EXPECT_TRUE(vou1 >= vou3);
  EXPECT_TRUE(vou2 >= vou1);
}
#else  // C++20 or later
TEST(VectorOfUniqueTest, ComparisonOperatorsWithStringCXX20) {
  vector_of_unique<std::string> vou1;
  vector_of_unique<std::string> vou2;
  vector_of_unique<std::string> vou3;

  // Test spaceship operator (<=>) for C++20 or later
  EXPECT_TRUE((vou1 <=> vou2) == std::strong_ordering::equal);
  vou1.push_back("Hello");
  vou2.push_back("Hello");
  EXPECT_TRUE((vou1 <=> vou2) == std::strong_ordering::equal);
  vou2.push_back("world");
  EXPECT_TRUE((vou1 <=> vou2) != std::strong_ordering::equal);
  EXPECT_TRUE((vou1 <=> vou2) == std::strong_ordering::less);

  vou3.push_back("Apple");
  EXPECT_TRUE((vou3 <=> vou1) == std::strong_ordering::less);
  EXPECT_TRUE((vou1 <=> vou3) == std::strong_ordering::greater);
}
#endif

TEST(VectorOfUniqueTest, Find) {
  // Test 1: Find in an empty vou
  vector_of_unique<int> vou_empty;
  auto it_empty = vou_empty.find(10);
  EXPECT_EQ(it_empty, vou_empty.cend());

  // Test 2: Find an element that exists in the vou
  vector_of_unique<int> vou = {10, 20, 30};
  auto it = vou.find(20);
  EXPECT_NE(it, vou.cend());
  EXPECT_EQ(*it, 20);

  // Test 3: Find an element that does not exist
  auto it_not_found = vou.find(40);
  EXPECT_EQ(it_not_found, vou.cend());

  // Test 4: Find element at the beginning
  auto it_first = vou.find(10);
  EXPECT_NE(it_first, vou.cend());
  EXPECT_EQ(*it_first, 10);

  // Test 5: Find element at the end
  auto it_last = vou.find(30);
  EXPECT_NE(it_last, vou.cend());
  EXPECT_EQ(*it_last, 30);

  // Test 6: Multiple lookups
  auto it_again = vou.find(20);
  EXPECT_EQ(it_again, it);
  EXPECT_EQ(*it_again, 20);

  // Test 7: Find in a vou with complex data types (e.g., std::string)
  vector_of_unique<std::string> vou_str = {"hello", "world", "goodbye"};
  auto it_str = vou_str.find("world");
  EXPECT_NE(it_str, vou_str.cend());
  EXPECT_EQ(*it_str, "world");

  auto it_str_not_found = vou_str.find("unknown");
  EXPECT_EQ(it_str_not_found, vou_str.cend());
}

#if __cplusplus >= 202002L  // C++20 or later
TEST(VectorOfUniqueTest, ContainsKeyType) {
  vector_of_unique<int> vou = {1, 2, 3};

  EXPECT_TRUE(vou.contains(1));
  EXPECT_TRUE(vou.contains(2));
  EXPECT_FALSE(vou.contains(4));
}

TEST(VectorOfUniqueTest, ContainsCompatibleType) {
  vector_of_unique<int> vou = {1, 2, 3};
  EXPECT_TRUE(vou.contains(1));
  EXPECT_TRUE(vou.contains(1.0));  // Assuming compatibility
}

TEST(VectorOfUniqueTest, ContainsInEmptyVector) {
  vector_of_unique<int> vou;
  EXPECT_FALSE(vou.contains(1));
}

TEST(VectorOfUniqueTest, ContainsWithVariousIntTypes) {
  vector_of_unique<int32_t> vou = {1, 2, 3};

  EXPECT_TRUE(vou.contains(int16_t(1)));
  EXPECT_FALSE(vou.contains(int16_t(4)));
}

// Heterogeneous lookup requires Hash::is_transparent.
struct StringHash {
  using is_transparent = void;
  size_t operator()(std::string_view sv) const {
    return std::hash<std::string_view>{}(sv);
  }
};

struct StringEqual {
  using is_transparent = void;
  bool operator()(std::string_view a, std::string_view b) const {
    return a == b;
  }
};

// Positive: find<K> and contains<K> are available with a transparent Hash.
TEST(VectorOfUniqueTest, Find_HeterogeneousLookup) {
  vector_of_unique<std::string, StringHash, StringEqual> vou = {"hello",
                                                                "world"};
  std::string_view sv_found = "hello";
  std::string_view sv_missing = "foo";

  EXPECT_NE(vou.find(sv_found), vou.cend());
  EXPECT_EQ(*vou.find(sv_found), "hello");
  EXPECT_EQ(vou.find(sv_missing), vou.cend());
  EXPECT_TRUE(vou.contains(sv_found));
  EXPECT_FALSE(vou.contains(sv_missing));
}

// Negative: find<K>/contains<K> are not available without Hash::is_transparent.
template <typename C, typename K>
concept VectorCanFindWith = requires(const C& c, K k) { c.find(k); };
template <typename C, typename K>
concept VectorCanContainsWith = requires(const C& c, K k) { c.contains(k); };
static_assert(
    !VectorCanFindWith<vector_of_unique<std::string>, std::string_view>);
static_assert(
    !VectorCanContainsWith<vector_of_unique<std::string>, std::string_view>);
#endif

TEST(VectorOfUniqueTest, NonmemberEraseWithStrings) {
  vector_of_unique<std::string> vou = {"apple", "banana", "cherry"};

  // Test 1: Erase a string element that exists in the vou
  // Erase an existing string element
  EXPECT_EQ(erase(vou, "banana"), 1);
  EXPECT_EQ(vou.size(), 2);
  EXPECT_EQ(vou.find("banana"), vou.cend());

  // Test 2: Erase a string element that does not exist in the vou
  EXPECT_EQ(erase(vou, "grape"), 0);
  EXPECT_EQ(vou.size(), 2);

  // Test 3: Erase the last string element in the vou
  EXPECT_EQ(erase(vou, "apple"), 1);
  EXPECT_EQ(vou.size(), 1);

  // Test 4: Erase the remaining string element
  EXPECT_EQ(erase(vou, "cherry"), 1);
  EXPECT_EQ(vou.size(), 0);

  // Test 5: Erase from an empty vou
  EXPECT_EQ(erase(vou, "grape"), 0);
  EXPECT_EQ(vou.size(), 0);
}

TEST(VectorOfUniqueTest, NonmemberEraseMultipleStringElements) {
  vector_of_unique<std::string> vou;

  // Test 1: Erase multiple string elements sequentially
  vou.push_back("apple");
  vou.push_back("banana");
  vou.push_back("cherry");

  // Remove first element
  EXPECT_EQ(erase(vou, "apple"), 1);
  EXPECT_EQ(vou.size(), 2);
  EXPECT_EQ(vou.find("apple"), vou.cend());  // "apple" should be removed

  // Remove second element
  EXPECT_EQ(erase(vou, "banana"), 1);
  EXPECT_EQ(vou.size(), 1);
  EXPECT_EQ(vou.find("banana"), vou.cend());  // "banana" should be removed

  // Remove last element
  EXPECT_EQ(erase(vou, "cherry"), 1);
  EXPECT_EQ(vou.size(), 0);
  EXPECT_EQ(vou.find("cherry"), vou.cend());  // "cherry" should be removed
}

TEST(VectorOfUniqueTest, NonmemberEraseEdgeCasesWithStrings) {
  vector_of_unique<std::string> vou;

  // Test 1: Erase from an empty vou
  EXPECT_EQ(erase(vou, "orange"), 0);
  EXPECT_EQ(vou.size(), 0);

  // Test 2: Erase from a vou with a single string element
  vou.push_back("apple");
  EXPECT_EQ(erase(vou, "apple"), 1);
  EXPECT_EQ(vou.size(), 0);
}

TEST(VectorOfUniqueTest, NonmemberEraseNonExistentStringElement) {
  vector_of_unique<std::string> vou;

  // Test 1: Erase non-existent string element from an empty vou
  EXPECT_EQ(erase(vou, "orange"), 0);
  EXPECT_EQ(vou.size(), 0);

  // Test 2: Erase non-existent string element from a vou with some
  // elements
  vou.push_back("apple");
  vou.push_back("banana");
  vou.push_back("cherry");
  EXPECT_EQ(erase(vou, "grape"), 0);
  EXPECT_EQ(vou.size(), 3);
}

TEST(VectorOfUniqueTest, EraseIfBasicFunctionality) {
  vector_of_unique<int> vou = {1, 2, 3, 4, 5, 6};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(vou, pred);
  EXPECT_EQ(removed_count, 3);
  EXPECT_EQ(vou.size(), 3);
  EXPECT_TRUE(vou.find(2) == vou.cend());
  EXPECT_TRUE(vou.find(4) == vou.cend());
  EXPECT_TRUE(vou.find(6) == vou.cend());
}

TEST(VectorOfUniqueTest, EraseIfNoElementsRemoved) {
  vector_of_unique<int> vou = {1, 3, 5, 7, 9};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(vou, pred);
  EXPECT_EQ(removed_count, 0);
  EXPECT_EQ(vou.size(), 5);
}

TEST(VectorOfUniqueTest, EraseIfAllElementsRemoved) {
  vector_of_unique<int> vou = {2, 4, 6, 8, 10};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(vou, pred);
  EXPECT_EQ(removed_count, 5);
  EXPECT_EQ(vou.size(), 0);
}

TEST(VectorOfUniqueTest, EraseIfEmptyContainer) {
  vector_of_unique<int> vou;
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(vou, pred);
  EXPECT_EQ(removed_count, 0);
  EXPECT_EQ(vou.size(), 0);
}

TEST(VectorOfUniqueTest, EraseIfSingleElementRemoved) {
  vector_of_unique<int> vou = {4};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(vou, pred);
  EXPECT_EQ(removed_count, 1);
  EXPECT_EQ(vou.size(), 0);
}

TEST(VectorOfUniqueTest, EraseIfSingleElementNotRemoved) {
  vector_of_unique<int> vou = {3};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(vou, pred);
  EXPECT_EQ(removed_count, 0);
  EXPECT_EQ(vou.size(), 1);
}

TEST(VectorOfUniqueTest, EraseIfWithStrings) {
  vector_of_unique<std::string> vou = {"apple", "banana", "cherry", "date"};
  auto pred = [](const std::string& s) { return s[0] == 'b'; };
  size_t removed_count = erase_if(vou, pred);
  EXPECT_EQ(removed_count, 1);
  EXPECT_EQ(vou.size(), 3);
  EXPECT_TRUE(vou.find("banana") == vou.cend());
}

TEST(VectorOfUniqueTest, EraseIfWithComplexPredicate) {
  vector_of_unique<std::string> vou = {"apple", "banana", "cherry", "date"};
  auto pred = [](const std::string& s) { return s.length() > 5; };
  size_t removed_count = erase_if(vou, pred);
  EXPECT_EQ(removed_count, 2);
  EXPECT_EQ(vou.size(), 2);
  EXPECT_TRUE(vou.find("banana") == vou.cend());
  EXPECT_TRUE(vou.find("cherry") == vou.cend());
}

TEST(VectorOfUniqueTest, EraseIf_RemainingElementsPreserveOrder) {
  vector_of_unique<int> vou = {1, 2, 3, 4, 5, 6};
  erase_if(vou, [](int x) { return x % 2 == 0; });
  EXPECT_EQ(vou.vector(), std::vector<int>({1, 3, 5}));
}

