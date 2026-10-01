#pragma once
#include "gatekeep.h"
#include "/public/read.h" // IWYU pragma: keep
#include <cstdint>
#include <vector>         // IWYU pragma: keep
#include <iostream>
#include <cstdlib>

using namespace std;

class Circuit {
	public:
	vector<Gate> gates;
	uint64_t numIn = 0;
	vector<int> uses;   // NEW: uses[i] = how many gates take index i as an input (pins first, then gates)
	
	Circuit (uint64_t inputs) {
		numIn = inputs;
		uses.assign(inputs, 0);
	}
	
	// NEW: an input index must exist and must not already feed another gate (README: "exactly one gate")
	void claim(int index) {
		if (index < 0 || index >= (int)uses.size() || uses.at(index) != 0) {
			cout << "BAD INPUT!\n";
			exit(1);
		}
		uses.at(index)++;
	}

	void addGate(const Gate& gate) {
		claim(gate.get_in1());
		if (gate.get_type() != "NOT") claim(gate.get_in2());
		gates.push_back(gate);
		uses.push_back(0);   // the new gate's own output starts out unused
	}

	// NEW: true when every input pin and every gate except the last feeds exactly one gate
	bool isComplete() const {
		if (gates.empty()) return false;
		for (size_t i = 0; i + 1 < uses.size(); i++)
			if (uses.at(i) != 1) return false;
		return true;
	}

	// NEW: index of the gate that takes `index` as an input (-1 if none)
	int consumerOf(int index) const {
		for (size_t k = 0; k < gates.size(); k++) {
			if (gates.at(k).get_in1() == index) return (int)(numIn + k);
			if (gates.at(k).get_type() != "NOT" && gates.at(k).get_in2() == index) return (int)(numIn + k);
		}
		return -1;
	}

	// FILLED IN (was empty): prints the circuit block exactly as in the README
	void printCircuit() const {
		for (uint64_t i = 0; i < numIn; i++) {
			cout << "Gate Type: INPUT\n";
			cout << "\tInput Connected to Index: N.C. and N.C.\n";
			cout << "\tOutput Connected to Index: " << consumerOf((int)i) << "\n";
			cout << "\tValue: X\n\n";
		}
		for (size_t j = 0; j < gates.size(); j++) {
			cout << "Gate Type: " << gates.at(j).get_type() << "\n";
			cout << "\tInput Connected to Index: " << gates.at(j).get_in1();
			if (gates.at(j).get_type() != "NOT") cout << " and " << gates.at(j).get_in2();
			cout << "\n";
			if (j + 1 == gates.size()) cout << "\tOutput Connected to Index: OUTPUT PIN\n";
			else cout << "\tOutput Connected to Index: " << consumerOf((int)(numIn + j)) << "\n";
			cout << "\tValue: X\n\n";
		}
	}

	// NEW: prints the truth table exactly as in the README (all 1s first, down to all 0s)
	void printTruthTable() const {
		cout << "Input Pins (Numbers), Output Pin (O):\n";
		for (uint64_t i = 0; i < numIn; i++) cout << i << "|";
		cout << "O\n";
		for (uint64_t row = (1ULL << numIn); row-- > 0; ) {
			for (uint64_t i = 0; i < numIn; i++) cout << ((row >> (numIn - 1 - i)) & 1) << "|";
			cout << evaluateCircuit(row) << "\n";
		}
	}

	bool gateInUse(int index) {
		return gates.at(index).connection;
	}
	
	void gateMark(int index) {
		gates.at(index).connection = true;
	}

	bool evaluateCircuit(uint64_t inBits) const {
		return gates.at(gates.size() - 1).hell(gates, inBits, numIn);
	}





};
