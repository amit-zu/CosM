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
#include <chrono>
#include <cctype>
#include <cstdlib>

using namespace std;
using U64 = unsigned long long;
using U8 = uint8_t;

static const auto program_start = std::chrono::steady_clock::now();

// fen string
string empty_board_fen = "8/8/8/8/8/8/8/8 w - - 0 1";
string starting_pos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
string tricky_pos_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

const U64 not_a_file{ 18374403900871474942ULL };
const U64 not_ab_file{ 18229723555195321596ULL };
const U64 not_h_file{ 9187201950435737471ULL };
const U64 not_gh_file{ 4557430888798830399ULL };

const U64 bishop_magics[64]{
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

// castling rights update constants
const int castling_rights[64] = {
	 7, 15, 15, 15,  3, 15, 15, 11,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	13, 15, 15, 15, 12, 15, 15, 14
};

enum Color {
	white, black, both
};

enum Piece {
	pawn, king, queen, knight, bishop, rook, no_piece
};

enum PieceChar {
	X, P, K, Q, N, B, R, p, k, q, n, b, r // DO NOT CHANGE ORDER
};

char piece_chars[14] = "-PKQNBRpkqnbr"; // DO NOT CHANGE ORDER

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

enum MoveType {
	general_move, capture_move
};

struct MoveList {
	int moves[256];
	int count;
};

struct BoardState {
	U64 bitboards[13];
	U64 occupancies[3]; //white, black, none
	Color side;
	int enpassant, castle;
};

BoardState board_current{};
BoardState board_copy{};

constexpr int encode_move(int source, int target, int piece, int promoted,
	bool capture, bool double_push,
	bool enpassant, bool castling) {
	return source
		| (target << 6)
		| (piece << 12)
		| (promoted << 16)
		| (capture << 20)
		| (double_push << 21)
		| (enpassant << 22)
		| (castling << 23);
}

constexpr int get_move_source(int move) {
	return move & 0x3f;
}

constexpr int get_move_target(int move) {
	return (move & 0xfc0) >> 6;
}

constexpr int get_move_piece(int move) {
	return (move & 0xf000) >> 12;
}

constexpr int get_move_promoted_piece(int move) {
	return (move & 0xf0000) >> 16;
}

constexpr bool get_move_capture_flag(int move) {
	return (move & 0x100000) >> 20;
}

constexpr bool get_move_double_push_flag(int move) {
	return (move & 0x200000) >> 21;
}

constexpr bool get_move_enpassant_flag(int move) {
	return (move & 0x400000) >> 22;
}

constexpr bool get_move_castle_flag(int move) {
	return (move & 0x800000) >> 23;
}

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

string get_move_string(int move) {
	if (move == 0) {
		return "illegal move";
	}

	std::string result;

	// piece (if you actually want it)
	result += piece_chars[get_move_piece(move)];

	// source square
	result += square_to_coordinate(get_move_source(move));

	// target square
	result += square_to_coordinate(get_move_target(move));

	// promotion (only if exists)
	int promoted = get_move_promoted_piece(move);
	if (promoted != X) {
		result += piece_chars[promoted];
	}

	return result;
}

void print_move(int move) {
	cout << get_move_string(move) << endl;
}

void print_move_list(MoveList& moves) {
	if (moves.count == 0) {
		cout << "no moves" << endl;
		return;
	}

	for (int i = 0; i < moves.count; i++) {
		print_move(moves.moves[i]);
	}
}

void add_move(int move, MoveList& moves) {
	moves.moves[moves.count] = move;
	moves.count++;
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
		return count_bits((bb & -bb) - 1);
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

			for (int color_piece = P; color_piece <= r; color_piece++) {
				U64 piece_occupancy = board_current.bitboards[color_piece];

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

	cout << "    ________________" << endl;
	cout << "    a b c d e f g h" << endl << endl;

	cout << std::format("to move: {}, ", ((board_current.side == white) ? "white" : ((board_current.side == black) ? "black" : "none")));

	cout << std::format("en passant: {}, ", (board_current.enpassant != no_square) ? square_to_coordinate(board_current.enpassant) : "-");

	cout << std::format("castle: {}{}{}{}",
		(board_current.castle & wk) ? "K" : "-",
		(board_current.castle & wq) ? "Q" : "-",
		(board_current.castle & bk) ? "k" : "-",
		(board_current.castle & bq) ? "q" : "-");

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
	std::memset(board_current.bitboards, 0ULL, sizeof(board_current.bitboards));
	std::memset(board_current.occupancies, 0ULL, sizeof(board_current.occupancies));

	board_current.side = both;
	board_current.enpassant = no_square;
	board_current.castle = 0;

	vector<string> fen_parts = split(fen, ' ');

	string pieces_string = fen_parts[0];
	string side_to_move_string = fen_parts[1];
	string castling_string = fen_parts[2];
	string en_passant_string = fen_parts[3];
	//string half_move_string = fen_parts[4];
	//string full_move_string = fen_parts[5];

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
				set_bit(board_current.bitboards[piece], square);
				square++;
			}
		}
	}

	for (char c : castling_string) {
		switch (c) {
		case '-': break;
		case 'K': board_current.castle |= wk; break;
		case 'Q': board_current.castle |= wq; break;
		case 'k': board_current.castle |= bk; break;
		case 'q': board_current.castle |= bq; break;
		}
	}

	board_current.side = (side_to_move_string == "w") ? white : black;

	board_current.enpassant = (en_passant_string == "-") ? no_square : coordinate_to_square(en_passant_string);

	for (int piece = P; piece <= R; piece++) {
		board_current.occupancies[white] |= board_current.bitboards[piece];
	}

	for (int piece = p; piece <= r; piece++) {
		board_current.occupancies[black] |= board_current.bitboards[piece];
	}

	board_current.occupancies[both] = board_current.occupancies[white] | board_current.occupancies[black];
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
	else if (color == black) {
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
		if (pawn_attacks[black][square] & board_current.bitboards[P]) {
			return true;
		}
		if (king_attacks[square] & board_current.bitboards[K]) {
			return true;
		}
		if (knight_attacks[square] & board_current.bitboards[N]) {
			return true;
		}
		if (get_bishop_attacks(square, board_current.occupancies[both]) & board_current.bitboards[B]) {
			return true;
		}
		if (get_rook_attacks(square, board_current.occupancies[both]) & board_current.bitboards[R]) {
			return true;
		}
		if (get_queen_attacks(square, board_current.occupancies[both]) & board_current.bitboards[Q]) {
			return true;
		}
	}
	else if (side == black) {
		if (pawn_attacks[white][square] & board_current.bitboards[p]) {
			return true;
		}
		if (king_attacks[square] & board_current.bitboards[k]) {
			return true;
		}
		if (knight_attacks[square] & board_current.bitboards[n]) {
			return true;
		}
		if (get_bishop_attacks(square, board_current.occupancies[both]) & board_current.bitboards[b]) {
			return true;
		}
		if (get_rook_attacks(square, board_current.occupancies[both]) & board_current.bitboards[r]) {
			return true;
		}
		if (get_queen_attacks(square, board_current.occupancies[both]) & board_current.bitboards[q]) {
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

void generate_moves(MoveList& moves) {
	moves.count = 0;

	int from_square, target_square;

	U64 bitboard, attacks;

	for (int piece = P; piece <= r; piece++) {
		bitboard = board_current.bitboards[piece];

		if (board_current.side == white) {
			if (piece == P) {

				while (bitboard) {

					from_square = get_least_significant_1_bit(bitboard);
					target_square = from_square - 8;

					if (!(target_square < a8) && !get_bit(board_current.occupancies[both], target_square)) {
						if (from_square >= a7 && from_square <= h7) { // promotion
							add_move(encode_move(from_square, target_square, P, Q, false, false, false, false), moves);
							add_move(encode_move(from_square, target_square, P, R, false, false, false, false), moves);
							add_move(encode_move(from_square, target_square, P, B, false, false, false, false), moves);
							add_move(encode_move(from_square, target_square, P, N, false, false, false, false), moves);

						}
						else {
							add_move(encode_move(from_square, target_square, P, X, false, false, false, false), moves);

							if (from_square >= a2 && from_square <= h2 &&
								!get_bit(board_current.occupancies[both], target_square - 8)) { // double pawn push
								add_move(encode_move(from_square, target_square - 8, P, X, false, true, false, false), moves);
							}
						}
					}

					attacks = pawn_attacks[board_current.side][from_square] & board_current.occupancies[black];

					while (attacks) {
						target_square = get_least_significant_1_bit(attacks);

						if (from_square >= a7 && from_square <= h7) { // promotion
							add_move(encode_move(from_square, target_square, P, Q, true, false, false, false), moves);
							add_move(encode_move(from_square, target_square, P, R, true, false, false, false), moves);
							add_move(encode_move(from_square, target_square, P, B, true, false, false, false), moves);
							add_move(encode_move(from_square, target_square, P, N, true, false, false, false), moves);
						}
						else {
							add_move(encode_move(from_square, target_square, P, X, true, false, false, false), moves);
						}

						pop_bit(attacks, target_square);
					}

					// generate en passant captures

					if (board_current.enpassant != no_square) {
						U64 enpassant_attacks = pawn_attacks[board_current.side][from_square] & (1ULL << board_current.enpassant);

						if (enpassant_attacks) {
							int target_enpassant = get_least_significant_1_bit(enpassant_attacks);

							add_move(encode_move(from_square, target_enpassant, P, X, true, false, true, false), moves);
						}
					}

					pop_bit(bitboard, from_square);
				}
			}

			if (piece == K) {

				if (board_current.castle & wk) {
					if (!get_bit(board_current.occupancies[both], f1) && !get_bit(board_current.occupancies[both], g1)) {
						if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black) && !is_square_attacked(g1, black)) {
							add_move(encode_move(e1, g1, K, X, false, false, false, true), moves);
						}
					}
				}

				if (board_current.castle & wq) {
					if (!get_bit(board_current.occupancies[both], d1) && !get_bit(board_current.occupancies[both], c1) && !get_bit(board_current.occupancies[both], b1)) {
						if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black) && !is_square_attacked(c1, black)) {
							add_move(encode_move(e1, c1, K, X, false, false, false, true), moves);
						}
					}
				}

			}
		}
		else if (board_current.side == black) {
			if (piece == p) {

				while (bitboard) {

					from_square = get_least_significant_1_bit(bitboard);
					target_square = from_square + 8;

					if (!(target_square > h1) && !get_bit(board_current.occupancies[both], target_square)) {
						if (from_square >= a2 && from_square <= h2) { // promotion
							add_move(encode_move(from_square, target_square, p, q, false, false, false, false), moves);
							add_move(encode_move(from_square, target_square, p, r, false, false, false, false), moves);
							add_move(encode_move(from_square, target_square, p, b, false, false, false, false), moves);
							add_move(encode_move(from_square, target_square, p, n, false, false, false, false), moves);
						}
						else {
							add_move(encode_move(from_square, target_square, p, X, false, false, false, false), moves);

							if (from_square >= a7 && from_square <= h7 &&
								!get_bit(board_current.occupancies[both], target_square + 8)) { // double pawn push
								add_move(encode_move(from_square, target_square + 8, p, X, false, true, false, false), moves);
							}
						}
					}

					attacks = pawn_attacks[board_current.side][from_square] & board_current.occupancies[white];

					while (attacks) {
						target_square = get_least_significant_1_bit(attacks);

						if (from_square >= a2 && from_square <= h2) { // promotion
							add_move(encode_move(from_square, target_square, p, q, true, false, false, false), moves);
							add_move(encode_move(from_square, target_square, p, r, true, false, false, false), moves);
							add_move(encode_move(from_square, target_square, p, b, true, false, false, false), moves);
							add_move(encode_move(from_square, target_square, p, n, true, false, false, false), moves);
						}
						else {
							add_move(encode_move(from_square, target_square, p, X, true, false, false, false), moves);
						}

						pop_bit(attacks, target_square);
					}

					// generate en passant captures

					if (board_current.enpassant != no_square) {
						U64 enpassant_attacks = pawn_attacks[board_current.side][from_square] & (1ULL << board_current.enpassant);

						if (enpassant_attacks) {
							int target_enpassant = get_least_significant_1_bit(enpassant_attacks);

							add_move(encode_move(from_square, target_enpassant, p, X, true, false, true, false), moves);
						}
					}

					pop_bit(bitboard, from_square);
				}
			}

			if (piece == k) {

				if (board_current.castle & bk) {
					if (!get_bit(board_current.occupancies[both], f8) && !get_bit(board_current.occupancies[both], g8)) {
						if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white) && !is_square_attacked(g8, white)) {
							add_move(encode_move(e8, g8, k, X, false, false, false, true), moves);
						}
					}
				}

				if (board_current.castle & bq) {
					if (!get_bit(board_current.occupancies[both], d8) && !get_bit(board_current.occupancies[both], c8) && !get_bit(board_current.occupancies[both], b8)) {
						if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white) && !is_square_attacked(c8, white)) {
							add_move(encode_move(e8, c8, k, X, false, false, false, true), moves);
						}
					}
				}
			}
		}

		if ((board_current.side == white) ? piece == N : piece == n) {
			while (bitboard) {
				from_square = get_least_significant_1_bit(bitboard);

				attacks = knight_attacks[from_square] & ((board_current.side == white) ? ~board_current.occupancies[white] : ~board_current.occupancies[black]);

				while (attacks) {
					target_square = get_least_significant_1_bit(attacks);

					bool is_capture = get_bit(
						(board_current.side == white) ? board_current.occupancies[black] : board_current.occupancies[white],
						target_square
					);

					add_move(encode_move(from_square, target_square, (board_current.side == white) ? N : n, X, is_capture, false, false, false), moves);

					pop_bit(attacks, target_square);
				}

				pop_bit(bitboard, from_square);
			}
		}

		if ((board_current.side == white) ? piece == B : piece == b) {
			while (bitboard) {
				from_square = get_least_significant_1_bit(bitboard);

				attacks = get_bishop_attacks(from_square, board_current.occupancies[both]) & ((board_current.side == white) ? ~board_current.occupancies[white] : ~board_current.occupancies[black]);

				while (attacks) {
					target_square = get_least_significant_1_bit(attacks);

					bool is_capture = get_bit(
						(board_current.side == white) ? board_current.occupancies[black] : board_current.occupancies[white],
						target_square
					);

					add_move(encode_move(from_square, target_square, (board_current.side == white) ? B : b, X, is_capture, false, false, false), moves);

					pop_bit(attacks, target_square);
				}

				pop_bit(bitboard, from_square);
			}
		}

		if ((board_current.side == white) ? piece == R : piece == r) {
			while (bitboard) {
				from_square = get_least_significant_1_bit(bitboard);

				attacks = get_rook_attacks(from_square, board_current.occupancies[both]) & ((board_current.side == white) ? ~board_current.occupancies[white] : ~board_current.occupancies[black]);

				while (attacks) {
					target_square = get_least_significant_1_bit(attacks);

					bool is_capture = get_bit(
						(board_current.side == white) ? board_current.occupancies[black] : board_current.occupancies[white],
						target_square
					);

					add_move(encode_move(from_square, target_square, (board_current.side == white) ? R : r, X, is_capture, false, false, false), moves);

					pop_bit(attacks, target_square);
				}

				pop_bit(bitboard, from_square);
			}
		}

		if ((board_current.side == white) ? piece == Q : piece == q) {
			while (bitboard) {
				from_square = get_least_significant_1_bit(bitboard);

				attacks = get_queen_attacks(from_square, board_current.occupancies[both]) & ((board_current.side == white) ? ~board_current.occupancies[white] : ~board_current.occupancies[black]);

				while (attacks) {
					target_square = get_least_significant_1_bit(attacks);

					bool is_capture = get_bit(
						(board_current.side == white) ? board_current.occupancies[black] : board_current.occupancies[white],
						target_square
					);

					add_move(encode_move(from_square, target_square, (board_current.side == white) ? Q : q, X, is_capture, false, false, false), moves);

					pop_bit(attacks, target_square);
				}

				pop_bit(bitboard, from_square);
			}
		}

		if ((board_current.side == white) ? piece == K : piece == k) {
			while (bitboard) {
				from_square = get_least_significant_1_bit(bitboard);

				attacks = king_attacks[from_square] & ((board_current.side == white) ? ~board_current.occupancies[white] : ~board_current.occupancies[black]);

				while (attacks) {
					target_square = get_least_significant_1_bit(attacks);

					bool is_capture = get_bit(
						(board_current.side == white) ? board_current.occupancies[black] : board_current.occupancies[white],
						target_square
					);

					add_move(encode_move(from_square, target_square, (board_current.side == white) ? K : k, X, is_capture, false, false, false), moves);

					pop_bit(attacks, target_square);
				}

				pop_bit(bitboard, from_square);
			}
		}
	}
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

