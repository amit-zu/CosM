// CosM.cpp : Defines the entry point for the application.
//

#include "CosM.h"
#include <cstring>
#include <cstdint>
#include <format>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>

using namespace std;
using U64 = unsigned long long;
using U8 = uint8_t;

// fen string
string empty_board_fen = "8/8/8/8/8/8/8/8 w - - 0 1";
string starting_pos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
string tricky_pos_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

const U64 not_a_file{ 18374403900871474942ULL };
const U64 not_ab_file{ 18229723555195321596ULL };
const U64 not_h_file{ 9187201950435737471ULL };
const U64 not_gh_file{ 4557430888798830399ULL };

const U64 bishop_magics[64] {
1187782080865792ULL,
292735109795185953ULL,
1321844025459736576ULL,
2291936283084801ULL,
4611985360535290882ULL,
571754770595904ULL,
13871792773172166676ULL,
84587664116942848ULL,
144150668819040528ULL,
92703748653104ULL,
1243293698465267747ULL,
4630971469567886208ULL,
5070960646423808ULL,
9232488104978448448ULL,
1153066657338951937ULL,
4635048596130498562ULL,
9227875773989273728ULL,
40541227367596112ULL,
19149274965283330ULL,
1441435692738707490ULL,
1189231811499655200ULL,
4573976966864930ULL,
11592335818588685312ULL,
162169177602394122ULL,
1775650815124582400ULL,
9224501381869076736ULL,
2325335428510615809ULL,
2306406096740319264ULL,
14356048513089600ULL,
9367773134494667776ULL,
9243788937057601668ULL,
56442956984615936ULL,
1198502105354240ULL,
72207265329054786ULL,
864726897480695842ULL,
648694818499199488ULL,
1130315133227024ULL,
290271640289792ULL,
9227876813303649408ULL,
2392011736021598336ULL,
2324421474913370176ULL,
6900689640955936ULL,
9571249332226048ULL,
440792433794516992ULL,
38439072544339986ULL,
2315133916861956672ULL,
2676245695569984ULL,
4539909289283624ULL,
9289292975702481ULL,
5765241944734179332ULL,
36029899349426193ULL,
722830081026164738ULL,
148619371869372416ULL,
70377401290753ULL,
361449088846200834ULL,
585751703169541824ULL,
2378045774354319368ULL,
1161376639222432ULL,
1297036694834907200ULL,
9802084606153885761ULL,
580965752226515203ULL,
1159896841272688776ULL,
9799850450081026177ULL,
18093571948281872ULL,
};

const U64 rook_magics[64]{
11565243981600133249ULL,
4629770785949491200ULL,
72092778695245834ULL,
6088872262483050624ULL,
3602888515202846752ULL,
504425148600877064ULL,
72131262155850884ULL,
144117422549381252ULL,
9710887237582976ULL,
422349929201664ULL,
72339206588923968ULL,
141287378391040ULL,
288371663462991872ULL,
13837872874910130688ULL,
3377704619999760ULL,
36591755566556161ULL,
1170971637248426112ULL,
319777838672643392ULL,
2683358664511488ULL,
4622101692346009632ULL,
2393087192073216ULL,
9873016832892370948ULL,
144119586160640272ULL,
2594119564858036396ULL,
436919581991272480ULL,
13916125204368589952ULL,
9236918022264852480ULL,
9584222999951712272ULL,
504407558460014720ULL,
1154049605684691072ULL,
11541113995527680ULL,
5908793638200279105ULL,
17293858028646301825ULL,
360358341123252224ULL,
140874935701508ULL,
1162183941558276ULL,
1550364446734942336ULL,
9232942203251196544ULL,
76569998415368193ULL,
613628679909541977ULL,
105828137140228ULL,
2310346884255858692ULL,
281612483887168ULL,
4612970252330926090ULL,
4621537668516347920ULL,
1407385688145928ULL,
1127283163398152ULL,
281545848455170ULL,
4611967839148976384ULL,
1297388547143109248ULL,
439814851691648ULL,
4899925192820654720ULL,
1130300101361792ULL,
18579547553300608ULL,
2267877016576ULL,
288793946392429056ULL,
4611868816531554305ULL,
9297963180591424137ULL,
3027017084203698241ULL,
9512201664174493697ULL,
1873779091766789143ULL,
294423135149687874ULL,
1333910206732173345ULL,
20068301866146ULL,
};

