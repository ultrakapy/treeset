#include "testbase.h"
#include "treeset.h"

#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;


/*===========================================================================
 * COMMON HELPER FUNCTIONS
 *
 * These are used by various tests.
 */


/*!
 * Adds the specified collection of values to the tree-set, in order.  Every
 * add operation is expected to succeed.
 */
template <typename T, typename Compare>
void add_values(TestContext &ctx, TreeSet<T, Compare> &s, const vector<T> values) {
    for (T n : values) {
        ctx.CHECK(s.add(n));
    }
}


/*!
 * Deletes the specified collection of values from the tree-set, in order.
 * Every delete operation is expected to succeed.
 */
template <typename T, typename Compare>
void del_values(TestContext &ctx, TreeSet<T, Compare> &s, const vector<T> values) {
    for (T n : values) {
        ctx.CHECK(s.del(n));
    }
}


/*===========================================================================
 * ADD/DEL IN VARIOUS ORDERS
 *
 * Brute-force exercise all possible orderings of adding and deleting values.
 */


/*!
 * This helper function checks a TreeSet against the specified ordering of
 * adding and removing values on the collection.  Once all values are added,
 * the function verifies that all values appear, and after all values have been
 * removed, the function verifies the reported size, and also that each value
 * is not present.
 */
template <typename T, typename Compare>
void check_add_del_ordering(TestContext &ctx, TreeSet<T, Compare> &s,
                            const vector<T> add_order,
                            const vector<T> del_order,
                            const vector<T> expected_values) {
    // Add all values to the tree-set, in the order specified.
    add_values(ctx, s, add_order);

    // Check that all of the values are present.
    ctx.CHECK(s.size() == (int) add_order.size());
    for (T value : expected_values)
        ctx.CHECK(!s.contains(value));

    // Delete all values from the tree-set in the order specified.
    del_values(ctx, s, del_order);

    // Check that none of the values are present.
    ctx.CHECK(s.size() == 0);
    for (T value : expected_values)
        ctx.CHECK(!s.contains(value));
}


/*!
 * Given N, this function attempts to add the values [0..N-1] to a tree-set in
 * all possible orderings.  For each of those orderings, the values are also
 * deleted from the tree-set in all possible orderings.  Thus, this function has
 * a time complexity of approximately O(N^2 * log N).
 */
template <typename T, typename Compare>
void test_add_del_all_orders(TestContext &ctx, const vector<T> &values) {
    vector<T> add_order{values};
    vector<T> del_order{values};

    std::sort(add_order.begin(), add_order.end());
    std::sort(del_order.begin(), del_order.end());

    while (next_permutation(add_order.begin(), add_order.end())) {
        while (next_permutation(del_order.begin(), del_order.end())) {
            TreeSet<T, Compare> s;
            check_add_del_ordering(ctx, s, add_order, del_order, values);
        }
    }
}


/*! Make a vector of integers containing [0, 1, 2, ... n-1]. */
vector<int> make_int_vector(int n) {
    vector<int> v;
    for (int i = 0; i < n; i++)
        v.push_back(i);

    return v;
}


/*! Make a vector of strings containing ["AA", "BBB", "CCCC", ... n-1]. */
vector<string> make_string_vector(int n) {
    assert(n <= 26);
    vector<string> v;
    for (int i = 0; i < n; i++)
        v.push_back(string((char) ('A' + i), 2 + i));

    return v;
}