// WARNING: NOT safe for use in recursive algorithms!
// Use local BoardState backup instead.
void copy_board() {
	std::memcpy(&board_copy, &board_current, sizeof(board_copy));
}
// WARNING: NOT safe for use in recursive algorithms!
// Use local BoardState backup instead.
void take_back() {
	std::memcpy(&board_current, &board_copy, sizeof(board_current));
}

bool make_move(int move, MoveType type) {
	if (type == general_move) {
		copy_board();

		int from_square = get_move_source(move);
		int target_square = get_move_target(move);
		int move_piece = get_move_piece(move);
		int move_promoted_piece = get_move_promoted_piece(move);
		bool move_capture_flag = get_move_capture_flag(move);
		bool move_double_push_flag = get_move_double_push_flag(move);
		bool move_enpassant_flag = get_move_enpassant_flag(move);
		bool move_castle_flag = get_move_castle_flag(move);

		// move piece
		pop_bit(board_current.bitboards[move_piece], from_square);
		set_bit(board_current.bitboards[move_piece], target_square);

		// handle captures
		if (move_capture_flag) {
			int start_piece = 0, end_piece = 0;

			if (board_current.side == white) {
				start_piece = p;
				end_piece = r;
			}
			else if (board_current.side == black) {
				start_piece = P;
				end_piece = R;
			}

			for (int piece_index = start_piece; piece_index <= end_piece; piece_index++) {
				if (get_bit(board_current.bitboards[piece_index], target_square)) {
					pop_bit(board_current.bitboards[piece_index], target_square);
					break;
				}
			}
		}

		// handle promotion
		if (move_promoted_piece != X) {
			pop_bit(board_current.bitboards[board_current.side == white ? P : p], target_square);

			set_bit(board_current.bitboards[move_promoted_piece], target_square);
		}

		// handle en passant
		if (move_enpassant_flag) {
			(board_current.side == white) ?
				pop_bit(board_current.bitboards[p], target_square + 8) :
				pop_bit(board_current.bitboards[P], target_square - 8);
		}

		board_current.enpassant = no_square;

		// handle double pawn push
		if (move_double_push_flag) {
			board_current.enpassant = (board_current.side == white) ? target_square + 8 : target_square - 8;
		}

		// handle castling
		if (move_castle_flag) {
			switch (target_square) {
			case g1: { // white kingside'
				pop_bit(board_current.bitboards[R], h1);
				set_bit(board_current.bitboards[R], f1);
				break;
			}
			case c1: { // white queenside
				pop_bit(board_current.bitboards[R], a1);
				set_bit(board_current.bitboards[R], d1);
				break;
			}
			case g8: { // black kingside
				pop_bit(board_current.bitboards[r], h8);
				set_bit(board_current.bitboards[r], f8);
				break;
			}
			case c8: { // black queenside
				pop_bit(board_current.bitboards[r], a8);
				set_bit(board_current.bitboards[r], d8);
				break;
			}
			}
		}

		// update castling rights
		board_current.castle &= castling_rights[from_square];
		board_current.castle &= castling_rights[target_square];

		// update occupancies
		std::memset(board_current.occupancies, 0ULL, sizeof(board_current.occupancies));

		for (int piece = P; piece <= R; piece++) {
			board_current.occupancies[white] |= board_current.bitboards[piece];
		}

		for (int piece = p; piece <= r; piece++) {
			board_current.occupancies[black] |= board_current.bitboards[piece];
		}

		board_current.occupancies[both] = board_current.occupancies[white] | board_current.occupancies[black];

		// change turn
		board_current.side = (board_current.side == white) ? black : white;

		int king_square = get_least_significant_1_bit(board_current.side == white ? board_current.bitboards[k] : board_current.bitboards[K]);

		if (is_square_attacked(king_square, board_current.side)) {
			// illegal move, take it back

			take_back();

			return false;
		}
		return true;
	}
	else {
		if (get_move_capture_flag(move)) {
			make_move(move, general_move);
		}
		else {
			return false;
		}
	}
}

