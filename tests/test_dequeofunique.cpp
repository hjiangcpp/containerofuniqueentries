#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <compare>
#include <concepts>
#include <deque>
#include <numeric>
#include <stdexcept>
#include <unordered_set>
#include <utility>

#include "dequeofunique.h"

using namespace containerofunique;

TEST(DequeOfUniqueTest, DefaultConstructor) {
  deque_of_unique<int> dou;
  std::deque<int> emptydq;
  std::unordered_set<int> emptyset;

  EXPECT_EQ(dou.deque(), emptydq);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(emptyset));
}

TEST(DequeOfUniqueTest, ConstructorFromEmptyRange) {
  std::deque<int> empty;
  deque_of_unique<int> dou(empty.begin(), empty.end());
  EXPECT_TRUE(dou.empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, ConstructorInitializesFromIterators) {
  std::deque<int> dq1 = {3, 1, 2, 3, 4, 5};
  std::deque<int> dq2 = {3, 1, 2, 4, 5};
  containerofunique::deque_of_unique<int> vou1(dq1.begin(), dq1.end());
  containerofunique::deque_of_unique<int> vou2(dq2.begin(), dq2.end());

  EXPECT_EQ(vou1.deque(), dq2);
  EXPECT_THAT(std::deque<int>(vou1.set().begin(), vou1.set().end()),
              ::testing::UnorderedElementsAreArray(dq2));
  EXPECT_EQ(vou2.deque(), dq2);
  EXPECT_THAT(std::deque<int>(vou2.set().begin(), vou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq2));
}

TEST(DequeOfUniqueTest, ConstructorWithInitializerListChecksDequeAndSet) {
  deque_of_unique<int> dou1 = {1};
  deque_of_unique<int> dou2 = {1, 2};
  deque_of_unique<int> dou3 = {1, 2, 3, 3};  // duplicate elements

  std::deque<int> dq1 = {1};
  std::deque<int> dq2 = {1, 2};
  std::deque<int> dq3 = {1, 2, 3};

  EXPECT_EQ(dou1.deque(), dq1);
  EXPECT_EQ(dou2.deque(), dq2);
  EXPECT_EQ(dou3.deque(), dq3);

  EXPECT_THAT(std::deque<int>(dou1.set().begin(), dou1.set().end()),
              ::testing::UnorderedElementsAreArray(dq1));
  EXPECT_THAT(std::deque<int>(dou2.set().begin(), dou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq2));
  EXPECT_THAT(std::deque<int>(dou3.set().begin(), dou3.set().end()),
              ::testing::UnorderedElementsAreArray(dq3));
}

TEST(DequeOfUniqueTest, CopyConstructor_EmptyDeque) {
  deque_of_unique<int> dou1;
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  deque_of_unique<int> dou2(dou1);
  EXPECT_TRUE(dou2.deque().empty());
  EXPECT_TRUE(dou2.set().empty());
}

TEST(DequeOfUniqueTest, CopyConstructor_SingleElement) {
  deque_of_unique<int> dou1 = {42};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  deque_of_unique<int> dou2(dou1);
  std::deque<int> dq = {42};
  EXPECT_EQ(dou2.deque(), dq);
  EXPECT_THAT(dou2.set(), ::testing::UnorderedElementsAreArray(dq));
}

TEST(DequeOfUniqueTest, CopyConstructor_Independence) {
  deque_of_unique<int> dou1 = {1, 2, 3};
  deque_of_unique<int> dou2(dou1);

  dou1.push_back(4);  // Modify the original
  EXPECT_EQ(dou1.deque(), std::deque<int>({1, 2, 3, 4}));
  EXPECT_EQ(dou2.deque(), std::deque<int>({1, 2, 3}));
}

TEST(DequeOfUniqueTest, CopyConstructor_LargeData) {
  std::deque<int> large_data(1000);
  // NOLINTNEXTLINE(modernize-use-std-ranges, modernize-use-algorith)
  std::iota(large_data.begin(), large_data.end(), 0);

  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  deque_of_unique<int> dou1(large_data.begin(), large_data.end());
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  deque_of_unique<int> dou2(dou1);

  EXPECT_EQ(dou1.deque(), dou2.deque());
  EXPECT_THAT(dou2.set(), ::testing::UnorderedElementsAreArray(large_data));
}

TEST(DequeOfUniqueTest, MoveConstructor) {
  deque_of_unique<int> dou1 = {1, 2, 3, 4};
  deque_of_unique<int> dou2(std::move(dou1));
  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(dou2.deque(), dq);
}

TEST(DequeOfUniqueTest, MoveConstructor_SourceIsEmpty) {
  deque_of_unique<int> dou1 = {1, 2, 3, 4};
  deque_of_unique<int> dou2(std::move(dou1));
  EXPECT_EQ(dou2.deque(), std::deque<int>({1, 2, 3, 4}));
  EXPECT_TRUE(dou1.empty());
  EXPECT_TRUE(dou1.set().empty());
}

TEST(DequeOfUniqueTest, CopyAssignmentOperator) {
  deque_of_unique<int> dou1 = {1, 2, 3, 4};
  deque_of_unique<int> dou2 = dou1;
  std::deque<int> dq = {1, 2, 3, 4};

  EXPECT_EQ(dou2.deque(), dou1.deque());
  EXPECT_THAT(std::deque<int>(dou2.set().begin(), dou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
  dou1.push_back(
      5);  // This is used to suppress warning of
           // [performance-unnecessary-copy-initialization,-warnings-as-errors]
}

TEST(DequeOfUniqueTest, MoveAssignmentOperator) {
  deque_of_unique<int> dou1 = {1, 2, 3, 4};
  deque_of_unique<int> dou2 = std::move(dou1);
  std::deque<int> dq = {1, 2, 3, 4};

  EXPECT_EQ(dou2.deque(), dq);
  EXPECT_THAT(std::deque<int>(dou2.set().begin(), dou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
}

// Conditionally define a test for C++17 or later
#if __cplusplus >= 201703L
TEST(DequeOfUniqueTest, MoveAssignmentIsNoexcept) {
  deque_of_unique<std::string> dou1;
  deque_of_unique<std::string> dou2;
  deque_of_unique<std::string> dou3;

  // Static assertion to check if the move assignment operator is noexcept
  static_assert(noexcept(std::declval<deque_of_unique<std::string> &>() =
                             std::declval<deque_of_unique<std::string> &&>()),
                "Move assignment operator should be noexcept.");

  // Test empty dous
  EXPECT_NO_THROW(dou1 = std::move(dou2));

  // Test non-empty dous
  dou3.push_back("Hello, world!");
  EXPECT_NO_THROW(dou1 = std::move(dou3));
  // NOLINTNEXTLINE(bugprone-use-after-move,-warnings-as-errors)
  EXPECT_TRUE(dou3.empty());

  // Self-assignment test (optional, but good for robustness)
  dou1 = std::move(dou1);
  EXPECT_NO_THROW(dou1 = std::move(dou1));
}
#endif

TEST(DequeOfUniqueTest, InitializerListAssignmentOperator) {
  deque_of_unique<int> dou = {1, 2, 3, 4};
  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(dou.deque(), dq);
  EXPECT_THAT(std::deque<int>(dou.set().begin(), dou.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
}

TEST(DequeOfUniqueTest, InitializerListAssignment_OverwritesNonEmpty) {
  deque_of_unique<int> dou = {1, 2, 3};
  dou = {4, 5, 6};
  EXPECT_EQ(dou.deque(), std::deque<int>({4, 5, 6}));
  EXPECT_EQ(dou.size(), 3);
  EXPECT_FALSE(dou.find(1) != dou.cend());
  EXPECT_FALSE(dou.find(2) != dou.cend());
  EXPECT_FALSE(dou.find(3) != dou.cend());
}

TEST(DequeOfUniqueTest, AssignEmptyRange) {
  deque_of_unique<int> dou;
  std::deque<int> empty_range;

  dou.assign(empty_range.begin(), empty_range.end());
  EXPECT_EQ(dou.size(), 0);
}

TEST(DequeOfUniqueTest, AssignEmptyInitializerList) {
  deque_of_unique<int> dou;

  dou.assign({});

  EXPECT_EQ(dou.size(), 0);
}

TEST(DequeOfUniqueTest, AssignSingleElement) {
  deque_of_unique<int> dou;
  std::deque<int> single_element = {42};

  dou.assign(single_element.begin(), single_element.end());
  EXPECT_EQ(dou.size(), 1);
  EXPECT_TRUE(dou.find(42) != dou.cend());
}

TEST(DequeOfUniqueTest, AssignSingleElementInitializerList) {
  deque_of_unique<int> dou;

  dou.assign({42});

  EXPECT_EQ(dou.size(), 1);
  EXPECT_TRUE(dou.find(42) != dou.cend());
}

TEST(DequeOfUniqueTest, AssignMultipleUniqueElements) {
  deque_of_unique<int> dou;
  std::deque<int> unique_elements = {1, 2, 3, 4, 5};

  dou.assign(unique_elements.begin(), unique_elements.end());

  // Check deque_ contains the correct elements in order
  EXPECT_EQ(dou.deque().size(), 5);
  for (size_t i = 0; i < unique_elements.size(); ++i) {
    EXPECT_EQ(dou.deque()[i], unique_elements[i]);
  }

  // Check set_ contains the correct elements (no duplicates)
  EXPECT_EQ(dou.set().size(), 5);
  for (int elem : unique_elements) {
    EXPECT_TRUE(dou.set().find(elem) != dou.set().end());
  }
}

TEST(DequeOfUniqueTest, AssignMultipleUniqueElementsInitializerList) {
  deque_of_unique<int> dou;
  std::deque<int> unique_elements = {1, 2, 3, 4, 5};

  dou.assign({1, 2, 3, 4, 5});

  // Check deque_ contains the correct elements in order
  EXPECT_EQ(dou.deque().size(), 5);
  for (size_t i = 0; i < unique_elements.size(); ++i) {
    EXPECT_EQ(dou.deque()[i], unique_elements[i]);
  }

  // Check set_ contains the correct elements (no duplicates)
  EXPECT_EQ(dou.set().size(), 5);
  for (int elem : unique_elements) {
    EXPECT_TRUE(dou.set().find(elem) != dou.set().end());
  }
}

TEST(DequeOfUniqueTest, AssignWithDuplicates) {
  deque_of_unique<int> dou;
  std::deque<int> elements_with_duplicates = {1, 2, 2, 3, 3, 4};

  dou.assign(elements_with_duplicates.begin(), elements_with_duplicates.end());

  // Check deque_ contains only unique elements in order
  std::deque<int> expected = {1, 2, 3, 4};
  EXPECT_EQ(dou.deque().size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(dou.deque()[i], expected[i]);
  }

  // Check set_ contains the same unique elements
  EXPECT_EQ(dou.set().size(), expected.size());
  for (int elem : expected) {
    EXPECT_TRUE(dou.set().find(elem) != dou.set().end());
  }
}

TEST(DequeOfUniqueTest, AssignWithDuplicatesInitializerList) {
  deque_of_unique<int> dou;

  dou.assign({1, 2, 2, 3, 3, 4});

  // Check deque_ contains only unique elements in order
  std::deque<int> expected = {1, 2, 3, 4};
  EXPECT_EQ(dou.deque().size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(dou.deque()[i], expected[i]);
  }

  // Check set_ contains the same unique elements
  EXPECT_EQ(dou.set().size(), expected.size());
  for (int elem : expected) {
    EXPECT_TRUE(dou.set().find(elem) != dou.set().end());
  }
}

TEST(DequeOfUniqueTest, ReassignWithDifferentElements) {
  deque_of_unique<int> dou;
  std::deque<int> initial_elements = {1, 2, 3};
  dou.assign(initial_elements.begin(), initial_elements.end());

  std::deque<int> new_elements = {4, 5, 6};
  dou.assign(new_elements.begin(), new_elements.end());

  // Check deque_ has new elements
  EXPECT_EQ(dou.deque().size(), new_elements.size());
  for (size_t i = 0; i < new_elements.size(); ++i) {
    EXPECT_EQ(dou.deque()[i], new_elements[i]);
  }

  // Check set_ has the same new elements
  EXPECT_EQ(dou.set().size(), new_elements.size());
  for (int elem : new_elements) {
    EXPECT_TRUE(dou.set().find(elem) != dou.set().end());
  }
}

TEST(DequeOfUniqueTest, ReassignWithDifferentElementsInitializerList) {
  deque_of_unique<int> dou;
  std::deque<int> initial_elements = {1, 2, 3};
  std::deque<int> new_elements = {4, 5, 6};

  dou.assign({1, 2, 3});

  dou.assign({4, 5, 6});

  // Check deque_ has new elements
  EXPECT_EQ(dou.deque().size(), new_elements.size());
  for (size_t i = 0; i < new_elements.size(); ++i) {
    EXPECT_EQ(dou.deque()[i], new_elements[i]);
  }

  // Check set_ has the same new elements
  EXPECT_EQ(dou.set().size(), new_elements.size());
  for (int elem : new_elements) {
    EXPECT_TRUE(dou.set().find(elem) != dou.set().end());
  }
}

TEST(DequeOfUniqueTest, MixedInsertions) {
  deque_of_unique<int> dou;
  std::deque<int> initial_elements = {1, 2, 3};
  dou.assign(initial_elements.begin(), initial_elements.end());

  std::deque<int> new_elements = {3, 4, 5};
  dou.assign(new_elements.begin(), new_elements.end());

  // Check deque_ contains only the new unique elements
  std::deque<int> expected = {3, 4, 5};
  ASSERT_EQ(dou.deque().size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    ASSERT_EQ(dou.deque()[i], expected[i]);
  }

  // Check set_ contains the new unique elements
  ASSERT_EQ(dou.set().size(), expected.size());
  for (int elem : expected) {
    ASSERT_TRUE(dou.set().find(elem) != dou.set().end());
  }
}

TEST(DequeOfUniqueTest, AssignEmptyRangeAfterClearing) {
  deque_of_unique<int> dou;
  std::deque<int> initial_elements = {1, 2, 3};
  dou.assign(initial_elements.begin(), initial_elements.end());

  // Now clear and reassign an empty range
  std::deque<int> empty_range;
  dou.assign(empty_range.begin(), empty_range.end());

  // Check that deque_ and set_ are empty after clearing and assigning empty
  // range
  ASSERT_EQ(dou.deque().size(), 0);
  ASSERT_EQ(dou.set().size(), 0);
}

TEST(DequeOfUniqueTest, ElementAccess) {
  deque_of_unique<int> dou = {1, 2, 3, 4};
  EXPECT_EQ(dou.front(), 1);
  EXPECT_EQ(dou.at(1), 2);
  EXPECT_EQ(dou[2], 3);
  EXPECT_EQ(dou.back(), 4);
}

TEST(DequeOfUniqueTest, At_OutOfRange) {
  deque_of_unique<int> dou = {1, 2, 3, 4};
  const deque_of_unique<std::string> dou_const = {"hello", "world"};
  EXPECT_THROW(dou.at(4), std::out_of_range);
  EXPECT_THROW(dou_const.at(2), std::out_of_range);
}

TEST(DequeOfUniqueTest, ElementAccess_ConstDeque) {
  const deque_of_unique<std::string> dou = {"hello", "world"};
  EXPECT_EQ(dou.front(), "hello");
  EXPECT_EQ(dou.at(0), "hello");
  EXPECT_EQ(dou.at(1), "world");
  EXPECT_EQ(dou[0], "hello");
  EXPECT_EQ(dou[1], "world");
  EXPECT_EQ(dou.back(), "world");
}

TEST(DequeOfUniqueTest, CbeginCend_Iteration) {
  deque_of_unique<int> dou = {1, 2, 3, 4};

  auto it = dou.cbegin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(it, dou.cend());
}

TEST(DequeOfUniqueTest, CrbeginCrend_Iteration) {
  deque_of_unique<int> dou = {1, 2, 3, 4};

  auto rit = dou.crbegin();
  EXPECT_EQ(*rit, 4);
  ++rit;
  EXPECT_EQ(*rit, 3);
  ++rit;
  EXPECT_EQ(*rit, 2);
  ++rit;
  EXPECT_EQ(*rit, 1);
  ++rit;
  EXPECT_EQ(rit, dou.crend());
}

TEST(DequeOfUniqueTest, IteratorsAreNoexcept) {
  // Test with empty dou
  deque_of_unique<int> dou1;
  static_assert(noexcept(dou1.cbegin()), "cbegin() should be noexcept.");
  EXPECT_NO_THROW(dou1.cbegin());
  static_assert(noexcept(dou1.cend()), "cend() should be noexcept.");
  EXPECT_NO_THROW(dou1.cend());
  static_assert(noexcept(dou1.crbegin()), "crbegin() should be noexcept.");
  EXPECT_NO_THROW(dou1.crbegin());
  static_assert(noexcept(dou1.crend()), "crend() should be noexcept.");
  EXPECT_NO_THROW(dou1.crend());

  // Test with non-empty dou
  deque_of_unique<int> dou2 = {1, 2, 3, 4};
  static_assert(noexcept(dou2.cbegin()), "cbegin() should be noexcept.");
  EXPECT_NO_THROW(dou2.cbegin());
  static_assert(noexcept(dou2.cend()), "cend() should be noexcept.");
  EXPECT_NO_THROW(dou2.cend());
  static_assert(noexcept(dou2.crbegin()), "crbegin() should be noexcept.");
  EXPECT_NO_THROW(dou2.crbegin());
  static_assert(noexcept(dou2.crend()), "crend() should be noexcept.");
  EXPECT_NO_THROW(dou2.crend());

  // Test with complex data (std::string)
  deque_of_unique<std::string> dou3 = {"apple", "banana", "cherry"};
  static_assert(noexcept(dou3.cbegin()), "cbegin() should be noexcept.");
  EXPECT_NO_THROW(dou3.cbegin());
  static_assert(noexcept(dou3.cend()), "cend() should be noexcept.");
  EXPECT_NO_THROW(dou3.cend());
  static_assert(noexcept(dou3.crbegin()), "crbegin() should be noexcept.");
  EXPECT_NO_THROW(dou3.crbegin());
  static_assert(noexcept(dou3.crend()), "crend() should be noexcept.");
  EXPECT_NO_THROW(dou3.crend());

  // Test with self-move assignment
  deque_of_unique<int> dou5 = {1, 2, 3};
  static_assert(noexcept(dou5.cbegin()), "cbegin() should be noexcept.");
  EXPECT_NO_THROW(dou5 = std::move(dou5));
  EXPECT_NO_THROW(dou5.cbegin());
  static_assert(noexcept(dou5.cend()), "cend() should be noexcept.");
  EXPECT_NO_THROW(dou5.cend());
  static_assert(noexcept(dou5.crbegin()), "crbegin() should be noexcept.");
  EXPECT_NO_THROW(dou5.crbegin());
  static_assert(noexcept(dou5.crend()), "crend() should be noexcept.");
  EXPECT_NO_THROW(dou5.crend());
}

TEST(DequeOfUniqueTest, EmptyContainerIterators) {
  deque_of_unique<int> empty_dou;

  EXPECT_EQ(empty_dou.cbegin(), empty_dou.cend());
  EXPECT_EQ(empty_dou.crbegin(), empty_dou.crend());
}

TEST(DequeOfUniqueTest, ConstCorrectness_Iterators) {
  deque_of_unique<int> dou = {1, 2, 3, 4};
#if __cplusplus >= 202002L
  EXPECT_TRUE((std::same_as<decltype(*dou.cbegin()), const int &>));
  EXPECT_TRUE((std::same_as<decltype(*dou.cend()), const int &>));
  EXPECT_TRUE((std::same_as<decltype(*dou.crbegin()), const int &>));
  EXPECT_TRUE((std::same_as<decltype(*dou.crend()), const int &>));
#else
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE((std::is_same<decltype(*dou.cbegin()), const int &>::value));
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE((std::is_same<decltype(*dou.cend()), const int &>::value));
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE((std::is_same<decltype(*dou.crbegin()), const int &>::value));
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE((std::is_same<decltype(*dou.crend()), const int &>::value));
#endif
}

TEST(DequeOfUniqueTest, Iterator_ModificationNotAllowed) {
  deque_of_unique<int> dou = {1, 2, 3, 4};
  auto const_it = dou.cbegin();
  ASSERT_EQ(*const_it, 1);
  ASSERT_TRUE(
      // NOLINTNEXTLINE(modernize-type-traits)
      std::is_const<std::remove_reference_t<decltype(*const_it)>>::value);
}

TEST(DequeOfUniqueTest, BeginEnd_Iteration) {
  deque_of_unique<int> dou = {1, 2, 3, 4};

  auto it = dou.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(it, dou.end());
}

TEST(DequeOfUniqueTest, BeginEnd_RangeBasedFor) {
  deque_of_unique<int> dou = {1, 2, 3, 4};
  std::deque<int> result;
  for (const auto &x : dou) {
    result.push_back(x);
  }
  EXPECT_EQ(result, (std::deque<int>{1, 2, 3, 4}));
}

TEST(DequeOfUniqueTest, BeginEnd_ConstCorrectness) {
  deque_of_unique<int> dou = {1, 2, 3, 4};
  // NOLINTNEXTLINE(modernize-type-traits)
  EXPECT_TRUE(
      std::is_const<std::remove_reference_t<decltype(*dou.begin())>>::value);
}

TEST(DequeOfUniqueTest, Clear_EmptyContainer) {
  deque_of_unique<int> dou;
  EXPECT_NO_THROW(dou.clear());
  EXPECT_TRUE(dou.empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, Clear) {
  deque_of_unique<int> dou = {1, 2, 3, 4, 5};
  static_assert(noexcept(dou.clear()), "clear() should be noexcept.");

  EXPECT_NO_THROW(dou.clear());
  EXPECT_EQ(dou.deque().size(), 0);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre());
}

TEST(DequeOfUniqueTest, Erase_SingleElement) {
  deque_of_unique<int> dou = {1, 2, 3, 4, 5};
  std::deque<int> expected_deque = {2, 3, 4, 5};
  std::unordered_set<int> expected_set = {2, 3, 4, 5};

  dou.erase(dou.cbegin());
  EXPECT_EQ(dou.deque(), expected_deque);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected_set));
}

TEST(DequeOfUniqueTest, Erase_FromEmptyContainer) {
  deque_of_unique<int> dou;
  EXPECT_NO_THROW(dou.erase(dou.cbegin()));
  EXPECT_EQ(dou.deque().size(), 0);
}

TEST(DequeOfUniqueTest, EraseEmptyRange) {
  deque_of_unique<int> dou1 = {1, 2, 3, 4, 5, 6};
  std::deque<int> dq2 = {1, 2, 3, 4, 5, 6};
  std::unordered_set<int> set2 = {1, 2, 3, 4, 5, 6};

  auto result = dou1.erase(dou1.cbegin(), dou1.cbegin());
  EXPECT_EQ(result, dou1.cbegin());
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(DequeOfUniqueTest, EraseRangeOfElements) {
  deque_of_unique<int> dou1 = {1, 2, 3, 4, 5, 6};
  std::deque<int> dq2 = {4, 5, 6};
  std::unordered_set<int> set2 = {4, 5, 6};

  dou1.erase(dou1.cbegin(), dou1.cbegin() + 3);
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(DequeOfUniqueTest, EraseAllElements) {
  deque_of_unique<int> dou1 = {1, 2, 3, 4, 5, 6};
  std::deque<int> dq2 = {};
  std::unordered_set<int> set2 = {};

  auto result = dou1.erase(dou1.cbegin(), dou1.cend());
  EXPECT_EQ(result, dou1.cend());
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(DequeOfUniqueTest, InsertLvalueRvalue) {
  std::cout << "Test inserting a unique element" << '\n';
  deque_of_unique<int> dou1 = {1};
  std::deque<int> dq1 = {1};
  auto result1 = dou1.insert(dou1.cbegin(), 2);
  auto expected_result1 = dq1.insert(dq1.cbegin(), 2);
  EXPECT_EQ(*result1.first, *expected_result1);
  EXPECT_TRUE(result1.second);

  std::cout << "Test inserting a duplicate element" << '\n';
  deque_of_unique<int> dou2 = {1};
  std::deque<int> dq2 = {1, 2};
  auto result2 = dou2.insert(dou2.cbegin(), 1);
  EXPECT_EQ(*result2.first, *dou2.cbegin());
  EXPECT_FALSE(result2.second);

  std::cout << "Test inserting a unique rvalue string element" << '\n';
  deque_of_unique<std::string> dou3 = {"hello", "world"};
  std::deque<std::string> dq3 = {"hello", "world"};
  std::string str1 = "good";
  auto expected_result3 = dq3.insert(dq3.cbegin(), std::move("good"));
  auto result3 = dou3.insert(dou3.cbegin(), std::move(str1));
  EXPECT_EQ(dou3.deque(), (std::deque<std::string>{"good", "hello", "world"}));
  EXPECT_EQ(*result3.first, *expected_result3);
  EXPECT_TRUE(result3.second);

  std::cout << "Test inserting a duplicate rvalue string element" << '\n';
  deque_of_unique<std::string> dou4 = {"hello", "world"};
  std::deque<std::string> dq4 = {"hello", "world"};
  std::string str2 = "hello";
  auto result4 = dou4.insert(dou4.cbegin(), std::move(str2));
  EXPECT_EQ(dou4.deque(), dq4);
  EXPECT_EQ(*result4.first, *dou4.cbegin());
  EXPECT_FALSE(result4.second);
}

TEST(DequeOfUniqueTest, InsertRangeTest) {
  deque_of_unique<std::string> dou5_1 = {"hello", "world"};
  deque_of_unique<std::string> dou5_2 = {"good", "morning"};
  deque_of_unique<std::string> dou5_3 = {"hello", "world"};
  std::deque<std::string> dq5 = {"good", "morning", "hello", "world"};
  auto result5_1 =
      dou5_1.insert(dou5_1.cbegin(), dou5_2.cbegin(), dou5_2.cbegin() + 2);
  EXPECT_EQ(dou5_1.deque(), dq5);
  EXPECT_EQ(*result5_1, *dou5_1.cbegin());
  auto result5_2 =
      dou5_1.insert(dou5_1.cbegin(), dou5_3.cbegin(), dou5_3.cbegin() + 2);
  EXPECT_EQ(dou5_1.deque(), dq5);
  EXPECT_EQ(*result5_2, *(dou5_1.cbegin()));

  deque_of_unique<std::string> dou6 = {"hello", "world"};
  std::deque<std::string> dq6 = {"good", "morning", "hello", "world"};
  auto result6_1 = dou6.insert(dou6.cbegin(), {"good", "morning"});
  EXPECT_EQ(dou6.deque(), dq6);
  EXPECT_EQ(*result6_1, *dou6.cbegin());
  auto result6_2 = dou6.insert(dou6.cbegin(), {"good", "morning"});
  EXPECT_EQ(dou6.deque(), dq6);
  EXPECT_EQ(*result6_2, *dou6.cbegin());
}

TEST(DequeOfUniqueTest, InsertEmptyRange) {
  deque_of_unique<std::string> dou1 = {"existing"};
  std::deque<std::string> dq1 = {"existing"};
  std::deque<std::string> dq2 = {"hello", "world", "apple", "fruit"};

  auto result1 = dou1.insert(dou1.cbegin(), dq2.begin(), dq2.begin());
  EXPECT_EQ(result1, dou1.cbegin());
  EXPECT_EQ(dou1.deque(), dq1);

  auto result2 = dou1.insert(dou1.cbegin(), {});
  EXPECT_EQ(result2, dou1.cbegin());
  EXPECT_EQ(dou1.deque(), dq1);
}

TEST(DequeOfUniqueTest, InsertAtEnd) {
  deque_of_unique<std::string> dou = {"hello"};
  auto result = dou.insert(dou.cend(), "world");
  EXPECT_EQ(*result.first, dou.back());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
}

TEST(DequeOfUniqueTest, InsertAtBeginning) {
  deque_of_unique<std::string> dou = {"world"};
  auto result = dou.insert(dou.cbegin(), "hello");
  EXPECT_EQ(*result.first, dou.front());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
}

TEST(DequeOfUniqueTest, InsertDuplicateElement) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  auto result = dou.insert(dou.cend(), "hello");
  EXPECT_EQ(result.first, dou.cend());
  EXPECT_EQ(result.second, false);
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
}

TEST(DequeOfUniqueTest, InsertIntoEmptyDeque) {
  deque_of_unique<std::string> dou;
  auto result = dou.insert(dou.cend(), "first");
  EXPECT_EQ(*result.first, dou.front());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"first"}));
}

TEST(DequeOfUniqueTest, InsertAtSpecificPosition) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  auto it = dou.cbegin();
  auto result = dou.insert(it + 1, "goodbye");

  EXPECT_EQ(result.first, dou.cbegin() + 1);
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(dou.deque(),
            (std::deque<std::string>{"hello", "goodbye", "world"}));
}

TEST(DequeOfUniqueTest, Insert_DuplicateRvalue_SourceNotMoved) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str = "hello";
  auto result = dou.insert(dou.cbegin(), std::move(str));
  EXPECT_FALSE(result.second);
  // NOLINTNEXTLINE(bugprone-use-after-move,-warnings-as-errors)
  EXPECT_EQ(str, "hello");
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
}

TEST(DequeOfUniqueTest, EmplaceIntoEmpty) {
  deque_of_unique<std::string> dou;
  std::deque<std::string> dq;

  auto result = dou.emplace(dou.cbegin(), "hello");
  dq.emplace(dq.begin(), "hello");

  EXPECT_EQ(dou.deque(), dq);
  EXPECT_EQ(*result.first, *dou.cbegin());
  EXPECT_TRUE(result.second);
}

TEST(DequeOfUniqueTest, EmplaceAtEnd) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};

  auto result = dou.emplace(dou.cend(), "goodbye");
  dq.emplace(dq.end(), "goodbye");

  EXPECT_EQ(dou.deque(), dq);
  EXPECT_EQ(*result.first, *(dou.cend() - 1));
  EXPECT_TRUE(result.second);
}

