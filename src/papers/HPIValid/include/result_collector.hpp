#pragma once

#include <chrono>
#include <filesystem>

#include "hypergraph.hpp"

namespace hpiv {

namespace timer {
enum name {
  total,
  total_preprocessing,
  read_table,
  create_subtable,
  construct_clusters,
  total_enum_algo,
  sample_diff_sets,
  cluster_intersect,
  num_of_timers
};

extern std::vector<std::string> description;

}  // namespace timer

class ResultCollector {
  using clock = std::chrono::high_resolution_clock;

 public:
  ResultCollector(double timeout = 3600.0);

  //////////////////////////////////////////////////////////////////////////////
  // collecting information

  // Report that a UCC has been found.  The return value is false if
  // the timeout is reached.  In the base class, this only counts the
  // UCCs.  This can be altered by derived classes.
  virtual bool ucc_found(const edge& ucc);

  // Report the final hypergraph of difference sets.  In this base
  // class, this method does nothing.  For different behavior, see use
  // classes.
  virtual void final_hypergraph(const Hypergraph& hg);

  // Start a timer (one of `timer::name`).
  void start_timer(timer::name timer);

  // Start a timer (one of `timer::name`).
  void stop_timer(timer::name timer);

  // Counts the `number` of difference sets being sampled.
  void count_diff_sets(unsigned int number);

  // Notify that the end of the initial sampling is reached.
  void stop_initial_sampling();

  // Counts the tree complexity in terms of `number` of candidate
  // edges that are considered to branch on.
  void count_tree_complexity(unsigned int number);

  // Count the number of tree nodes.
  void count_tree_node();

  // Count the number of times we intersect clusters.
  void count_intersections();

  // Count the total cluster size in intersections.
  void count_intersection_cluster_size(unsigned int cluster_size);

  // Set the size of the table (just for later printing purpose).
  void set_size(unsigned int nr_rows, unsigned int nr_cols);

  //////////////////////////////////////////////////////////////////////////////
  // getting statistics

  // Number of found UCCs.
  unsigned int uccs() const { return m_ucc_count; }

  // Time in seconds between starting and stopping `timer`.
  double time(timer::name timer) const;

  // Total number of sampled difference sets.
  unsigned int diff_sets() const { return m_diff_sets; }

  // Number of difference set in the initial sampling.
  unsigned int diff_sets_initial() const { return m_diff_sets_initial; }

  // Number of difference sets in the final hypergraph.
  unsigned int diff_sets_final() const { return m_diff_sets_final; }

  // Three complexity in terms of the number of candidate edges
  // considered to branch on.
  unsigned int tree_complexity() const { return m_tree_complexity; }

  // Size of the search tree.
  unsigned int tree_nodes() const { return m_tree_nodes; }

  // Number of cluster intersections.
  unsigned int intersections() const { return m_intersections; }

  // Total cluster size in intersections.
  unsigned int intersection_cluster_size() const {
    return m_intersection_cluster_size;
  }

  // nr of rows of the dataset
  unsigned nr_rows() const { return m_rows; }

  // nr of cols of the dataset
  unsigned nr_cols() const { return m_cols; }

 private:
  //////////////////////////////////////////////////////////////////////////////
  // private members

  double m_timeout;
  unsigned int m_ucc_count;
  unsigned m_diff_sets_final;

  std::vector<clock::time_point> m_timer_beg;
  std::vector<clock::time_point> m_timer_end;
  std::vector<double> m_timer_elapsed;

  unsigned int m_diff_sets;
  unsigned int m_diff_sets_initial;
  unsigned int m_tree_complexity;
  unsigned int m_tree_nodes;
  unsigned int m_intersections;
  unsigned int m_intersection_cluster_size;

  unsigned int m_rows;
  unsigned int m_cols;
};

}  // namespace hpiv
