#include "tester.h"
#include "graphestimator.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <QTime>

#define REPEAT_ITER 10

Tester::Tester()
{
}

void Tester::runFullGraphTest() {
    std::cout << "Testing full graph" << std::endl;
    std::ofstream out("full_graph.csv");
    std::vector<std::pair<size_t, double> > v;
    v.push_back(std::make_pair(1, 0.0));
    std::vector<RGEdge> e;
    for (size_t n = 2; n <= 8; ++n) {
        if (e.size() == 19) {
            break;
        }
        v.push_back(std::make_pair(n, 0.0));
        for (size_t h = 1; h <= n - 1; ++h) {
            double total_time = 0.0;
            RGEdge rge;
            rge.v1 = h;
            rge.v2 = n;
            e.push_back(rge);
            std::cout << "Edge amount = " << e.size() << std::endl;
            for (size_t t = 0; t < REPEAT_ITER; ++t) {
                std::cout << "\tIteration = " << t << std::endl;
                for (size_t i = 0; i < v.size(); ++i)
                    v[i].second = (rand() % 10000) / 10000.0;
                for (size_t i = 0; i < e.size(); ++i)
                    e[i].rel = (rand() % 10000) / 10000.0;
                GraphEstimator solver;
                solver.loadGraph(e, v);
                solver.setSrcDst(1, n);
                solver.setTransitNodeAmount(n);
                QTime timer;
                timer.start();
                solver.solve();
                uint elapsed = timer.elapsed();
                std::cout << "\t Time elapsed = " << elapsed / 1000.0 << std::endl;
                total_time += elapsed / 1000.0;
                double prob = solver.extractProb();
//                assert(prob > -1E-6 && prob < 1 + 1E-6);
            }
            total_time /= REPEAT_ITER;
            out << e.size() << ";" << total_time << std::endl;
            if (e.size() == 19)
                break;
        }
    }
}

void Tester::runFullGraphRestrictedTest() {
    std::cout << "Testing full graph restricted" << std::endl;
    std::ofstream out("full_graph_restricted.csv");
    std::vector<std::pair<size_t, double> > v;
    v.push_back(std::make_pair(1, 0.0));
    std::vector<RGEdge> e;
    for (size_t n = 2; n <= 8; ++n) {
        if (e.size() == 21) {
            break;
        }
        v.push_back(std::make_pair(n, 0.0));
        for (size_t h = 1; h <= n - 1; ++h) {
            RGEdge rge;
            rge.v1 = h;
            rge.v2 = n;
            e.push_back(rge);
            double total_time = 0.0;
            std::cout << "Edge amount = " << e.size() << std::endl;
            for (size_t t = 0; t < REPEAT_ITER; ++t) {
                std::cout << "\tIteration = " << t << std::endl;
                for (size_t i = 0; i < v.size(); ++i)
                    v[i].second = (rand() % 10000) / 10000.0;
                for (size_t i = 0; i < e.size(); ++i)
                    e[i].rel = (rand() % 10000) / 10000.0;
                GraphEstimator solver;
                solver.loadGraph(e, v);
                solver.setSrcDst(1, n);
                solver.setTransitNodeAmount(n / 2);
                QTime timer;
                timer.start();
                solver.solve();
                uint elapsed = timer.elapsed();
                std::cout << "\t Time elapsed = " << elapsed / 1000.0 << std::endl;
                total_time += elapsed / 1000.0;
                double prob = solver.extractProb();
//                assert(prob > -1E-6 && prob < 1 + 1E-6);
            }
            total_time /= REPEAT_ITER;
            out << e.size() << ";" << total_time << std::endl;
            if (e.size() == 21)
                break;
        }
    }
}

void singleSolve(std::vector<std::pair<size_t, double> >& v,
                 std::vector<RGEdge>& e,
                 std::ofstream& out, size_t last, size_t trans) {
    double total_time = 0.0;
    std::cout << "Edge amount = " << e.size() << std::endl;
    for (size_t t = 0; t < REPEAT_ITER; ++t) {
        std::cout << "\tIteration = " << t << std::endl;
        for (size_t i = 0; i < v.size(); ++i)
            v[i].second = (rand() % 10000) / 10000.0;
        for (size_t i = 0; i < e.size(); ++i)
            e[i].rel = (rand() % 10000) / 10000.0;
        GraphEstimator solver;
        solver.loadGraph(e, v);
        solver.setSrcDst(1, last);
        solver.setTransitNodeAmount(trans);
        QTime timer;
        timer.start();
        solver.solve();
        uint elapsed = timer.elapsed();
        std::cout << "\t Time elapsed = " << elapsed / 1000.0 << std::endl;
        total_time += elapsed / 1000.0;
        double prob = solver.extractProb();
//                assert(prob > -1E-6 && prob < 1 + 1E-6);
    }
    out << e.size() << ";" << total_time / REPEAT_ITER << std::endl;
}