TEST(DequeOfUniqueTest, EmplaceNonString) {
  deque_of_unique<int> dou = {1, 2, 3};
  std::deque<int> dq = {1, 2, 3};

  auto result = dou.emplace(dou.cbegin(), 4);
  dq.emplace(dq.begin(), 4);

  EXPECT_EQ(dou.deque(), dq);
  EXPECT_EQ(*result.first, *dou.cbegin());
  EXPECT_TRUE(result.second);

  // Attempt to emplace a duplicate
  result = dou.emplace(dou.cbegin(), 4);
  EXPECT_EQ(dou.deque(), dq);  // No change
  EXPECT_FALSE(result.second);
}

#if __cplusplus < 201703L  // Before C++20
TEST(DequeOfUniqueTest, EmplaceFrontSingleElement) {
  deque_of_unique<int> dou;
  dou.emplace_front(42);
  EXPECT_EQ(dou.size(), 1);
  EXPECT_EQ(dou.front(), 42);
}

TEST(DequeOfUniqueTest, EmplaceFrontDuplicateElement) {
  deque_of_unique<int> dou;
  dou.emplace_front(42);
  dou.emplace_front(42);
  EXPECT_EQ(dou.size(), 1);
  EXPECT_EQ(dou.front(), 42);
}

TEST(DequeOfUniqueTest, EmplaceFrontMultipleUniqueElements) {
  deque_of_unique<int> dou;
  dou.emplace_front(1);
  dou.emplace_front(2);
  dou.emplace_front(3);
  EXPECT_EQ(dou.size(), 3);
  EXPECT_EQ(dou[0], 3);
  EXPECT_EQ(dou[1], 2);
  EXPECT_EQ(dou[2], 1);
}

