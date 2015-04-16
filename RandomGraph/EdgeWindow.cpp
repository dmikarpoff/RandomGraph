#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

#include "EdgeWindow.h"

EdgeWindow::EdgeWindow(QWidget* parent, EdgeWindowType type):
        QWidget(parent, Qt::Window) {
    setFixedSize(350, 130);
    setWindowModality(Qt::ApplicationModal);
    if (type == EW_ADD)
        setWindowTitle("Добавление ребра");
    else
        setWindowTitle("Редактирование ребра");
    QGridLayout* layout = new QGridLayout;
    setLayout(layout);
    QWidget* edit_widget = new QWidget(this);
    QGridLayout* edit_layout = new QGridLayout();
    layout->addWidget(edit_widget, 0, 0);
    edit_widget->setLayout(edit_layout);
    QLabel* v1_lbl = new QLabel(edit_widget);
    v1_lbl->setText("Вершина");
    v1_lbl->setAlignment(Qt::AlignCenter);
    edit_layout->addWidget(v1_lbl, 0, 0);
    QLabel* v2_lbl = new QLabel(edit_widget);
    v2_lbl->setText("Вершина");
    v2_lbl->setAlignment(Qt::AlignCenter);
    edit_layout->addWidget(v2_lbl, 0, 1);
    QLabel* rel_lbl = new QLabel(edit_widget);
    rel_lbl->setText("Надежность");
    rel_lbl->setAlignment(Qt::AlignCenter);
    edit_layout->addWidget(rel_lbl, 0, 2);
    v1_combo = new QComboBox(edit_widget);
    if (type == EW_EDIT)
        v1_combo->setEditable(false);
    edit_layout->addWidget(v1_combo, 1, 0);
    v2_combo = new QComboBox(edit_widget);
    if (type == EW_EDIT)
        v2_combo->setEditable(false);
    edit_layout->addWidget(v2_combo, 1, 1);
    rel_spin = new QDoubleSpinBox(edit_widget);
    rel_spin->setRange(0, 1);
    rel_spin->setSingleStep(0.001);
    rel_spin->setDecimals(6);
    rel_spin->setAlignment(Qt::AlignCenter);
    edit_layout->addWidget(rel_spin, 1, 2);

    QWidget* control_widget = new QWidget(this);
    QGridLayout* control_layout = new QGridLayout();
    control_widget->setLayout(control_layout);
    layout->addWidget(control_widget, 1, 0);
    QPushButton* ok_button = new QPushButton(control_widget);
    if (type == EW_ADD)
        ok_button->setText("Добавить");
    else
        ok_button->setText("Готово");
    control_layout->addWidget(ok_button, 0, 0);
    QPushButton* cancel_button = new QPushButton(control_widget);
    cancel_button->setText("Отмена");
    control_layout->addWidget(cancel_button, 0, 1);

    connect(ok_button, SIGNAL(clicked()), SLOT(okButtonClicked()));
    connect(cancel_button, SIGNAL(clicked()), SLOT(close()));
}

void EdgeWindow::loadVertexList(const std::vector<size_t> &verts) {
    for (size_t i = 0; i < verts.size(); ++i) {
        v1_combo->addItem(QString::number(verts[i]));
        v2_combo->addItem(QString::number(verts[i]));
    }
}

void EdgeWindow::setEditableEdge(const size_t &v1, const size_t &v2,
                                 const double &rel) {
    v1_combo->clear();
    v2_combo->clear();
    v1_combo->addItem(QString::number(v1));
    v1_combo->setCurrentIndex(0);
    v2_combo->addItem(QString::number(v2));
    v2_combo->setCurrentIndex(0);
    v1_combo->setEnabled(false);
    v2_combo->setEnabled(false);
    rel_spin->setValue(rel);
    old_rel = rel;
}

void EdgeWindow::okButtonClicked() {
    unsigned long v1 = v1_combo->currentText().toInt();
    unsigned long v2 = v2_combo->currentText().toInt();
    double rel = rel_spin->value();
    emit addEdgeSig(v1, v2, old_rel, rel);
}
