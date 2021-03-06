# interval_dict


An associative array / interval library for c++17/20:
 - Key / value lookup dictionary (like `std::map` etc.) 
 - Associations are only valid over specific intervals.
Useful for building time-varying lookup dictionaries.

## Overview:

Interval Dictionaries are an extension of the associative containers that allow efficient lookup of Key&rarr;Values that are valid only over a specified interval.

Two stage lookup: 

- Stage 1: Search, removal, and insertion operations for key &rarr; interval/values have logarithmic complexity (`std::map` under the hood).
- Stage 2: Values specified by query interval: Search operations have logarithmic complexity. Different implementations have different complexity for insertions and deletions.

## Documentation:

Woefully incomplete documentation [here](https://github.com/bunbun/interval_dict).
Better documentation coming soon!

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
  This requires a compiler that supports the coroutines TS (C++17) or C++20.
  
## Supported Compilers

The code is known to work on the following compilers:

- Linux + Clang 5.0/6.0 + - Linux + Clang 5.0/6.0 + libc++ [![Build Status](https://travis-ci.org/lewissbaker/cppcoro.svg?branch=master)](https://travis-ci.org/lewissbaker/cppcoro)

This reflects the requirements of the cppcoro library

### Current Status:

`interval dict` is under active development.  

1. Support open/close/mixed intervals comprising 
    - int / double
    - [boost:gregorian::date](https://www.boost.org/doc/libs/release/doc/html/date_time/gregorian.html)
    - [boost::posix_time::ptime](https://www.boost.org/doc/libs/release/doc/html/date_time/posix_time.html)
    
    Empty intervals are ignored 

1. Two choices of implementation:
 
   1. `IntervalDictICL` of `interval_dict` uses a [Boost ICL](https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html) [`interval_map`](https://www.boost.org/doc/libs/release/libs/icl/doc/html/header/boost/icl/interval_map_hpp.html) of `std::set`
       This was straightforward to implement, and stores the underlying data as disjoint intervals for quick lookups.
       
       Search operations have <img src="https://render.githubusercontent.com/render/math?math=O(n%20\log%20n)"> complexity.
       However, for insertion/removal of elements, this data structure can result in polynomial or worse complexity .
       
       This typically is noticeable if there are large number of intervals (>50,000) for a single key.
       This happens if the associations are very imbalanced (each key has many tens of thousands of values with overlapping validity intervals).
   
    1. The second implementation `IntervalDictITree` of `interval_dict` uses an [Interval Tree](https://en.wikipedia.org/wiki/Interval_tree#Augmented_tree).
       This stores the underlying data as continguous non-overlapping intervals.
       
       Search operations have <img src="https://render.githubusercontent.com/render/math?math=O(n%20\log%20n%20%2B%20m)"> complexity.
       However, insertion and deletions can be much more efficient: <img src="https://render.githubusercontent.com/render/math?math=O(n%20\log%20n)">.
       
       Implemented using [tinloaf/ygg](https://github.com/tinloaf/ygg) and [boost::intrusive](https://www.boost.org/doc/libs/release/doc/html/intrusive/set_multiset.html),
       both red-black trees underneath.
  
1. Tests
   100% coverage
   
1. Bidirectional dictionary.
   This can be parameterised for different implementation in each direction.

### In progress:

1. ![#f03c15](https://via.placeholder.com/15/f03c15/000000?text=+) Benchmarks 
   - For successively overlapping intervals
   - For nested intervals (like a pyramid)
   - Simulate a random proportion of values swapping between different keys at successive intervals
1. ![#f03c15](https://via.placeholder.com/15/f03c15/000000?text=+) Fuzzing tests for implementation 1 vs 2
1. Abandon ygg and move to boost::intrusive
1. Write docs
1. Doxygen for new implementation
1. Doxygen html for website 
1. Build and integration tests for website 
   
### Not yet started :bowtie:      

1. ![#f03c15](https://fakeimg.pl/20x10/0000ff,0/ff0000/?retina=1&text=%E9%9A%A8%E7%B7%A3&font=noto&font_size=8) Tutorial

1. ![#f03c15](https://fakeimg.pl/20x10/0000ff,0/ff0000/?retina=1&text=%E9%9A%A8%E7%B7%A3&font=noto&font_size=8) Cython / Python wrapper    

1. ![#f03c15](https://fakeimg.pl/20x10/0000ff,0/ff0000/?retina=1&text=%E9%9A%A8%E7%B7%A3&font=noto&font_size=8) Other performant alternative algorithms from bioinformatics.
     - [Nested Containment Lists](https://academic.oup.com/bioinformatics/article/23/11/1386/199545). See
       [ncls](https://github.com/biocore-ntnu/ncls)
     - [cGranges](https://github.com/lh3/cgranges)
     - [quicksect](https://github.com/brentp/quicksect)
     - [Julia Discussion](https://github.com/BioJulia/Bio.jl/issues/340)
     
## Build Status

- [![Build Status](https://travis-ci.org/bunbun/intervaldict.svg?branch=master)](https://travis-ci.org/bunbun/intervaldict)