TEST(DequeOfUniqueTest, EmplaceFrontExistingElement) {
  deque_of_unique<int> dou;
  dou.emplace_front(42);
  dou.emplace_front(42);
  EXPECT_EQ(dou.size(), 1);
  EXPECT_EQ(dou.front(), 42);
}
#else
TEST(DequeOfUniqueTest, EmplaceFront_NewElement) {
  // Test 1: Emplace a new element "good" to the front of dou
  // Expected: "good" should be at the front of the deque
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};
  auto result = dou.emplace_front("good");
  dq.emplace_front("good");
  EXPECT_EQ(*dou.cbegin(), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_DuplicateElement) {
  // Test 2: Try emplacing "good" twice (duplicate value)
  // Expected: No insertion, deque remains unchanged
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};
  dou.emplace_front("good");
  dq.emplace_front("good");
  auto result = dou.emplace_front("good");
  EXPECT_EQ(*dou.cbegin(), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_Rvalue) {
  // Test 3: Emplace an rvalue "good" to the front of dou
  // Expected: "good" should be inserted at the front of the deque
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str = "good";
  auto result = dou.emplace_front(std::move(str));
  std::deque<std::string> dq = {"good", "hello", "world"};
  EXPECT_EQ(*dou.cbegin(), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_DuplicateRvalue) {
  // Test 4: Try emplacing the rvalues "good" twice (duplicate value)
  // Expected: No insertion, deque remains unchanged
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str1 = "good";
  std::string str2 = "good";
  dou.emplace_front(std::move(str1));
  auto result = dou.emplace_front(std::move(str2));
  std::deque<std::string> dq = {"good", "hello", "world"};
  EXPECT_EQ(*dou.cbegin(), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_EmptyDeque) {
  // Test 5: Emplace a new element "first" to an empty deque
  // Expected: "first" should be at the front of the deque
  deque_of_unique<std::string> dou_empty;
  auto result_empty = dou_empty.emplace_front("first");
  std::deque<std::string> dq = {"first"};
  EXPECT_EQ(*dou_empty.cbegin(), "first");
  ASSERT_TRUE(result_empty.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result_empty.value().get(), "first");
  EXPECT_EQ(dou_empty.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_MultipleElements) {
  // Test 6: Emplace multiple distinct elements to the front of the deque
  // Expected: Elements should be inserted at the front in the order of
  // emplace_front calls
  deque_of_unique<std::string> dou = {"hello"};
  dou.emplace_front("world");
  dou.emplace_front("good");
  dou.emplace_front("morning");

  std::deque<std::string> dq = {"morning", "good", "world", "hello"};

  EXPECT_EQ(*dou.cbegin(), "morning");
  EXPECT_EQ(*(dou.cbegin() + 1), "good");
  EXPECT_EQ(*(dou.cbegin() + 2), "world");
  EXPECT_EQ(*(dou.cbegin() + 3), "hello");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_NonStringType) {
  // Test 7: Emplace an integer to the front of an integer deque (non-string
  // type) Expected: The integer 4 should be at the front of the deque
  deque_of_unique<int> dou = {1, 2, 3};
  auto result = dou.emplace_front(4);
  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(*dou.cbegin(), 4);
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), 4);
}
#endif

#if __cplusplus < 201703L  // Before C++20
TEST(DequeOfUniqueTest, EmplaceBackSingleElement) {
  deque_of_unique<int> dou;
  dou.emplace_back(42);
  EXPECT_EQ(dou.size(), 1);
  EXPECT_EQ(dou.back(), 42);
}

TEST(DequeOfUniqueTest, EmplaceBackDuplicateElement) {
  deque_of_unique<int> dou;
  dou.emplace_back(42);
  dou.emplace_back(42);
  EXPECT_EQ(dou.size(), 1);
  EXPECT_EQ(dou.back(), 42);
}

TEST(DequeOfUniqueTest, EmplaceBackMultipleUniqueElements) {
  deque_of_unique<int> dou;
  dou.emplace_back(1);
  dou.emplace_back(2);
  dou.emplace_back(3);
  EXPECT_EQ(dou.size(), 3);
  EXPECT_EQ(dou[2], 3);
  EXPECT_EQ(dou[1], 2);
  EXPECT_EQ(dou[0], 1);
}

TEST(DequeOfUniqueTest, EmplaceBackExistingElement) {
  deque_of_unique<int> dou;
  dou.emplace_back(42);
  dou.emplace_back(42);
  EXPECT_EQ(dou.size(), 1);
  EXPECT_EQ(dou.back(), 42);
}
#else
TEST(DequeOfUniqueTest, EmplaceBack_NewElement) {
  // Test 1: Emplace a new element "good" to the end of dou
  // Emplace_back "good" to dou1
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};
  auto result = dou.emplace_back("good");
  dq.emplace_back("good");
  EXPECT_EQ(*(dou.cend() - 1), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_DuplicateElement) {
  // Test 2: Try emplacing "good" twice (duplicate value)
  // Expected: No insertion, deque remains unchanged
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};
  dou.emplace_back("good");
  dq.emplace_back("good");
  auto result = dou.emplace_back("good");
  EXPECT_EQ(*(dou.cend() - 1), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_Rvalue) {
  // Test 3: Emplace an rvalue "good" to the back of dou
  // Expected: "good" should be inserted at the end of the deque
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str = "good";
  auto result = dou.emplace_back(std::move(str));
  std::deque<std::string> dq = {"hello", "world", "good"};
  EXPECT_EQ(*(dou.cend() - 1), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_DuplicateRvalue) {
  // Test 4: Try emplacing the rvalues "good" twice (duplicate value)
  // Expected: No insertion, deque remains unchanged
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str1 = "good";
  std::string str2 = "good";
  dou.emplace_back(std::move(str1));
  auto result = dou.emplace_back(std::move(str2));
  std::deque<std::string> dq = {"hello", "world", "good"};
  EXPECT_EQ(*(dou.cend() - 1), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_EmptyDeque) {
  // Test 5: Emplace a new element "first" to an empty deque
  // Expected: "first" should be at the back of the deque
  deque_of_unique<std::string> dou_empty;
  auto result_empty = dou_empty.emplace_back("first");
  std::deque<std::string> dq = {"first"};
  EXPECT_EQ(*dou_empty.cbegin(), "first");
  ASSERT_TRUE(result_empty.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result_empty.value().get(), "first");
  EXPECT_EQ(dou_empty.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_MultipleElements) {
  // Test 6: Emplace multiple distinct elements to the back of the deque
  // Expected: Elements should be inserted at the back in the order of
  // emplace_back calls
  deque_of_unique<std::string> dou = {"hello"};
  dou.emplace_back("world");
  dou.emplace_back("good");
  dou.emplace_back("morning");

  std::deque<std::string> dq = {"hello", "world", "good", "morning"};

  EXPECT_EQ(*(dou.cend() - 1), "morning");
  EXPECT_EQ(*(dou.cend() - 2), "good");
  EXPECT_EQ(*(dou.cend() - 3), "world");
  EXPECT_EQ(*(dou.cend() - 4), "hello");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_NonStringType) {
  // Test 7: Emplace an integer to the back of an integer deque (non-string
  // type) Expected: The integer 4 should be at the back of the deque
  deque_of_unique<int> dou = {1, 2, 3};
  auto result = dou.emplace_back(4);
  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(*(dou.cend() - 1), 4);
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access,-warnings-as-errors)
  EXPECT_EQ(result.value().get(), 4);
  EXPECT_EQ(dou.deque(), dq);
}
#endif

TEST(DequeOfUniqueTest, PopFront_EmptyDeque) {
  deque_of_unique<std::string> dou;
  EXPECT_NO_THROW(dou.pop_front());
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PopFront_SingleElement) {
  deque_of_unique<std::string> dou = {"hello"};
  dou.pop_front();
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PopFront_MultipleSequential) {
  deque_of_unique<std::string> dou = {"hello", "world", "goodbye"};
  dou.pop_front();
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"world", "goodbye"}));
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("world", "goodbye"));

  dou.pop_front();
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"goodbye"}));
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("goodbye"));

  dou.pop_front();
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, Front_AfterModification) {
  deque_of_unique<std::string> dou = {"hello", "world"};

  // Add a new element at the front
  dou.emplace_front("good");
  EXPECT_EQ(dou.front(), "good");  // The front should now be "good"

  // Remove the front element
  dou.pop_front();
  EXPECT_EQ(dou.front(), "hello");  // The front should now be "hello"
}

