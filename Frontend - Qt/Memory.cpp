#include "Memory.h"

Memory::Memory(){}

void Memory::init(unsigned long mem_size, QVector<unsigned long> &base, QVector<unsigned long> &size){
	memory.clear();
	free.clear();
	process.clear();

	unsigned short N = size.size();
    process.push_back(QMap<QString, unsigned long>());

	// add holes
	for (unsigned short i = 0; i < N; ++i){
		free[base[i]] = size[i];
		memory[base[i]] = "";
	}

	// if there is two adjacent holes
	auto it = free.begin();
	auto next = it;
	++next;
	while (next != free.end()){
        if (it.key() + it.value() == next.key()){
            it.value() += next.value();
            memory.remove(next.key());
			free.erase(next);
			next = it;
		}
		else
			++it;
		++next;
	}

	// if there is a free hole at address 0
    if (free.begin().key() == 0)
		memory[0] = "";
	else {
		memory[0] = "Preallocated 0";
		process[0]["0"] = 0;
	}
	memory[mem_size] = "";

	// determine preallocated segments
	for (it = free.begin(); it != free.end(); ++it){
        memory[it.key() + it.value()] = "Preallocated " + QString::number(process[0].size());
        process[0][QString::number(process[0].size())] = it.key() + it.value();
	}

	// correct the last hole
    if (!memory[mem_size].isEmpty()){
		memory[mem_size] = "";
		auto last = process[0].end();
		--last;
		process[0].erase(last);
	}
}

bool Memory::best(QVector<QString> &name, QVector<unsigned long> &size){
	unsigned short N = size.size();
    QVector<unsigned long> base;
    QMap<unsigned long, unsigned long> FREE(free);

	// best fit
	for (unsigned short i = 0; i < N; ++i){
		// determine first fit
		unsigned long min;
		auto it = FREE.begin();
		while (it != FREE.end()){
            if (size[i] <= it.value()){
                min = it.key();
				break;
			}
			++it;
		}

		// check if no block fits the segement
		if (it == FREE.end())
			return false;

		// determine best fit
		++it;
		while (it != FREE.end()){
            if (size[i] <= it.value() && it.value() < FREE[min])
                min = it.key();
			++it;
		}

		// update FREE
		base.push_back(min);
		if (size[i] < FREE[min])
			FREE[min + size[i]] = FREE[min] - size[i];
        FREE.remove(min);
	}

	// update free
	free = FREE;

	// update memory
	for (unsigned short i = 0; i < N; ++i){
        memory[base[i]] = "P" + QString::number(process.size()) + ", " + name[i];
		if (memory.find(base[i] + size[i]) == memory.end())
			memory[base[i] + size[i]] = "";
	}

	// update process
    process.push_back(QMap<QString, unsigned long>());
	for (unsigned short i = 0; i < N; ++i)
		process.back()[name[i]] = base[i];
	return true;
}

bool Memory::first(QVector<QString> &name, QVector<unsigned long> &size){
	unsigned short N = size.size();
    QMap<unsigned long, unsigned long> FREE(free);
    QVector<unsigned long> base;

	// first fit
	for (unsigned short i = 0; i < N; ++i){
		for (auto it = FREE.begin(); it != FREE.end(); ++it){
            if (size[i] <= it.value()){
                base.push_back(it.key());
                if (size[i] < it.value())
                    FREE[it.key() + size[i]] = it.value() - size[i];
				FREE.erase(it);
				break;
			}
		}
		if (base.size() == i)
			return false;
	}

	// update free
	free = FREE;

	// update memory
	for (unsigned short i = 0; i < N; ++i){
        memory[base[i]] = "P" + QString::number(process.size()) + ", " + name[i];
		if (memory.find(base[i] + size[i]) == memory.end())
			memory[base[i] + size[i]] = "";
	}

	// update process
    process.push_back(QMap<QString, unsigned long>());
	for (unsigned short i = 0; i < N; ++i)
		process.back()[name[i]] = base[i];
	return true;
}

