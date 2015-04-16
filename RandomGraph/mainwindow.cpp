#include <QApplication>
#include <QComboBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsTextItem>
#include <QGraphicsItem>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <QRect>
#include <QSlider>
#include <QThread>
#include <QTimer>
#include <QWidget>

#include <cmath>
#include <set>

#include "EdgeWindow.h"
#include "graphestimator.h"
#include "mainwindow.h"
#include "VertexWindow.h"

const float MainWindow::FRACT_DISPLAY = 0.6;

MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent, Qt::Window), layouter(NULL) {
    setupUi(this);
    work_timer_ = new QTimer();

    vert_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    vert_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    edge_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    edge_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    edge_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    QRect desk_rect = QApplication::desktop()->screen()->rect();
    resize(FRACT_DISPLAY * desk_rect.size());
    move(desk_rect.center() - rect().center());
    scene = new QGraphicsScene();
    canvas->setScene(scene);

    menu_bar = new QMenuBar(this);
    base_layout->addWidget(menu_bar, 0, 0, 1, 2);
    createMenus();

    connect(create_vert, SIGNAL(clicked()),SLOT(onAddVertexClicked()));
    connect(edit_vert, SIGNAL(clicked()), SLOT(onEditVertexClicked()));
    connect(remove_vert, SIGNAL(clicked()), SLOT(onRemoveVertexClicked()));
    connect(create_edge, SIGNAL(clicked()), SLOT(onAddEdgeClicked()));
    connect(edit_edge, SIGNAL(clicked()), SLOT(onEditEdgeClicked()));
    connect(remove_edge, SIGNAL(clicked()), SLOT(onRemoveEdgeClicked()));

    layouter = new GraphLayouter(graph);
    layouter->processLayout();
    connect(this, SIGNAL(updateLayouterSig()), SLOT(updateLayouter()));
    connect(vert_table, SIGNAL(itemSelectionChanged()), SLOT(redrawScene()));
    connect(edge_table, SIGNAL(itemSelectionChanged()), SLOT(redrawScene()));
    connect(this, SIGNAL(vertexListChangedSig()), SLOT(vertexListChanged()));
    connect(calc_button, SIGNAL(clicked()), this, SLOT(estimateButtonClicked()));
    connect(reset_button, SIGNAL(clicked()), this, SLOT(resetButtonClicked()));

    reset_button->setEnabled(false);

    emit updateLayouterSig();
}

MainWindow::~MainWindow() {
    delete layouter;
}

void MainWindow::onAddVertexClicked() {
    VertexWindow* vw = new VertexWindow(this, ADD);
    modalWindow = vw;
    connect(vw, SIGNAL(addVertexSig(int, double)),
                SLOT(addVertex(int, double)));
    modalWindow->show();
}

void MainWindow::onEditVertexClicked() {
    QItemSelectionModel* model = vert_table->selectionModel();
    QModelIndexList sel_rows = model->selectedRows();
    if (sel_rows.size() != 1)
        return;
    int row = sel_rows.first().row();
    VertexWindow* vw = new VertexWindow(this, EDIT);
    int idx = vert_table->item(row, 0)->text().toInt();
    double rel = vert_table->item(row, 1)->text().toDouble();
    vw->setIndex(idx);
    vw->setReliability(rel);
    modalWindow = vw;
    connect(vw, SIGNAL(addVertexSig(int, double)),
                SLOT(editVertex(int, double)));
    modalWindow->show();
}

void MainWindow::onRemoveVertexClicked() {
    QItemSelectionModel* model = vert_table->selectionModel();
    QModelIndexList sel_rows = model->selectedRows();
    qSort(sel_rows.begin(), sel_rows.end(), qGreater<QModelIndex>());
    foreach (QModelIndex idx, sel_rows) {
        int v_idx = vert_table->item(idx.row(), 0)->text().toInt();
        vert_table->removeRow(idx.row());
        graph.removeVertex(v_idx);
        int edge_amount = edge_table->rowCount();
        for (int i = edge_amount - 1; i >= 0; --i)
            if (edge_table->item(i, 0)->text().toInt() == v_idx ||
                    edge_table->item(i, 1)->text().toInt() == v_idx)
                edge_table->removeRow(i);
    }
    emit updateLayouterSig();
}