TEST(DequeOfUniqueTest, PopBack_EmptyDeque) {
  deque_of_unique<std::string> dou;
  EXPECT_NO_THROW(dou.pop_back());
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PopBack_SingleElement) {
  deque_of_unique<std::string> dou = {"hello"};
  dou.pop_back();
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PopBack_MultipleSequential) {
  deque_of_unique<std::string> dou = {"hello", "world", "goodbye"};
  dou.pop_back();
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("hello", "world"));

  dou.pop_back();
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello"}));
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("hello"));

  dou.pop_back();
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PushFront_NewElement) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"good", "hello", "world"};

  // Test pushing a new element to the front
  bool result = dou.push_front("good");
  EXPECT_TRUE(result);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushFront_DuplicateElement) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world"};

  // Test pushing a duplicate element
  bool result = dou.push_front("hello");
  EXPECT_FALSE(result);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushFront_Rvalue) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"good", "hello", "world"};

  // Test pushing an rvalue to the front
  std::string str = "good";
  bool result = dou.push_front(std::move(str));
  EXPECT_TRUE(result);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushFront_DuplicateRvalue) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world"};

  // Duplicate rvalue: push_front should fail and leave str intact
  std::string str = "hello";
  bool result = dou.push_front(std::move(str));
  EXPECT_FALSE(result);
  // NOLINTNEXTLINE(bugprone-use-after-move,-warnings-as-errors)
  EXPECT_EQ(str, "hello");
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushFront_EmptyRvalue) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"", "hello", "world"};

  // Test pushing an empty string as an rvalue
  std::string str = "";
  bool result = dou.push_front(std::move(str));
  EXPECT_TRUE(result);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushFront_EmptyContainer) {
  deque_of_unique<std::string> dou;
  bool result = dou.push_front("hello");
  EXPECT_TRUE(result);
  EXPECT_EQ(dou.size(), 1);
  EXPECT_EQ(dou.front(), "hello");
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("hello"));
}

