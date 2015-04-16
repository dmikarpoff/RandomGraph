#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QTableWidget>

#include "ui_mainwindow.h"

#include "graphlayouter.h"
#include "RandomGraph.h"

class MainWindow : public QWidget,
                   public Ui_MainWindow {
    Q_OBJECT
 public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

 signals:
    void updateLayouterSig();
    void vertexListChangedSig();

 private slots:
    void onAddVertexClicked();
    void onEditVertexClicked();
    void onRemoveVertexClicked();
    void onAddEdgeClicked();
    void onEditEdgeClicked();
    void onRemoveEdgeClicked();
    void addVertex(int idx, double rel);
    void editVertex(int idx, double rel);
    void addEdge(unsigned long v1, unsigned long v2, double old_rel, double rel);
    void editEdge(ulong v1, ulong v2, double old_rel, double rel);
    void updateLayouter();
    void redrawScene();
    void vertexListChanged();
    void estimateButtonClicked();
    void resetButtonClicked();
    void updateSlider();
    void estimated();
    void updateResult(double p);
    void onTimeout();

    void open();
    void save();

 protected:
    void contextMenuEvent(QContextMenuEvent *event);

 private:
    void createMenus();
    static const float FRACT_DISPLAY;

    void initComponents();

    QGraphicsScene* scene;
    QWidget* modalWindow;
    QMenuBar* menu_bar;
    QMenu* file_menu;
    QAction *open_act, *save_act, *exit_act;
    RandomGraph graph;
    GraphLayouter* layouter;
    int time_elapsed_;
    QTimer* work_timer_;
};

#endif // MAINWINDOW_H
