#ifndef MOVE_HPP
#define MOVE_HPP

#include "assert.hpp"
#include <cstdint>
#include <string>
#include "types.hpp"

struct Move {
   public:
    Move() : data_{0} {
    }

    Move(const Square from,
         const Square to,
         const MoveType type,
         const PieceType piece) {
        // To
        data_ = to & 0x3F;

        // From
        data_ |= (from & 0x3F) << 6;

        // Type
        data_ |= (type & 0xF) << 12;

        // Piece
        data_ |= (piece & 0x7) << 16;

        // Captured
        data_ |= (PieceType::NONE & 0x7) << 19;

        // Promotion
        data_ |= (PieceType::NONE & 0x7) << 22;

        UCI_ASSERT(this->to() == to);
        UCI_ASSERT(this->from() == from);
        UCI_ASSERT(this->type() == type);
        UCI_ASSERT(this->piece() == piece);
        UCI_ASSERT(this->captured() == PieceType::NONE);
        UCI_ASSERT(this->promo() == PieceType::NONE);
    }

    Move(const Square from,
         const Square to,
         const MoveType type,
         const PieceType piece,
         const PieceType captured) {
        // To
        data_ = to & 0x3F;

        // From
        data_ |= (from & 0x3F) << 6;

        // Type
        data_ |= (type & 0xF) << 12;

        // Piece
        data_ |= (piece & 0x7) << 16;

        // Captured
        data_ |= (captured & 0x7) << 19;

        // Promotion
        data_ |= (PieceType::NONE & 0x7) << 22;

        UCI_ASSERT(this->to() == to);
        UCI_ASSERT(this->from() == from);
        UCI_ASSERT(this->type() == type);
        UCI_ASSERT(this->piece() == piece);
        UCI_ASSERT(this->captured() == captured);
        UCI_ASSERT(this->promo() == PieceType::NONE);
    }

    Move(const Square from,
         const Square to,
         const MoveType type,
         const PieceType piece,
         const PieceType captured,
         const PieceType promo) {
        // To
        data_ = to & 0x3F;

        // From
        data_ |= (from & 0x3F) << 6;

        // Type
        data_ |= (type & 0xF) << 12;

        // Piece
        data_ |= (piece & 0x7) << 16;

        // Captured
        data_ |= (captured & 0x7) << 19;

        // Promotion
        data_ |= (promo & 0x7) << 22;

        UCI_ASSERT(this->to() == to);
        UCI_ASSERT(this->from() == from);
        UCI_ASSERT(this->type() == type);
        UCI_ASSERT(this->piece() == piece);
        UCI_ASSERT(this->captured() == captured);
        UCI_ASSERT(this->promo() == promo);
    }

    [[nodiscard]] Square to() const noexcept {
        return static_cast<Square>(data_ & 0x3F);
    }

    [[nodiscard]] Square from() const noexcept {
        return static_cast<Square>((data_ >> 6) & 0x3F);
    }

    [[nodiscard]] MoveType type() const noexcept {
        return static_cast<MoveType>((data_ >> 12) & 0xF);
    }

    [[nodiscard]] PieceType piece() const noexcept {
        return static_cast<PieceType>((data_ >> 16) & 0x7);
    }

    [[nodiscard]] PieceType captured() const noexcept {
        return static_cast<PieceType>((data_ >> 19) & 0x7);
    }

    [[nodiscard]] PieceType promo() const noexcept {
        return static_cast<PieceType>((data_ >> 22) & 0x7);
    }

    [[nodiscard]] std::string uci(const bool flipped) const noexcept {
        const MoveType type = this->type();
        Square from = this->from();
        Square to = this->to();

        if (flipped == true) {
            from = Square(from ^ 56);
            to = Square(to ^ 56);
        }

        std::string ans = SquareString[from] + SquareString[to];

        if (type == MoveType::PROMO || type == MoveType::PROMO_CAPTURE) {
            switch (promo()) {
                case PieceType::QUEEN:
                    ans += "q";
                    break;
                case PieceType::ROOK:
                    ans += "r";
                    break;
                case PieceType::BISHOP:
                    ans += "b";
                    break;
                case PieceType::KNIGHT:
                    ans += "n";
                    break;
                default:
                    UCI_ASSERT(false);
                    break;
            }
        }

        return ans;
    }

    [[nodiscard]] bool operator==(const Move &rhs) const noexcept {
        return data_ == rhs.data_;
    }

    [[nodiscard]] bool operator!=(const Move &rhs) const noexcept {
        return data_ != rhs.data_;
    }

   public:
    std::uint32_t data_;
};

#define NO_MOVE (Move())

#endif