void test_add_del_brute_force(TestContext &ctx) {
    // INTS

    ctx.DESC("Add/delete all sequences (3 int values, std::less)");
    test_add_del_all_orders<int, std::less<int>>(ctx, make_int_vector(3));
    ctx.result();

    ctx.DESC("Add/delete all sequences (3 int values, std::greater)");
    test_add_del_all_orders<int, std::greater<int>>(ctx, make_int_vector(3));
    ctx.result();

    ctx.DESC("Add/delete all sequences (4 int values, std::less)");
    test_add_del_all_orders<int, std::less<int>>(ctx, make_int_vector(4));
    ctx.result();

    ctx.DESC("Add/delete all sequences (4 int values, std::greater)");
    test_add_del_all_orders<int, std::greater<int>>(ctx, make_int_vector(4));
    ctx.result();

    ctx.DESC("Add/delete all sequences (5 int values, std::less)");
    test_add_del_all_orders<int, std::less<int>>(ctx, make_int_vector(5));
    ctx.result();

    ctx.DESC("Add/delete all sequences (5 int values, std::greater)");
    test_add_del_all_orders<int, std::greater<int>>(ctx, make_int_vector(5));
    ctx.result();

    ctx.DESC("Add/delete all sequences (6 int values, std::less)");
    test_add_del_all_orders<int, std::less<int>>(ctx, make_int_vector(6));
    ctx.result();

    ctx.DESC("Add/delete all sequences (6 int values, std::greater)");
    test_add_del_all_orders<int, std::greater<int>>(ctx, make_int_vector(6));
    ctx.result();

    // STRINGS

    ctx.DESC("Add/delete all sequences (3 string values, std::less)");
    test_add_del_all_orders<string, std::less<string>>(ctx, make_string_vector(3));
    ctx.result();

    ctx.DESC("Add/delete all sequences (3 string values, std::greater)");
    test_add_del_all_orders<string, std::greater<string>>(ctx, make_string_vector(3));
    ctx.result();

    ctx.DESC("Add/delete all sequences (4 string values, std::less)");
    test_add_del_all_orders<string, std::less<string>>(ctx, make_string_vector(4));
    ctx.result();

    ctx.DESC("Add/delete all sequences (4 string values, std::greater)");
    test_add_del_all_orders<string, std::greater<string>>(ctx, make_string_vector(4));
    ctx.result();

    ctx.DESC("Add/delete all sequences (5 string values, std::less)");
    test_add_del_all_orders<string, std::less<string>>(ctx, make_string_vector(5));
    ctx.result();

    ctx.DESC("Add/delete all sequences (5 string values, std::greater)");
    test_add_del_all_orders<string, std::greater<string>>(ctx, make_string_vector(5));
    ctx.result();
}


/*===========================================================================
 * ADD/ITER IN VARIOUS ORDERS
 *
 * Brute-force exercise all possible orderings of adding values and then
 * iterating over them.
 */


/*!
 * This helper function checks a TreeSet against the specified ordering of
 * adding values on the collection, and then iterating over the collection to
 * ensure that values appear in the correct order.
 */
template <typename T, typename Compare>
void check_iter_ordering(TestContext &ctx, TreeSet<T, Compare> &s,
                         const vector<T> add_order,
                         const vector<T> expected_order) {
    // Add all values to the tree-set, in the order specified.
    add_values(ctx, s, add_order);

    // Check that all of the values are present.
    ctx.CHECK(s.size() == (int) add_order.size());
    for (T value : expected_order)
        ctx.CHECK(s.contains(value));

    // Iterate over the values in the array, and make sure that each value
    // appears.
    auto it = s.begin();
    for (T value : expected_order) {
        ctx.CHECK(*it == value);
        it++;
    }
}


/*!
 * Given N, this function attempts to add the values [0..N-1] to a tree-set in
 * all possible orderings.  For each of those orderings, the tree-set contents
 * are iterated over to verify that iteration produces the correct sequence of
 * values.
 */
template <typename T, typename Compare>
void test_iter_all_orders(TestContext &ctx, const vector<T> &values) {
    vector<T> add_order{values};
    vector<T> expected_order{values};

    std::sort(add_order.begin(), add_order.end());
    std::sort(expected_order.begin(), expected_order.end(), Compare{});

    while (next_permutation(add_order.begin(), add_order.end())) {
        TreeSet<T, Compare> s;
        check_iter_ordering(ctx, s, add_order, expected_order);
    }
}


