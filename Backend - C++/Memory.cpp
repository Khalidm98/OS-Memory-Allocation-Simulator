#include "Memory.h"

Memory::Memory(){}

void Memory::init(unsigned long mem_size, vector<unsigned long> &base, vector<unsigned long> &size){
	memory.clear();
	free.clear();
	process.clear();

	unsigned short N = size.size();
	process.push_back(map<string, unsigned long>());

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
		if (it->first + it->second == next->first){
			it->second += next->second;
			memory.erase(next->first);
			free.erase(next);
			next = it;
		}
		else
			++it;
		++next;
	}

	// if there is a free hole at address 0
	if (free.begin()->first == 0)
		memory[0] = "";
	else {
		memory[0] = "Preallocated 0";
		process[0]["0"] = 0;
	}
	memory[mem_size] = "";

	// determine preallocated segments
	for (it = free.begin(); it != free.end(); ++it){
		memory[it->first + it->second] = "Preallocated " + to_string(process[0].size());
		process[0][to_string(process[0].size())] = it->first + it->second;
	}

	// correct the last hole
	if (!memory[mem_size].empty()){
		memory[mem_size] = "";
		auto last = process[0].end();
		--last;
		process[0].erase(last);
	}
}

bool Memory::best(vector<string> &name, vector<unsigned long> &size){
	unsigned short N = size.size();
	vector<unsigned long> base;
	map<unsigned long, unsigned long> FREE(free.begin(), free.end());

	// best fit
	for (unsigned short i = 0; i < N; ++i){
		// determine first fit
		unsigned long min;
		auto it = FREE.begin();
		while (it != FREE.end()){
			if (size[i] <= it->second){
				min = it->first;
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
			if (size[i] <= it->second && it->second < FREE[min])
				min = it->first;
			++it;
		}

		// update FREE
		base.push_back(min);
		if (size[i] < FREE[min])
			FREE[min + size[i]] = FREE[min] - size[i];
		FREE.erase(min);
	}

	// update free
	free = FREE;

	// update memory
	for (unsigned short i = 0; i < N; ++i){
		memory[base[i]] = "P" + to_string(process.size()) + ", " + name[i];
		if (memory.find(base[i] + size[i]) == memory.end())
			memory[base[i] + size[i]] = "";
	}

	// update process
	process.push_back(map<string, unsigned long>());
	for (unsigned short i = 0; i < N; ++i)
		process.back()[name[i]] = base[i];
	return true;
}

bool Memory::first(vector<string> &name, vector<unsigned long> &size){
	unsigned short N = size.size();
	map<unsigned long, unsigned long> FREE(free.begin(), free.end());
	vector<unsigned long> base;

	// first fit
	for (unsigned short i = 0; i < N; ++i){
		for (auto it = FREE.begin(); it != FREE.end(); ++it){
			if (size[i] <= it->second){
				base.push_back(it->first);
				if (size[i] < it->second)
					FREE[it->first + size[i]] = it->second - size[i];
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
		memory[base[i]] = "P" + to_string(process.size()) + ", " + name[i];
		if (memory.find(base[i] + size[i]) == memory.end())
			memory[base[i] + size[i]] = "";
	}

	// update process
	process.push_back(map<string, unsigned long>());
	for (unsigned short i = 0; i < N; ++i)
		process.back()[name[i]] = base[i];
	return true;
}

bool Memory::pre_free(unsigned short num){
	auto it = process[0].find(to_string(num));
	if (it == process[0].end())
		return false;

	auto last = memory.end();				// iterator to last memory block
	--last;
	unsigned long mem_size = last->first;
	--last;

	// segment is allocated at address 0
	if(it->second == 0){
		auto next = memory.begin();
		++next;

		// if next segment is free
		if((next->second).empty()){
			free[0] = next->first + free[next->first];
			free.erase(next->first);
			memory.erase(next);
		}
		// if next segment is allocated
		else
			free[0] = next->first;

		memory[0] = "";
	}

	// segment is allocated at last memory block
	else if (it->second == last->first){
		auto prev = last;
		--prev;

		// if previous segment is free
		if ((prev->second).empty()){
			free[prev->first] += mem_size - last->first;
			memory.erase(last);
		}
		// if previous segment is allocated
		else {
			free[last->first] = mem_size - last->first;
			last->second = "";
		}
	}

	// segment is allocated in an inner block
	else {
		auto prev = memory.find(it->second);
		auto next = memory.find(it->second);
		--prev;
		++next;

		// if both prev and next are free
		if ((prev->second).empty() && (next->second).empty()){
			free[prev->first] += next->first - it->second + free[next->first];
			free.erase(next->first);
			memory.erase(it->second);
			memory.erase(next);
		}

		// if only next is free
		else if ((next->second).empty()){
			free[it->second] = next->first - it->second + free[next->first];
			free.erase(next->first);
			memory.erase(next);
			memory[it->second] = "";
		}

		// if only prev is free
		else if ((prev->second).empty()){
			free[prev->first] += next->first - it->second;
			memory.erase(it->second);
		}

		// if both prev and next are allocated
		else {
			free[it->second] = next->first - it->second;
			memory[it->second] = "";
		}
	}
	process[0].erase(it);
	return true;
}

bool Memory::user_free(unsigned short num){
	if (num == 0 || num >= process.size() || process[num].empty())
		return false;

	for (auto it = process[num].begin(); it != process[num].end(); ++it){
		auto last = memory.end();				// iterator to last memory block
		--last;
		unsigned long mem_size = last->first;
		--last;

		// segment is allocated at address 0
		if (it->second == 0){
			auto next = memory.begin();
			++next;

			// if next segment is free
			if ((next->second).empty()){
				free[0] = next->first + free[next->first];
				free.erase(next->first);
				memory.erase(next);
			}
			// if next segment is allocated
			else
				free[0] = next->first;

			memory[0] = "";
		}

		// segment is allocated at last memory block
		else if (it->second == last->first){
			auto prev = last;
			--prev;

			// if previous segment is free
			if ((prev->second).empty()){
				free[prev->first] += mem_size - last->first;
				memory.erase(last);
			}
			// if previous segment is allocated
			else {
				free[last->first] = mem_size - last->first;
				last->second = "";
			}
		}

		// segment is allocated in an inner block
		else {
			auto prev = memory.find(it->second);
			auto next = memory.find(it->second);
			--prev;
			++next;

			// if both prev and next are free
			if ((prev->second).empty() && (next->second).empty()){
				free[prev->first] += next->first - it->second + free[next->first];
				free.erase(next->first);
				memory.erase(it->second);
				memory.erase(next);
			}

			// if only next is free
			else if ((next->second).empty()){
				free[it->second] = next->first - it->second + free[next->first];
				free.erase(next->first);
				memory.erase(next);
				memory[it->second] = "";
			}

			// if only prev is free
			else if ((prev->second).empty()){
				free[prev->first] += next->first - it->second;
				memory.erase(it->second);
			}

			// if both prev and next are allocated
			else {
				free[it->second] = next->first - it->second;
				memory[it->second] = "";
			}
		}
	}
	process[num].clear();
	return true;
}

void Memory::print(){
	// memory print
	for (auto it = memory.begin(); it != memory.end(); ++it)
		cout << it->first << " [" << it->second << "] ";
	cout << "\b\b\b   " << endl << endl;

	// segment tables print
	for (size_t i = 0; i < process.size(); ++i){
		if (!process[i].empty()){
			if (i == 0)
				cout << "   Preallocated Segments" << endl;
			else
				cout << "\t   Process " << i << endl;
			cout << "num\tname\tbase\tsize\n";
			unsigned short j = 0;
			for (auto it = process[i].begin(); it != process[i].end(); ++it){
				cout << j << "\t" << it->first << "\t" << it->second << "\t";
				auto next = memory.find(it->second);
				++next;
				cout << next->first - it->second << endl;
				++j;
			}
			cout << endl;
		}
	}
}
