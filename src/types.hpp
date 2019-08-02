#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <string>

#define U64_FILE_A 0x0101010101010101ULL
#define U64_FILE_B 0x0202020202020202ULL
#define U64_FILE_C 0x0404040404040404ULL
#define U64_FILE_D 0x0808080808080808ULL
#define U64_FILE_E 0x1010101010101010ULL
#define U64_FILE_F 0x2020202020202020ULL
#define U64_FILE_G 0x4040404040404040ULL
#define U64_FILE_H 0x8080808080808080ULL
#define U64_RANK_1 0x00000000000000FFULL
#define U64_RANK_2 0x000000000000FF00ULL
#define U64_RANK_3 0x0000000000FF0000ULL
#define U64_RANK_4 0x00000000FF000000ULL
#define U64_RANK_5 0x000000FF00000000ULL
#define U64_RANK_6 0x0000FF0000000000ULL
#define U64_RANK_7 0x00FF000000000000ULL
#define U64_RANK_8 0xFF00000000000000ULL
#define U64_CENTER                                        \
    (U64_FILE_C | U64_FILE_D | U64_FILE_E | U64_FILE_F) & \
        (U64_RANK_3 | U64_RANK_4 | U64_RANK_5 | U64_RANK_6)

#define U64_A1 (U64_FILE_A & U64_RANK_1)
#define U64_B1 (U64_FILE_B & U64_RANK_1)
#define U64_C1 (U64_FILE_C & U64_RANK_1)
#define U64_D1 (U64_FILE_D & U64_RANK_1)
#define U64_E1 (U64_FILE_E & U64_RANK_1)
#define U64_F1 (U64_FILE_F & U64_RANK_1)
#define U64_G1 (U64_FILE_G & U64_RANK_1)
#define U64_H1 (U64_FILE_H & U64_RANK_1)

#define INF 1000000
#define MAX_DEPTH 256
#define MAX_MOVES 256

// clang-format off
const std::string SquareString[] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};
// clang-format on

const char piece_char[] = {'P', 'N', 'B', 'R', 'Q', 'K', '-'};

const std::string piece_string[] =
    {"pawn", "knight", "bishop", "rook", "queen", "king", "none"};

// clang-format off
enum Square : int
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};
// clang-format on

enum Colour : bool
{
    US,
    THEM
};

enum Castling : int
{
    usKSC,
    usQSC,
    themKSC,
    themQSC
};

enum PieceType : int
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
};

enum MoveType : int
{
    QUIET,
    DOUBLE,
    CAPTURE,
    ENPASSANT,
    KSC,
    QSC,
    PROMO,
    PROMO_CAPTURE
};

#endif
