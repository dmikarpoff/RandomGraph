#include <bitset>
#include <iostream>
#include <fstream>
#include <set>

#include "graphestimator.h"

#define BITMAP_MODULE_SIZE (8 * sizeof(unsigned int))

PathUnion::PathUnion(const size_t &max_hash) {
    size_t vsize = max_hash / (BITMAP_MODULE_SIZE) + 1;
    elem_set_.resize(vsize, 0);
}

bool PathUnion::combine(const std::vector<size_t> &vhash,
                        const std::vector<size_t> &ehash) {
    bool update = false;
    for (size_t i = 0; i < vhash.size(); ++i) {
        unsigned int c = 1 << (vhash[i] % BITMAP_MODULE_SIZE);
        if ((elem_set_[vhash[i] / BITMAP_MODULE_SIZE] & c) == 0)
            update = true;
        elem_set_[vhash[i] / BITMAP_MODULE_SIZE] |= c;
    }
    for (size_t i = 0; i < ehash.size(); ++i) {
        unsigned int c = 1 << (ehash[i] % BITMAP_MODULE_SIZE);
        if ((elem_set_[ehash[i] / BITMAP_MODULE_SIZE] & c) == 0)
            update = true;
        elem_set_[ehash[i] / BITMAP_MODULE_SIZE] |= c;
    }
    return update;
}

bool PathUnion::operator <(const PathUnion& x) const {
    if (x.elem_set_.size() > elem_set_.size())
        return true;
    if (elem_set_.size() > x.elem_set_.size())
        return false;
    for (size_t i = 0; i < elem_set_.size(); ++i) {
        if (elem_set_[i] < x.elem_set_[i])
            return true;
    }
    return false;
}

GraphEstimator::GraphEstimator() {
}

void GraphEstimator::solve() {
    used_.clear();
    mult_.clear();
    mult_.push_back(std::set<ProbMultiplier>());
    ProbMultiplier m(last_hash_);
    m.scalar_ = 1.0;
    mult_[0].insert(m);
    findPath(src_);
    double p = extractProb();
    std::cout << "Result probability = " << p << std::endl;
    emit notifyResult(p);
}

void GraphEstimator::findPath(size_t v) {
//    std::cout << "Start searching from " << v << std::endl;
    if (cur_path_.size() > amount_ + 1) {
//        std::cout << "Path too long" << std::endl;
        return;
    }
    if (v == dst_) {
        used_.insert(v);
        cur_path_.push_back(v);
//        std::cout << "Path found" << std::endl;
        addPath();
        cur_path_.pop_back();
        used_.erase(v);
        return;
    }
    cur_path_.push_back(v);
    used_.insert(v);
    const std::vector<pair_ud>& neigh = adj_[v];
    for (size_t i = 0; i < neigh.size(); ++i)
        if (used_.find(neigh[i].first) == used_.end()) {
            findPath(neigh[i].first);
        }
    cur_path_.pop_back();
    used_.erase(v);
//    std::cout << "Paths from " << v << " processed" << std::endl;
}