long long get_time_ms()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(steady_clock::now() - program_start).count();
}

// leaf nodes (number of positions reached for a given depth)
long nodes;

void perft_driver(int depth) {
	if (depth == 0) {
		nodes++;
		return;
	}

	MoveList move_list{};
	generate_moves(move_list);

	for (int i = 0; i < move_list.count; i++) {
		BoardState backup = board_current;

		if (!make_move(move_list.moves[i], general_move)) {
			board_current = backup;
			continue;
		}

		perft_driver(depth - 1);

		board_current = backup;
	}
}

void perft_test(int depth) {
	cout << "---performance test--- " << endl;

	MoveList move_list{};
	generate_moves(move_list);

	long long start_time = get_time_ms();

	for (int i = 0; i < move_list.count; i++) {
		BoardState backup = board_current;

		if (!make_move(move_list.moves[i], general_move)) {
			board_current = backup;
			continue;
		}

		long cumulative_nodes = nodes;

		perft_driver(depth - 1);

		long old_nodes = nodes - cumulative_nodes;

		board_current = backup;

		cout << piece_chars[get_move_piece(move_list.moves[i])];
		cout << square_to_coordinate(get_move_source(move_list.moves[i]));
		cout << square_to_coordinate(get_move_target(move_list.moves[i]));
		cout << piece_chars[get_move_promoted_piece(move_list.moves[i])];
		cout << " nodes: " << old_nodes << endl;
	}

	cout << "depth: " << depth << endl;
	cout << "nodes: " << nodes << endl;
	cout << "time:  " << (get_time_ms() - start_time) << "ms" << endl;
}

