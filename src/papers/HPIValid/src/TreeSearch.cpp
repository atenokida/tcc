#include "TreeSearch.hpp"

#include "PLITable.hpp"
#include "config.hpp"
#include "result_collector.hpp"

namespace hpiv {

TreeSearch::TreeSearch(const PLITable& tab, const Config& cfg,
                       ResultCollector& RC)
    : m_tab(tab),
      m_cfg(cfg),
      m_RC(RC),
      m_partial_HG(tab.nr_cols),
      m_clusterid_to_recordindeces(tab.nr_rows),
      m_gen(cfg.seed) {
  // add single edge containing all vertices to partial hypergraph
  m_partial_HG.addEdge(~edge(m_partial_HG.numVertices()));

  if (m_cfg.tiebreaker_heuristic) {
    compute_niceness();
  }
}

void TreeSearch::run() {
  m_RC.start_timer(timer::sample_diff_sets);
  for (const auto& PLI : m_tab.PLIs) {
    Hypergraph gen = sample(PLI);
    for (const edge& e : gen) {
      m_partial_HG.addEdgeAndMinimizeInclusion(e);
    }
  }
  m_RC.stop_initial_sampling();
  m_RC.stop_timer(timer::sample_diff_sets);

  // S, CAND
  edge S(m_partial_HG.numVertices());
  edge CAND(m_partial_HG.numVertices());
  CAND.set();

  // crit, uncov
  std::vector<edgemark> crit;
  edgemark uncov(m_partial_HG.numEdges());
  uncov.set();

  // vertexhittings
  std::vector<edgemark> vertexhittings(m_partial_HG.numVertices(),
                                       edgemark(m_partial_HG.numEdges()));
  for (std::vector<edge>::size_type i_e = 0; i_e < m_partial_HG.numEdges();
       ++i_e) {
    for (edge::size_type i_v = m_partial_HG[i_e].find_first();
         i_v != edge::npos; i_v = m_partial_HG[i_e].find_next(i_v)) {
      vertexhittings[i_v].set(i_e);
    }
  }

  // removed_criticals_stack
  std::vector<std::vector<edgemark>> removed_criticals_stack;

  // intersections
  std::stack<std::vector<std::vector<unsigned>>> intersection_stack;
  std::deque<edge::size_type> tointersect_queue;

  // Searching
  // find edge from uncov with smallest intersecton C with CAND
  edge C = m_partial_HG[uncov.find_first()] & CAND;
  for (edge::size_type i_e = uncov.find_next(uncov.find_first());
       i_e != edge::npos; i_e = uncov.find_next(i_e)) {
    if ((m_partial_HG[i_e] & CAND).count() < C.count())
      C = m_partial_HG[i_e] & CAND;
  }

  CAND -= C;

  try {
    for (edge::size_type v = C.find_first(); v != edge::npos;
         v = C.find_next(v)) {
      // update crit and uncov
      update_crit_and_uncov(removed_criticals_stack, crit, uncov,
                            vertexhittings[v]);

      // branch
      S.set(v);
      intersection_stack.push(m_tab.PLIs[v]);
      extend_or_confirm_S(S, CAND, crit, uncov, vertexhittings,
                          removed_criticals_stack, intersection_stack,
                          tointersect_queue);
      intersection_stack.pop();
      S.reset(v);

      // reset update of crit and uncov
      restore_crit_and_uncov(removed_criticals_stack, crit, uncov);

      // update CAND
      CAND.set(v);
    }
    // report final hypergraph
    m_RC.final_hypergraph(m_partial_HG);
  } catch (unsigned timeout) {
    // report current partial hypergraph
    m_RC.final_hypergraph(m_partial_HG);
  }
}

void TreeSearch::compute_niceness() {
  std::vector<std::pair<unsigned long, unsigned long>> sq_sizes_col_id_pairs(
      m_tab.nr_cols);

  for (unsigned long col = 0; col < m_tab.nr_cols; ++col) {
    unsigned long sq_size = 0;
    for (const auto& cluster : m_tab.PLIs[col]) {
      sq_size += cluster.size() * cluster.size();
    }
    sq_sizes_col_id_pairs[col] = std::make_pair(sq_size, col);
  }

  std::sort(sq_sizes_col_id_pairs.begin(), sq_sizes_col_id_pairs.end());
  m_niceness.clear();
  m_niceness.resize(m_tab.nr_cols);
  for (unsigned int pos = 0; pos < m_tab.nr_cols; ++pos) {
    unsigned int col = sq_sizes_col_id_pairs[pos].second;
    m_niceness[col] = pos;
  }
}

unsigned long TreeSearch::niceness(const edge& e) {
  if (!m_cfg.tiebreaker_heuristic) {
    return 0;
  }

  unsigned long niceness = 0;
  for (unsigned long col = e.find_first(); col != edge::npos;
       col = e.find_next(col)) {
    if (niceness < m_niceness[col]) {
      niceness = m_niceness[col];
    }
  }
  return niceness;
}

Hypergraph TreeSearch::sample(const std::vector<std::vector<unsigned>>& PLI) {
  Hypergraph difference_graph(m_tab.nr_cols);
  edge temp_edge(m_tab.nr_cols);

  if (PLI.size() == 0) {
    return difference_graph;
  }

  std::vector<unsigned long> weights;
  unsigned long total_pairs = 0;
  for (const auto& cluster : PLI) {
    unsigned long pairs = (cluster.size() * (cluster.size() - 1)) / 2;
    total_pairs += pairs;
    weights.push_back(pairs);
  }

  // calculate how many to view
  std::size_t to_view = static_cast<std::size_t>(
      round(std::pow(static_cast<double>(total_pairs), m_cfg.sample_exponent)));
  if (to_view < 1) to_view = 1;

  m_RC.count_diff_sets(to_view);

  std::discrete_distribution<int> rand_cluster(weights.begin(), weights.end());
  std::uniform_int_distribution<> rand_int(0, std::numeric_limits<int>::max());
  std::vector<std::tuple<int, int, int>> samples(to_view);
  for (unsigned int i = 0; i < to_view; ++i) {
    std::get<0>(samples[i]) = rand_cluster(m_gen);
    unsigned int size = PLI[std::get<0>(samples[i])].size();
    int i_i_r1 = rand_int(m_gen) % size;
    int i_i_r2 = (i_i_r1 + 1 + rand_int(m_gen) % (size - 1)) % size;
    std::get<1>(samples[i]) = i_i_r1;
    std::get<2>(samples[i]) = i_i_r2;
  }
  std::sort(samples.begin(), samples.end());
  for (unsigned int i = 0; i < to_view; ++i) {
    const auto& cluster = PLI[std::get<0>(samples[i])];
    int i_i_r1 = std::get<1>(samples[i]);
    int i_i_r2 = std::get<2>(samples[i]);

    temp_edge.reset();

    // calculate difference edge
    for (unsigned i_c = 0; i_c < m_tab.nr_cols; ++i_c) {
      // set bit if records have different cluster id's or if they
      // have maxint as id (indicating that they are unique)
      if (m_tab.inverse_mapping[i_c][cluster[i_i_r1]] !=
              m_tab.inverse_mapping[i_c][cluster[i_i_r2]] ||
          m_tab.inverse_mapping[i_c][cluster[i_i_r1]] == size_one_cluster) {
        temp_edge.set(i_c);
      }
    }
    // add difference edge to difference graph
    difference_graph.addEdgeAndMinimizeInclusion(temp_edge);
  }

  // std::cout << "sampled pairs: " << to_view << std::endl;
  // std::cout << "number of new edges: " << difference_graph.numEdges() <<
  // std::endl;
  return difference_graph;
}

inline void TreeSearch::update_crit_and_uncov(
    std::vector<std::vector<edgemark>>& removed_criticals_stack,
    std::vector<edgemark>& crit, edgemark& uncov, const edgemark& v_hittings) {
  // update crit[] for vertices in S and put changes on stack

  removed_criticals_stack.emplace_back(crit.size());

  for (std::vector<edgemark>::size_type i = 0; i < crit.size(); ++i) {
    removed_criticals_stack.back()[i] = crit[i] & v_hittings;
    crit[i] -= v_hittings;
  }

  // set critical edges for v and remove them from uncov

  crit.emplace_back(v_hittings & uncov);
  uncov -= v_hittings;
}

inline void TreeSearch::restore_crit_and_uncov(
    std::vector<std::vector<edgemark>>& removed_criticals_stack,
    std::vector<edgemark>& crit, edgemark& uncov) {
  uncov |= crit.back();
  crit.pop_back();

  for (std::vector<edgemark>::size_type i = 0; i < crit.size(); ++i) {
    crit[i] |= removed_criticals_stack.back()[i];
  }

  removed_criticals_stack.pop_back();
}

inline bool TreeSearch::extend_or_confirm_S(
    edge& S, edge& CAND, std::vector<edgemark>& crit, edgemark& uncov,
    std::vector<edgemark>& vertexhittings,
    std::vector<std::vector<edgemark>>& removed_criticals_stack,
    std::stack<std::vector<std::vector<unsigned>>>& intersection_stack,
    std::deque<edge::size_type>& tointersect_queue) {
  m_RC.count_tree_node();
  if (uncov.none()) {
    pullUpIntersections(intersection_stack, tointersect_queue);

    if (intersection_stack.top().empty()) {
      if (!m_RC.ucc_found(S)) {
        // timeout
        throw timeout;
      }
      return false;
    }

    // gain new edges and minimize
    updateEdges(crit, uncov, vertexhittings, removed_criticals_stack,
                intersection_stack.top());

    // check if minimality still holds
    if (!S_fulfills_minimality_condition(crit)) return true;
  }

  // find edge from uncov with smallest intersecton C with CAND
  m_RC.count_tree_complexity(uncov.count());
  edge C = m_partial_HG[uncov.find_first()] & CAND;
  for (edge::size_type i_e = uncov.find_next(uncov.find_first());
       i_e != edge::npos; i_e = uncov.find_next(i_e)) {
    edge C_new = (m_partial_HG[i_e] & CAND);
    if (C_new.count() < C.count() ||
        (C_new.count() == C.count() && niceness(C_new) < niceness(C))) {
      C = C_new;
    }
  }

  CAND -= C;

  for (edge::size_type v = C.find_first(); v != edge::npos;
       v = C.find_next(v)) {
    // don't branch if v is violater for S
    if (isViolater(crit, vertexhittings[v])) continue;

    // branch
    update_crit_and_uncov(removed_criticals_stack, crit, uncov,
                          vertexhittings[v]);

    S.set(v);
    tointersect_queue.push_back(v);
    bool check = extend_or_confirm_S(S, CAND, crit, uncov, vertexhittings,
                                     removed_criticals_stack,
                                     intersection_stack, tointersect_queue);
    if (tointersect_queue.empty())
      intersection_stack.pop();
    else
      tointersect_queue.pop_back();
    S.reset(v);
    restore_crit_and_uncov(removed_criticals_stack, crit, uncov);

    // prove if deeper update of edges destroyed minimality condition
    if (check && !S_fulfills_minimality_condition(crit)) {
      // add C now to CAND; this also adds the violaters to CAND again and it is
      // equal to the CAND passed in
      CAND |= C;

      return true;
    }

    // update CAND
    CAND.set(v);
  }

  // add C now to CAND; this also adds the violaters to CAND again and it is
  // equal to the CAND passed in
  CAND |= C;

  return false;
}

inline void TreeSearch::pullUpIntersections(
    std::stack<std::vector<std::vector<unsigned>>>& intersection_stack,
    std::deque<edge::size_type>& tointersect_queue) {
  m_RC.start_timer(timer::cluster_intersect);
  while (!tointersect_queue.empty()) {
    intersection_stack.push(intersectClusterListAndClusterMapping(
        intersection_stack.top(),
        m_tab.inverse_mapping[tointersect_queue.front()]));

    tointersect_queue.pop_front();
  }
  m_RC.stop_timer(timer::cluster_intersect);
}

std::vector<std::vector<unsigned>>
TreeSearch::intersectClusterListAndClusterMapping(
    const std::vector<std::vector<unsigned>>& PLI,
    const std::vector<unsigned>& inverse_mapping) {
  m_RC.count_intersections();
  std::vector<std::vector<unsigned>> intersection;

  std::vector<unsigned long> clusterids;
  for (const auto& cluster : PLI) {
    m_RC.count_intersection_cluster_size(cluster.size());
    clusterids.clear();
    for (std::vector<unsigned>::size_type i_r : cluster) {
      if (inverse_mapping[i_r] != size_one_cluster) {
        auto& map_entry = m_clusterid_to_recordindeces[inverse_mapping[i_r]];
        if (map_entry.size() == 0) {
          clusterids.push_back(inverse_mapping[i_r]);
        }
        map_entry.push_back(i_r);
      }
    }
    for (auto clusterid : clusterids) {
      auto& map_entry = m_clusterid_to_recordindeces[clusterid];
      if (map_entry.size() != 1) {
        intersection.emplace_back(std::move(map_entry));
      }
      m_clusterid_to_recordindeces[clusterid] = {};
    }
  }

  return intersection;
}

inline void TreeSearch::updateEdges(
    std::vector<edgemark>& crit, edgemark& uncov,
    std::vector<edgemark>& vertexhittings,
    std::vector<std::vector<edgemark>>& removed_criticals_stack,
    const std::vector<std::vector<unsigned>>& PLI) {
  // sample new edges
  m_RC.start_timer(timer::sample_diff_sets);
  Hypergraph new_edges = sample(PLI);
  m_RC.stop_timer(timer::sample_diff_sets);

  // find out which edges are supersets and therefore can be removed and save
  // indeces in descending order
  std::vector<std::vector<edge>::size_type> supsets_indeces;
  for (std::vector<edge>::size_type i_e = m_partial_HG.numEdges(); i_e > 0;
       /* gets decreased below */) {
    --i_e;

    for (const edge& new_edge : new_edges) {
      if (new_edge.is_subset_of(m_partial_HG[i_e])) {
        supsets_indeces.push_back(i_e);
        break;
      }
    }
  }

  // remove these edges from difference_graph, vertexhittings, uncov, crit,
  // removed_criticals

  for (std::vector<edge>::size_type i_e : supsets_indeces) {
    // difference_graph
    m_partial_HG[i_e] = m_partial_HG[m_partial_HG.numEdges() - 1];
    m_partial_HG.removeLastEdge();

    // vertexhittings
    for (edgemark& hittings : vertexhittings) {
      hittings[i_e] = hittings[hittings.size() - 1];
      hittings.pop_back();
    }

    // uncov
    uncov[i_e] = uncov[uncov.size() - 1];
    uncov.pop_back();

    // crit
    for (edgemark& em_crit : crit) {
      em_crit[i_e] = em_crit[em_crit.size() - 1];
      em_crit.pop_back();
    }

    // removed_criticals
    for (auto& removed_criticals : removed_criticals_stack) {
      for (auto& removed : removed_criticals) {
        removed[i_e] = removed[removed.size() - 1];
        removed.pop_back();
      }
    }
  }

  // insert the new edges in difference_graph, vertexhittings, uncov, crit,
  // removed_criticals

  // difference graph
  for (const edge& e : new_edges) {
    m_partial_HG.addEdge(e);
  }

  // vertexhittings
  for (edge::size_type i_v = 0; i_v < m_partial_HG.numVertices(); ++i_v) {
    vertexhittings[i_v].resize(m_partial_HG.numEdges());
  }
  for (std::size_t i_e = m_partial_HG.numEdges() - new_edges.numEdges();
       i_e < m_partial_HG.numEdges(); ++i_e) {
    for (edge::size_type i_v = m_partial_HG[i_e].find_first();
         i_v != edge::npos; i_v = m_partial_HG[i_e].find_next(i_v)) {
      vertexhittings[i_v].set(i_e);
    }
  }

  // uncov
  uncov.resize(m_partial_HG.numEdges(), true);

  // crit
  for (edgemark& em : crit) {
    em.resize(m_partial_HG.numEdges());
  }

  // removed_criticals
  for (auto& removed_criticals : removed_criticals_stack) {
    for (auto& removed : removed_criticals) {
      removed.resize(m_partial_HG.numEdges());
    }
  }
}

inline bool TreeSearch::S_fulfills_minimality_condition(
    const std::vector<edgemark>& crit) {
  for (const edgemark& em : crit) {
    if (em.none()) return false;
  }

  return true;
}

inline bool TreeSearch::isViolater(const std::vector<edgemark>& crit,
                                   const edgemark& v_hittings) {
  for (const edgemark& em : crit) {
    if (em.is_subset_of(v_hittings)) return true;
  }

  return false;
}

}  // namespace hpiv