bool Memory::pre_free(unsigned short num){
    auto it = process[0].find(QString::number(num));
	if (it == process[0].end())
		return false;

	auto last = memory.end();				// iterator to last memory block
	--last;
    unsigned long mem_size = last.key();
	--last;

	// segment is allocated at address 0
    if(it.value() == 0){
		auto next = memory.begin();
		++next;

		// if next segment is free
        if((next.value()).isEmpty()){
            free[0] = next.key() + free[next.key()];
            free.remove(next.key());
			memory.erase(next);
		}
		// if next segment is allocated
		else
            free[0] = next.key();

		memory[0] = "";
	}

	// segment is allocated at last memory block
    else if (it.value() == last.key()){
		auto prev = last;
		--prev;

		// if previous segment is free
        if ((prev.value()).isEmpty()){
            free[prev.key()] += mem_size - last.key();
			memory.erase(last);
		}
		// if previous segment is allocated
		else {
            free[last.key()] = mem_size - last.key();
            last.value() = "";
		}
	}

	// segment is allocated in an inner block
	else {
        auto prev = memory.find(it.value());
        auto next = memory.find(it.value());
		--prev;
		++next;

		// if both prev and next are free
        if ((prev.value()).isEmpty() && (next.value()).isEmpty()){
            free[prev.key()] += next.key() - it.value() + free[next.key()];
            free.remove(next.key());
            memory.remove(it.value());
			memory.erase(next);
		}

		// if only next is free
        else if ((next.value()).isEmpty()){
            free[it.value()] = next.key() - it.value() + free[next.key()];
            free.remove(next.key());
			memory.erase(next);
            memory[it.value()] = "";
		}

		// if only prev is free
        else if ((prev.value()).isEmpty()){
            free[prev.key()] += next.key() - it.value();
            memory.remove(it.value());
		}

		// if both prev and next are allocated
		else {
            free[it.value()] = next.key() - it.value();
            memory[it.value()] = "";
		}
	}
	process[0].erase(it);
	return true;
}

bool Memory::user_free(unsigned short num){
    if (num == 0 || num >= process.size() || process[num].isEmpty())
		return false;

	for (auto it = process[num].begin(); it != process[num].end(); ++it){
		auto last = memory.end();				// iterator to last memory block
		--last;
        unsigned long mem_size = last.key();
		--last;

		// segment is allocated at address 0
        if (it.value() == 0){
			auto next = memory.begin();
			++next;

			// if next segment is free
            if ((next.value()).isEmpty()){
                free[0] = next.key() + free[next.key()];
                free.remove(next.key());
				memory.erase(next);
			}
			// if next segment is allocated
			else
                free[0] = next.key();

			memory[0] = "";
		}

		// segment is allocated at last memory block
        else if (it.value() == last.key()){
			auto prev = last;
			--prev;

			// if previous segment is free
            if ((prev.value()).isEmpty()){
                free[prev.key()] += mem_size - last.key();
				memory.erase(last);
			}
			// if previous segment is allocated
			else {
                free[last.key()] = mem_size - last.key();
                last.value() = "";
			}
		}

		// segment is allocated in an inner block
		else {
            auto prev = memory.find(it.value());
            auto next = memory.find(it.value());
			--prev;
			++next;

			// if both prev and next are free
            if ((prev.value()).isEmpty() && (next.value()).isEmpty()){
                free[prev.key()] += next.key() - it.value() + free[next.key()];
                free.remove(next.key());
                memory.remove(it.value());
				memory.erase(next);
			}

			// if only next is free
            else if ((next.value()).isEmpty()){
                free[it.value()] = next.key() - it.value() + free[next.key()];
                free.remove(next.key());
				memory.erase(next);
                memory[it.value()] = "";
			}

			// if only prev is free
            else if ((prev.value()).isEmpty()){
                free[prev.key()] += next.key() - it.value();
                memory.remove(it.value());
			}

			// if both prev and next are allocated
			else {
                free[it.value()] = next.key() - it.value();
                memory[it.value()] = "";
			}
		}
	}
	process[num].clear();
	return true;
}

void Memory::clear(){
    memory.clear();
    free.clear();
    process.clear();
}

QMap<unsigned long, QString> Memory::get_memory(){
    return memory;
}

QVector< QMap<QString, unsigned long> > Memory::get_process(){
    return process;
}