enum Color {
	white, black, none
};

enum Piece {
	pawn, king, queen, knight, bishop, rook, no_piece
};

enum PieceChar {
	P, K, Q, N, B, R, p, k, q, n, b, r, X
};

char piece_chars[13] = "PKQNBRpkqnbr";

int get_piece_from_char(char c) {
	switch (c) {
	case 'P': return P;
	case 'K': return K;
	case 'Q': return Q;
	case 'N': return N;
	case 'B': return B;
	case 'R': return R;
	case 'p': return p;
	case 'k': return k;
	case 'q': return q;
	case 'n': return n;
	case 'b': return b;
	case 'r': return r;
	default:  return X;
	}
}

const char* piece_unicode[12] = {
	"♙", "♔", "♕", "♘", "♗", "♖",
	"♟", "♚", "♛", "♞", "♝", "♜"
};

enum Square {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1, no_square
};

string square_to_coordinate(int square) {
	if (square == no_square) {
		return "no_square";
	}

	if (square < a8 || square > h1) {
		return "invalid";
	}

	char file = 'a' + (square % 8);
	char rank = '8' - (square / 8);

	return string{ file, rank };
}

int coordinate_to_square(string& coord) {
	if (coord == "no_square") {
		return no_square;
	}

	if (coord.size() != 2) {
		return no_square; // invalid
	}

	char file = coord[0];
	char rank = coord[1];

	if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
		return no_square; // invalid
	}

	int file_index = file - 'a';
	int rank_index = '8' - rank;

	return rank_index * 8 + file_index;
}

/*
	white can castle kingside bits - 0001
	white can castle queenside bits - 0010
	black can castle kingside bits - 0100
	black can castle queenside bits - 1000

	example:

	1011 means black can castle queenside and white can castle both ways
*/
enum CastlingType {
	wk = 1, wq = 2, bk = 4, bq = 8
};

int relevant_bishop_bits[64] = {
	6, 5, 5, 5, 5, 5, 5, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 5, 5, 5, 5, 5, 5, 6
};

