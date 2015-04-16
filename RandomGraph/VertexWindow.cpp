#include <QDoubleValidator>
#include <QGridLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>

#include "VertexWindow.h"

VertexWindow::VertexWindow(QWidget *parent, VertexWindowType type):
        QWidget(parent, Qt::Window) {
    setFixedSize(300, 100);
    if (type == ADD)
        setWindowTitle("Добавление вершины");
    else
        setWindowTitle("Редактирование вершины");
    setWindowModality(Qt::ApplicationModal);
    QGridLayout* grid_layout = new QGridLayout();
    setLayout(grid_layout);
    QLabel* vert_lbl = new QLabel(this);
    vert_lbl->setText("Вершина");
    vert_lbl->setAlignment(Qt::AlignCenter);
    grid_layout->addWidget(vert_lbl, 0, 0);
    vert_edit = new QSpinBox(this);
    vert_edit->setMinimum(1);
    vert_edit->setMaximum(100000);
    vert_edit->setAlignment(Qt::AlignCenter);
    if (type == EDIT)
        vert_edit->setEnabled(false);
    grid_layout->addWidget(vert_edit, 1, 0);
    QLabel* rel_lbl = new QLabel(this);
    rel_lbl->setText("Надежность");
    rel_lbl->setAlignment(Qt::AlignCenter);
    grid_layout->addWidget(rel_lbl, 0, 1);
    rel_edit = new QDoubleSpinBox(this);
    rel_edit->setMinimum(0.0);
    rel_edit->setMaximum(1.0);
    rel_edit->setDecimals(6);
    rel_edit->setSingleStep(0.001);
    rel_edit->setAlignment(Qt::AlignCenter);
    grid_layout->addWidget(rel_edit, 1, 1);

    QPushButton* ok_button = new QPushButton(this);
    if (type == ADD)
        ok_button->setText("Добавить");
    else
        ok_button->setText("Готово");
    grid_layout->addWidget(ok_button, 2, 0);

    QPushButton* cancel_button = new QPushButton(this);
    cancel_button->setText("Отмена");
    grid_layout->addWidget(cancel_button, 2, 1);

    connect(ok_button, SIGNAL(clicked()),
                       SLOT(addButtonClicked()));
    connect(cancel_button, SIGNAL(clicked()),
                           SLOT(cancelButtonClicked()));
}

void VertexWindow::addButtonClicked() {
    int id = vert_edit->value();
    double rel = rel_edit->value();
    emit addVertexSig(id, rel);
}

void VertexWindow::cancelButtonClicked() {
    close();
}