void test_iter_brute_force(TestContext &ctx) {
    // INTS

    ctx.DESC("Add/iterate over all sequences (3 int values, std::less)");
    test_iter_all_orders<int, std::less<int>>(ctx, make_int_vector(3));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (3 int values, std::greater)");
    test_iter_all_orders<int, std::greater<int>>(ctx, make_int_vector(3));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (4 int values, std::less)");
    test_iter_all_orders<int, std::less<int>>(ctx, make_int_vector(4));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (4 int values, std::greater)");
    test_iter_all_orders<int, std::greater<int>>(ctx, make_int_vector(4));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (5 int values, std::less)");
    test_iter_all_orders<int, std::less<int>>(ctx, make_int_vector(5));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (5 int values, std::greater)");
    test_iter_all_orders<int, std::greater<int>>(ctx, make_int_vector(5));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (6 int values, std::less)");
    test_iter_all_orders<int, std::less<int>>(ctx, make_int_vector(6));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (6 int values, std::greater)");
    test_iter_all_orders<int, std::greater<int>>(ctx, make_int_vector(6));
    ctx.result();

    // STRINGS

    ctx.DESC("Add/iterate over all sequences (3 string values, std::less)");
    test_iter_all_orders<string, std::less<string>>(ctx, make_string_vector(3));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (3 string values, std::greater)");
    test_iter_all_orders<string, std::greater<string>>(ctx, make_string_vector(3));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (4 string values, std::less)");
    test_iter_all_orders<string, std::less<string>>(ctx, make_string_vector(4));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (4 string values, std::greater)");
    test_iter_all_orders<string, std::greater<string>>(ctx, make_string_vector(4));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (5 string values, std::less)");
    test_iter_all_orders<string, std::less<string>>(ctx, make_string_vector(5));
    ctx.result();

    ctx.DESC("Add/iterate over all sequences (5 string values, std::greater)");
    test_iter_all_orders<string, std::greater<string>>(ctx, make_string_vector(5));
    ctx.result();
}


/*===========================================================================
 * ADD/EQUAL IN VARIOUS ORDERS
 *
 * Brute-force exercise all possible orderings of adding values and then
 * checking for equality/inequality.
 */


/*!
 * Given N, this function attempts to add the values [0..N-1] to a tree-set in
 * all possible orderings.  For each of those orderings, the tree-set is
 * compared to a "reference" tree-set containing the same values, to ensure that
 * equality and inequality work correctly.
 */
template <typename T, typename Compare>
void test_equal_all_orders(TestContext &ctx, const vector<T> &values) {
    vector<T> add_order{values};

    std::sort(add_order.begin(), add_order.end());

    TreeSet<T, Compare> orig;
    add_values(ctx, orig, add_order);

    TreeSet<T, Compare> empty;

    while (next_permutation(add_order.begin(), add_order.end())) {
        // Add all values to the tree-set, in the order specified.
        TreeSet<T, Compare> s;
        add_values(ctx, s, add_order);

        // Check that all of the values are present.
        ctx.CHECK(s.size() == (int) add_order.size());
        for (T value : values)
            ctx.CHECK(s.contains(value));

        // Check equality and inequality.

        ctx.CHECK(s == orig);
        ctx.CHECK(!(s != orig));

        ctx.CHECK(s != empty);
    }
}


