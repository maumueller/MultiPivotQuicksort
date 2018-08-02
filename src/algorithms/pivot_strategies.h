namespace pivot_strategies {

template<typename iter>
inline void sort_pair(iter i1, iter i2) {
	typedef typename std::iterator_traits<iter>::value_type T;
	bool smaller = *i2 < *i1;
	T temp = std::move(smaller ? *i1 : temp);
	*i1 = std::move(smaller ? *i2 : *i1);
	*i2 = std::move(smaller ? temp : *i2);
}

template <typename iter>
iter direct(iter i1) {
    return i1;
}

template <typename iter>
iter median_of_3(iter i1, iter i2) {
	iter i3 = i1 + ((i2 - i1 - 1) / 2);
    sort_pair(i1, i2);
    sort_pair(i2, i3);
    sort_pair(i1, i2);
	return i2;
}

template<typename iter>
inline iter median_of_5(iter i1, iter i2, iter i3 , iter i4, iter i5) {
	sort_pair(i1, i2);
	sort_pair(i4, i5);
	sort_pair(i1, i4);
	sort_pair(i2, i5);
	sort_pair(i3, i4);
	sort_pair(i2, i3);
	sort_pair(i3, i4);
	return i3;
}

template<typename iter>
inline iter median_of_5(iter begin, iter end) {
	iter left = begin + (end - begin) / 4;
	iter mid = begin + (end - begin) / 2;
	iter right = begin + (3 * (end - begin)) / 4;
	iter last = end;

	sort_pair(begin, left);
	sort_pair(right, last);
	sort_pair(begin, right);
	sort_pair(left, last);
	sort_pair(mid, right);
	sort_pair(left, mid);
	sort_pair(mid, right);
	return mid;
}

template<typename iter>
inline iter median_of_5(iter begin) {
	sort_pair(begin + 0, begin + 1);
	sort_pair(begin + 3, begin + 4);
	sort_pair(begin + 0, begin + 3);
	sort_pair(begin + 1, begin + 4);
	sort_pair(begin + 2, begin + 3);
	sort_pair(begin + 1, begin + 2);
	sort_pair(begin + 2, begin + 3);
	return begin + 2;
}


template<typename iter>
inline iter median_of_5_medians_of_5(iter begin, iter end) {
	assert(end - begin > 70);
	iter left = begin + (end - begin) / 4;
	iter mid = begin + (end - begin) / 2 - 1;
	iter right = begin + (3 * (end - begin)) / 4 - 2;
	iter last = end - 4;
	iter first = median_of_5(begin);
	left = median_of_5(left);
	mid = median_of_5(mid);
	right = median_of_5(right);
	last = median_of_5(last);
	std::iter_swap(begin, first);
	std::iter_swap(last, end);
	return median_of_5(begin, left, mid, right, end);
}

// two pivots

template<typename iter>
inline void first_two_of_three(iter i1, iter i2, iter i3) {
    sort_pair(i2, i3);
    sort_pair(i1, i3);
    sort_pair(i1, i2);
}

template<typename iter>
inline void first_two_of_three(iter begin, iter end) {
    iter mid = begin + ((end - begin) / 2);
    first_two_of_three(begin, end, mid);
}

template<typename iter>
inline void sort_five(iter i1, iter i2, iter i3, iter i4, iter i5) {
    sort_pair(i1, i2);
    sort_pair(i4, i5);
    sort_pair(i3, i5);
    sort_pair(i3, i4);
    sort_pair(i1, i4);
    sort_pair(i1, i3);
    sort_pair(i2, i5);
    sort_pair(i2, i4);
    sort_pair(i2, i3);
}

template<typename iter>
inline void sort_five(iter begin) {
    sort_five(begin + 1, begin + 2, begin, begin + 3, begin + 4);
}

template<typename iter>
inline void first_third_of_five(iter begin, iter end) {
    iter left = begin + ((end - begin) / 4);
    iter mid = begin + ((end - begin) / 2);
    iter right = begin + 3 * ((end - begin) / 4);
    sort_five(begin, left, end, mid, right);
}

template<typename iter>
inline void second_fourth_of_five(iter begin, iter end) {
    iter left = begin + ((end - begin) / 4) - 2;
    iter mid = begin + ((end - begin) / 2) - 2;
    iter right = begin + 3 * ((end - begin) / 4) - 2;
    sort_five(begin, left, end, mid, right);
}

template<typename iter>
inline void medians_first_third_of_five(iter begin, iter end) {
    iter left = begin + ((end - begin) / 4) - 2;
    iter mid = begin + ((end - begin) / 2) - 2;
    iter right = begin + 3 * ((end - begin) / 4) - 2;
    sort_five(begin);
    sort_five(left);
    sort_five(mid);
    sort_five(right);
    sort_five(end - 4);
    std::iter_swap(end - 4, end);
    sort_five(begin, left, end, mid, right);
}

}
