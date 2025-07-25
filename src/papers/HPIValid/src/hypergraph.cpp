#include "hypergraph.hpp"

#include <fstream>

Hypergraph::Hypergraph(edge::size_type numVertices)
    : m_numVertices(numVertices), m_Edges() {}

void Hypergraph::addEdgeAndMinimizeInclusion(const edge &newEdge) {
  // is newEdge a supset of an edge in m_Edges?
  bool is_supset = false;
  // list of indeces of supsets of newEdge from m_Edges in descending order
  std::vector<std::vector<edge>::size_type> supsets_indeces;
  for (std::vector<edge>::size_type i_e = this->numEdges(); i_e > 0;
       /* gets decreased below */) {
    --i_e;

    if (m_Edges[i_e].is_subset_of(newEdge)) {
      is_supset = true;
      break;
    }

    if (newEdge.is_subset_of(m_Edges[i_e])) {
      supsets_indeces.push_back(i_e);
    }
  }

  if (!is_supset) {
    for (std::vector<edge>::size_type i_e : supsets_indeces) {
      m_Edges[i_e] = m_Edges[m_Edges.size() - 1];
      m_Edges.pop_back();
    }
    m_Edges.push_back(newEdge);
  }
}