TEST(DequeOfUniqueTest, PushBack_NewElement) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world", "good"};

  // Test pushing a new element to the back
  bool result = dou.push_back("good");
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_DuplicateElement) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world"};

  // Test pushing a duplicate element
  bool result = dou.push_back("hello");
  EXPECT_FALSE(result);  // Should return false
  EXPECT_EQ(dou.size(), 2);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_Rvalue) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world", "good"};

  // Test pushing an rvalue to the back
  std::string str = "good";
  bool result = dou.push_back(std::move(str));
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_DuplicateRvalue) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world"};

  // Duplicate rvalue: push_back should fail and leave str intact
  std::string str = "hello";
  bool result = dou.push_back(std::move(str));
  EXPECT_FALSE(result);
  // NOLINTNEXTLINE(bugprone-use-after-move,-warnings-as-errors)
  EXPECT_EQ(str, "hello");
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_EmptyRvalue) {
  deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world", ""};

  // Test pushing an empty string as an rvalue
  std::string str = "";
  bool result = dou.push_back(std::move(str));
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_EmptyContainer) {
  deque_of_unique<std::string> dou;
  std::deque<std::string> expected = {"hello"};

  // Test pushing to an initially empty dou
  bool result = dou.push_back("hello");
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, MemberFunctionSwap) {
  deque_of_unique<std::string> dou1 = {"hello", "world"};
  deque_of_unique<std::string> dou2 = {"good", "morning"};
  std::deque<std::string> dq1 = {"hello", "world"};
  std::deque<std::string> dq2 = {"good", "morning"};

  dou1.swap(dou2);
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(dq2));
  EXPECT_EQ(dou2.deque(), dq1);
  EXPECT_THAT(dou2.set(), ::testing::UnorderedElementsAreArray(dq1));
}

