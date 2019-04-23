#include <cmath>
#include <algorithm>
#include <iterator>
#include <vector>
#include <type_traits>
#include <functional>
namespace tinystl {

using std::vector;

template <typename Iterator, typename Compare = std::less_equal<typename std::iterator_traits<Iterator>::value_type>>
void insert_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    static_assert(std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>::value, "only allow random iterator");
    auto cur = first;
    while (cur != last) {
        auto value = *cur;
        auto next = cur + 1;
        auto prev = cur - 1;
        while (cur > first && compare(value, *prev)) {
            *cur = *prev;
            --cur;
            --prev;
        }
        *cur = value;
        cur = next;
    }
}

template <typename Iterator, typename Compare = std::less_equal<typename std::iterator_traits<Iterator>::value_type>>
void enable_midian(Iterator first, Iterator last, Compare compare) {
    static_assert(std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>::value, "only allow random iterator");
    auto middle = first + (last - first) / 2;
    --last;
    if (compare(*middle, *first)) {
        std::swap(*middle, *first);
    }
    if (compare(*last, *first)) {
        std::swap(*last, *first);
    }
    if (compare(*last, *middle)) {
        std::swap(*middle, *last);
    }
    std::swap(*first, *middle);
}

template <typename Iterator, typename Compare = std::less_equal<typename std::iterator_traits<Iterator>::value_type>>
void quick_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    static_assert(std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>::value, "only allow random iterator");
    if (first == last || first + 1 == last) {
        return;
    }
    if (last - first <= 10) {
        insert_sort(first, last, compare);
        return;
    }
    enable_midian(first, last, compare);
    auto pivot = *first;
    auto f = first;
    auto l = last;
    --last;

    int left_pivot_size = 0;
    int right_pivot_size = 0;
    while (first < last) {
        while (first < last && (!compare(*last, pivot) || (compare(*last, pivot) && compare(pivot, *last)))) {
            if (compare(*last, pivot) && compare(pivot, *last)) {
                std::swap(*last, *(l - right_pivot_size++ - 1));
            }
            --last;
        }
        if (first < last) {
            std::swap(*(first++), *last);
        }
        while (first < last && compare(*first, pivot)) {
            if (compare(*first, pivot) && compare(pivot, *first)) {
                std::swap(*first, *(f + left_pivot_size++));
            }
            ++first;
        }
        if (first < last) {
            std::swap(*first, *(last--));
        }
    }
    *first = pivot;
    --first;
    ++last;
    for (int i = 0; i < left_pivot_size; ++i) {
        std::swap(*(first--), *(f + i));
    }
    for (int i = 0; i < right_pivot_size; ++i) {
        std::swap(*(last++), *(l - i - 1));
    }
    quick_sort(f, first + 1, compare);
    quick_sort(last, l, compare);
}


template <typename Iterator, typename Compare = std::less_equal<typename std::iterator_traits<Iterator>::value_type>>
void merge_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    static_assert(std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>::value, "only allow random iterator");
    if (last - first <= 1) {
        return;
    }
    auto middle = first + (last - first) / 2;
    merge_sort(first, middle, compare);
    merge_sort(middle, last, compare);

    using value_type = typename std::iterator_traits<Iterator>::value_type;
    vector<value_type> result(last - first, value_type{});

    auto left = first;
    auto right = middle;
    int index = 0;
    while (left < middle && right < last) {
        if (compare(*left, *right)) {
            result[index++] = *(left++);
        }
        else {
            result[index++] = *(right++);
        }
    }
    while (left < middle) {
        result[index++] = *(left++);
    }
    while (right < last) {
        result[index++] = *(right++);
    }
    std::copy(result.begin(), result.end(), first);
    // assert(std::is_sorted(first, last, compare));
}


template <typename Iterator, typename Compare = std::less_equal<typename std::iterator_traits<Iterator>::value_type>>
void select_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    static_assert(std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>::value, "only allow random iterator");
    auto right = last - 1;
    while (right > first) {
        auto max_iter = first;
        auto cur = first + 1;
        while (cur <= right) {
            if (compare(*max_iter, *cur)) {
                max_iter = cur;
            }
            ++cur;
        }
        std::swap(*max_iter, *right);
        --right;
    }
}