int relevant_rook_bits[64] = {
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

U64 piece_occupancies[12];
U64 color_occupancies[3]; //white, black, none
Color side_to_move = none;
int enpassant_square = no_square;
U8 castle;

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

int get_least_significant_1_bit(U64 bb) {
	if (bb) {
		return count_bits((bb & -bb) -1);
	}
	else {
		return -1;
	}
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

	cout << "   ________________" << endl;
	cout << "   a b c d e f g h" << endl;
}

void print_board() {
	const char* GREEN = "\033[92m";
	const char* RESET = "\033[0m";

	for (int rank{ 0 }; rank < 8; ++rank) {
		cout << (8 - rank) << "| ";

		for (int file{ 0 }; file < 8; ++file) {
			int square = rank * 8 + file;

			int piece = X;

			for (int color_piece = P; color_piece < X; color_piece++) {
				U64 piece_occupancy = piece_occupancies[color_piece];

				if (get_bit(piece_occupancy, square)) {
					piece = color_piece;
				}
			}

			char display_char[2] = { (piece == X) ? '.' : piece_chars[piece], '\0' };

			if (piece == X)
				cout << std::format(" {}", display_char);
			else
				cout << GREEN << std::format(" {}", display_char) << RESET;
		}

		cout << endl;
	}

	cout << "   ________________" << endl;
	cout << "   a b c d e f g h" << endl << endl;

	cout << std::format("to move: {}, ", ((side_to_move == white) ? "white" : ((side_to_move == black) ? "black" : "none")));

	cout << std::format("en passant: {}, ", (enpassant_square != no_square) ? square_to_coordinate(enpassant_square) : "-");

	cout << std::format("castle: {}{}{}{}", 
		(castle & wk) ? "K" : "-", 
		(castle & wq) ? "Q" : "-", 
		(castle & bk) ? "k" : "-", 
		(castle & bq) ? "q" : "-");

	cout << endl;
}

vector<string> split(const string& s, char delimiter) {
	vector<string> result;
	string current;

	for (char c : s) {
		if (c == delimiter) {
			result.push_back(current);
			current.clear();
		}
		else {
			current += c;
		}
	}

	result.push_back(current);
	return result;
}

void parse_fen(const string& fen) {
	std::memset(piece_occupancies, 0ULL, sizeof(piece_occupancies));
	std::memset(color_occupancies, 0ULL, sizeof(color_occupancies));

	side_to_move = none;
	enpassant_square = no_square;
	castle = 0;

	vector<string> fen_parts = split(fen, ' ');

	string pieces_string = fen_parts[0];
	string side_to_move_string = fen_parts[1];
	string castling_string = fen_parts[2];
	string en_passant_string = fen_parts[3];
	string half_move_string = fen_parts[4];
	string full_move_string = fen_parts[5];

	int square = 0;

	for (int i = 0; i < pieces_string.length(); i++)
	{
		char c = fen[i];

		if (c == ' ') {
			break;
		} 

		if (c == '/') {
			continue;
		}

		if (c >= '1' && c <= '8') {
			square += (c - '0');
		}
		else {
			int piece = get_piece_from_char(c);

			if (piece != X) {
				set_bit(piece_occupancies[piece], square);
				square++;
			}
		}
	}

	for (char c : castling_string) {
		switch (c) {
		case '-': break;
		case 'K': castle |= wk; break;
		case 'Q': castle |= wq; break;
		case 'k': castle |= bk; break;
		case 'q': castle |= bq; break;
		}
	}

	side_to_move = (side_to_move_string == "w") ? white : black;

	enpassant_square = (en_passant_string == "-") ? no_square : coordinate_to_square(en_passant_string);

	for (int piece = P; piece <= R; piece++) {
		color_occupancies[white] |= piece_occupancies[piece];
	}

	for (int piece = p; piece <= r; piece++) {
		color_occupancies[black] |= piece_occupancies[piece];
	}

	color_occupancies[none] |= color_occupancies[white];
	color_occupancies[none] |= color_occupancies[black];
}

U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask) {
	U64 occupancy{ 0ULL };

	for (int count{ 0 }; count < bits_in_mask; count++) {
		int square = get_least_significant_1_bit(attack_mask);

		pop_bit(attack_mask, square);

		if (index & (1 << count)) {
			occupancy |= (1ULL << square);
		}
	}

	return occupancy;
}

// Attacks

U64 pawn_attacks[2][64];
U64 knight_attacks[64];
U64 king_attacks[64];
U64 bishop_masks[64];
U64 rook_masks[64];
U64 bishop_attacks[64][512];
U64 rook_attacks[64][4096];

U64 generate_pawn_attacks(int square, Color color) {
	U64 pawn_bb{ 0ULL };

	set_bit(pawn_bb, square);

	U64 attacks_bb{ 0ULL };

	if (color == white) {
		if (pawn_bb & not_h_file) {
			attacks_bb |= (pawn_bb >> 7); // right attack
		}
		if (pawn_bb & not_a_file) {
			attacks_bb |= (pawn_bb >> 9); // left attack
		}
	}
	else if (color == black){
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
	U64 knight_bb{ 0ULL };

	set_bit(knight_bb, square);

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
		set_bit(attacks, rank * 8 + file);
		if (get_bit(block, rank * 8 + file)) {
			break;
		}
	}

	for (rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; rank++, file--) {
		set_bit(attacks, rank * 8 + file);
		if (get_bit(block, rank * 8 + file)) {
			break;
		}
	}

	for (rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; rank--, file++) {
		set_bit(attacks, rank * 8 + file);
		if (get_bit(block, rank * 8 + file)) {
			break;
		}
	}

	for (rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; rank--, file--) {
		set_bit(attacks, rank * 8 + file);
		if (get_bit(block, rank * 8 + file)) {
			break;
		}
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
		set_bit(attacks, rank * 8 + target_file);
		if (get_bit(block, rank * 8 + target_file)) {
			break;
		}
	}

	for (rank = target_rank - 1; rank >= 0; rank--) {
		set_bit(attacks, rank * 8 + target_file);
		if (get_bit(block, rank * 8 + target_file)) {
			break;
		}
	}

	for (file = target_file + 1; file <= 7; file++) {
		set_bit(attacks, target_rank * 8 + file);
		if (get_bit(block, target_rank * 8 + file)) {
			break;
		}
	}

	for (file = target_file - 1; file >= 0; file--) {
		set_bit(attacks, target_rank * 8 + file);
		if (get_bit(block, target_rank * 8 + file)) {
			break;
		}
	}

	return attacks;
}