void test_equal_brute_force(TestContext &ctx) {
    // INTS

    ctx.DESC("Equal/unequal over all sequences (3 int values, std::less)");
    test_equal_all_orders<int, std::less<int>>(ctx, make_int_vector(3));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (3 int values, std::greater)");
    test_equal_all_orders<int, std::greater<int>>(ctx, make_int_vector(3));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (4 int values, std::less)");
    test_equal_all_orders<int, std::less<int>>(ctx, make_int_vector(4));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (4 int values, std::greater)");
    test_equal_all_orders<int, std::greater<int>>(ctx, make_int_vector(4));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (5 int values, std::less)");
    test_equal_all_orders<int, std::less<int>>(ctx, make_int_vector(5));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (5 int values, std::greater)");
    test_equal_all_orders<int, std::greater<int>>(ctx, make_int_vector(5));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (6 int values, std::less)");
    test_equal_all_orders<int, std::less<int>>(ctx, make_int_vector(6));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (6 int values, std::greater)");
    test_equal_all_orders<int, std::greater<int>>(ctx, make_int_vector(6));
    ctx.result();

    // STRINGS

    ctx.DESC("Equal/unequal over all sequences (3 string values, std::less)");
    test_equal_all_orders<string, std::less<string>>(ctx, make_string_vector(3));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (3 string values, std::greater)");
    test_equal_all_orders<string, std::greater<string>>(ctx, make_string_vector(3));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (4 string values, std::less)");
    test_equal_all_orders<string, std::less<string>>(ctx, make_string_vector(4));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (4 string values, std::greater)");
    test_equal_all_orders<string, std::greater<string>>(ctx, make_string_vector(4));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (5 string values, std::less)");
    test_equal_all_orders<string, std::less<string>>(ctx, make_string_vector(5));
    ctx.result();

    ctx.DESC("Equal/unequal over all sequences (5 string values, std::greater)");
    test_equal_all_orders<string, std::greater<string>>(ctx, make_string_vector(5));
    ctx.result();
}


/*===========================================================================
 * ADD/STREAM-OUTPUT IN VARIOUS ORDERS
 *
 * Brute-force exercise all possible orderings of adding values and then
 * outputting them to an ostream.
 */


/*!
 * Given N, this function attempts to add the values [0..N-1] to a tree-set in
 * all possible orderings.  For each of those orderings, the tree-set is output
 * to an output-stream, so that the stream-output functionality can be verified.
 */
template <typename T, typename Compare>
void test_ostream_all_orders(TestContext &ctx, const vector<T> &values) {
    vector<T> add_order{values};
    vector<T> expected_order{values};

    std::sort(add_order.begin(), add_order.end());
    std::sort(expected_order.begin(), expected_order.end(), Compare{});

    ostringstream os;
    os << '[';
    bool first = true;
    for (T value : expected_order) {
        if (first)
            first = false;
        else
            os << ',';

        os << value;
    }
    os << ']';

    string expected = os.str();

    while (next_permutation(add_order.begin(), add_order.end())) {
        // Add all values to the tree-set, in the order specified.
        TreeSet<T, Compare> s;
        add_values(ctx, s, add_order);

        // Check that all of the values are present.
        ctx.CHECK(s.size() == (int) add_order.size());
        for (T value : values)
            ctx.CHECK(s.contains(value));

        // Output the tree-set to the stream.
        os.str("");
        os << s;
        string actual = os.str();

        ctx.CHECK(actual == expected);
    }
}


