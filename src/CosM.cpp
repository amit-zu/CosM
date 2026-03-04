// CosM.cpp : Defines the entry point for the application.
//

#include "CosM.h"

using namespace std;
using U64 = unsigned long long;

enum Squares {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1,
};

bool getBit(U64 bb, int square) {
	return (bb & (1ULL << square));
}

void setBit(U64& bb, int square) {
	bb |= 1ULL << square;
}

void popBit(U64& bb, int square) {
	bb &= ~(1ULL << square);
}

void printBitboard(U64 bb) {
	cout << endl;
	for (int rank{ 0 }; rank < 8; ++rank) {
		cout << (8 - rank) << "| ";

		for (int file{ 0 }; file < 8; ++file) {
			int square = rank * 8 + file;
			cout << (getBit(bb, square) ? 1 : 0) << " ";
		}

		cout << endl;
	}

	cout << "   _______________" << endl;
	cout << "   a b c d e f g h" << endl;
}

int main()
{
	U64 bb{0ULL};
	printBitboard(bb);

	return 0;
}