template <typename Iterator, typename Compare = std::less_equal<typename std::iterator_traits<Iterator>::value_type>>
void bubble_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    static_assert(std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>::value, "only allow random iterator");
    auto right = last - 1;
    while (right > first) {
        auto max_iter = first;
        auto cur = first + 1;
        while (cur <= right) {
            if (compare(*cur, *(cur - 1))) {
                std::swap(*cur, *(cur - 1));
            }
            ++cur;
        }
        --right;
    }
}

void bubble_sort(vector<int>& nums) {
    for (int i = 0; i < nums.size() - 1; ++i) {
        for (int j = 1; j < nums.size() - i; ++j) {
            if (nums[j] < nums[j - 1]) {
                std::swap(nums[j], nums[j - 1]);
            }
        }
    }
}

void select_sort(vector<int>& nums) {
    for (int i = 0; i < nums.size() - 1; ++i) {
        auto max_iter = std::max_element(nums.begin(), nums.end() - i);
        std::swap(*max_iter, *(nums.end() - i - 1));
    }
}

void insert_sort(vector<int>& nums) {
    int cur = 0;
    for (int i = 1; i < nums.size(); ++i) {
        int value = nums[i];
        int cur = i;
        int prev = i - 1;
        while (cur > 0 && nums[prev] > value) {
            nums[cur] = nums[prev];
            --cur;
            --prev;
        }
        nums[cur] = value;
    }
}

void shell_sort(vector<int>& nums) {
    size_t len = nums.size();
    for (int gap = std::floor(len / 2); gap > 0; gap = std::floor(gap / 2)) {
        for (int i = gap; i < len; ++i) {
            auto value = nums[i];
            int index = i;
            while (index - gap >= 0 && nums[index - gap] > value) {
                nums[index] = nums[index - gap];
                index -= gap;
            }
            nums[index] = value;
        }
    }
}

void make_heap(vector<int>& nums) {
    int len = nums.size();
    int current = len;
    for (int i = len; i > 0; --i) {
        int value = nums[i - 1];
        int current = i;
        int child = current * 2;
        while (child <= len) {
            if (child < len && nums[child - 1] > nums[child]) {
                ++child;
            }
            if (nums[child - 1] >= value) {
                break;
            }
            nums[current - 1] = nums[child - 1];
            current = child;
            child = current * 2;
        }
        nums[current - 1] = value;
    }
}

void pop_heap(vector<int>& nums) {
    int len = nums.size();
    std::swap(nums[0], nums[len - 1]);
    int current = 1;
    int child = current * 2;
    int value = nums[current - 1];
    while (child <= len - 1) {
        if (child < len - 1 && nums[child - 1] > nums[child]) {
            ++child;
        }
        if (nums[child - 1] > value) {
            break;
        }
        nums[current - 1] = nums[child - 1];
        current = child;
        child = current * 2;
    }
    nums[current - 1] = value;
}

void push_heap(vector<int>& nums) {
    int len = nums.size();
    int current = len;
    int parent = current / 2;
    int value = nums[current - 1];
    while (parent > 0) {
        if (value >= nums[parent - 1]) {
            break;
        }
        nums[current - 1] = nums[parent - 1];
        current = parent;
        parent = current / 2;
    }
    nums[current - 1] = value;
}
void heap_sort(vector<int>& nums) {
    make_heap(nums);
    vector<int> results;
    while (!nums.empty()) {
        pop_heap(nums);
        results.emplace_back(nums.back());
        nums.pop_back();
    }
    nums.swap(results);
}

template <typename Iterator, typename Compare = std::less_equal<typename std::iterator_traits<Iterator>::value_type>>
void list_quick_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    static_assert(std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::bidirectional_iterator_tag>::value, "only allow bidirectional iterator");
    if (first == last) {
        return;
    }
    Iterator p = first;
    Iterator q = first;
    ++q;
    while (q != last) {
        if (compare(*q, *first)) {
            ++p;
            std::swap(*p, *q);
        }
        ++q;
    }
    std::swap(*p, *first);
    list_quick_sort(first, p);
    list_quick_sort(++p, last);
}


