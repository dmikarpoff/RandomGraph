#ifndef GRAPHLAYOUTER_H
#define GRAPHLAYOUTER_H

#include "RandomGraph.h"

struct Point2D {
    double x;
    double y;
    Point2D(double x_, double y_):
        x(x_), y(y_) {}
};

class GraphLayouter {
 public:
    GraphLayouter(const RandomGraph& g);
    void processLayout();
    const std::map<size_t, Point2D>& getLayout() const {
        return pts;
    }
 private:
    RandomGraph graph;
    bool has_layout;
    std::map<size_t, Point2D> pts;
};

#endif // GRAPHLAYOUTER_H