#if __cplusplus >= 201703L
TEST(DequeOfUniqueTest, SwapIsNoexcept) {
  deque_of_unique<std::string> dou1;
  deque_of_unique<std::string> dou2;
  deque_of_unique<std::string> dou3;

  // Static assertion to check if the swap function is noexcept
  static_assert(noexcept(dou1.swap(dou2)), "Swap function should be noexcept.");

  // Test empty dous
  EXPECT_NO_THROW(dou1.swap(dou2));

  // Test non-empty dous
  dou1.push_back("hello");
  dou3.push_back("world");
  EXPECT_NO_THROW(dou1.swap(dou3));
  EXPECT_TRUE(dou1.front() == "world");
  EXPECT_TRUE(dou3.front() == "hello");

  // Self-swap test (optional but good for robustness)
  EXPECT_NO_THROW(dou1.swap(dou1));  // Self-swap should not throw an exception
}
#endif

TEST(DequeOfUniqueTest, StdSwap) {
  deque_of_unique<std::string> dou1 = {"hello", "world"};
  deque_of_unique<std::string> dou2 = {"good", "morning"};
  std::deque<std::string> dq1 = {"hello", "world"};
  std::deque<std::string> dq2 = {"good", "morning"};

  std::swap(dou1, dou2);
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(dq2));
  EXPECT_EQ(dou2.deque(), dq1);
  EXPECT_THAT(dou2.set(), ::testing::UnorderedElementsAreArray(dq1));
}

