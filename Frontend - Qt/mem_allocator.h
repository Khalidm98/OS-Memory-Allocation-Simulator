#ifndef MEM_ALLOCATOR_H
#define MEM_ALLOCATOR_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QMap>
#include <QVector>
#include <QString>
#include "Memory.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Mem_Allocator; }
QT_END_NAMESPACE

class Mem_Allocator : public QMainWindow
{
    Q_OBJECT

private:
    Ui::Mem_Allocator *ui;
    Memory mem;
    QGraphicsScene *mem_scene;
    QGraphicsScene *seg_scene;

public:
    Mem_Allocator(QWidget *parent = nullptr);
    ~Mem_Allocator();

private slots:
    void on_Update_clicked();

    void on_Clear_clicked();

    void draw_mem(QMap<unsigned long, QString> memory);

    void draw_seg(QVector< QMap<QString, unsigned long> > process, QMap<unsigned long, QString> memory);

    void draw_cell(unsigned long x, unsigned long y, QString str);

};
#endif // MEM_ALLOCATOR_H