void MainWindow::onAddEdgeClicked() {
    EdgeWindow* ew = new EdgeWindow(this, EW_ADD);
    std::vector<std::pair<size_t, double> >
            vert_list = graph.getVertexList();
    std::vector<size_t> vert_idx;
    vert_idx.reserve(vert_list.size());
    for (size_t i = 0; i < vert_list.size(); ++i)
        vert_idx.push_back(vert_list[i].first);
    ew->loadVertexList(vert_idx);
    connect(ew, SIGNAL(addEdgeSig(ulong,ulong,double,double)),
            SLOT(addEdge(ulong,ulong,double,double)));
    modalWindow = ew;
    ew->show();
    emit vertexListChangedSig();
}

void MainWindow::onEditEdgeClicked() {
    QItemSelectionModel* model = edge_table->selectionModel();
    QModelIndexList sel_rows = model->selectedRows();
    if (sel_rows.size() != 1)
        return;
    int row = sel_rows.first().row();
    int v1 = edge_table->item(row, 0)->text().toInt();
    int v2 = edge_table->item(row, 1)->text().toInt();
    double rel = edge_table->item(row, 2)->text().toDouble();
    EdgeWindow* ew = new EdgeWindow(this, EW_EDIT);
    modalWindow = ew;
    ew->setEditableEdge(v1, v2, rel);
    connect(ew, SIGNAL(addEdgeSig(ulong,ulong,double,double)),
                SLOT(editEdge(ulong,ulong,double,double)));
    ew->show();
    emit vertexListChangedSig();
}

void MainWindow::onRemoveEdgeClicked() {
    QItemSelectionModel* model = edge_table->selectionModel();
    QModelIndexList sel_rows = model->selectedRows();
    qSort(sel_rows.begin(), sel_rows.end(), qGreater<QModelIndex>());
    foreach (QModelIndex idx, sel_rows) {
        ulong v1 = edge_table->item(idx.row(), 0)->text().toULong();
        ulong v2 = edge_table->item(idx.row(), 1)->text().toULong();
        double rel = edge_table->item(idx.row(), 2)->text().toDouble();
        edge_table->removeRow(idx.row());
        graph.removeEdge(v1, v2, rel);
    }
    emit updateLayouterSig();
    emit vertexListChangedSig();
}

void MainWindow::addVertex(int idx, double rel) {
    if (rel > 1.0 || rel < 0.0) {
        QMessageBox* error_msg = new QMessageBox(modalWindow);
        error_msg->critical(modalWindow, "Ошибка",
                "Надежность должна быть числом в диапазоне [0, 1]");
        return;
    }
    if (!graph.addVertex(idx, rel)) {
        QMessageBox* error_msg = new QMessageBox(modalWindow);
        error_msg->critical(modalWindow, "Ошибка",
                "Вершина с таким индексом уже существует");
        return;
    }
    vert_table->insertRow(vert_table->rowCount());
    QTableWidgetItem* id_item = new QTableWidgetItem(QString::number(idx));
    vert_table->setItem(vert_table->rowCount() - 1, 0, id_item);
    QTableWidgetItem* rel_item = new QTableWidgetItem(QString::number(rel));
    vert_table->setItem(vert_table->rowCount() - 1, 1, rel_item);
    modalWindow->close();

    emit updateLayouterSig();
}

void MainWindow::editVertex(int idx, double rel) {
    graph.resetRel(idx, rel);
    for (int i = 0; i < vert_table->rowCount(); ++i) {
        if (vert_table->item(i, 0)->text().toInt() == idx)
            vert_table->setItem(i, 1,
                    new QTableWidgetItem(QString::number(rel)));
    }
    modalWindow->close();
    emit updateLayouterSig();
}