void GraphEstimator::addPath() {
//    std::ofstream out("log.txt", std::ios_base::app);
//    out << "Start adding new path" << std::endl;
    std::vector<size_t> vhash, ehash;
    for (size_t i = 0; i < cur_path_.size(); ++i) {
//        out << cur_path_[i] << " ";
        vhash.push_back(vertex_hash[cur_path_[i]]);
//        out << "(" << vhash[i] << ") ";
    }
//    out << std::endl;
    for (size_t i = 0; i < cur_path_.size() - 1; ++i) {
        size_t min_v = std::min(cur_path_[i], cur_path_[i + 1]);
        size_t max_v = std::max(cur_path_[i], cur_path_[i + 1]);
//        out << "(" << min_v << ", " << max_v << ") ";
        ehash.push_back(edge_hash[std::make_pair(min_v, max_v)]);
//        out << "[" << edge_hash[std::make_pair(min_v, max_v)] << "] ";
    }
//    out << std::endl;
//    out << "Old mult size = ";
//    out << mult_.size() << std::endl;
//    out << "------------------------------------" << std::endl;
//    out << "Old  state:" << std::endl;
//    for (size_t i = 0; i < mult_.size(); ++i) {
//        out << "Sets of size " << i << std::endl;
//        for (std::set<ProbMultiplier>::iterator
//                    j = mult_[i].begin(); j != mult_[i].end(); ++j) {
//            out << "Scalar = " << j->scalar_ << " ";
//            out << "Bitmap = ";
//            for (size_t h = 0; h < j->path_.elem_set_.size(); ++h) {
//                std::bitset<BITMAP_MODULE_SIZE> bs(j->path_.elem_set_[h]);
//                out << bs << " ";
//            }
//            out << std::endl;
//        }
//    }
    std::set<ProbMultiplier> next = mult_[0];
    std::set<ProbMultiplier> cur;
    mult_.push_back(std::set<ProbMultiplier>());
    for (size_t i = 0; i < mult_.size() - 1; ++i) {
        std::swap(cur, next);
        next = mult_[i + 1];
        for (std::set<ProbMultiplier>::iterator
                j = cur.begin(); j != cur.end(); ++j) {
            ProbMultiplier pm = *j;
//            out << "Probmultiplier: " << "scalar=" << pm.scalar_ <<
//                         " path_amount_=" << pm.path_amount_ << std::endl;
            pm.path_amount_++;
            pm.path_.combine(vhash, ehash);
            std::set<ProbMultiplier>::iterator h = mult_[i + 1].find(pm);
            if (h == mult_[i + 1].end()) {
                mult_[i + 1].insert(pm);
            } else {
                BigInteger old_scalar = h->scalar_;
                mult_[i + 1].erase(h);
                pm.scalar_ += old_scalar;
                mult_[i + 1].insert(pm);
            }
        }
    }
//    out << "------------------------------------" << std::endl;
//    out << "New  state:" << std::endl;
//    for (size_t i = 0; i < mult_.size(); ++i) {
//        out << "Sets of size " << i << std::endl;
//        for (std::set<ProbMultiplier>::iterator
//                    j = mult_[i].begin(); j != mult_[i].end(); ++j) {
//            out << "Scalar = " << j->scalar_ << " ";
//            out << "Bitmap = ";
//            for (size_t h = 0; h < j->path_.elem_set_.size(); ++h) {
//                std::bitset<BITMAP_MODULE_SIZE> bs(j->path_.elem_set_[h]);
//                out << bs << " ";
//            }
//            out << std::endl;
//        }
//    }
//    out << "Path added" << std::endl;
}

double GraphEstimator::extractProb() {
    long double prob(0.0) ;
    for (size_t i = 1; i < mult_.size(); ++i) {
        long double sign = static_cast<int>((i % 2)) * 2 - 1;
        for (std::set<ProbMultiplier>::iterator j =
                    mult_[i].begin(); j != mult_[i].end(); ++j) {
            const ProbMultiplier& pm = *j;
            BigInteger scalar = pm.scalar_;
            long double p = 1.0;
            const PathUnion& path_un = pm.path_;
            for (size_t h = 0; h < path_un.elem_set_.size(); ++h) {
                unsigned int c = path_un.elem_set_[h];
                for (size_t o = 0; o < BITMAP_MODULE_SIZE; ++o, c >>= 1)
                    if (c & 1) {
                        size_t cur_hash = h * BITMAP_MODULE_SIZE + o;
                        if (cur_hash < last_edge_hash_)
                            p *= edge_to_prob_[redge_hash[cur_hash]];
                        else
                            p *= vert_to_prob_[rvertex_hash[cur_hash]];
                    }
            }
            prob += sign * scalar.toDouble() * p;
        }
    }
    return prob;
}

void GraphEstimator::loadGraph(const std::vector<RGEdge> &edges,
                               const std::vector<std::pair<size_t, double> > &vertexes) {
    adj_.clear();
    vert_to_prob_.clear();
    edge_to_prob_.clear();
    vertex_hash.clear();
    rvertex_hash.clear();
    edge_hash.clear();
    redge_hash.clear();
    size_t counter = 0;
    for (size_t i = 0; i < edges.size(); ++i) {
        const RGEdge& e = edges[i];
        adj_[e.v1].push_back(std::make_pair(e.v2, e.rel));
        adj_[e.v2].push_back(std::make_pair(e.v1, e.rel));
        edge_to_prob_[std::make_pair(e.v1, e.v2)] = e.rel;
        edge_to_prob_[std::make_pair(e.v2, e.v1)] = e.rel;
        size_t min_v = std::min(e.v1, e.v2);
        size_t max_v = std::max(e.v1, e.v2);
        edge_hash[std::make_pair(min_v, max_v)] = counter;
        redge_hash[counter] = std::make_pair(min_v, max_v);
        ++counter;
    }
    last_edge_hash_ = counter;
    for (size_t i = 0; i < vertexes.size(); ++i) {
        vert_to_prob_[vertexes[i].first] = vertexes[i].second;
        vertex_hash[vertexes[i].first] = counter;
        rvertex_hash[counter] = vertexes[i].first;
        ++counter;
    }
    last_hash_ = counter;
}