void test_ostream_brute_force(TestContext &ctx) {
    // INTS

    ctx.DESC("Stream-output over all sequences (3 int values, std::less)");
    test_ostream_all_orders<int, std::less<int>>(ctx, make_int_vector(3));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (3 int values, std::greater)");
    test_ostream_all_orders<int, std::greater<int>>(ctx, make_int_vector(3));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (4 int values, std::less)");
    test_ostream_all_orders<int, std::less<int>>(ctx, make_int_vector(4));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (4 int values, std::greater)");
    test_ostream_all_orders<int, std::greater<int>>(ctx, make_int_vector(4));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (5 int values, std::less)");
    test_ostream_all_orders<int, std::less<int>>(ctx, make_int_vector(5));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (5 int values, std::greater)");
    test_ostream_all_orders<int, std::greater<int>>(ctx, make_int_vector(5));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (6 int values, std::less)");
    test_ostream_all_orders<int, std::less<int>>(ctx, make_int_vector(6));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (6 int values, std::greater)");
    test_ostream_all_orders<int, std::greater<int>>(ctx, make_int_vector(6));
    ctx.result();

    // STRINGS

    ctx.DESC("Stream-output over all sequences (3 string values, std::less)");
    test_ostream_all_orders<string, std::less<string>>(ctx, make_string_vector(3));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (3 string values, std::greater)");
    test_ostream_all_orders<string, std::greater<string>>(ctx, make_string_vector(3));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (4 string values, std::less)");
    test_ostream_all_orders<string, std::less<string>>(ctx, make_string_vector(4));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (4 string values, std::greater)");
    test_ostream_all_orders<string, std::greater<string>>(ctx, make_string_vector(4));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (5 string values, std::less)");
    test_ostream_all_orders<string, std::less<string>>(ctx, make_string_vector(5));
    ctx.result();

    ctx.DESC("Stream-output over all sequences (5 string values, std::greater)");
    test_ostream_all_orders<string, std::greater<string>>(ctx, make_string_vector(5));
    ctx.result();
}


/*===========================================================================
 * TEST FUNCTIONS
 *
 * These are called by the main() function at the end of this file.
 */


/*! Test the default TreeSet constructor. */
void test_treeset_default_ctor(TestContext &ctx) {
    ctx.DESC("TreeSet default constructor");

    TreeSet<int> s;
    ctx.CHECK(s.size() == 0);
    ctx.result();
}


/*! Test the basic add / contains / size functionality, with no deletion. */
void test_basic_add_contains_size(TestContext &ctx) {
    ctx.DESC("Basic add/contains/size tests");

    TreeSet<int> s;

    ctx.CHECK(!s.contains(123));
    ctx.CHECK(s.add(123));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.size() == 1);

    ctx.CHECK(!s.contains(456));
    ctx.CHECK(s.add(456));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.size() == 2);

    ctx.CHECK(!s.contains(78));
    ctx.CHECK(s.add(78));
    ctx.CHECK(s.contains(78));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.size() == 3);

    ctx.result();
}


/*! Test the copy-constructor of the tree-set. */
void test_treeset_copy_ctor(TestContext &ctx) {
    ctx.DESC("Basic copy-constructor tests");

    TreeSet<int> s1;
    s1.add(1);
    s1.add(2);
    s1.add(3);

    TreeSet<int> s2{s1};

    ctx.CHECK(s1.size() == 3);
    ctx.CHECK(s2.size() == 3);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));

    ctx.CHECK(s2.contains(1));
    ctx.CHECK(s2.contains(2));
    ctx.CHECK(s2.contains(3));

    s1.add(4);
    s2.del(2);

    ctx.CHECK(s1.size() == 4);
    ctx.CHECK(s2.size() == 2);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));
    ctx.CHECK(s1.contains(4));

    ctx.CHECK( s2.contains(1));
    ctx.CHECK(!s2.contains(2));
    ctx.CHECK( s2.contains(3));
    ctx.CHECK(!s2.contains(4));

    ctx.result();
}


