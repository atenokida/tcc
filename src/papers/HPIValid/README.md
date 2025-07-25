# HPIValid #

_HPIValid_ is an algorithm that enumerates all unique column
combinations (UCCs) of a given database.  For more details, see our
publication:
	
> **Hitting Set Enumeration with Partial Information for Unique Column
> Combination Discovery** by _Johann Birnick, Thomas Bläsius, Tobias
> Friedrich, Felix Naumann, Thorsten Papenbrock, Martin Schirneck:_

This repository contains a C++ implementation of _HPIValid_ as well as
the experimental setup for the experiments in the above paper.
_HPIValid_ can be run via a command line interface or directly form
C++.


## Table of Contents ##

  * [Requirements](#requirements)
  * [Command Line Interface](#command-line-interface)
      * [Building and Using HPIValid](#building-and-using-hpivalid)
      * [Command Line Options](#command-line-options)
  * [Calling HPIValid from C++](#calling-hpivalid-from-c)
  * [Experiments](#experiments)
	  * [Running the Experiments](#running-the-experiments)
	  * [Evaluating the Measurements](#evaluating-the-measurements)
  * [Known Issues](#known-issues)

## Requirements ##

Besides basic build tools (cmake and C++ compiler), _HPIValid_
requires [Boost](https://www.boost.org/).  Though we have only tested
it on GNU/Linux, there is no particular reason why it should not also
work on other operating systems.

Running our [experiments](#experiments) has the following additional
requirements.
  * Java for the HyUCC comparison
  * Python for various scripts
  * R with packages ggplot2, egg, reshape2, tikzDevice, and xtable for
    the evaluation
  * LaTeX for compiling the output created by R

## Command Line Interface ##

### Building and Using HPIValid ###

_HPIValid_ can be built with CMake.

``` console
mkdir release
cd release/
cmake ..
make
```

To execute _HPIValid_ on the _flight_ dataset included as example in
this repository, run

``` console
./HPIValid -h -i ../experiments/data_examples/flight_r1001_c109.csv
```

which should produce an output similar to this:

``` console
dataset: flight_r1001_c109
rows: 1001, columns: 109
run time: 0.036s (total) = 0.005s (preprocessing) + 0.031s (enumeration)
minimal UCCs: 26652
minimal difference sets: 161
sampled difference sets: 4618 (initial: 4386)
PLI intersections: 31057
tree size: 31852
```

Moreover, _HPIValid_ creates two files under `output/`.  The file
`*_UCCs.hg` contains the list of UCCs, with one UCC per line with
column indices separated by ",".  The file `*_diff_sets.hg` contains a
list of all minimal difference sets in the same format.

### Command Line Options ###

The command line options can be categorized as follows.

  * **Input/Output:** Control input and output.
  * **Subtables:** Control options related to running _HPIValid_ only
    on a subtable of the given input.
  * **Algorithm Configuration:** Control settings of the algorithm
    itself.
  * **Miscellaneous:** Everything not fitting one of the other
    categories.

#### Input/Output ####

  * **``--help``**: Print a help screen.
  * **``-i [--input-file] <arg>``**: Input file in csv format.
  * **``-o [--output-dir] <arg>``**: Output directory for the lists of
    UCCs and minimal difference sets.  **Default: ``"output/"``**
  * **``-n [--no-output]``**: Generate no output files (but still
    output statistics of the run).  Useful for measuring run times.
  * **``-h [--human-readable]``**: Print statistics in a form more
    readable for humans.  If not set, the statistics are printed as a
    line of a csv-file.
  * **``--header``**: Print a header line for the statistics output.

#### Subtables ####

Whenever _HPIValid_ is requested to run on a subtable, it checks
whether the subtable has already been created before.  If not, it
creates and saves the subtable, and then runs the enumeration on it.

  * **``-r [--rows] <arg>``**: Truncate the table after the first
    `<arg>` rows.  For `<arg> = 0`, all rows are used.  **Default:
    ``0``**
  * **``-c [--cols] <arg>``**: As **``-r``** but with columns instead
    of rows.
  * **``--subtable-dir <arg>``**: The directory that is checked for the
    existance of subtables and where new subtables are stored.
    **Default: ``"../experiments/data_subtables/"``**
  * **``--do-not-run``**: Do not run the algorithm.  This can be used
    to just create a subtable, without enumerating UCCs.

#### Algorithm Configuration ####

These options are mainly here for experimenting with the algorithm.
For optimal performance, we recommend using the default settings.

  * **``-x [--sample-exponent] <arg>``**: The sample exponent controls
    how many pairs of rows are sampled for difference sets.  Whenever
    there are p pairs available, p<sup>x</sup> pairs are sampled, for
    x = ``<arg>``.  **Default: ``0.3``**
  * **``--no-copy-PLIs``**: In its default configuration, the
    preprocessing of _HPIValid_ copies the PLI data structure to a new
    chunk of memory.  This costs little time in the preprocessing and
    heavily speeds up the enumeration due to cache efficiency.  If
    this is not desired, it can be deactivated passing this flag.
  * **``--no-tiebreaker-heuristic``**: _HPIValid_ regular branches on
	the smallest uncovered difference set.  If there are ties, it
	uses a heuristic that aims to speed up the validation of found
	UCC-candidates.  While this does nothing on most instances, it
	leads to a significant speed up on some instances.  If this is not
	desired, it can be deactivated passing this flag.

#### Miscellaneous ####

  * **``-t [--timeout] <arg>``**: Number of seconds before timeout.
    _HPIValid_ stops when finding the first UCC after exceeding the
    timeout.  **Default: ``3600``**
  * **``-s [--seed] <arg>``**: _HPIValid_ does random sampling of row
    pairs.  The pseudorandom generator is seeded with ``<arg>`` (i.e.,
    using a fixed seed leads to the same deterministic behavior for
    different runs).  **Default:
    ``std::chrono::system_clock::now().time_since_epoch().count()``**


## Calling HPIValid from C++ ##

Here is a minimal example of how to call _HPIValid_ from C++.

``` c++
#include "HPIValid.hpp"

int main() {
  std::string input_file("../experiments/data_examples/flight_r1001_c109.csv");
  hpiv::Config cfg;
  hpiv::ResultCollector RC;

  hpiv::HPIValid(input_file, cfg, RC);

  std::cout << RC.uccs() << " UCCs found after " << RC.time(hpiv::timer::total)
            << "s" << std::endl;

  return 0;
}
```

The first parameter of `hpiv::HPIValid` is `input_file`, which should
be self-explanatory.  The second parameter `cfg` can be used to alter
settings of the algorithm; see [config.hpp](include/config.hpp) in
conjunction with the corresponding command line options described
[above](#command-line-options).  The third parameter `RC` collects
various statistics during the run; see
[result_collector.hpp](include/result_collector.hpp).  In the above
example, `RC` is an instance of the base class
`hpiv::ResultCollector`, which does not actually process or store the
UCCs found by HPIValid but merely counts them.  If the UCCs as well as
the final hypergraph of difference sets should be written to a file,
use an instance of the subclass `hpiv::ResultCollectorOutput` from
[result_collector_output.hpp](include/result_collector_output.hpp)
instead.  To use the UCCs somehow differently, e.g., to process
each UCC on the fly whenever _HPIValid_ finds it, create your own
subclass of `hpiv::ResultCollector` that overwrites the method
`ucc_found(const edge& ucc)`.  _HPIValid_ calls this method whenever
it finds a new minimal UCC and passes the found UCC to it.  The
subclass `hpiv::ResultCollectorOutput` already present in
[result_collector_output.hpp](include/result_collector_output.hpp)
with implementation in
[result_collector_output.cpp](src/result_collector_output.cpp) can
serve as an example of how to do that.

## Experiments ##

For the following, we assume to be in the directory `experiments/`.

``` console
cd experiments/
```

There are two parts to the experiments.  First, running _HPIValid_ and
for comparison _HyUCC_ on various instances, recording measurements,
and second, evaluating the measurements.

### Running the Experiments ###

To run the experiments, the [command line
interface](#command-line-interface) has to be compiled with the
_HPIValid_ being located in `release/`.  Moreover, the datasets (as
csv files) have to be located in `experiments/data_raw/`.  They are
not provided with this repository.  To get them, download [this zip
file (3.5GB)](https://owncloud.hpi.de/s/j6Z0yvXC0qhtGCk/download). It
contains all but four of the datasets used in our experiments
(`ZBC00DT`, `VTTS`, `ILOA`, `CE4HI01` are not publicly available).

To run the experiments, simply execute the python script
[run_main_exp.py](experiments/scripts/run_main_exp.py).
Alternatively, to exclude the datasets that are not contained in the
above zip file, run
[run_available_exp.py](experiments/scripts/run_available_exp.py).

``` console
python scripts/run_available_exp.py
```

**Warning:** This runs **all** experiments, which takes multiple days
and uses up to 30GB of RAM.

In the default configuration,
[run_main_exp.py](experiments/scripts/run_main_exp.py) first creates
all necessary subtables that do not yet exist.  Afterwards it runs all
_HPIValid_ experiments followed by all _HyUCC_ experiments.  To run
only a subset of the experiments or adapt, e.g., the number of
iterations, change the
[run_main_exp.py](experiments/scripts/run_main_exp.py) accordingly (it
should be self explanatory).

The results are written to the `results/` folder (this can be changed
in the [run_main_exp.py](experiments/scripts/run_main_exp.py) by
changing the value of `eh.out_dir`).

### Evaluating the Measurements ###

To evaluate the measurements, simply execute the python script
[run_evaluation.py](experiments/scripts/run_evaluation.py).

``` console
python scripts/run_evaluation.py
```

This runs all R scripts, which create LaTeX output in
`evaluation/output/latex/plots/`, and afterwards runs LaTeX on
[evaluation.tex](experiments/evaluation/output/latex/evaluation.tex)
to create the PDF file `evaluation/output/latex/evaluation.pdf` that
contains all plots and tables.

**Note:** In the default configuration, our measurements in
`results_final` are used.  To use different measurements (e.g., the
ones created by [Rerunning the
Experiments](#running-the-experiments)), adapt
[helper.R](experiments/evaluation/helper.R) accordingly.

## Known Issues ##

The csv-parser does currently not support multi-line entries, i.e., a
`\n` character is always interpreted as the beginning of a new row.
