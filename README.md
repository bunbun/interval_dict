# interval_dict


An associative array library for c++17/20. 
Unlike normal dictionaries (e.g. `std::map`), the associations are only valid over specific intervals.
This is useful for building time-varying lookup dictionaries

## About:

Interval Dictionaries are an extension of the associative types found in the c++ standard library that allows efficient lookup of Key->Values.

Searching for values that are valid for a specified query interval involves a 2-stage lookup. 

- The first stage, from key to values per interval, currently uses `std::map` as the underlying container, and thus has logarithmic complexity.
- The second stage, efficient identification of all intervals overlapping the query is a matter of active research, especially in bioinformatics.
  The interval dict library eventually intends to provide a number of alternative choices that may suit different data but all with the same API.

## Documentation:

Check out the (woefully incomplete) documentation [here](https://github.com/bunbun/interval_dict).

#### 1. Chaining lookups
`interval dict` dictionaries can be "chained" together.
Dictionaries of `A`&rarr;`B` and `B`&rarr;`C` can be combined to a dictionary of `A`&rarr;`C`.
The associations will take into account the overlapping intervals for `A`&rarr;`B` and `B`&rarr;`C`

#### 2. Interval gaps
Very often, intervals without any valid association for a key may be due to missing data or errors.

For example, a interval dictionary from `company`&rarr;`website` may have gaps every weekend if spider only runs on weekdays.
If we reasonably assume no weekend website changes, we can fill gaps by either extending the Friday data (forward filling) by two days.
Or, even more conservatively, fill in the weekend gap if the Friday and Monday values are the same.

Different gap-filling functions in `interval dict` support diverse practical use-cases.   

#### 3. Bidirectional lookup
It is often useful to have associative types that support two-way lookup: from key to value or value to key.
If this is a one-off, then we can use the inverse dictionary (`interval_dict.invert()`).

Other times, it is more convenient or efficient to use a dictionary that supports two-way lookup directly.
Under the hood, the bidirectional interval map stores the data in both direction, so there is savings in space.

## License:

[Boost Software License](https://github.com/bunbun/interval_dict/blob/master/LICENSE)

## Dependencies

Interval dict library has two major dependencies:

- Interval handling code is from the [Boost Interval Container Library (ICL)](https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html).
  IntervalDict thus supports different variants of intervals: open, close, left-open, right-open, mixed etc.
  
- Lazy, synchronous iteration of intervals uses co-routines, currently supported by `generator<T>` from [cppcoro](https://github.com/lewissbaker/cppcoro).  
  This requires a compiler that supports the coroutines TS or c++20.
  
## Supported Compilers

The code is known to work on the following compilers:

- Linux + Clang 5.0/6.0 + - Linux + Clang 5.0/6.0 + libc++ [![Build Status](https://travis-ci.org/lewissbaker/cppcoro.svg?branch=master)](https://travis-ci.org/lewissbaker/cppcoro)

This reflects the requirements of the cppcoro library

## Development Status
`interval dict` is under active development. The current plan is to 

1. Initial implementation `IntervalDictICL` of `interval dict` using a [Boost ICL](https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html) [`interval_map`](https://www.boost.org/doc/libs/release/libs/icl/doc/html/header/boost/icl/interval_map_hpp.html) of `std::set`
   This is straightforward to implement, and stores the underlying data as disjoint intervals for quick lookups.
   
   However, this data structure has polynomial if not exponential complexity for insertion/removal of elements.
   This typically is noticeable if there are large number of intervals (>50,000) for a single key.
   This is true only if the associations are very imbalanced (each key has many tens of thousands of values).   
  
1. Abstract out the per-key "implementation" API necessary for an alternative data structure to support `interval dict`
   1. insert
   2. erase
   3. `gaps()` Get all gaps
   4. `sandwiched_gaps()` Get all gaps along with values on either side as std::vector  
   5. iterate through per contiguous interval per value, given a query interval
   6. iterate through per disjoint interval per values, given a query interval
   7. first / last disjoint interval
   8. merge_with (`+=`)
   9. subtract_by (`-=`)

1. Test / Benchmark Framework
   
1. More performant alternatives that store the data as overlapping intervals with traditional or bioinformatics algorithms. 
   Candidates include
   - RTrees
   - [Nested Containment Lists](https://academic.oup.com/bioinformatics/article/23/11/1386/199545). See
     - [ncls](https://github.com/biocore-ntnu/ncls)
   - [Interval Trees](https://en.wikipedia.org/wiki/Interval_tree).
     1. Centered? Make O(NlogN) Query O(logN+M)
     2.  
     For implementations, see  
     - [cGranges](https://github.com/lh3/cgranges)
     - [quicksect](https://github.com/brentp/quicksect)
     - [Erik Garrison's Interval Tree](https://github.com/ekg/intervaltree/blob/master/IntervalTree.h)
     - [Julia Discussion](https://github.com/BioJulia/Bio.jl/issues/340)
     
1. Documentation

1. Cython / Python wrapper    


## Build Status

- [![Build Status](https://travis-ci.org/bunbun/intervaldict.svg?branch=master)](https://travis-ci.org/bunbun/intervaldict)