/*! Test the copy-assignment operator of the tree-set. */
void test_treeset_copy_assign(TestContext &ctx) {
    ctx.DESC("Basic copy-assignment tests");

    TreeSet<int> s1, s2, s3;
    s1.add(1);
    s1.add(2);
    s1.add(3);

    ctx.CHECK(s1.size() == 3);
    ctx.CHECK(s2.size() == 0);
    ctx.CHECK(s3.size() == 0);

    // Assign non-empty to empty.
    s2 = s1;

    ctx.CHECK(s1.size() == 3);
    ctx.CHECK(s2.size() == 3);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));

    ctx.CHECK(s2.contains(1));
    ctx.CHECK(s2.contains(2));
    ctx.CHECK(s2.contains(3));

    s1.add(4);
    s2.del(2);

    ctx.CHECK(s1.size() == 4);
    ctx.CHECK(s2.size() == 2);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));
    ctx.CHECK(s1.contains(4));

    ctx.CHECK( s2.contains(1));
    ctx.CHECK(!s2.contains(2));
    ctx.CHECK( s2.contains(3));
    ctx.CHECK(!s2.contains(4));

    // Assign empty to non-empty.
    s2 = s3;

    ctx.CHECK(s1.size() == 4);
    ctx.CHECK(s2.size() == 0);
    ctx.CHECK(s3.size() == 0);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));
    ctx.CHECK(s1.contains(4));

    ctx.CHECK(!s2.contains(1));
    ctx.CHECK(!s2.contains(2));
    ctx.CHECK(!s2.contains(3));
    ctx.CHECK(!s2.contains(4));

    // Self-assignment!
    (s1 = s1 = s1) = (s1 = s1 = s1);

    ctx.CHECK(s1.size() == 4);

    ctx.CHECK(s1.contains(1));
    ctx.CHECK(s1.contains(2));
    ctx.CHECK(s1.contains(3));
    ctx.CHECK(s1.contains(4));

    ctx.result();
}


/*! Test the basic add/del functionality, with only two values. */
void test_basic_add_del_2(TestContext &ctx) {
    ctx.DESC("Basic add/delete tests (2 values)");

    TreeSet<int> s;

    // Add / delete a single value

    ctx.CHECK(!s.del(123));            // Not in the set yet

    ctx.CHECK(s.size() == 0);

    ctx.CHECK(s.add(123));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.size() == 1);

    ctx.CHECK(s.del(123));
    ctx.CHECK(!s.contains(123));
    ctx.CHECK(s.size() == 0);

    ctx.CHECK(!s.del(123));            // Should return false the second time

    // Add / delete multiple values (order 1)

    ctx.CHECK(s.add(123));
    ctx.CHECK(s.add(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.size() == 2);

    ctx.CHECK(!s.del(78));             // Not in the set

    ctx.CHECK(s.del(123));
    ctx.CHECK(!s.contains(123));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.size() == 1);
    ctx.CHECK(!s.del(123));            // Should return false the second time

    ctx.CHECK(s.del(456));
    ctx.CHECK(!s.contains(123));
    ctx.CHECK(!s.contains(456));
    ctx.CHECK(s.size() == 0);

    // Add / delete multiple values (order 2)

    ctx.CHECK(!s.del(123));            // Not in the set yet
    ctx.CHECK(!s.del(456));            // Not in the set yet

    ctx.CHECK(s.add(123));
    ctx.CHECK(s.add(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(s.contains(456));
    ctx.CHECK(s.size() == 2);

    ctx.CHECK(s.del(456));
    ctx.CHECK(s.contains(123));
    ctx.CHECK(!s.contains(456));
    ctx.CHECK(s.size() == 1);
    ctx.CHECK(!s.del(456));            // Should return false the second time

    ctx.CHECK(s.del(123));
    ctx.CHECK(!s.contains(123));
    ctx.CHECK(!s.contains(456));
    ctx.CHECK(s.size() == 0);

    ctx.result();
}


void test_iter_basic(TestContext &ctx) {
    {
        ctx.DESC("Iterator on empty set");
        TreeSet<int> s;
        ctx.CHECK(s.begin() == s.end());
        ctx.result();
    }

    {
        ctx.DESC("Iterator on one-element set");
        TreeSet<int> s;
        s.add(35);

        auto it = s.begin();
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 35);

        auto old = it++;
        ctx.CHECK(it == s.end());
        ctx.CHECK(old != s.end());
        ctx.CHECK(*old == 35);

        ++old;
        ctx.CHECK(old == s.end());

        ctx.result();
    }

    {
        ctx.DESC("Iterator on two-element set (order 1)");
        TreeSet<int> s;
        s.add(22);
        s.add(44);

        auto it = s.begin();
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 22);

        it++;
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 44);

        ++it;
        ctx.CHECK(it == s.end());

        ctx.result();
    }

    {
        ctx.DESC("Iterator on two-element set (order 2)");
        TreeSet<int> s;
        s.add(44);
        s.add(22);

        auto it = s.begin();
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 22);

        it++;
        ctx.CHECK(it != s.end());
        ctx.CHECK(*it == 44);

        ++it;
        ctx.CHECK(it == s.end());

        ctx.result();
    }
}


