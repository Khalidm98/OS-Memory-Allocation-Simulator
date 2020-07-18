#include "mem_allocator.h"
#include "ui_mem_allocator.h"

#include <QGraphicsItem>
#include <QMessageBox>

Mem_Allocator::Mem_Allocator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Mem_Allocator)
{
    ui->setupUi(this);

    setWindowTitle("Memory Allocator");
    ui->InitWidget->setVisible(false);
    ui->AllocWidget->setVisible(false);
    ui->DeallocWidget->setVisible(false);

    mem_scene = new QGraphicsScene(this);
    ui->MemoryView->setScene(mem_scene);
    seg_scene = new QGraphicsScene(this);
    ui->SegmentsView->setScene(seg_scene);
}

Mem_Allocator::~Mem_Allocator()
{
    delete ui;
}


void Mem_Allocator::on_Update_clicked()
{
    if (ui->Initialize->isChecked()){
        unsigned long mem_size = (ui->MemSize->text()).toDouble();
        QStringList list = (ui->HolesAddresses->text()).split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QVector<unsigned long> holes_addresses(list.size());
        for (int i = 0; i < list.size(); ++i)
            holes_addresses[i] = list[i].toDouble();
        list = (ui->HolesSizes->text()).split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QVector<unsigned long> holes_sizes(list.size());
        for (int i = 0; i < list.size(); ++i)
            holes_sizes[i] = list[i].toDouble();

        mem.clear();
        mem.init(mem_size, holes_addresses, holes_sizes);
        draw_mem(mem.get_memory());
        draw_seg(mem.get_process(), mem.get_memory());
        ui->MemSize->clear();
        ui->HolesAddresses->clear();
        ui->HolesSizes->clear();
    }

    else if (ui->Allocate->isChecked()){
        QStringList list = (ui->SegNames->text()).split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QVector<QString> seg_names(list.size());
        for (int i = 0; i < list.size(); ++i)
            seg_names[i] = list[i];
        list = (ui->SegSizes->text()).split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QVector<unsigned long> seg_sizes(list.size());
        for (int i = 0; i < list.size(); ++i)
            seg_sizes[i] = list[i].toDouble();

        if ((ui->BestFit->isChecked() && mem.best(seg_names, seg_sizes)) ||
            (ui->FirstFit->isChecked() && mem.first(seg_names, seg_sizes))){
                draw_mem(mem.get_memory());
                draw_seg(mem.get_process(), mem.get_memory());
                ui->SegNames->clear();
                ui->SegSizes->clear();
                ui->BestFit->setAutoExclusive(false);
                ui->FirstFit->setAutoExclusive(false);
                ui->BestFit->setChecked(false);
                ui->FirstFit->setChecked(false);
                ui->BestFit->setAutoExclusive(true);
                ui->FirstFit->setAutoExclusive(true);
        }
        else {
            QMessageBox* error = new QMessageBox;
            error->warning(this, "Error",
                           "Cannot allocate the process.\nNo sufficient space!",
                           QMessageBox::NoButton, QMessageBox::NoButton);
        }
    }

    else if (ui->Deallocate->isChecked()){
        unsigned short num = (ui->DeallocNum->text()).toDouble();
        if ((ui->PreAllocated->isChecked() && mem.pre_free(num)) ||
            (ui->UserAllocated->isChecked() && mem.user_free(num))){
                draw_mem(mem.get_memory());
                draw_seg(mem.get_process(), mem.get_memory());
                ui->DeallocNum->clear();
                ui->PreAllocated->setAutoExclusive(false);
                ui->UserAllocated->setAutoExclusive(false);
                ui->PreAllocated->setChecked(false);
                ui->UserAllocated->setChecked(false);
                ui->PreAllocated->setAutoExclusive(true);
                ui->UserAllocated->setAutoExclusive(true);
        }
        else if (ui->PreAllocated->isChecked()){
            QMessageBox* error = new QMessageBox;
            error->warning(this, "Error",
                           "Cannot deallocate the segment.\n"
                           "Pre-allocated segment no. " + QString::number(num) + " doesn't exist!",
                           QMessageBox::NoButton, QMessageBox::NoButton);
        }
        else if (ui->UserAllocated->isChecked()){
            QMessageBox* error = new QMessageBox;
            error->warning(this, "Error",
                           "Cannot deallocate the process.\n"
                           "Process " + QString::number(num) + " doesn't exist!",
                           QMessageBox::NoButton, QMessageBox::NoButton);
        }
    }
}