void Tester::runGridGraphTest() {
    std::cout << "Testing grid graph" << std::endl;
    std::ofstream out("grid_graph.csv");
    std::vector<std::pair<size_t, double> > v;
    v.push_back(std::make_pair(1, 0.0));
    std::vector<RGEdge> e;
    out << "Edge amount;Time" << std::endl;
    for (size_t m = 2; m <= 6; ++m) {
        if (e.size() > 25)
            break;
        v.push_back(std::make_pair(m * m, 0.0));
        RGEdge ne;
        ne.v1 = (m - 1) * (m - 1);
        ne.v2 = m * m;
        e.push_back(ne);
        singleSolve(v, e, out, m * m, m * m);
        if (e.size() > 25)
            break;
        for (size_t i = m - 1; i >= 1; --i) {
            v.push_back(std::make_pair(i * m + m, 0.0));
            RGEdge te;
            te.v1 = i * m + m;
            te.v2 = (i + 1) * m + m;
            e.push_back(te);
            singleSolve(v, e, out, m * m, m * m);
            te.v2 = (i + 1) * m + m - 1;
            e.push_back(te);
            singleSolve(v, e, out, m * m, m * m);
            if (i > 1) {
                te.v2 = (i - 1) * m + m - 1;
                e.push_back(te);
                singleSolve(v, e, out, m * m, m * m);
            }
            if (e.size() > 25)
                break;
        }
        if (e.size() > 25)
            break;
        for (size_t i = m - 1; i >= 1; --i) {
            v.push_back(std::make_pair(m + m, 0.0));
            RGEdge te;
            te.v1 = (m - 1)* m + i;
            te.v2 = (m - 1)* m + i - 1;
            e.push_back(te);
            singleSolve(v, e, out, m * m, m * m);
            te.v2 = (m - 2)* m + i;
            e.push_back(te);
            singleSolve(v, e, out, m * m, m * m);
            if (i > 1) {
                te.v2 = (m - 2) * m + i - 1;
                e.push_back(te);
                singleSolve(v, e, out, m * m, m * m);
            }
            if (e.size() > 25)
                break;
        }
    }
}

void Tester::runGridGraphRestrictedTest() {
    std::cout << "Testing grid graph" << std::endl;
    std::ofstream out("grid_graph_restricted.csv");
    std::vector<std::pair<size_t, double> > v;
    v.push_back(std::make_pair(1, 0.0));
    std::vector<RGEdge> e;
    out << "Edge amount;Time" << std::endl;
    for (size_t m = 2; m <= 6; ++m) {
        if (e.size() > 30)
            break;
        v.push_back(std::make_pair(m * m, 0.0));
        RGEdge ne;
        ne.v1 = (m - 1) * (m - 1);
        ne.v2 = m * m;
        e.push_back(ne);
        singleSolve(v, e, out, m * m, v.size() / 2);
        if (e.size() > 30)
            break;
        for (size_t i = m - 1; i >= 1; --i) {
            v.push_back(std::make_pair(i * m + m, 0.0));
            RGEdge te;
            te.v1 = i * m + m;
            te.v2 = (i + 1) * m + m;
            e.push_back(te);
            singleSolve(v, e, out, m * m, v.size() / 2);
            te.v2 = (i + 1) * m + m - 1;
            e.push_back(te);
            singleSolve(v, e, out, m * m, v.size() / 2);
            if (i > 1) {
                te.v2 = (i - 1) * m + m - 1;
                e.push_back(te);
                singleSolve(v, e, out, m * m, v.size() / 2);
            }
            if (e.size() > 30)
                break;
        }
        if (e.size() > 30)
            break;
        for (size_t i = m - 1; i >= 1; --i) {
            v.push_back(std::make_pair(m + m, 0.0));
            RGEdge te;
            te.v1 = (m - 1)* m + i;
            te.v2 = (m - 1)* m + i - 1;
            e.push_back(te);
            singleSolve(v, e, out, m * m, v.size() / 2);
            te.v2 = (m - 2)* m + i;
            e.push_back(te);
            singleSolve(v, e, out, m * m, v.size() / 2);
            if (i > 1) {
                te.v2 = (m - 2) * m + i - 1;
                e.push_back(te);
                singleSolve(v, e, out, m * m, v.size() / 2);
            }
            if (e.size() > 30)
                break;
        }
    }
}
