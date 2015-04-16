#ifndef GRAPHESTIMATOR_H
#define GRAPHESTIMATOR_H

#include <set>
#include <stddef.h>
#include <vector>
#include <QObject>
#include "bignumber.h"
#include "RandomGraph.h"

struct PathUnion {
 public:
    PathUnion(const size_t& max_hash);
    bool combine(const std::vector<size_t>& vhash,
                 const std::vector<size_t>& ehash);
    bool operator<(const PathUnion& x) const;
    std::vector<unsigned int> elem_set_;
};

struct ProbMultiplier {
    ProbMultiplier(const size_t& max_hash_):
        scalar_(0), path_amount_(0),
        path_(max_hash_) {}
    bool operator <(const ProbMultiplier& x) const {
        return path_ < x.path_;
    }

    BigInteger scalar_;
    size_t path_amount_;
    PathUnion path_;
};

class GraphEstimator : public QObject{
    Q_OBJECT
 public:
    typedef std::pair<size_t, double> pair_ud;

    GraphEstimator();
    void loadGraph(const std::vector<RGEdge>& edges,
                   const std::vector<std::pair<size_t, double> >& vertexes);
    void setSrcDst(size_t src, size_t dst) {
        src_ = src;
        dst_ = dst;
    }
    void setTransitNodeAmount(size_t amount) {
        amount_ = amount;
    }

 signals:
    void notifyResult(double val);

 public slots:
    void solve();
    double extractProb();

 private:
    void findPath(size_t v);
    void addPath();

    std::map<size_t, std::vector<pair_ud> > adj_;
    size_t src_, dst_;
    size_t amount_;
    std::set<size_t> used_;
    std::vector<size_t> cur_path_;
    std::map<size_t, double> vert_to_prob_;
    std::map<std::pair<size_t, size_t>, double> edge_to_prob_;
    std::map<size_t, size_t> vertex_hash;
    std::map<size_t, size_t> rvertex_hash;
    std::map<std::pair<size_t, size_t>, size_t> edge_hash;
    std::map<size_t, std::pair<size_t, size_t> > redge_hash;
    size_t last_edge_hash_;
    size_t last_hash_;
    std::vector<std::set<ProbMultiplier> > mult_;
};

#endif // GRAPHESTIMATOR_H
