
#include "result_collector.hpp"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <string>
#include <utility>

namespace hpiv {

namespace timer {
std::vector<std::string> description = {"Total Execution Time",
                                        "Total Preprocessing Time",
                                        "Table Reading and Preprocessing",
                                        "Create Subtable (top-left)",
                                        "Cluster-Structures Construction",
                                        "Total Enumeration Algo Time"
                                        "Difference Set Sampling (all)",
                                        "Cluster Intersection (validation)"};
}  // namespace timer

ResultCollector::ResultCollector(double timeout)
    : m_timeout(timeout),
      m_ucc_count(0),
      m_timer_beg(timer::num_of_timers),
      m_timer_end(timer::num_of_timers),
      m_timer_elapsed(timer::num_of_timers, 0),
      m_diff_sets(0),
      m_diff_sets_initial(0),
      m_tree_complexity(0),
      m_tree_nodes(0),
      m_intersections(0),
      m_intersection_cluster_size(0) {}

bool ResultCollector::ucc_found(const edge& ucc) {
  (void)ucc;
  m_ucc_count++;
  return std::chrono::duration_cast<std::chrono::duration<double>>(
             clock::now() - m_timer_beg[timer::total])
             .count() <= m_timeout;
}

void ResultCollector::final_hypergraph(const Hypergraph& hg) {
  m_diff_sets_final = hg.numEdges();
}

void ResultCollector::start_timer(timer::name timer) {
  m_timer_beg[timer] = clock::now();
}

void ResultCollector::stop_timer(timer::name timer) {
  m_timer_end[timer] = clock::now();
  m_timer_elapsed[timer] +=
      std::chrono::duration_cast<std::chrono::duration<double>>(
          m_timer_end[timer] - m_timer_beg[timer])
          .count();
  // if (timer == timer::total) {
  //   std::cerr << "TIMER REPORT '" << timer::description[timer] << "':\t"
  //             << std::fixed << std::setprecision(3) << time(timer) << "s"
  //             << std::endl;
  // }
}

double ResultCollector::time(timer::name timer) const {
  return m_timer_elapsed[timer];
  // return std::chrono::duration_cast<std::chrono::duration<double>>(
  //            m_timer_end[timer] - m_timer_beg[timer])
  //     .count();
}

void ResultCollector::count_diff_sets(unsigned int number) {
  m_diff_sets += number;
}

void ResultCollector::stop_initial_sampling() {
  m_diff_sets_initial = m_diff_sets;
}

void ResultCollector::count_tree_complexity(unsigned int number) {
  m_tree_complexity += number;
}
void ResultCollector::count_tree_node() { m_tree_nodes++; }

void ResultCollector::count_intersections() { m_intersections++; }

void ResultCollector::count_intersection_cluster_size(
    unsigned int cluster_size) {
  m_intersection_cluster_size += cluster_size;
}

void ResultCollector::set_size(unsigned int nr_rows, unsigned int nr_cols) {
  m_rows = nr_rows;
  m_cols = nr_cols;
}

}  // namespace hpiv