void MainWindow::addEdge(unsigned long v1, unsigned long v2,
                         double, double rel) {
    if (!graph.addEdge(v1, v2, rel)) {
        QMessageBox* error = new QMessageBox(modalWindow);
        error->critical(modalWindow, "Ошибка!",
            "Либо такое ребро в графе уже существует, либо ребро является петлей");
        return;
    }
    edge_table->insertRow(edge_table->rowCount());
    edge_table->setItem(edge_table->rowCount() - 1, 0,
        new QTableWidgetItem(QString::number(v1)));
    edge_table->setItem(edge_table->rowCount() - 1, 1,
        new QTableWidgetItem(QString::number(v2)));
    edge_table->setItem(edge_table->rowCount() - 1, 2,
        new QTableWidgetItem(QString::number(rel)));
    modalWindow->close();
    emit updateLayouterSig();
}

void MainWindow::editEdge(ulong v1, ulong v2, double old_rel, double rel) {
    if (!graph.editEdge(v1, v2, old_rel, rel)) {
        QMessageBox* error = new QMessageBox(modalWindow);
        error->critical(modalWindow, "Ошибка",
                        "Такого ребра в графе не существует");
        return;
    }
    int rowCount = edge_table->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        ulong kv1 = edge_table->item(i, 0)->text().toULong();
        ulong kv2 = edge_table->item(i, 1)->text().toULong();
        double trel = edge_table->item(i, 2)->text().toDouble();
        if (v1 == kv1 && v2 == kv2 &&
                std::abs(trel - old_rel) < 1E-8) {
            edge_table->item(i, 2)->setText(QString::number(rel));
            break;
        }
    }
    modalWindow->close();
    emit updateLayouterSig();
}

void MainWindow::updateLayouter() {
    delete layouter;
    layouter = new GraphLayouter(graph);
    layouter->processLayout();
    redrawScene();
}

void MainWindow::redrawScene() {
    scene->clear();
    QPen pen;
    pen.setColor(Qt::blue);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    const std::map<size_t, Point2D>& glayout = layouter->getLayout();
    std::vector<RGEdge> edges = graph.getEdgeList();
    pen.setColor(Qt::black);

    std::set<std::pair<ulong, ulong> > sel_edge;
    QItemSelectionModel* model_edge = edge_table->selectionModel();
    QModelIndexList sel_rows_edge = model_edge->selectedRows();
    foreach (QModelIndex idx, sel_rows_edge) {
        ulong v1 = edge_table->item(idx.row(), 0)->text().toULong();
        ulong v2 = edge_table->item(idx.row(), 1)->text().toULong();
        sel_edge.insert(std::make_pair(v1, v2));
    }

    Point2D top_left(1E+9, 1E+9), buttom_right(-1E+9, -1E+9);
    buttom_right.x = buttom_right.y = -1E+9;
    for (std::map<size_t, Point2D>::const_iterator
            i = glayout.begin(); i != glayout.end(); ++i) {
        const Point2D& pt = i->second;
        if (pt.x < top_left.x)
            top_left.x = pt.x;
        if (pt.y < top_left.y)
            top_left.y = pt.y;
        if (pt.x > buttom_right.x)
            buttom_right.x = pt.x;
        if (pt.y > buttom_right.y)
            buttom_right.y = pt.y;
    }
    top_left.x -= 5.0;
    top_left.y -= 5.0;
    buttom_right.x += 5.0;
    buttom_right.y += 5.0;
    canvas->invalidateScene();
    canvas->setSceneRect(top_left.x, top_left.y,
                buttom_right.x, buttom_right.y);

    for (size_t i = 0; i < edges.size(); ++i) {
        const RGEdge& edge = edges[i];
        size_t v1 = edge.v1;
        size_t v2 = edge.v2;
        const Point2D& p1 = glayout.find(v1)->second;
        const Point2D& p2 = glayout.find(v2)->second;
        if (sel_edge.find(std::make_pair(v1, v2)) == sel_edge.end())
            pen.setColor(Qt::black);
        else
            pen.setColor(Qt::green);
        QGraphicsLineItem* item =
                new QGraphicsLineItem(p1.x, p1.y, p2.x, p2.y);
        item->setPen(pen);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        scene->addItem(item);
    }
    brush.setColor(Qt::blue);
    pen.setColor(Qt::blue);
    std::set<ulong> sel_vert;
    QItemSelectionModel* model_vert = vert_table->selectionModel();
    QModelIndexList sel_rows_vert = model_vert->selectedRows();
    foreach (QModelIndex idx, sel_rows_vert) {
        ulong v1 = vert_table->item(idx.row(), 0)->text().toULong();
        sel_vert.insert(v1);
    }
    for (std::map<size_t, Point2D>::const_iterator
            i = glayout.begin(); i != glayout.end(); ++i) {
        if (sel_vert.find(i->first) != sel_vert.end()) {
            brush.setColor(Qt::green);
            pen.setColor(Qt::green);
        } else {
            brush.setColor(Qt::blue);
            pen.setColor(Qt::blue);
        }
        const Point2D& pt = i->second;
        QGraphicsRectItem* item = new QGraphicsRectItem(pt.x - 5.0, pt.y - 5.0,
                                                    10.0, 10.0);
        item->setBrush(brush);
        item->setPen(pen);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        scene->addItem(item);
    }

    pen.setColor(Qt::black);
    for (std::map<size_t, Point2D>::const_iterator
            i = glayout.begin(); i != glayout.end(); ++i) {
        const Point2D& pt = i->second;
        QGraphicsTextItem* t_item = new QGraphicsTextItem();
        t_item->setPos(pt.x - 6.0, pt.y + 5.0);
        t_item->setPlainText(QString::number(i->first));
        t_item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        scene->addItem(t_item);
    }

    canvas->invalidateScene();
}

