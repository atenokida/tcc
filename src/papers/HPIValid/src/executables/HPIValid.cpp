
#include "HPIValid.hpp"

#include <boost/program_options.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "config.hpp"
#include "helper.hpp"
#include "result_collector.hpp"
#include "result_collector_output.hpp"

namespace bpo = boost::program_options;

namespace hpiv {

// command line options
struct CLOptions {
  bool help;
  std::filesystem::path input_file;
  std::filesystem::path output_dir;
  bool no_output;
  unsigned rows;
  unsigned cols;
  std::filesystem::path subtable_dir;
  bool do_not_run;
  double sample_exponent;
  unsigned seed;
  double timeout;
  bool human_readable;
  bool header;
  bool copy_PLIs;
  bool tiebreaker_heuristic;
};

CLOptions parse_options(int argc, const char* argv[]);

}  // namespace hpiv

int main(int argc, const char* argv[]) {
  std::ios::sync_with_stdio(false);

  try {
    //////////////////////////////////////////////////////////////////////
    // parsing command line options and handling some special cases
    hpiv::CLOptions opt = hpiv::parse_options(argc, argv);

    // do nothing, if help was printed
    if (opt.help) {
      return 0;
    }
    // print header
    if (opt.header) {
      std::cout
          << "dataset,rows,cols,sample_exponent,copy_PLIs,tiebreaker_heuristic,"
          << "t_total,"
          << "t_total_prep,t_read_table,t_create_subtable,t_construct_"
             "clusters,"
          << "t_total_enum_algo,t_sample_diff_sets,t_intersect_clusters,"
          << "tree_complexity,tree_nodes,diff_sets,diff_sets_initial,diff_sets_"
             "final,"
          << "intersections,intersection_cluster_size," << "ucc_count"
          << std::endl;
      return 0;
    }
    // no input file
    if (opt.input_file == "") {
      if (opt.header) {
        return 0;
      } else {
        throw std::runtime_error("No input file specified.");
      }
    }
    // input file does not exist
    if (!std::filesystem::exists(opt.input_file)) {
      throw std::runtime_error("Input file does not exist: " +
                               opt.input_file.string());
    }

    //////////////////////////////////////////////////////////////////////
    // prepare some stuff

    // create output directory, if necessary
    if (!opt.no_output) {
      std::filesystem::create_directory(opt.output_dir);
    }

    // name of the dataset
    std::string dataset_name = hlpr::dataset_name(opt.input_file);

    // if necessary, create subtable and use the resulting file in the
    // following
    opt.input_file = hlpr::subtable_input_file(opt.input_file, opt.rows,
                                               opt.cols, opt.subtable_dir);

    // stop here, if the algorithm should not be run
    if (opt.do_not_run) {
      return 0;
    }

    // algorithm config
    hpiv::Config cfg = {opt.sample_exponent, opt.seed, opt.copy_PLIs,
                        opt.tiebreaker_heuristic};

    // result collector
    std::optional<hpiv::ResultCollector> RC_no_output;
    std::optional<hpiv::ResultCollectorOutput> RC_output;
    if (opt.no_output) {
      RC_no_output = hpiv::ResultCollector(opt.timeout);
    } else {
      RC_output = hpiv::ResultCollectorOutput(dataset_name, opt.output_dir,
                                              opt.timeout);
    }
    hpiv::ResultCollector& RC = opt.no_output ? *RC_no_output : *RC_output;

    //////////////////////////////////////////////////////////////////////
    // run HPIValid
    hpiv::HPIValid(opt.input_file, cfg, RC);

    //////////////////////////////////////////////////////////////////////
    // output results
    if (opt.human_readable) {
      std::cout << "dataset: " << dataset_name;
      if (opt.rows != 0 && opt.cols != 0) {
        std::cout << " truncated to " << opt.rows << " rows and " << opt.cols
                  << " columns";
      } else if (opt.rows != 0) {
        std::cout << " truncated to " << opt.rows << " rows";
      } else if (opt.cols != 0) {
        std::cout << " truncated to " << opt.cols << " columns";
      }
      std::cout << std::endl;

      std::cout << "rows: " << RC.nr_rows() << ", columns: " << RC.nr_cols()
                << std::endl;

      std::cout << std::fixed << std::setprecision(3)
                << "run time: " << RC.time(hpiv::timer::total)
                << "s (total) = " << RC.time(hpiv::timer::total_preprocessing)
                << "s (preprocessing) + "
                << RC.time(hpiv::timer::total_enum_algo) << "s (enumeration)"
                << std::endl;

      std::cout << "minimal UCCs: " << RC.uccs() << std::endl;
      std::cout << "minimal difference sets: " << RC.diff_sets_final()
                << std::endl;
      std::cout << "sampled difference sets: " << RC.diff_sets()
                << " (initial: " << RC.diff_sets_initial() << ")" << std::endl;
      std::cout << "PLI intersections: " << RC.intersections() << std::endl;
      std::cout << "tree size: " << RC.tree_nodes() << std::endl;

    } else {
      std::cout << dataset_name << ",";
      std::cout << RC.nr_rows() << "," << RC.nr_cols() << ",";
      std::cout << opt.sample_exponent << ",";
      std::cout << opt.copy_PLIs << ",";
      std::cout << opt.tiebreaker_heuristic << ",";
      // std::cout << std::fixed << std::setprecision(3);
      std::cout << RC.time(hpiv::timer::total) << ","
                << RC.time(hpiv::timer::total_preprocessing) << ","
                << RC.time(hpiv::timer::read_table) << ","
                << RC.time(hpiv::timer::create_subtable) << ","
                << RC.time(hpiv::timer::construct_clusters) << ","
                << RC.time(hpiv::timer::total_enum_algo) << ","
                << RC.time(hpiv::timer::sample_diff_sets) << ","
                << RC.time(hpiv::timer::cluster_intersect) << ",";
      std::cout << RC.tree_complexity() << "," << RC.tree_nodes() << ",";
      std::cout << RC.diff_sets() << "," << RC.diff_sets_initial() << ","
                << RC.diff_sets_final() << ",";
      std::cout << RC.intersections() << "," << RC.intersection_cluster_size()
                << ",";
      std::cout << RC.uccs() << std::endl;
    }

  } catch (const std::exception& e) {
    std::cout << std::flush;
    std::cerr << "ERROR: " << e.what() << std::endl;
    std::cerr << "If you need help handing over the right arguments, call '" +
                     std::string(argv[0]) + " --help' to list all options."
              << std::endl;
    return -1;
  }
  return 0;
}