void Mem_Allocator::on_Clear_clicked()
{
    mem.clear();
    mem_scene->clear();
    seg_scene->clear();

    ui->SegNames->clear();
    ui->SegSizes->clear();
    ui->BestFit->setAutoExclusive(false);
    ui->FirstFit->setAutoExclusive(false);
    ui->BestFit->setChecked(false);
    ui->FirstFit->setChecked(false);
    ui->BestFit->setAutoExclusive(true);
    ui->FirstFit->setAutoExclusive(true);

    ui->DeallocNum->clear();
    ui->PreAllocated->setAutoExclusive(false);
    ui->UserAllocated->setAutoExclusive(false);
    ui->PreAllocated->setChecked(false);
    ui->UserAllocated->setChecked(false);
    ui->PreAllocated->setAutoExclusive(true);
    ui->UserAllocated->setAutoExclusive(true);
}

void Mem_Allocator::draw_mem(QMap<unsigned long, QString> memory)
{
    QMap<unsigned long, QString>::iterator it = memory.begin();
    QMap<unsigned long, QString>::iterator next = memory.begin();
    next++;
    QMap<unsigned long, QString>::iterator mem_size = memory.end();
    mem_size--;

    double scale = 750.0 / mem_size.key();
    QGraphicsTextItem *text;
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(1);

    unsigned short digits = 0;
    for (int i = 1; mem_size.key() / i != 0; i *= 10)
        ++digits;
    mem_scene->clear();

    while (it != mem_size){
        if (it.value().isEmpty())
            mem_scene->addRect(0, it.key() * scale, 400 - 15 * digits, (next.key() - it.key()) * scale, outlinePen, Qt::white);

        else {
            mem_scene->addRect(0, it.key() * scale, 400 - 15 * digits, (next.key() - it.key()) * scale, outlinePen, Qt::gray);
            text = mem_scene->addText(it.value(), QFont("Arial", 12));
            text->setX(100);
            text->setY((it.key() + next.key()) / 2 * scale - 15);
        }

        text = mem_scene->addText(QString::number(it.key()), QFont("Arial", 12));
        text->setX(-15 * digits);
        text->setY(it.key() * scale - 15);
        it++;
        next++;
    }
    text = mem_scene->addText(QString::number(mem_size.key()), QFont("Arial", 12));
    text->setX(-15 * digits);
    text->setY(mem_size.key() * scale - 15);
}

void Mem_Allocator::draw_seg(QVector< QMap<QString, unsigned long> > process, QMap<unsigned long, QString> memory)
{
    QGraphicsTextItem *text;
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(1);
    unsigned long y = 0;
    seg_scene->clear();

    for (int i = 1; i < process.size(); i++){
        if(!process[i].empty()){
            seg_scene->addRect(0, y, 400, 30, outlinePen, Qt::gray);
            text = seg_scene->addText("Process " + QString::number(i), QFont("Arial", 12));
            text->setX(150);
            text->setY(y);
            y += 30;

            draw_cell(0, y, "Number");
            draw_cell(100, y, "Name");
            draw_cell(200, y, "Base");
            draw_cell(300, y, "Size");
            y += 30;

            unsigned short num = 0;
            auto it = process[i].begin();
            while (it != process[i].end())
            {
                draw_cell(0, y, QString::number(num));
                draw_cell(100, y, it.key());
                draw_cell(200, y, QString::number(it.value()));
                auto next = memory.find(it.value());
                next++;
                draw_cell(300, y, QString::number(next.key() - it.value()));
                it++;
                num++;
                y += 30;
            }
            y += 25;
        }
    }
}

void Mem_Allocator::draw_cell(unsigned long x, unsigned long y, QString str)
{
    QGraphicsTextItem *text;
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(1);
    seg_scene->addRect(x, y, 100, 30, outlinePen, Qt::gray);
    text = seg_scene->addText(str, QFont("Arial", 12));
    text->setX(x+10);
    text->setY(y);
}