void MainWindow::vertexListChanged() {
    const std::vector<std::pair<size_t, double> >& vert =
                                        graph.getVertexList();
    from_list->clear();
    to_list->clear();
    for (size_t i = 0; i < vert.size(); ++i) {
        const std::pair<size_t, double> v = vert[i];
        from_list->addItem(QString::number(v.first));
        to_list->addItem(QString::number(v.first));
    }
}

void MainWindow::estimateButtonClicked() {
    reset_button->setEnabled(true);
    calc_button->setEnabled(false);
    from_list->setEnabled(false);
    to_list->setEnabled(false);
    QThread* est_thread = new QThread();
    GraphEstimator* estimator = new GraphEstimator();
    const std::vector<RGEdge>& edges = graph.getEdgeList();
    if (edges.size() == 0) {
        QMessageBox error(this);
        error.critical(this, "Ошибка", "Граф не содержит ребер");
        delete estimator;
        delete est_thread;
        resetButtonClicked();
        return;
    }
    estimator->loadGraph(edges, graph.getVertexList());
    int from = from_list->currentIndex();
    int to = to_list->currentIndex();
    if (from == -1) {
        QMessageBox error(this);
        error.critical(this, "Ошибка", "Вершина-исток не выбрана");
        delete estimator;
        delete est_thread;
        resetButtonClicked();
        return;
    }
    if (to == -1) {
        QMessageBox error(this);
        error.critical(this, "Ошибка", "Вершина-сток не выбрана");
        delete estimator;
        delete est_thread;
        resetButtonClicked();
        return;
    }
    if (from == to) {
        QMessageBox error(this);
        error.critical(this, "Ошибка", "Вершина-исток и вершина-сток должны быть различны");
        delete estimator;
        delete est_thread;
        resetButtonClicked();
        return;
    }
    from = from_list->currentText().toInt();
    to = to_list->currentText().toInt();
    estimator->setSrcDst(static_cast<size_t>(from),
                         static_cast<size_t>(to));
    int tr_amount = transit_spinbox->value();
    estimator->setTransitNodeAmount(static_cast<size_t>(tr_amount));
    estimator->moveToThread(est_thread);
    connect(est_thread, SIGNAL(started()), estimator, SLOT(solve()));
    connect(est_thread, SIGNAL(finished()), SLOT(estimated()));
    connect(estimator, SIGNAL(notifyResult(double)),
                       SLOT(updateResult(double)));
    worktime_lbl->setText("Прошло");
    time_elapsed_ = 0;
    work_timer_->start(1000);
    QTimer::singleShot(1, est_thread, SLOT(start()));
}

