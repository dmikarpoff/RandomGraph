#ifndef RANDOMGRAPH_H
#define RANDOMGRAPH_H

#include <cstdlib>
#include <map>
#include <vector>

#include <QFile>

struct RGVertex {
    size_t idx;
    double rel;
};

struct RGEdge {
    size_t v1, v2;
    double rel;
};

class RandomGraph {
 public:
    bool addVertex(size_t idx, double rel);
    bool resetRel(size_t idx, double rel);
    bool removeVertex(size_t idx);
    std::vector<std::pair<size_t, double> > getVertexList() const;
    std::vector<RGEdge> getEdgeList() const;
    bool addEdge(size_t v1, size_t v2, double rel);
    bool editEdge(const size_t& v1, const size_t& v2,
                  const double& old_rel, const double& rel);
    bool removeEdge(size_t v1, size_t v2, const double &rel);
    bool import(QFile* file);
    bool save(QFile* file);
 private:
    std::map<size_t, double> vertex;
    std::map<std::pair<size_t, size_t>, double> edge;
};

#endif // RANDOMGRAPH_H