TEST(DequeOfUniqueTest, Empty) {
  // Test for empty dou
  deque_of_unique<std::string> dou1;
  static_assert(noexcept(dou1.empty()), "empty() should be noexcept.");
  EXPECT_NO_THROW({
    bool result = dou1.empty();
    EXPECT_TRUE(result);
  });
  EXPECT_TRUE(dou1.empty());

  // Test for non-empty dous
  deque_of_unique<std::string> dou2 = {"good"};
  EXPECT_NO_THROW({
    bool result = dou2.empty();
    EXPECT_FALSE(result);
  });
  EXPECT_FALSE(dou2.empty());

  deque_of_unique<std::string> dou3 = {"good", "morning", "hello", "world"};
  EXPECT_NO_THROW({
    bool result = dou3.empty();
    EXPECT_FALSE(result);
  });
  EXPECT_FALSE(dou3.empty());
}

TEST(DequeOfUniqueTest, Size) {
  // Test 1: Deque with a single element
  deque_of_unique<std::string> dou1 = {"good"};
  EXPECT_EQ(dou1.size(), 1);

  // Test 2: Deque with multiple unique elements
  deque_of_unique<std::string> dou2 = {"good", "morning", "hello", "world"};
  EXPECT_EQ(dou2.size(), 4);

  // Adding a new unique element increases the size
  dou2.push_back("new");
  EXPECT_EQ(dou2.size(), 5);

  // Removing an element decreases the size
  dou2.pop_front();
  EXPECT_EQ(dou2.size(), 4);

  // Attempting to add a duplicate element does not change the size
  dou2.push_back("morning");  // "morning" is already in the deque
  EXPECT_EQ(dou2.size(), 4);

  // Test 3: Empty deque
  deque_of_unique<std::string> dou3;
  EXPECT_EQ(dou3.size(), 0);  // Corrected to check dou3
}

#if __cplusplus < 202002L  // Before C++20
TEST(DequeOfUniqueTest, ComparisonOperatorsWithString) {
  deque_of_unique<std::string> dou1;
  deque_of_unique<std::string> dou2;
  deque_of_unique<std::string> dou3;

  // Test equality operator (operator==)
  EXPECT_TRUE(dou1 == dou2);
  dou1.push_back("Hello");
  EXPECT_FALSE(dou1 == dou2);

  // Test inequality operator (operator!=)
  dou2.push_back("World");
  EXPECT_TRUE(dou1 != dou2);

  // Test less-than operator (operator<)
  dou3.push_back("Apple");
  EXPECT_TRUE(dou3 < dou1);

  // Test less-than-or-equal operator (operator<=)
  EXPECT_TRUE(dou3 <= dou1);
  EXPECT_TRUE(dou1 <= dou2);

  // Test greater-than operator (operator>)
  EXPECT_TRUE(dou1 > dou3);

  // Test greater-than-or-equal operator (operator>=)
  EXPECT_TRUE(dou1 >= dou3);
  EXPECT_TRUE(dou2 >= dou1);
}
#else  // C++20 or later
TEST(DequeOfUniqueTest, ComparisonOperatorsWithStringCXX20) {
  deque_of_unique<std::string> dou1;
  deque_of_unique<std::string> dou2;
  deque_of_unique<std::string> dou3;

  // Test spaceship operator (<=>) for C++20 or later
  EXPECT_TRUE((dou1 <=> dou2) == std::strong_ordering::equal);
  dou1.push_back("Hello");
  dou2.push_back("Hello");
  EXPECT_TRUE((dou1 <=> dou2) == std::strong_ordering::equal);
  dou2.push_back("world");
  EXPECT_TRUE((dou1 <=> dou2) != std::strong_ordering::equal);
  EXPECT_TRUE((dou1 <=> dou2) == std::strong_ordering::less);

  dou3.push_back("Apple");
  EXPECT_TRUE((dou3 <=> dou1) == std::strong_ordering::less);
  dou3.push_front("morning");
  EXPECT_TRUE((dou3 <=> dou1) == std::strong_ordering::greater);
}
#endif

TEST(DequeOfUniqueTest, Find) {
  // Test 1: Find in an empty dou
  deque_of_unique<int> dou_empty;
  auto it_empty = dou_empty.find(10);
  EXPECT_EQ(it_empty, dou_empty.cend());

  // Test 2: Find an element that exists in the dou
  deque_of_unique<int> dou = {10, 20, 30};
  auto it = dou.find(20);
  EXPECT_NE(it, dou.cend());
  EXPECT_EQ(*it, 20);

  // Test 3: Find an element that does not exist
  auto it_not_found = dou.find(40);
  EXPECT_EQ(it_not_found, dou.cend());

  // Test 4: Find element at the beginning
  auto it_first = dou.find(10);
  EXPECT_NE(it_first, dou.cend());
  EXPECT_EQ(*it_first, 10);

  // Test 5: Find element at the end
  auto it_last = dou.find(30);
  EXPECT_NE(it_last, dou.cend());
  EXPECT_EQ(*it_last, 30);

  // Test 6: Multiple lookups
  auto it_again = dou.find(20);
  EXPECT_EQ(it_again, it);
  EXPECT_EQ(*it_again, 20);

  // Test 7: Find in a dou with complex data types (e.g., std::string)
  deque_of_unique<std::string> dou_str = {"hello", "world", "goodbye"};
  auto it_str = dou_str.find("world");
  EXPECT_NE(it_str, dou_str.cend());
  EXPECT_EQ(*it_str, "world");

  auto it_str_not_found = dou_str.find("unknown");
  EXPECT_EQ(it_str_not_found, dou_str.cend());
}

