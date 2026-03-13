// CosM.cpp : Defines the entry point for the application.
//

#include "CosM.h"

using namespace std;
using U64 = unsigned long long;

const U64 not_a_file{ 18374403900871474942ULL };
const U64 not_ab_file{ 18229723555195321596ULL };
const U64 not_h_file{ 9187201950435737471ULL };
const U64 not_gh_file{ 4557430888798830399ULL };


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

int count_bits(U64 bb) {
	int counter = 0;

	while (bb > 0) {
		counter++;

		bb &= (bb - 1);
	}

	return counter;
}

void print_bitboard(U64 bb) {
	const char* GREEN = "\033[92m";
	const char* RESET = "\033[0m";

	cout << endl;
	for (int rank{ 0 }; rank < 8; ++rank) {
		cout << (8 - rank) << "| ";

		for (int file{ 0 }; file < 8; ++file) {
			int square = rank * 8 + file;

			if (get_bit(bb, square))
				cout << GREEN << "1" << RESET << " ";
			else
				cout << "0 ";
		}

		cout << endl;
	}

	cout << "   _______________" << endl;
	cout << "   a b c d e f g h" << endl;
}

// Attacks

U64 pawn_attacks[2][64];
U64 knight_attacks[64];
U64 king_attacks[64];

U64 generate_pawn_attacks(int square, Color color) {
	// piece bitboard
	U64 pawn_bb{ 0ULL };

	set_bit(pawn_bb, square);

	// attacks bitboard
	U64 attacks_bb{ 0ULL };

	if (color == white) {
		if (pawn_bb & not_h_file) {
			attacks_bb |= (pawn_bb >> 7); // right attack
		}
		if (pawn_bb & not_a_file) {
			attacks_bb |= (pawn_bb >> 9); // left attack
		}
	}
	else {
		if (pawn_bb & not_a_file) {
			attacks_bb |= (pawn_bb << 7); // left attack
		}
		if (pawn_bb & not_h_file) {
			attacks_bb |= (pawn_bb << 9); // right attack
		}
	}

	return attacks_bb;
}

U64 generate_knight_attacks(int square) {
	// piece bitboard
	U64 knight_bb{ 0ULL };

	set_bit(knight_bb, square);

	// attacks bitboard
	U64 attacks_bb{ 0ULL };

	if (knight_bb & not_a_file) {
		attacks_bb |= (knight_bb >> 17); // two up one left
		attacks_bb |= (knight_bb << 15); // two down one left
	}
	if (knight_bb & not_h_file) {
		attacks_bb |= (knight_bb >> 15); // two up one right 
		attacks_bb |= (knight_bb << 17); // two down one right
	}
	if (knight_bb & not_ab_file) {
		attacks_bb |= (knight_bb >> 10); // two left one up
		attacks_bb |= (knight_bb << 6); // two left one down
	}
	if (knight_bb & not_gh_file) {
		attacks_bb |= (knight_bb >> 6); // two right one up
		attacks_bb |= (knight_bb << 10); // two right one down 
	}

	return attacks_bb;
}

U64 generate_king_attacks(int square) {
	// piece bitboard
	U64 king_bb{ 0ULL };

	set_bit(king_bb, square);

	U64 attacks_bb{ 0ULL };

	if (king_bb & not_a_file) {
		attacks_bb |= (king_bb >> 9); // top left
		attacks_bb |= (king_bb >> 1); // left
		attacks_bb |= (king_bb << 7); // bottom left
	}
	if (king_bb & not_h_file) {
		attacks_bb |= (king_bb << 9); // top right
		attacks_bb |= (king_bb << 1); // right
		attacks_bb |= (king_bb >> 7); // bottom right
	}

	attacks_bb |= (king_bb << 8); // one down
	attacks_bb |= (king_bb >> 8); // one up
	
	return attacks_bb;
}