void MainWindow::resetButtonClicked() {
    reset_button->setEnabled(false);
    calc_button->setEnabled(true);
    from_list->setEnabled(true);
    to_list->setEnabled(true);
    res_lbl->setText("");
    restime_lbl->setText("");
}

void MainWindow::updateSlider() {
    size_t vert_amount = graph.getVertexList().size();
    if (vert_amount == 0) {
        transit_spinbox->setEnabled(false);
        return;
    }
    transit_spinbox->setEnabled(true);
    transit_spinbox->setMinimum(0);
    transit_spinbox->setMaximum(vert_amount);
    transit_spinbox->setValue(0);
}

void MainWindow::estimated() {

}

void MainWindow::updateResult(double p) {
    res_lbl->setText(QString::number(p));
    time_elapsed_ += 1000 - work_timer_->remainingTime();
    restime_lbl->setText(QString::number(time_elapsed_ / 1000.0) + "c");
    worktime_lbl->setText("Время работы");
    work_timer_->stop();
}

void MainWindow::onTimeout() {
    time_elapsed_ += 1000;
    restime_lbl->setText(QString::number(time_elapsed_ / 1000) + "с");
}

void MainWindow::open() {
    QString file_name = QFileDialog::getOpenFileName(this,
                                "Открыть", "c:/", "*.xml");
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox error(this);
        error.critical(this, "Ошибка", "Невозможно открыть файл");
        return;
    }
    if (!graph.import(&file)) {
        QMessageBox error(this);
        error.critical(this, "Ошибка", "Ошибка при чтении Файла");
    }
    std::vector<std::pair<size_t, double> > verts = graph.getVertexList();
    for (size_t i = 0; i < verts.size(); ++i) {
        QString id = QString::number(verts[i].first);
        QString rel = QString::number(verts[i].second);
        int count = vert_table->rowCount();
        vert_table->insertRow(count);
        vert_table->setItem(count, 0, new QTableWidgetItem(id));
        vert_table->setItem(count, 1, new QTableWidgetItem(rel));
    }
    std::vector<RGEdge> edges = graph.getEdgeList();
    for (size_t i = 0; i < edges.size(); ++i) {
        QString v1 = QString::number(edges[i].v1);
        QString v2 = QString::number(edges[i].v2);
        QString rel = QString::number(edges[i].rel);
        int count = edge_table->rowCount();
        edge_table->insertRow(count);
        edge_table->setItem(count, 0, new QTableWidgetItem(v1));
        edge_table->setItem(count, 1, new QTableWidgetItem(v2));
        edge_table->setItem(count, 2, new QTableWidgetItem(rel));
    }
    emit updateLayouterSig();
    emit vertexListChangedSig();
}

void MainWindow::save() {
    QString file_name = QFileDialog::getSaveFileName(this,
                    "Сохранить", "c:/", "*.xml");
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox error(modalWindow);
        error.critical(modalWindow, "Ошибка", "Невозможно открыть файл для записи");
        return;
    }
    graph.save(&file);
    file.close();
}

void MainWindow::contextMenuEvent(QContextMenuEvent */*event*/) {

}

void MainWindow::createMenus() {
    file_menu = menu_bar->addMenu("Файл");
    open_act = file_menu->addAction("Открыть");
    connect(open_act, SIGNAL(triggered()), SLOT(open()));
    save_act = file_menu->addAction("Сохранить");
    connect(save_act, SIGNAL(triggered()), SLOT(save()));
    exit_act = file_menu->addAction("Выйти");
    connect(exit_act, SIGNAL(triggered()), SLOT(close()));
}

void MainWindow::initComponents() {
}
