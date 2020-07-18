#ifndef MEMORY_H
#define MEMORY_H

#include <QMap>
#include <QVector>
#include <QString>

class Memory {
private:
    QMap<unsigned long, QString> memory;				// memory segments and info
    QMap<unsigned long, unsigned long> free;			// base addresses and sizes of free holes
    QVector< QMap<QString, unsigned long> > process;	// process segments info
	// memory<base address, segment info>
	// free<base address, hole size>
	// process[1] = map<segment name, base address>
	// process[0] is reserved for initially allocated segments

public:
    Memory();
    void init(unsigned long mem_size, QVector<unsigned long> &base, QVector<unsigned long> &size);
    bool best(QVector<QString> &name, QVector<unsigned long> &size);
    bool first(QVector<QString> &name, QVector<unsigned long> &size);
	bool pre_free(unsigned short num);
	bool user_free(unsigned short num);
    void clear();
    QMap<unsigned long, QString> get_memory();
    QVector< QMap<QString, unsigned long> > get_process();
};

#endif // MEMORY_H