int material_score[13] = {
	0, // no piece
	100, // white pawn
	10000, // white king
	1000, // white queen
	300, // white knight
	350, // white bishop
	500, // white rook
	-100, // black pawn
	-10000, // black king
	-1000, // black queen
	-300, // black knight
	-350, // black bishop
	-500, // black rook
};

// pawn positional score
const int pawn_score[64] =
{
	90,  90,  90,  90,  90,  90,  90,  90,
	30,  30,  30,  40,  40,  30,  30,  30,
	20,  20,  20,  30,  30,  30,  20,  20,
	10,  10,  10,  20,  20,  10,  10,  10,
	 5,   5,  10,  20,  20,   5,   5,   5,
	 0,   0,   0,   5,   5,   0,   0,   0,
	 0,   0,   0, -10, -10,   0,   0,   0,
	 0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knight_score[64] =
{
	-5,   0,   0,   0,   0,   0,   0,  -5,
	-5,   0,   0,  10,  10,   0,   0,  -5,
	-5,   5,  20,  20,  20,  20,   5,  -5,
	-5,  10,  20,  30,  30,  20,  10,  -5,
	-5,  10,  20,  30,  30,  20,  10,  -5,
	-5,   5,  20,  10,  10,  20,   5,  -5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
	-5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const int bishop_score[64] =
{
	 0,   0,   0,   0,   0,   0,   0,   0,
	 0,   0,   0,   0,   0,   0,   0,   0,
	 0,   0,   0,  10,  10,   0,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,  10,   0,   0,   0,   0,  10,   0,
	 0,  30,   0,   0,   0,   0,  30,   0,
	 0,   0, -10,   0,   0, -10,   0,   0

};

// rook positional score
const int rook_score[64] =
{
	50,  50,  50,  50,  50,  50,  50,  50,
	50,  50,  50,  50,  50,  50,  50,  50,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
const int king_score[64] =
{
	 0,   0,   0,   0,   0,   0,   0,   0,
	 0,   0,   5,   5,   5,   5,   0,   0,
	 0,   5,   5,  10,  10,   5,   5,   0,
	 0,   5,  10,  20,  20,  10,   5,   0,
	 0,   5,  10,  20,  20,  10,   5,   0,
	 0,   0,   5,  10,  10,   5,   0,   0,
	 0,   5,   5,  -5,  -5,   0,   5,   0,
	 0,   0,   5,   0, -15,   0,  10,   0
};

// mirror positional score tables for opposite side
const int mirror_score[128] =
{
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8
};

int evaluate() {
	int score = 0;

	U64 bitboard;

	int piece, square;

	for (int bb_piece = P; bb_piece <= r; bb_piece++) {
		bitboard = board_current.bitboards[bb_piece];

		while (bitboard) {
			piece = bb_piece;
			square = get_least_significant_1_bit(bitboard);

			score += material_score[piece];

			switch (piece) {
			case P: score += pawn_score[square]; break;
			case K: score += king_score[square]; break;
			case N: score += knight_score[square]; break;
			case B: score += bishop_score[square]; break;
			case R: score += rook_score[square]; break;
			case p: score -= pawn_score[square]; break;
			case k: score -= king_score[square]; break;
			case n: score -= knight_score[square]; break;
			case b: score -= bishop_score[square]; break;
			case r: score -= rook_score[square]; break;
			}

			pop_bit(bitboard, square);
		}
	}

	return (board_current.side == white) ? score : -score;
}

// half move
int ply = 0;

int best_move;

int nega_max(int alpha, int beta, int depth) {
	if (depth == 0) {
		return evaluate();
	}

	nodes++;

	int legal_moves = 0;

	MoveList move_list;
	generate_moves(move_list);

	for (int move_count = 0; move_count < move_list.count; move_count++) {
		BoardState backup = board_current;

		ply++;

		if (make_move(move_list.moves[move_count], general_move) == 0) {
			ply--;
			continue;
		}

		legal_moves++;

		int score = -nega_max(-beta, -alpha, depth - 1);

		board_current = backup;

		ply--;

		if (score >= beta) {
			return beta;
		}

		if (score > alpha) {
			alpha = score;

			if (ply == 0) {
				best_move = move_list.moves[move_count];
			}
		}
	}

	// No legal moves - checkmate or stalemate
	if (legal_moves == 0) {
		// TODO: Distinguish between checkmate and stalemate
		// For now, treat as checkmate
		return -49000 + ply;  // prefer longer mates
	}

	return alpha;
}

void search_position(int depth) {
	int score = nega_max(-50000, 50000, depth);

	print_move(best_move);
}

int parse_move(const char* move_string) {
	if (!move_string)
		return 0;

	// determine token length only until space or newline
	size_t len = 0;
	while (move_string[len] && move_string[len] != ' ' && move_string[len] != '\n')
		len++;

	if (len < 4 || len > 5)
		return 0;

	MoveList moves{};
	generate_moves(moves);

	int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;
	int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;

	for (int move_count = 0; move_count < moves.count; move_count++) {
		int move = moves.moves[move_count];

		if (source_square == get_move_source(move) &&
			target_square == get_move_target(move))
		{
			int promoted_piece = get_move_promoted_piece(move);

			if (promoted_piece != X) {
				if (len != 5)
					return 0;

				char promoted_piece_char = move_string[4];

				if (std::tolower(static_cast<unsigned char>(promoted_piece_char)) ==
					std::tolower(static_cast<unsigned char>(piece_chars[promoted_piece])))
				{
					return move;
				}

				continue;
			}

			if (len == 5)
				return 0;

			return move;
		}
	}

	return 0;
}

void parse_position(const char* command) {
	command += 9;

	const char* current_char = command;

	if (strncmp(command, "startpos", 8) == 0) {
		parse_fen(starting_pos_fen);
	}
	else {
		current_char = strstr(command, "fen");

		if (current_char == NULL) {
			parse_fen(starting_pos_fen);
		}
		else {
			current_char += 4;
			parse_fen(current_char);
		}
	}

	current_char = strstr(command, "moves");

	if (current_char != NULL) {
		current_char += 6;

		while (*current_char) {
			while (*current_char == ' ')
				current_char++;

			if (!*current_char)
				break;

			int move = parse_move(current_char);

			if (move == 0) {
				break;
			}

			make_move(move, general_move);

			while (*current_char && *current_char != ' ')
				current_char++;
		}
	}
}

void parse_go(const char* command) {
	int depth = 6;  // default depth

	const char* depth_ptr = strstr(command, "depth");

	if (depth_ptr != NULL) {
		depth = atoi(depth_ptr + 6);  // "depth " is 6 characters
	}

	search_position(depth);
}

void uci_loop() {
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	char input[2000];

	cout << "CosM 0.1.0 by Amit Zuarets" << endl;

	while (true) {
		memset(input, 0, sizeof(input));
		fflush(stdout);

		if (!fgets(input, 2000, stdin)) {
			continue;
		}
		else if (input[0] == '\n') {
			continue;
		}
		else if (strncmp(input, "isready", 7) == 0) {
			cout << "readyok" << endl;
			continue;
		}
		else if (strncmp(input, "position", 8) == 0) {
			parse_position(input);
		}
		else if (strncmp(input, "ucinewgame", 10) == 0) {
			parse_position("position startpos");
		}
		else if (strncmp(input, "go", 2) == 0) {
			parse_go(input);
		}
		else if (strncmp(input, "quit", 4) == 0) {
			break;
		}
		else if (strncmp(input, "uci", 3) == 0) {
			cout << "id name CosM 0.1.0" << endl;
			cout << "id author Amit Zuarets" << endl;
			cout << "uciok" << endl;
		}
		else if (strncmp(input, "d", 1) == 0) {
			cout << endl;
			print_board();
			cout << endl;
		}
	}
}

int main() {
	init_sliders_attack_tables(bishop);
	init_sliders_attack_tables(rook);
	init_all_pawn_attacks();
	init_all_king_attacks();
	init_all_knight_attacks();

	uci_loop();

	return 0;
}