#if __cplusplus >= 202002L  // C++20 or later
TEST(DequeOfUniqueTest, ContainsKeyType) {
  deque_of_unique<int> dou = {1, 2, 3};

  EXPECT_TRUE(dou.contains(1));
  EXPECT_TRUE(dou.contains(2));
  EXPECT_FALSE(dou.contains(4));
}

TEST(DequeOfUniqueTest, ContainsCompatibleType) {
  deque_of_unique<int> dou = {1, 2, 3};
  EXPECT_TRUE(dou.contains(1));
  EXPECT_TRUE(dou.contains(1.0));  // Assuming compatibility
}

TEST(DequeOfUniqueTest, ContainsInEmptyDeque) {
  deque_of_unique<int> dou;
  EXPECT_FALSE(dou.contains(1));
}

TEST(DequeOfUniqueTest, ContainsWithVariousIntTypes) {
  deque_of_unique<int32_t> dou = {1, 2, 3};

  EXPECT_TRUE(dou.contains(int16_t(1)));
  EXPECT_FALSE(dou.contains(int16_t(4)));
}
#endif

TEST(DequeOfUniqueTest, NonmemberEraseWithStrings) {
  deque_of_unique<std::string> dou = {"apple", "banana", "cherry"};

  // Test 1: Erase a string element that exists in the dou
  // Erase an existing string element
  EXPECT_EQ(erase(dou, "banana"), 1);
  EXPECT_EQ(dou.size(), 2);
  EXPECT_EQ(dou.find("banana"), dou.cend());

  // Test 2: Erase a string element that does not exist in the dou
  EXPECT_EQ(erase(dou, "grape"), 0);
  EXPECT_EQ(dou.size(), 2);

  // Test 3: Erase the last string element in the dou
  EXPECT_EQ(erase(dou, "apple"), 1);
  EXPECT_EQ(dou.size(), 1);

  // Test 4: Erase the remaining string element
  EXPECT_EQ(erase(dou, "cherry"), 1);
  EXPECT_EQ(dou.size(), 0);

  // Test 5: Erase from an empty dou
  EXPECT_EQ(erase(dou, "grape"), 0);
  EXPECT_EQ(dou.size(), 0);
}

TEST(DequeOfUniqueTest, NonmemberEraseMultipleStringElements) {
  deque_of_unique<std::string> dou;

  // Test 1: Erase multiple string elements sequentially
  dou.push_back("apple");
  dou.push_back("banana");
  dou.push_back("cherry");

  // Remove first element
  EXPECT_EQ(erase(dou, "apple"), 1);
  EXPECT_EQ(dou.size(), 2);
  EXPECT_EQ(dou.find("apple"), dou.cend());  // "apple" should be removed

  // Remove second element
  EXPECT_EQ(erase(dou, "banana"), 1);
  EXPECT_EQ(dou.size(), 1);
  EXPECT_EQ(dou.find("banana"), dou.cend());  // "banana" should be removed

  // Remove last element
  EXPECT_EQ(erase(dou, "cherry"), 1);
  EXPECT_EQ(dou.size(), 0);
  EXPECT_EQ(dou.find("cherry"), dou.cend());  // "cherry" should be removed
}

TEST(DequeOfUniqueTest, NonmemberEraseEdgeCasesWithStrings) {
  deque_of_unique<std::string> dou;

  // Test 1: Erase from an empty dou
  EXPECT_EQ(erase(dou, "orange"), 0);
  EXPECT_EQ(dou.size(), 0);

  // Test 2: Erase from a dou with a single string element
  dou.push_back("apple");
  EXPECT_EQ(erase(dou, "apple"), 1);
  EXPECT_EQ(dou.size(), 0);
}

TEST(DequeOfUniqueTest, NonmemberEraseNonExistentStringElement) {
  deque_of_unique<std::string> dou;

  // Test 1: Erase non-existent string element from an empty dou
  EXPECT_EQ(erase(dou, "orange"), 0);
  EXPECT_EQ(dou.size(), 0);

  // Test 2: Erase non-existent string element from a dou with some
  // elements
  dou.push_back("apple");
  dou.push_back("banana");
  dou.push_back("cherry");
  EXPECT_EQ(erase(dou, "grape"), 0);
  EXPECT_EQ(dou.size(), 3);
}

TEST(DequeOfUniqueTest, EraseIfBasicFunctionality) {
  deque_of_unique<int> dou = {1, 2, 3, 4, 5, 6};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(dou, pred);
  EXPECT_EQ(removed_count, 3);
  EXPECT_EQ(dou.size(), 3);
  EXPECT_TRUE(dou.find(2) == dou.cend());
  EXPECT_TRUE(dou.find(4) == dou.cend());
  EXPECT_TRUE(dou.find(6) == dou.cend());
}

TEST(DequeOfUniqueTest, EraseIfNoElementsRemoved) {
  deque_of_unique<int> dou = {1, 3, 5, 7, 9};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(dou, pred);
  EXPECT_EQ(removed_count, 0);
  EXPECT_EQ(dou.size(), 5);
}

TEST(DequeOfUniqueTest, EraseIfAllElementsRemoved) {
  deque_of_unique<int> dou = {2, 4, 6, 8, 10};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(dou, pred);
  EXPECT_EQ(removed_count, 5);
  EXPECT_EQ(dou.size(), 0);
}

TEST(DequeOfUniqueTest, EraseIfEmptyContainer) {
  deque_of_unique<int> dou;
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(dou, pred);
  EXPECT_EQ(removed_count, 0);
  EXPECT_EQ(dou.size(), 0);
}

TEST(DequeOfUniqueTest, EraseIfSingleElementRemoved) {
  deque_of_unique<int> dou = {4};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(dou, pred);
  EXPECT_EQ(removed_count, 1);
  EXPECT_EQ(dou.size(), 0);
}

TEST(DequeOfUniqueTest, EraseIfSingleElementNotRemoved) {
  deque_of_unique<int> dou = {3};
  auto pred = [](int x) { return x % 2 == 0; };
  size_t removed_count = erase_if(dou, pred);
  EXPECT_EQ(removed_count, 0);
  EXPECT_EQ(dou.size(), 1);
}

TEST(DequeOfUniqueTest, EraseIfWithStrings) {
  deque_of_unique<std::string> dou = {"apple", "banana", "cherry", "date"};
  auto pred = [](const std::string &s) { return s[0] == 'b'; };
  size_t removed_count = erase_if(dou, pred);
  EXPECT_EQ(removed_count, 1);
  EXPECT_EQ(dou.size(), 3);
  EXPECT_TRUE(dou.find("banana") == dou.cend());
}

TEST(DequeOfUniqueTest, EraseIfWithComplexPredicate) {
  deque_of_unique<std::string> dou = {"apple", "banana", "cherry", "date"};
  auto pred = [](const std::string &s) { return s.length() > 5; };
  size_t removed_count = erase_if(dou, pred);
  EXPECT_EQ(removed_count, 2);
  EXPECT_EQ(dou.size(), 2);
  EXPECT_TRUE(dou.find("banana") == dou.cend());
  EXPECT_TRUE(dou.find("cherry") == dou.cend());
}

TEST(DequeOfUniqueTest, EraseIf_RemainingElementsPreserveOrder) {
  deque_of_unique<int> dou = {1, 2, 3, 4, 5, 6};
  erase_if(dou, [](int x) { return x % 2 == 0; });
  EXPECT_EQ(dou.deque(), std::deque<int>({1, 3, 5}));
}
