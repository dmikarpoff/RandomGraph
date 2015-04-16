#ifndef VERTEXWINDOW_H
#define VERTEXWINDOW_H

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

enum VertexWindowType {
    EDIT,
    ADD
};

class VertexWindow : public QWidget {
    Q_OBJECT
 public:
    VertexWindow(QWidget* parent, VertexWindowType type);
    void setIndex(int idx) {
        vert_edit->setValue(idx);
    }

    void setReliability(double rel) {
        rel_edit->setValue(rel);
    }

 signals:
    void addVertexSig(int id, double rel);

 private slots:
    void cancelButtonClicked();
    void addButtonClicked();

 private:
    QSpinBox* vert_edit;
    QDoubleSpinBox* rel_edit;
};

#endif // VERTEXWINDOW_H
