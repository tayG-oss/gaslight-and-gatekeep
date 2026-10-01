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
	int a = 0, b = 0;
	int marking = 0;
	string c;

	int x = read("How many inputs does your logic block have? (1 to 10)\n");
	if (x <= 0 || x > 10) die();
	Circuit circuit(x);
	for (int i = 0; i < x; i ++) {
		circuit.addGate(Gate("INPUT", -1, -1, -1));
	}
	//Looking back, I could've coded this w/ less lines :/
	while (true) {
		cout << "What sort of gate do you want to add?" << endl;
		int gate = read("0 - NOT, 1 - AND, 2 - OR, 3 - NAND, 4 - NOR, 5 - XOR, 6 - DONE\n");
		if (gate == 0) { //NOT
			c = "NOT";
		} else if (gate == 1) { //AND
			c = "AND";
		} else if (gate == 2) { //OR
			c = "OR";
		} else if (gate == 3) { //NAND
			c = "NAND";
		} else if (gate == 4) { //NOR
			c = "NOR";
		} else if (gate == 5) { //XOR
			c = "XOR";
		} else if (gate == 6) { //DONE
			cout << endl;
			break;
		} else die();
			
		int d = 0;
 		a = read("Give the index for the first input:\n");
		b = read("Give the index for the second input:\n");
		d = circuit.gates.size() - 1;

		if (c == "NOT") {
			circuit.addGate (Gate(c, a, d));
		} else {
			circuit.addGate (Gate(c, a, b, d));
		}

		//circuit.gateMark(a);
		//circuit.gateMark(b);	
	} //End of first loop

	while (true) {
		int y = read("1) Print Circuit Block or 2) Print Truth Table\n");
		if (y == 1) {
		/*	for (int i = 0; i < x; i++) {
				cout << "Gate Type: INPUT\n";
				cout << "\tInput Connected to Index: N.C. and N.C.\n";
				cout << "\tOutput Connected to Index: " << x << endl; // NOTE TO SELF: fix this pls :D
				cout << "\tValue: X\n";
				cout << endl;
			}
*/
			for (unsigned int j = 0; j < circuit.gates.size(); j++) {
				cout << "Gate Type: " << circuit.gates.at(j).get_type() << endl;
				cout << "\tInput Connected to Index: " << circuit.gates.at(j).get_in1();
				if (circuit.gates.at(j).get_type() != "NOT")
					cout << " and " << circuit.gates.at(j).get_in2() << endl;

				if (j == circuit.gates.size() - 1)
					cout << "\tOutput Connected to Index: OUTPUT PIN\n";
				else
					cout << "\tOutput Connected to Index: " <<circuit.gates.at(j).get_out() << endl;
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
