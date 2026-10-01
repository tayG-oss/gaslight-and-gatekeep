#include "/public/read.h" // IWYU pragma: keep
#include <cstdint>
#include <ranges>
#include <vector>         // IWYU pragma: keep
#include <map>
#include "gatekeep.h"
#include "circuit.h"
using namespace std;

int die() {
	cout << "BAD INPUT!\n";
	exit(1);
}


int main() {
	cout << "Welcome to the Gates of Babylon!" << endl;

	int x = read("How many inputs does your logic block have? (1 to 10)\n");
	if (x <= 0 || x > 10) die();

	int a = 0, b = 0;
	int marking = 0;
	Circuit circuit(x);
	//Looking back, I could've coded this w/ less lines :/
	while (true) {
		cout << "What sort of gate do you want to add?" << endl;
		int gate = read("0 - NOT, 1 - AND, 2 - OR, 3 - NAND, 4 - NOR, 5 - XOR, 6 - DONE\n");
		if (gate == 0) { //NOT
			a = read("Give the index for the first input:\n");
			
			circuit.addGate(Gate("NOT", a));
		} else if (gate == 1) { //AND
			a = read("Give the index for the first input:\n");
			b = read("Give the index for the second input:\n");
			
			circuit.addGate(Gate("AND", a, b));

		} else if (gate == 2) { //OR
			a = read("Give the index for the first input:\n");
			b = read("Give the index for the second input:\n");
			circuit.addGate(Gate("OR", a, b));

		} else if (gate == 3) { //NAND
			a = read("Give the index for the first input:\n");
			b = read("Give the index for the second input:\n");
			circuit.addGate(Gate("NAND", a, b));
			
		} else if (gate == 4) { //NOR
			a = read("Give the index for the first input:\n");
			b = read("Give the index for the second input:\n");
			circuit.addGate(Gate("NOR", a, b));
			
		} else if (gate == 5) { //XOR
			a = read("Give the index for the first input:\n");
			b = read("Give the index for the second input:\n");
			circuit.addGate(Gate("XOR", a, b));

		} else if (gate == 6) { //DONE
			cout << endl;
			break;
		} else die();
		circuit.gateMark(x);
	
	} //End of first loop

	while (true) {
		int y = read("1) Print Circuit Block or 2) Print Truth Table\n");
		if (y == 1) {
			for (int i = 0; i < x; i++) {
				cout << "Gate Type: INPUT\n";
				cout << "\tInput Connected to Index: N.C. and N.C.\n";
				cout << "\tOutput Connected to Index: " << x + 1 << endl; // NOTE TO SELF: fix this pls :D
				cout << "\tValue: X\n";
				cout << endl;
			}

			for (unsigned int j = 0; j < circuit.gates.size(); j++) {
				cout << "Gate Type: " << circuit.gates.at(j).get_type() << endl;
				cout << "\tInput Connected to Index: " <<  circuit.gates.at(j).get_in1();
				if (circuit.gates.at(j).get_type() != "NOT")
					cout << " and " << circuit.gates.at(j).get_in2() << endl;

				if (j == circuit.gates.size() - 1)
					cout << "\tOutput Connected to Index: OUTPUT PIN\n";
				else
					cout << "\tOutput Connected to Index: " << x + j + 1 << endl;
				cout << "\tValue: X\n";
				cout << endl;
			}

			break;
		} else if (y == 2) {
			cout << "Input Pins (Numbers), Output Pin (O);\n";
			break;
		} else die();
	}

}
