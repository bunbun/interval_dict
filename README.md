# interval_dict


An associative array / interval library for c++17/20:
 - Key / value lookup dictionary (like `std::map` etc.) 
 - Associations are only valid over specific intervals.
Useful for building time-varying lookup dictionaries.

## About:

Interval Dictionaries are an extension of the associative types found in the c++ standard library that allow efficient lookup of Key->Values valid only over a specified interval.

2-stage lookup for values valid over a query interval: 

- Stage 1: key &rarr; values uses `std::map`. O(log N).
- Stage 2: Match query interval: Different implementations for different data.

## Documentation:

Woefully incomplete documentation [here](https://github.com/bunbun/interval_dict).

## Special features:

#### 1. Chaining lookups
`interval dict` dictionaries can be "chained" together:

Dictionaries of `A`&rarr;`B` and `B`&rarr;`C` can be combined to a dictionary of `A`&rarr;`C`.
The associations will take into account the overlapping intervals for `A`&rarr;`B` and `B`&rarr;`C`

#### 2. Filling interval gaps
Missing data can produce breaks / gaps in associations.

For example, data for `company`&rarr;`website` may have gaps every weekend if the spider only scrapes on weekdays.
We can fill gaps by either 
- extending Friday data ("forward filling" by 2 days), or
- fill in weekend gaps only if Friday and Monday values match.

Gap-filling functions in `interval dict` support diverse, practical use-cases.   

#### 3. Bidirectional lookup
It is often useful to have associative types that support two-way lookup: both key &rarr; value and value &rarr; key.
For a one-off, make an inverse dictionary (`interval_dict.invert()`).

Other times, it is more convenient or efficient to use a dictionary that supports two-way lookup directly.
Under the hood, the bidirectional interval map stores the data in both direction, so there is no savings in space.

## License:

[Boost Software License](https://github.com/bunbun/interval_dict/blob/master/LICENSE)

## Dependencies

- Interval handling uses the [Boost Interval Container Library (ICL)](https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html).
  `IntervalDict` thus supports many interval variants: open, close, left-open, right-open, dynamic, discrete (dates) and continuous (floats) underlying types etc.
  
- Lazy / synchronous iteration of intervals uses co-routines, provided by `generator<T>` from [cppcoro](https://github.com/lewissbaker/cppcoro).  
  This requires a compiler that supports the coroutines TS or C++20.
  
## Supported Compilers

The code is known to work on the following compilers:

- Linux + Clang 5.0/6.0 + - Linux + Clang 5.0/6.0 + libc++ [![Build Status](https://travis-ci.org/lewissbaker/cppcoro.svg?branch=master)](https://travis-ci.org/lewissbaker/cppcoro)

This reflects the requirements of the cppcoro library

## Development Status
`interval dict` is under active development.

### Done:  

1. Initial implementation `IntervalDictICL` of `interval dict` using a [Boost ICL](https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html) [`interval_map`](https://www.boost.org/doc/libs/release/libs/icl/doc/html/header/boost/icl/interval_map_hpp.html) of `std::set`
   This is straightforward to implement, and stores the underlying data as disjoint intervals for quick lookups.
   
   However, for insertion/removal of elements, this data structure can have polynomial or worse complexity .
   This typically is noticeable if there are large number of intervals (>50,000) for a single key.
   This happens if the associations are very imbalanced (each key has many tens of thousands of values).   
  
1. Abstract common traits for "implementing" `interval dict` using alternative data structures / algorithms

### In progress:

1. Tests / Benchmarks
   - Exercise all functions testing edge cases for
   - as many different interval types as possible
   - implementing intervals with 
     - int
     - double
     - [boost:gregorian::date](https://www.boost.org/doc/libs/release/doc/html/date_time/gregorian.html)
     - [boost::posix_time::ptime](https://www.boost.org/doc/libs/release/doc/html/date_time/posix_time.html) 

### Early days:

1. A bidirectional dictionary wrapper 
   
1. More performant alternatives that store the data as overlapping intervals with traditional or bioinformatics algorithms. 
   The leading candidate is the Augmented [Interval Tree](https://en.wikipedia.org/wiki/Interval_tree) 
   using the [tinloaf/ygg](https://github.com/tinloaf/ygg) library
   
   Alternatives include 
     1. Centered interval trees
     1. [Nested Containment Lists](https://academic.oup.com/bioinformatics/article/23/11/1386/199545). See
        [ncls](https://github.com/biocore-ntnu/ncls)
     1. RTrees (ruled out)
     
     For implementations, see  
     - [cGranges](https://github.com/lh3/cgranges)
     - [quicksect](https://github.com/brentp/quicksect)
     - [Erik Garrison's Interval Tree](https://github.com/ekg/intervaltree/blob/master/IntervalTree.h)
     - [Julia Discussion](https://github.com/BioJulia/Bio.jl/issues/340)

### Not yet started :bowtie:      

1. Documentation

1. Cython / Python wrapper    


## Build Status

- [![Build Status](https://travis-ci.org/bunbun/intervaldict.svg?branch=master)](https://travis-ci.org/bunbun/intervaldict)
