// CosM.cpp : Defines the entry point for the application.
//

#include "CosM.h"

using namespace std;
using U64 = unsigned long long;

const U64 not_a_file{ 18374403900871474942ULL };
const U64 not_h_file{ 9187201950435737471ULL };

enum Color {
	white, black
};

enum Square {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1,
};

// Bit operations

bool get_bit(U64 bb, int square) {
	return (bb & (1ULL << square));
}

void set_bit(U64& bb, int square) {
	bb |= 1ULL << square;
}

void pop_bit(U64& bb, int square) {
	bb &= ~(1ULL << square);
}

void print_bitboard(U64 bb) {
	cout << endl;
	for (int rank{ 0 }; rank < 8; ++rank) {
		cout << (8 - rank) << "| ";

		for (int file{ 0 }; file < 8; ++file) {
			int square = rank * 8 + file;
			cout << (get_bit(bb, square) ? 1 : 0) << " ";
		}

		cout << endl;
	}

	cout << "   _______________" << endl;
	cout << "   a b c d e f g h" << endl;
}

// Attacks

U64 pawn_attacks[2][64];

U64 generate_pawn_attacks(int square, Color color) {
	// piece bitboard
	U64 pawn_bb{ 0ULL };

	set_bit(pawn_bb, square);

	// attack bitboard
	U64 attack_bb{ 0ULL };

	if (color == white) {
		if (pawn_bb & not_h_file) {
			attack_bb |= (pawn_bb >> 7); // right attack
		}
		if (pawn_bb & not_a_file) {
			attack_bb |= (pawn_bb >> 9); // left attack
		}
	}
	else {
		if (pawn_bb & not_a_file) {
			attack_bb |= (pawn_bb << 7); // left attack
		}
		if (pawn_bb & not_h_file) {
			attack_bb |= (pawn_bb << 9); // right attack
		}
	}

	return attack_bb;
}

void init_all_pawn_attacks() {
	for (int square{ 0 }; square < 64; square++) {
			pawn_attacks[white][square] = generate_pawn_attacks(square, white);
			pawn_attacks[black][square] = generate_pawn_attacks(square, black);
	}
}

int main()
{
	init_all_pawn_attacks();

	print_bitboard(pawn_attacks[black][d1]);

	return 0;
}
