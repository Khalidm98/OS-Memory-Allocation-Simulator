#ifndef MEMORY_H
#define MEMORY_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Memory {
private:
	map<unsigned long, string> memory;				// memory segments and info
	map<unsigned long, unsigned long> free;			// base addresses and sizes of free holes
	vector< map<string, unsigned long> > process;	// process segments info
	// memory<base address, segment info>
	// free<base address, hole size>
	// process[1] = map<segment name, base address>
	// process[0] is reserved for initially allocated segments

public:
    Memory();
    void init(unsigned long mem_size, vector<unsigned long> &base, vector<unsigned long> &size);
	bool best(vector<string> &name, vector<unsigned long> &size);
	bool first(vector<string> &name, vector<unsigned long> &size);
	bool pre_free(unsigned short num);
	bool user_free(unsigned short num);
	void print();
};

#endif // MEMORY_H