U64 get_bishop_attacks(int square, U64 occupancy) {
	occupancy &= bishop_masks[square];
	occupancy *= bishop_magics[square];
	occupancy >>= 64 - relevant_bishop_bits[square];

	return bishop_attacks[square][occupancy];
}

U64 get_rook_attacks(int square, U64 occupancy) {
	occupancy &= rook_masks[square];
	occupancy *= rook_magics[square];
	occupancy >>= 64 - relevant_rook_bits[square];

	return rook_attacks[square][occupancy];
}

U64 get_queen_attacks(int square, U64 occupancy) {
	U64 queen_attacks{ 0ULL };

	U64 bishop_occupancy = occupancy;
	U64 rook_occupancy = occupancy;

	bishop_occupancy &= bishop_masks[square];
	bishop_occupancy *= bishop_magics[square];
	bishop_occupancy >>= 64 - relevant_bishop_bits[square];

	queen_attacks |= bishop_attacks[square][bishop_occupancy];

	rook_occupancy &= rook_masks[square];
	rook_occupancy *= rook_magics[square];
	rook_occupancy >>= 64 - relevant_rook_bits[square];

	queen_attacks |= rook_attacks[square][rook_occupancy];

	return queen_attacks;
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

void init_sliders_attack_tables(Piece piece) {
	if (piece != bishop && piece != rook) {
		return;
	}

	for (int square{ 0 }; square < 64; square++)
	{
		bishop_masks[square] = generate_bishop_attacks(square);
		rook_masks[square] = generate_rook_attacks(square);

		U64 attack_mask = piece == bishop ? bishop_masks[square] : rook_masks[square];
		int relevant_bits_count = count_bits(attack_mask);
		int occupancy_indices = 1 << relevant_bits_count;

		for (int index{ 0 }; index < occupancy_indices; index++) {
			if (piece == bishop) {
				U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
				int magic_index = (occupancy * bishop_magics[square]) >> (64 - relevant_bishop_bits[square]);
				bishop_attacks[square][magic_index] = generate_bishop_attacks_on_the_fly(square, occupancy);
			}
			else {
				U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
				int magic_index = (occupancy * rook_magics[square]) >> (64 - relevant_rook_bits[square]);
				rook_attacks[square][magic_index] = generate_rook_attacks_on_the_fly(square, occupancy);
			}
		}
	}
}

unsigned int state = 12332112377;

unsigned int get_random_u32_number() {
	unsigned int number = state;

	number ^= number << 13;
	number ^= number >> 17;
	number ^= number << 5;

	state = number;

	return number;
}

U64 get_random_u64_number() {
	U64 n1, n2, n3, n4;

	n1 = (U64)(get_random_u32_number()) & 0xFFFF;
	n2 = (U64)(get_random_u32_number()) & 0xFFFF;
	n3 = (U64)(get_random_u32_number()) & 0xFFFF;
	n4 = (U64)(get_random_u32_number()) & 0xFFFF;

	return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

bool is_square_attacked(int square, Color side) {
	if (side == white) {
		if (pawn_attacks[black][square] & piece_occupancies[P]) {
			return true;
		}
		if (king_attacks[square] & piece_occupancies[K]) {
			return true;
		}
		if (knight_attacks[square] & piece_occupancies[N]) {
			return true;
		}
		if (get_bishop_attacks(square, color_occupancies[none]) & piece_occupancies[B]) {
			return true;
		}
		if (get_rook_attacks(square, color_occupancies[none]) & piece_occupancies[R]) {
			return true;
		}
		if (get_queen_attacks(square, color_occupancies[none]) & piece_occupancies[Q]) {
			return true;
		}
	}
	else if (side == black) {
		if (pawn_attacks[white][square] & piece_occupancies[p]) {
			return true;
		}
		if (king_attacks[square] & piece_occupancies[k]) {
			return true;
		}
		if (knight_attacks[square] & piece_occupancies[n]) {
			return true;
		}
		if (get_bishop_attacks(square, color_occupancies[none]) & piece_occupancies[b]) {
			return true;
		}
		if (get_rook_attacks(square, color_occupancies[none]) & piece_occupancies[r]) {
			return true;
		}
		if (get_queen_attacks(square, color_occupancies[none]) & piece_occupancies[q]) {
			return true;
		}
	}

	return false;
}

void print_all_attacks(Color side) {
	const char* GREEN = "\033[92m";
	const char* RESET = "\033[0m";

	cout << endl;
	for (int rank{ 0 }; rank < 8; ++rank) {
		cout << (8 - rank) << "| ";

		for (int file{ 0 }; file < 8; ++file) {
			int square = rank * 8 + file;

			if (is_square_attacked(square, side))
				cout << GREEN << "1" << RESET << " ";
			else
				cout << "0 ";
		}

		cout << endl;
	}

	cout << "   ________________" << endl;
	cout << "   a b c d e f g h" << endl;
}

//U64 generate_magic_number_candidate() {
//	return get_random_u64_number() & get_random_u64_number() & get_random_u64_number();
//}
//
//U64 find_magic_number(int square, int relevant_bits, Piece piece) {
//	U64 occupancies[4096];
//	U64 attacks[4096];
//	U64 used_attacks[4096];
//
//	U64 attack_mask{ 0ULL };
//
//	if (piece == bishop) {
//		attack_mask = generate_bishop_attacks(square);
//	}
//	else if (piece == rook) {
//		attack_mask = generate_rook_attacks(square);
//	}
//	else {
//		return 0ULL;
//	}
//
//	U64 occupancy_indices = 1 << relevant_bits;
//
//	for (int index{ 0 }; index < occupancy_indices; index++) {
//		occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);
//		attacks[index] = piece == bishop ? generate_bishop_attacks_on_the_fly(square, occupancies[index])
//			: generate_rook_attacks_on_the_fly(square, occupancies[index]);
//	}
//
//	for (int index{ 0 }; index < 10000000; index++) {
//		U64 magic_number_candidate = generate_magic_number_candidate();
//
//		if (count_bits((attack_mask * magic_number_candidate) & 0xFF00000000000000) < 6) {
//			continue;
//		}
//
//		std::memset(used_attacks, 0ULL, sizeof(used_attacks));
//
//		// test magic index
//
//		bool flag { false };
//
//		for (int index{ 0 }; !flag && index < occupancy_indices; index++) {
//			int magic_index = (int)((occupancies[index] * magic_number_candidate) >> (64 - relevant_bits));
//
//			if (used_attacks[magic_index] == 0ULL) {
//				used_attacks[magic_index] = attacks[index];
//			}
//			else if (used_attacks[magic_index] != attacks[index]) {
//				flag = true;
//			}
//		}
//
//		if (!flag) {
//			return magic_number_candidate;
//		}
//	}
//
//	return 0ULL;
//}
//
//void init_bishop_magic_numbers() {
//	for (int square{ 0 }; square < 64; square++) {
//		cout << find_magic_number(square, relevant_bishop_bits[square], bishop) << "ULL," << endl;
//	}
//}
//
//void init_rook_magic_numbers() {
//	for (int square{ 0 }; square < 64; square++) {
//		cout << find_magic_number(square, relevant_rook_bits[square], rook) << "ULL," << endl;
//	}
//}

int main()
{
	init_sliders_attack_tables(bishop);
	init_sliders_attack_tables(rook);
	init_all_pawn_attacks();
	init_all_king_attacks();
	init_all_knight_attacks();

	parse_fen(tricky_pos_fen);

	cout << endl;

	print_all_attacks(white);

	return 0;
}