U64 generate_bishop_attacks(int square) {
	U64 occupancy_squares{ 0ULL };

	int rank, file;
	int target_rank = square / 8;
	int target_file = square % 8;

	for (rank = target_rank + 1, file = target_file + 1; rank <= 6 && file <= 6; rank++, file++) {
		set_bit(occupancy_squares, rank * 8 + file);
	}

	for (rank = target_rank + 1, file = target_file - 1; rank <= 6 && file >= 1; rank++, file--) {
		set_bit(occupancy_squares, rank * 8 + file);
	}

	for (rank = target_rank - 1, file = target_file + 1; rank >= 1 && file <= 6; rank--, file++) {
		set_bit(occupancy_squares, rank * 8 + file);
	}

	for (rank = target_rank - 1, file = target_file - 1; rank >= 1 && file >= 1; rank--, file--) {
		set_bit(occupancy_squares, rank * 8 + file);
	}

	return occupancy_squares;
}

U64 generate_bishop_attacks_on_the_fly(int square, U64 block) {
	U64 attacks{ 0ULL };

	int rank, file;
	int target_rank = square / 8;
	int target_file = square % 8;

	for (rank = target_rank + 1, file = target_file + 1; rank <= 7 && file <= 7; rank++, file++) {
		if (get_bit(block, rank * 8 + file)) {
			break;
		}
		set_bit(attacks, rank * 8 + file);
	}

	for (rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; rank++, file--) {
		if (get_bit(block, rank * 8 + file)) {
			break;
		}
		set_bit(attacks, rank * 8 + file);
	}

	for (rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; rank--, file++) {
		if (get_bit(block, rank * 8 + file)) {
			break;
		}
		set_bit(attacks, rank * 8 + file);
	}

	for (rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; rank--, file--) {
		if (get_bit(block, rank * 8 + file)) {
			break;
		}
		set_bit(attacks, rank * 8 + file);
	}

	return attacks;
}

U64 generate_rook_attacks(int square) {
	U64 occupancy_squares{ 0ULL };

	int rank, file;
	int target_rank = square / 8;
	int target_file = square % 8;

	for (rank = target_rank + 1; rank <= 6; rank++) {
		set_bit(occupancy_squares, rank * 8 + target_file);
	}

	for (rank = target_rank - 1; rank >= 1; rank--) {
		set_bit(occupancy_squares, rank * 8 + target_file);
	}

	for (file = target_file + 1; file <= 6; file++) {
		set_bit(occupancy_squares, target_rank * 8 + file);
	}

	for (file = target_file - 1; file >= 1; file--) {
		set_bit(occupancy_squares, target_rank * 8 + file);
	}

	return occupancy_squares;
}

U64 generate_rook_attacks_on_the_fly(int square, U64 block) {
	U64 attacks{ 0ULL };

	int rank, file;
	int target_rank = square / 8;
	int target_file = square % 8;

	for (rank = target_rank + 1; rank <= 7; rank++) {
		if (get_bit(block, rank * 8 + target_file)) {
			break;
		}
		set_bit(attacks, rank * 8 + target_file);
	}

	for (rank = target_rank - 1; rank >= 0; rank--) {
		if (get_bit(block, rank * 8 + target_file)) {
			break;
		}
		set_bit(attacks, rank * 8 + target_file);
	}

	for (file = target_file + 1; file <= 7; file++) {
		if (get_bit(block, target_rank * 8 + file)) {
			break;
		}
		set_bit(attacks, target_rank * 8 + file);
	}

	for (file = target_file - 1; file >= 0; file--) {
		if (get_bit(block, target_rank * 8 + file)) {
			break;
		}
		set_bit(attacks, target_rank * 8 + file);
	}

	return attacks;
}

void init_all_pawn_attacks() {
	for (int square{ 0 }; square < 64; square++) {
			pawn_attacks[white][square] = generate_pawn_attacks(square, white);
			pawn_attacks[black][square] = generate_pawn_attacks(square, black);
	}
}

void init_all_knight_attacks() {
	for (int square{ 0 }; square < 64; square++) {
		knight_attacks[square] = generate_knight_attacks(square);
	}
}

void init_all_king_attacks() {
	for (int square{ 0 }; square < 64; square++) {
		king_attacks[square] = generate_king_attacks(square);
	}
}

int main()
{
	U64 blocker_bitboard{ 0ULL };

	set_bit(blocker_bitboard, a2);
	set_bit(blocker_bitboard, b2);
	set_bit(blocker_bitboard, c2);
	set_bit(blocker_bitboard, d2);
	set_bit(blocker_bitboard, e2);
	set_bit(blocker_bitboard, f2);
	set_bit(blocker_bitboard, g2);

	cout << count_bits(blocker_bitboard) << endl;

	return 0;
}
