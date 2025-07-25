#pragma once

#include <random>
#include <stack>

#include "hypergraph.hpp"

namespace hpiv {

struct Config;
struct PLITable;
class ResultCollector;

class TreeSearch {
 public:
  TreeSearch(const PLITable& tab, const Config& cfg, ResultCollector& RC);

  void run();

 private:
  const PLITable& m_tab;
  const Config& m_cfg;
  ResultCollector& m_RC;

  // the partial hypergraph of difference sets
  Hypergraph m_partial_HG;

  // exception to throw, when timeout happens
  const unsigned timeout = 10;

  // a mapping from clusterid to recordindices that is used for the
  // intersection of PLIs with single-column PLIs
  std::vector<std::vector<unsigned>> m_clusterid_to_recordindeces;

  // mapping from column to niceness (in [0, nr_cols)) with smaller
  // values being nicer columns
  std::vector<unsigned> m_niceness;
  void compute_niceness();
  unsigned long niceness(const edge& e);

  std::default_random_engine m_gen;
  Hypergraph sample(const std::vector<std::vector<unsigned>>& PLI);

  inline void update_crit_and_uncov(
      std::vector<std::vector<edgemark>>& removed_criticals_stack,
      std::vector<edgemark>& crit, edgemark& uncov, const edgemark& v_hittings);
  inline void restore_crit_and_uncov(
      std::vector<std::vector<edgemark>>& removed_criticals_stack,
      std::vector<edgemark>& crit, edgemark& uncov);

  inline bool extend_or_confirm_S(
      edge& S, edge& CAND, std::vector<edgemark>& crit, edgemark& uncov,
      std::vector<edgemark>& vertexhittings,
      std::vector<std::vector<edgemark>>& removed_criticals_stack,
      std::stack<std::vector<std::vector<unsigned>>>& intersection_stack,
      std::deque<edge::size_type>& tointersect_queue);

  inline void pullUpIntersections(
      std::stack<std::vector<std::vector<unsigned>>>& intersection_stack,
      std::deque<edge::size_type>& tointersect_queue);

  std::vector<std::vector<unsigned>> intersectClusterListAndClusterMapping(
      const std::vector<std::vector<unsigned>>& PLI,
      const std::vector<unsigned>& inverse_mapping);

  inline void updateEdges(
      std::vector<edgemark>& crit, edgemark& uncov,
      std::vector<edgemark>& vertexhittings,
      std::vector<std::vector<edgemark>>& removed_criticals_stack,
      const std::vector<std::vector<unsigned>>& PLI);

  inline bool S_fulfills_minimality_condition(
      const std::vector<edgemark>& crit);

  inline bool isViolater(const std::vector<edgemark>& crit,
                         const edgemark& v_hittings);
};

}  // namespace hpiv
