#pragma once

#include <boost/dynamic_bitset.hpp>
#include <filesystem>
#include <vector>

typedef boost::dynamic_bitset<> edge;
typedef boost::dynamic_bitset<> edgemark;

class Hypergraph {
 private:
  edge::size_type m_numVertices;
  std::vector<edge> m_Edges;

 public:
  Hypergraph() = delete;
  explicit Hypergraph(edge::size_type numVertices);

  std::vector<edge>::size_type numEdges() const { return m_Edges.size(); }
  edge::size_type numVertices() const { return m_numVertices; }

  inline void addEdge(const edge& newEdge) { m_Edges.push_back(newEdge); }
  inline void addEdge(edge&& newEdge) { m_Edges.push_back(newEdge); }
  inline void removeLastEdge() { m_Edges.pop_back(); }

  void addEdgeAndMinimizeInclusion(const edge& newEdge);

  // operators and related

  inline edge& operator[](std::vector<edge>::size_type i_e) {
    return m_Edges[i_e];
  }
  inline const edge& operator[](std::vector<edge>::size_type i_e) const {
    return m_Edges[i_e];
  }

  inline std::vector<edge>::iterator begin() { return m_Edges.begin(); }
  inline std::vector<edge>::const_iterator begin() const {
    return m_Edges.begin();
  }

  inline std::vector<edge>::iterator end() { return m_Edges.end(); }
  inline std::vector<edge>::const_iterator end() const { return m_Edges.end(); }
};
