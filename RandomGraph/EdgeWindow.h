#ifndef EDGEWINDOW_H
#define EDGEWINDOW_H

#include <vector>

#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>

enum EdgeWindowType {
    EW_ADD,
    EW_EDIT
};

class EdgeWindow : public QWidget {
    Q_OBJECT
 public:
    EdgeWindow(QWidget* parent, EdgeWindowType type);
    void loadVertexList(const std::vector<size_t>& verts);
    void setEditableEdge(const size_t& v1, const size_t& v2,
                         const double& rel);

 signals:
    void addEdgeSig(unsigned long v1, unsigned long v2, double rel, double upd_rel);

 private slots:
    void okButtonClicked();

 private:
    QComboBox* v1_combo, *v2_combo;
    double old_rel;
    QDoubleSpinBox* rel_spin;
};

#endif // EDGEWINDOW_H
