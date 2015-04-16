#include <QDomDocument>
#include <QTextStream>
#include <QXmlStreamReader>

#include <cmath>
#include <iostream>

#include "RandomGraph.h"

bool RandomGraph::addVertex(size_t idx, double rel) {
    if (vertex.find(idx) != vertex.end())
        return false;
    vertex[idx] = rel;
    return true;
}

bool RandomGraph::resetRel(size_t idx, double rel) {
    if (vertex.find(idx) == vertex.end())
        return false;
    vertex[idx] = rel;
    return true;
}

bool RandomGraph::removeVertex(size_t idx) {
    if (vertex.find(idx) == vertex.end())
        return false;
    vertex.erase(idx);
    for (std::map<std::pair<size_t, size_t>, double>::iterator
            i = edge.begin(); i != edge.end(); ++i)
        if (i->first.first == idx || i->first.second == idx) {
            edge.erase(i);
            --i;
        }
    return true;
}

std::vector<std::pair<size_t, double> > RandomGraph::getVertexList() const {
    std::vector<std::pair<size_t, double> > res;
    for (std::map<size_t, double>::const_iterator i =
                vertex.begin(); i != vertex.end(); ++i) {
        res.push_back(*i);
    }
    return res;
}

std::vector<RGEdge> RandomGraph::getEdgeList() const {
    std::vector<RGEdge> res;
    for (std::map<std::pair<size_t, size_t>, double>::const_iterator
                i = edge.begin(); i != edge.end(); ++i) {
        if (i->first.first > i->first.second)
            continue;
        res.push_back(RGEdge());
        res.back().v1 = i->first.first;
        res.back().v2 = i->first.second;
        res.back().rel = i->second;
    }
    return res;
}

bool RandomGraph::addEdge(size_t v1, size_t v2, double rel) {
    if (v1 == v2)
        return false;
    if (v1 > v2)
        std::swap(v1, v2);
    std::map<std::pair<size_t, size_t>, double>::const_iterator
            i = edge.find(std::make_pair(v1, v2));
    if (i != edge.end())
        return false;
    edge[std::make_pair(v1, v2)] = rel;
    return true;
}

bool RandomGraph::editEdge(const size_t &v1, const size_t &v2,
                           const double& old_rel,
                           const double &rel) {
    std::multimap<std::pair<size_t, size_t>, double>::iterator
            i = edge.find(std::make_pair(v1, v2));
    while (i != edge.end() && i->first == std::make_pair(v1, v2)) {
        if (std::abs(i->second - old_rel) < 1E-8) {
            i->second = rel;
            return true;
        }
        ++i;
    }
    return false;
}

bool RandomGraph::removeEdge(size_t v1, size_t v2,
                             const double& rel) {
    if (v1 > v2)
        std::swap(v1, v2);
    std::multimap<std::pair<size_t, size_t>, double>::iterator
            i = edge.find(std::make_pair(v1, v2));
    while (i != edge.end() && i->first == std::make_pair(v1, v2)) {
        if (std::abs(i->second - rel) < 1E-8) {
            edge.erase(i);
            return true;
        }
        ++i;
    }
    return false;
}

bool RandomGraph::import(QFile *file) {
    QDomDocument doc;
    QString error_msg;
    int line, column;
    if (!doc.setContent(file, &error_msg, &line, &column)) {
        std::cout << error_msg.toStdString() << std::endl;
        std::cout << line << " " << column << std::endl;
        return false;
    }
    QDomElement el = doc.firstChildElement();
    if (el.tagName() != "random_graph")
        return false;
    el = el.firstChildElement();
    std::vector<std::pair<std::pair<size_t, size_t>, double> > edges;
    while (!el.isNull()) {
        if (el.tagName() == "vertexes") {
            QDomElement child = el.firstChildElement();
            while (!child.isNull()) {
                if (child.tagName() != "vertex")
                    return false;
                if (child.hasAttribute("id") && child.hasAttribute("rel")) {
                    size_t id = child.attribute("id").toULong();
                    double rel = child.attribute("rel").toDouble();
                    if (!addVertex(id, rel))
                        return false;
                    child = child.nextSiblingElement();
                } else {
                    return false;
                }
            }
            el = el.nextSiblingElement();
            continue;
        }
        if (el.tagName() == "edges") {
            QDomElement child = el.firstChildElement();
            while (!child.isNull()) {
                if (child.tagName() != "edge")
                    return false;
                if (child.hasAttribute("v1") && child.hasAttribute("v2") &&
                        child.hasAttribute("rel")) {
                    size_t v1 = child.attribute("v1").toULong();
                    size_t v2 = child.attribute("v2").toULong();
                    double rel = child.attribute("rel").toDouble();
                    edges.push_back(std::make_pair(std::make_pair(v1, v2), rel));
                    child = child.nextSiblingElement();
                } else {
                    return false;
                }
            }
            el = el.nextSiblingElement();
            continue;
        }
        return false;
    }
    for (size_t i = 0; i < edges.size(); ++i)
        if (!addEdge(edges[i].first.first, edges[i].first.second,
                     edges[i].second))
            return false;
    return true;
}

bool RandomGraph::save(QFile *file) {
    QDomDocument doc;
    QDomElement rg = doc.createElement("random_graph");
    doc.appendChild(rg);
    QDomElement vert_tag = doc.createElement("vertexes");
    rg.appendChild(vert_tag);
    for (std::map<size_t, double>::iterator i = vertex.begin();
                        i != vertex.end(); ++i) {
        QDomElement single_vert = doc.createElement("vertex");
        single_vert.setAttribute("id", i->first);
        single_vert.setAttribute("rel", i->second);
        vert_tag.appendChild(single_vert);
    }
    QDomElement edge_tag = doc.createElement("edges");
    rg.appendChild(edge_tag);
    for (std::map<std::pair<size_t, size_t>, double>::iterator
                i = edge.begin(); i != edge.end(); ++i) {
        QDomElement single_edge = doc.createElement("edge");
        size_t v1 = i->first.first;
        size_t v2 = i->first.second;
        if (v1 > v2)
            continue;
        single_edge.setAttribute("v1", v1);
        single_edge.setAttribute("v2", v2);
        single_edge.setAttribute("rel", i->second);
        edge_tag.appendChild(single_edge);
    }
    QTextStream out(file);
    out << doc.toString();
    return true;
}
