#pragma once
#include "/public/read.h" // IWYU pragma: keep
#include <cstdint>
#include <vector>         // IWYU pragma: keep
using namespace std;

class Gate {
	public:
		//pair<int, bool> in1 = {0, 0};
		//pair<int, bool> in2 = {0, 0};
		int in1, in2, o = -1;
		string type = "";
		Gate();
		Gate(string c, int a, int b, int d) {
			in1 = a;
			in2 = b;
			type = c;
			o = d;
		}
		Gate(string c, int a, int d) {
			in1 = a;
			type = c;
			o = d;
		}

		//Use recursive functions!!!
		bool connection = false;
		vector<uint64_t> inputs;
		int idx = -1;
		bool hell(const vector<Gate>& keep, uint64_t input, uint64_t numIn) const {
			string c = keep.at(0).type;
		 
			if (type == "NOT") return !keep.at(inputs.at(0)).hell(keep, input, numIn);
			else if (type == "AND") return keep.at(inputs.at(0)).hell(keep, input, numIn) && keep.at(inputs.at(1)).hell(keep, input, numIn);
			else if (type == "OR") return keep.at(inputs.at(0)).hell(keep, input, numIn) || keep.at(inputs.at(1)).hell(keep, input, numIn);
		
			else 
				return (input >> (numIn - 1 - idx)) & 1;
		
			
		}

		int get_in1() { return in1; }
		int get_in2() { return in2; }
		string get_type() { return type; }
		int get_out() { return o; }
};