void make_heap(vector<int>& nums) {
    int len = nums.size();
    for (int i = len; i > 0; --i) {
        int current = i;
        int child = current * 2;
        int value = nums[current - 1];
        while (child <= len) {
            if (child < len && nums[child - 1] > nums[child]) {
                ++child;
            }
            if (nums[child - 1] >= value) {
                break;
            }
            nums[current - 1] = nums[child - 1];
            current = child;
            child = current * 2;
        }
        nums[current - 1] = value;
    }
}
void push_heap(vector<int>& nums) {
    int len = nums.size();
    int current = len;
    int parent = current / 2;
    int value = nums[current - 1];
    while (current > 1) {
        if (nums[parent - 1] <= value) {
            break;
        }
        nums[current - 1] = nums[parent - 1];
        current = parent;
        parent = current / 2;
    }
    nums[current - 1] = value;
}

void pop_heap(vector<int>& nums) {
    int len = nums.size();
    std::swap(nums[0], nums[len - 1]);
    int current = 1;
    int child = current * 2;
    int value = nums[current - 1];
    while (child <= len) {
        if (child < len && nums[child - 1] > nums[child]) {
            ++child;
        }
        nums[current - 1] = nums[child - 1];
        current = child;
        child = current * 2;
    }
    nums[current - 1] = value;
}

template <typename Iterator, typename Compare = std::less_equal<typename std::iterator_traits<Iterator>::value_type>>
void link_quick_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    if (first == last) {
        return;
    }
    Iterator p = first;
    Iterator q = first;
    ++q;
    while (q != last) {
        if (compare(*q, *first)) {
            ++p;
            std::swap(*p, *q);
        }
        ++q;
    }
    std::swap(*first, *p);
    link_quick_sort(first, p, compare);
    link_quick_sort(++p, last, compare);
}


template <typename Iterator, typename Compare = std::less_equal<std::iterator_traits<Iterator>::value_type>>
void insert_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    if (first == last) {
        return;
    }
    Iterator current = first;
    for (auto it = first; it != last; ++it) {
        current = it;
        int value = *current;
        while (current != first && compare(value, *(current - 1))) {
            *current = *(current - 1);
            --current;
        }
        *current = value;
    }
}

template <typename Iterator, typename Compare = std::less_equal<std::iterator_traits<Iterator>::value_type>>
void enable_median(Iterator first, Iterator last, Compare compare = Compare{}) {
    if (first == last) {
        return;
    }
    Iterator middle = first + (last - first) / 2;
    if (compare(*middle, *first)) {
        std::swap(*middle, *first);
    }
    if (compare(*last, *first)) {
        std::swap(*last, *first);
    }
    if (compare(*last, *middle)) {
        std::swap(*last, *middle);
    }
    std::swap(*first, *middle);
}

template <typename Iterator, typename Compare = std::less_equal<std::iterator_traits<Iterator>::value_type>>
void quick_sort(Iterator first, Iterator last, Compare compare = Compare{}) {
    if (first == last) {
        return;
    }
    if (std::distance(first, last) <= 10) {
        insert_sort(first, last, compare);
        return;
    }
    enable_median(first, last, compare);
    using value_type = std::iterator_traits<Iterator>::value_type;
    value_type pivot = *first;
    Iterator left = first;
    Iterator right = last;
    --right;
    size_t left_pivot_size = 0;
    size_t right_pivot_size = 0;
    while (left < last) {
        while (left < right && (compare(pivot, *right) || pivot == *right)) {
            if (pivot == *right) {
                std::swap(*right, *(last - right_pivot_size++ - 1));
            }
            --right;
        }
        if (left < right) {
            std::swap(*(left++), *right);
        }
        while (left < right && (compare(*left, pivot) || *left == pivot)) {
            if (*left == pivot) {
                std::swap(*left, *(first + left_pivot_size++));
            }
            ++left;
        }
        if (left < right) {
            std::swap(*left, *(right--));
        }
    }
    *left = pivot;
    for (size_t i = 0; i < left_pivot_size; ++i) {
        std::swap(*(left - i - 1), *(first + i));
    }
    for (size_t i = 0; i < right_pivot_size; ++i) {
        std::swap(*(right + i + 1), *(last - i - 1));
    }
    quick_sort(first, left - left_pivot_size, compare);
    quick_sort(right + right_pivot_size + 1, last, compare);
}

}