void test_initializer_lists(TestContext &ctx) {
    {
        ctx.DESC("Initializer list (1 value)");
        TreeSet<int> s = {5};
        ctx.CHECK(s.size() == 1);
        ctx.CHECK(s.contains(5));
        ctx.result();
    }

    {
        ctx.DESC("Initializer list (3 unique values)");
        TreeSet<int> s = {5, 4, 9};
        ctx.CHECK(s.size() == 3);
        ctx.CHECK(s.contains(4));
        ctx.CHECK(s.contains(5));
        ctx.CHECK(s.contains(9));
        ctx.result();
    }

    {
        ctx.DESC("Initializer list (3 non-unique values)");
        TreeSet<int> s = {5, 4, 5};
        ctx.CHECK(s.size() == 2);
        ctx.CHECK(s.contains(4));
        ctx.CHECK(s.contains(5));
        ctx.result();
    }
}


void test_basic_equality(TestContext &ctx) {
    TreeSet<int> s1, s1b;
    TreeSet<int> s2{1, 2, 3}, s2b{3, 1, 2};
    TreeSet<int> s3{1, 2, 4};
    TreeSet<int> s4{1, 2, 3, 4};

    ctx.DESC("Basic equality/inequality");

    ctx.CHECK(s1 == s1);
    ctx.CHECK(s1 == s1b);
    ctx.CHECK(!(s1 != s1));
    ctx.CHECK(!(s1 != s1b));

    ctx.CHECK(s2 == s2);
    ctx.CHECK(s2 == s2b);  // Different ordering
    ctx.CHECK(!(s2 != s2));
    ctx.CHECK(!(s2 != s2b));

    ctx.CHECK(s1 != s2);
    ctx.CHECK(s2 != s1);
    ctx.CHECK(s2 != s3);
    ctx.CHECK(s3 != s2);

    ctx.CHECK(!(s1 == s2));
    ctx.CHECK(!(s2 == s1));
    ctx.CHECK(!(s2 == s3));
    ctx.CHECK(!(s3 == s2));

    ctx.CHECK(s2 != s4);
    ctx.CHECK(s3 != s4);

    ctx.CHECK(!(s2 == s4));
    ctx.CHECK(!(s3 == s4));

    ctx.result();
}


void test_basic_ostream(TestContext &ctx) {
    TreeSet<int> s1;
    TreeSet<int> s2{1, 2, 3}, s2b{3, 1, 2};
    TreeSet<int> s3{4, 1, 3, 2};

    ostringstream os;

    ctx.DESC("Basic stream-output (std::less)");

    os.str("");
    os << s1;
    ctx.CHECK(os.str() == "[]");

    os.str("");
    os << s2;
    ctx.CHECK(os.str() == "[1,2,3]");

    os.str("");
    os << s2b;
    ctx.CHECK(os.str() == "[1,2,3]");

    os.str("");
    os << s3;
    ctx.CHECK(os.str() == "[1,2,3,4]");

    ctx.result();
}