namespace hpiv {

CLOptions parse_options(int argc, const char* argv[]) {
  bpo::options_description opt_desc("Options");
  opt_desc.add_options()                     //
      ("help", "prints this help screen\n")  //
      ("input-file,i", bpo::value<std::string>()->default_value(""),
       "input file in csv format\n")  //
      ("output-dir,o", bpo::value<std::string>()->default_value("output"),
       "output directory for the hypergraph of difference sets and its "
       "hitting sets (the UCCs)\n")  //
      ("no-output,n",
       "if set, no output files are generated and the hitting sets (UCCs) are "
       "only counted\n")  //
      ("rows,r", bpo::value<unsigned>()->default_value(0),
       "if non-zero, the table is truncated to the given number of rows (0 "
       "uses all rows)\n")  //
      ("cols,c", bpo::value<unsigned>()->default_value(0),
       "if non-zero, the table is truncated to the given number of columns "
       "(0 uses all columns)\n")  //
      ("subtable-dir",
       bpo::value<std::string>()->default_value(
           "../experiments/data_subtables"),
       "directory where subtables are cached\n")  //
      ("do-not-run",
       "don't run the enumeration algorithm (can be used to just create "
       "subtables)\n")  //
      ("sample-exponent,x", bpo::value<double>()->default_value(0.3),
       "whenever there are p row pairs available, p^x pairs are sampled\n")  //
      ("seed,s",
       bpo::value<unsigned>()->default_value(
           std::chrono::system_clock::now().time_since_epoch().count()),
       "initial seed for random number generator\n")  //
      ("timeout,t", bpo::value<double>()->default_value(3600),
       "number of seconds before timeout (execution stops with the first UCC "
       "found after timeout)\n")  //
      ("human-readable,h",
       "output stats in a human readable format instead of as csv-line\n")   //
      ("header", "print header for csv-style output and do nothing else\n")  //
      ("no-copy-PLIs",
       "disable the copying of PLIs after the initial read (copying increases "
       "cache efficiency)\n")  //
      ("no-tiebreaker-heuristic",
       "disable a tiebreaker heuristic for branching in MMCS");
  bpo::variables_map var_map;
  bpo::store(bpo::parse_command_line(argc, argv, opt_desc), var_map);

  CLOptions opt;
  opt.help = var_map.find("help") != var_map.end();
  opt.input_file = var_map["input-file"].as<std::string>();
  opt.output_dir = var_map["output-dir"].as<std::string>();
  opt.no_output = var_map.find("no-output") != var_map.end();
  opt.rows = var_map["rows"].as<unsigned>();
  opt.cols = var_map["cols"].as<unsigned>();
  opt.subtable_dir = var_map["subtable-dir"].as<std::string>();
  opt.do_not_run = var_map.find("do-not-run") != var_map.end();
  opt.sample_exponent = var_map["sample-exponent"].as<double>();
  opt.seed = var_map["seed"].as<unsigned>();
  opt.timeout = var_map["timeout"].as<double>();
  opt.human_readable = var_map.find("human-readable") != var_map.end();
  opt.header = var_map.find("header") != var_map.end();
  opt.copy_PLIs = var_map.find("no-copy-PLIs") == var_map.end();
  opt.tiebreaker_heuristic =
      var_map.find("no-tiebreaker-heuristic") == var_map.end();

  if (opt.help) {
    std::cout << opt_desc << std::endl;
  }

  return opt;
}

}  // namespace hpiv
