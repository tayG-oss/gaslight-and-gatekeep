#pragma once
#include "gatekeep.h"
#include "/public/read.h" // IWYU pragma: keep
#include <cstdint>
#include <vector>         // IWYU pragma: keep

using namespace std;

class Circuit {
	public:
	vector<Gate> gates;
	uint64_t numIn = 0;
	
	Circuit (uint64_t inputs) {
		numIn = inputs;
	}
	
	void addGate(const Gate& gate) {
		gates.push_back(gate);
	}

	void printCircuit() {
	
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
