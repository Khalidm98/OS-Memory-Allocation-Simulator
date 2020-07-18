#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "Memory.h"
using namespace std;

int main(){
	Memory mem;
	while(1){
		char choice;
		cin >> choice;

		// initialize
		if (choice == 'i' || choice == 'I'){
			unsigned long mem_size;							// total memory size
			unsigned short N;								// number of initial holes
			cin >> mem_size >> N;

			vector<unsigned long> base(N);					// base addresses of initial holes
			for (unsigned short i = 0; i < N; ++i)
				cin >> base[i];

			vector<unsigned long> size(N);					// sizes of initial holes
			for (unsigned short i = 0; i < N; ++i)
				cin >> size[i];

			mem.init(mem_size, base, size);
		}

		// allocate using best-fit or first-fit
		else if (choice == 'b' || choice == 'B' || choice == 'f' || choice == 'F'){
			unsigned short N;								// number of segments
			cin >> N;

			vector<string> name(N);							// segments names
			for (unsigned short i = 0; i < N; ++i)
				cin >> name[i];

			vector<unsigned long> size(N);					// segments sizes
			for (unsigned short i = 0; i < N; ++i)
				cin >> size[i];

			if ((choice == 'b' || choice == 'B') && mem.best(name, size))
				cout << "\nSuccessfully Allocated <3\n";
			else if ((choice == 'f' || choice == 'F') && mem.first(name, size))
				cout << "\nSuccessfully Allocated <3\n";
			else
				cout << "\nFailed to allocate :(\n";
		}

		// deallocate a preallocated segment
		else if (choice == 'p' || choice == 'P'){
			unsigned short num;								// number of segment
			cin >> num;
			if (mem.pre_free(num))
				cout << "\nSuccessfully Deallocated <3\n";
			else
				cout << "\nFailed to deallocate :(\n";
		}

		// deallocate a user allocated process
		else if (choice == 'u' || choice == 'U'){
			unsigned short num;								// number of process
			cin >> num;
			if (mem.user_free(num))
				cout << "\nSuccessfully Deallocated <3\n";
			else
				cout << "\nFailed to deallocate :(\n";
		}

		// quit
		else if (choice == 'q' || choice == 'Q')
			return 0;

		// memory print
		mem.print();
	}
}