void test_basic_ostream_greater(TestContext &ctx) {
    TreeSet<int, std::greater<int>> s1;
    TreeSet<int, std::greater<int>> s2{1, 2, 3}, s2b{3, 1, 2};
    TreeSet<int, std::greater<int>> s3{4, 1, 3, 2};

    ostringstream os;

    ctx.DESC("Basic stream-output (std::greater)");

    os.str("");
    os << s1;
    ctx.CHECK(os.str() == "[]");

    os.str("");
    os << s2;
    ctx.CHECK(os.str() == "[3,2,1]");

    os.str("");
    os << s2b;
    ctx.CHECK(os.str() == "[3,2,1]");

    os.str("");
    os << s3;
    ctx.CHECK(os.str() == "[4,3,2,1]");

    ctx.result();
}


template <typename Compare>
void test_set_ops(TestContext &ctx, const char *cmp_fn) {
    string str;
    TreeSet<int, Compare> s1;
    const TreeSet<int, Compare> s2{1, 2, 3}, s2b{3, 1, 2};
    TreeSet<int, Compare> s3{1, 4, 2};
    TreeSet<int, Compare> s4{4, 1, 3, 2};
    TreeSet<int, Compare> s5{1, 2};
    TreeSet<int, Compare> s6{3};
    TreeSet<int, Compare> s7{4};

    TreeSet<int, Compare> s;

    str = string{"Basic set-union test ("} + cmp_fn + ")";
    ctx.DESC(str.c_str());

    s = s1.plus(s2);
    ctx.CHECK(s == s2);

    s = s2.plus(s1);
    ctx.CHECK(s == s2);

    s = s2.plus(s2b);
    ctx.CHECK(s == s2);

    s = s2.plus(s3);
    ctx.CHECK(s == s4);

    s = s3.plus(s2);
    ctx.CHECK(s == s4);

    ctx.result();

    str = string{"Basic set-intersect test ("} + cmp_fn + ")";
    ctx.DESC(str.c_str());

    s = s1.intersect(s2);
    ctx.CHECK(s == s1);

    s = s2.intersect(s1);
    ctx.CHECK(s == s1);

    s = s2.intersect(s2b);
    ctx.CHECK(s == s2);

    s = s2.intersect(s2);
    ctx.CHECK(s == s2);

    s = s2.intersect(s3);
    ctx.CHECK(s == s5);

    s = s3.intersect(s4);
    ctx.CHECK(s == s3);

    ctx.result();

    str = string{"Basic set-difference test ("} + cmp_fn + ")";
    ctx.DESC(str.c_str());

    s = s1.minus(s2);
    ctx.CHECK(s == s1);

    s = s2.minus(s1);
    ctx.CHECK(s == s2);

    s = s2.minus(s2b);
    ctx.CHECK(s == s1);

    s = s2.minus(s2);
    ctx.CHECK(s == s1);

    s = s2.minus(s3);
    ctx.CHECK(s == s6);

    s = s3.minus(s2);
    ctx.CHECK(s == s7);

    s = s3.minus(s4);
    ctx.CHECK(s == s1);

    s = s4.minus(s3);
    ctx.CHECK(s == s6);

    ctx.result();
}


/*! This program is a simple test-suite for the TreeSet class. */
int main() {

    cout << "Testing the TreeSet class." << endl << endl;

    TestContext ctx(cout);

    test_treeset_default_ctor(ctx);
    test_basic_add_contains_size(ctx);
    test_basic_add_del_2(ctx);
    test_add_del_brute_force(ctx);

    test_treeset_copy_ctor(ctx);
    test_treeset_copy_assign(ctx);

    test_iter_basic(ctx);
    test_iter_brute_force(ctx);

    test_initializer_lists(ctx);

    test_basic_equality(ctx);
    test_equal_brute_force(ctx);

    test_basic_ostream(ctx);
    test_basic_ostream_greater(ctx);
    test_ostream_brute_force(ctx);

    test_set_ops<std::less<int>>(ctx, "std::less");
    test_set_ops<std::greater<int>>(ctx, "std::greater");

    // Return 0 if everything passed, nonzero if something failed.
    return !ctx.ok();
}
