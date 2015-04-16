#include "graphlayouter.h"

#include "ogdf/basic/Graph.h"
#include "ogdf/basic/GraphAttributes.h"
#include "ogdf/energybased/FMMMLayout.h"

GraphLayouter::GraphLayouter(const RandomGraph& g) :
    graph(g), has_layout(false) {}

void GraphLayouter::processLayout() {
    ogdf::Graph g;
    ogdf::GraphAttributes ga(g, ogdf::GraphAttributes::nodeGraphics |
                                ogdf::GraphAttributes::edgeGraphics |
                                ogdf::GraphAttributes::edgeDoubleWeight |
                                ogdf::GraphAttributes::nodeId);
    std::vector<std::pair<size_t, double> > vertexs = graph.getVertexList();
    std::map<size_t, size_t> pseudo_id;
    std::vector<ogdf::node> nodes;
    for (size_t i = 0; i < vertexs.size(); ++i) {
        nodes.push_back(g.newNode());
        ga.width(nodes.back()) = ga.height(nodes.back()) = 10.0;
        ga.idNode(nodes.back()) = vertexs[i].first;
        pseudo_id[vertexs[i].first] = i;
    }
    std::vector<RGEdge> edges = graph.getEdgeList();
    for (size_t i = 0; i < edges.size(); ++i) {
        const RGEdge& e = edges[i];
        ogdf::edge edge = g.newEdge(nodes[pseudo_id[e.v1]],
                                    nodes[pseudo_id[e.v2]]);
        ga.doubleWeight(edge) = e.rel;
        edge = g.newEdge(nodes[pseudo_id[e.v2]],
                         nodes[pseudo_id[e.v1]]);
        ga.doubleWeight(edge) = e.rel;
    }

    ogdf::FMMMLayout fmmm;
    fmmm.useHighLevelOptions(true);
    fmmm.unitEdgeLength(50);
    fmmm.newInitialPlacement(true);
    fmmm.qualityVersusSpeed(ogdf::FMMMLayout::qvsGorgeousAndEfficient);
    fmmm.call(ga);
    has_layout = true;
    ogdf::node v;
    forall_nodes(v, g) {
        int id = ga.idNode(v);
        double x = ga.x(v);
        double y = ga.y(v);
        pts.insert(std::make_pair(static_cast<size_t>(id), Point2D(x, y)));
    }
}
