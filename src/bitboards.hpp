#ifndef BITBOARDS_HPP
#define BITBOARDS_HPP

void bitboards_init();
uint64_t get_file(int file);
uint64_t get_adj_files(int file);
uint64_t magic_moves_pawn(bool side, int sq);
uint64_t magic_moves_knight(int sq);
uint64_t magic_moves_bishop(uint64_t occ, int sq);
uint64_t magic_moves_rook(uint64_t occ, int sq);
uint64_t magic_moves_queen(uint64_t occ, int sq);
uint64_t magic_moves_king(int sq);
bool is_passed_pawn(int side, int sq, uint64_t blockers);
bool is_backward_pawn_us(int sq, uint64_t friendly, uint64_t enemy);
bool is_backward_pawn_them(int sq, uint64_t friendly, uint64_t enemy);

const uint64_t light_squares = 0x55AA55AA55AA55AAULL;
const uint64_t dark_squares = 0xAA55AA55AA55AA55ULL;

#endif
