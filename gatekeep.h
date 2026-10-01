#pragma once
#include "/public/read.h" // IWYU pragma: keep
#include <cstdint>
#include <string>
#include <vector>         // IWYU pragma: keep
using namespace std;

class Gate {
	public:
		//pair<int, bool> in1 = {0, 0};
		//pair<int, bool> in2 = {0, 0};
		int in1 = -1, in2 = -1;   // FIX: was `int in1, in2 = -1;` (in1 was never initialized)
		string type = "";
		Gate() {}   // FIX: was declared but never defined
		Gate(string c, int a, int b) {
			in1 = a;
			in2 = b;
			type = c;
		}
		Gate(string c, int a) {
			in1 = a;
			type = c;
		}

		//Use recursive functions!!!
		bool connection = false;
		vector<uint64_t> inputs;
		int idx = -1;
		// FIX: value of circuit index i. Indexes 0..numIn-1 are the input pins (bit i of `input`,
		// leftmost pin = most significant bit), index numIn+k is gate k. Recursive, as the comment above wanted.
		static bool valueAt(const vector<Gate>& keep, int i, uint64_t input, uint64_t numIn) {
			if (i < (int)numIn) return (input >> (numIn - 1 - i)) & 1;
			return keep.at((size_t)(i - (int)numIn)).hell(keep, input, numIn);
		}

		// FIX: handles all six gate types (NAND, NOR and XOR were missing) and reads in1/in2 directly
		// (the `inputs` vector was never filled in, so the old version always threw).
		bool hell(const vector<Gate>& keep, uint64_t input, uint64_t numIn) const {
			if (type == "NOT") return !valueAt(keep, in1, input, numIn);
			bool a = valueAt(keep, in1, input, numIn);
			bool b = valueAt(keep, in2, input, numIn);
			if (type == "AND")  return a && b;
			if (type == "OR")   return a || b;
			if (type == "NAND") return !(a && b);
			if (type == "NOR")  return !(a || b);
			if (type == "XOR")  return a != b;
			return false;   // unknown gate type (cannot happen)
		}

		int get_in1() const { return in1; }
		int get_in2() const { return in2; }
		string get_type() const { return type; }

};
