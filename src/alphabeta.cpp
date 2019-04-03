#include <climits>
#include <cstring>
#include <iostream>
#include "attacks.hpp"
#include "display.hpp"
#include "eval.hpp"
#include "hashtable.hpp"
#include "makemove.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "other.hpp"
#include "search.hpp"
#include "sort.hpp"
#include "types.hpp"
#include "zobrist.hpp"

int reduction(const bool pvnode,
              const int move_num,
              const int depth,
              const int in_check,
              const int move_type) {
    if (pvnode || move_num < 4 || depth < 3 || in_check ||
        move_type == MoveType::CAPTURE || move_type == MoveType::PROMO ||
        move_type == MoveType::PROMO_CAPTURE) {
        return 0;
    }

    return 1;
}

int alphabeta(const Position &pos,
              SearchInfo &info,
              SearchStack *ss,
              PV &pv,
              int alpha,
              int beta,
              int depth) {
    assert(ss);
    assert(beta > alpha);
    assert(-INF <= alpha);
    assert(alpha <= INF);
    assert(-INF <= beta);
    assert(beta <= INF);
    assert(depth >= 0);
    assert(depth < MAX_DEPTH);

    if (*info.stop == true || clock() > info.end) {
        return 0;
    }

    if (ss->ply >= MAX_DEPTH) {
        info.leafnodes++;
        return eval(pos);
    }

    if (is_fifty_moves(pos) || repetitions(pos) == 2 ||
        (repetitions(pos) == 1 && ss->ply > 2)) {
        return 0;
    }

    bool in_check = check(pos, Colour::US);
    bool pvnode = (beta != alpha + 1);
    int best_score = INT_MIN;
    Move tt_move = NO_MOVE;
    Move best_move = NO_MOVE;
    int alpha_original = alpha;

    // Check extension
    if (in_check == true && depth < MAX_DEPTH) {
        depth++;
    }

    if (depth == 0) {
        info.leafnodes++;
        return qsearch(pos, info, ss, alpha, beta);
    }

    assert(pos.history_size > 0);
    const uint64_t key = pos.history[pos.history_size - 1];

    // Check the hash table
    Entry entry = info.tt->probe(key);
    if (key == entry.key()) {
        tt_move = entry.move();

#ifndef NDEBUG
        // info.hash_hits++;
        // if(legal_move(pos, tt_move) == false)
        //{
        //    info.hash_collisions++;
        //}
#endif

        if (!pvnode && entry.depth() >= depth &&
            legal_move(pos, tt_move) == true) {
            int score = eval_from_tt(entry.eval(), ss->ply);

            switch (entry.flag()) {
                case FLAGS::EXACT:
                    pv.length = 1;
                    pv.moves[0] = tt_move;
                    return score;
                    break;
                case FLAGS::LOWERBOUND:
                    alpha = (alpha > score ? alpha : score);
                    break;
                case FLAGS::UPPERBOUND:
                    beta = (beta < score ? beta : score);
                    break;
                default:
                    assert(false);
                    break;
            }

            if (alpha >= beta) {
                pv.length = 1;
                pv.moves[0] = tt_move;
                return score;
            }
        }
    }
    /*
        // Futility pruning
        if(!pvnode && ss->nullmove && depth == 1 && !in_check &&
       !is_endgame(pos))
        {
            int static_eval = eval(pos);

            //if(static_eval - 350 >= beta)
            //{
            //    return static_eval - 350;
            //    //return qsearch(info, stack, pos, alpha, beta);
            //}

            if(static_eval + 900 < alpha)
            {
                //return static_eval;
                return qsearch(pos, info, ss, alpha, beta);
            }
        }
    */
    // Nullmove
    if (ss->nullmove && !in_check && !pvnode && depth > 2 && !is_endgame(pos)) {
#define R 2
        Position npos = pos;
        PV npv;

        // Make nullmove
        flip(npos);
        npos.enpassant = Square::OFFSQ;
        npos.halfmoves = 0;
        npos.fullmoves = 0;
        std::uint64_t hash = calculate_hash(npos);
        npos.history[npos.history_size] = hash;
        npos.history_size++;

        (ss + 1)->nullmove = false;
        int score = -alphabeta(
            npos, info, ss + 1, npv, -beta, -beta + 1, depth - 1 - R);
        (ss + 1)->nullmove = true;

        if (score >= beta) {
            return score;
        }
    }
    /*
        // IID
        if(pvnode && tt_move == NO_MOVE && depth > 3)
        {
            Position npos = pos;
            PV npv;
            alphabeta(npos, info, ss+1, npv, -beta, -alpha, depth-1-3);
            tt_move = npv.moves[0];
        }
    */
    Move moves[MAX_MOVES];
    int num_moves = movegen(pos, moves);
    sort(pos, moves, num_moves, tt_move, ss->killer1, ss->killer2);

    int legal_moves = 0;
    for (int i = 0; i < num_moves; ++i) {
        assert(moves[i] != NO_MOVE);

        Position npos = pos;
        make_move(npos, moves[i]);
        if (check(npos, Colour::THEM) == true) {
            continue;
        }
        PV npv;

        // const bool move_checks = check(npos, Colour::US);

        info.nodes++;
        legal_moves++;

        int score = 0;
        if (legal_moves == 1) {
            score =
                -alphabeta(npos, info, ss + 1, npv, -beta, -alpha, depth - 1);
        } else {
            int r = reduction(
                pvnode, legal_moves - 1, depth, in_check, move_type(moves[i]));
            score = -alphabeta(
                npos, info, ss + 1, npv, -alpha - 1, -alpha, depth - 1 - r);

            if (alpha < score && score < beta) {
                score = -alphabeta(
                    npos, info, ss + 1, npv, -beta, -score, depth - 1);
            }
        }

        if (score > best_score) {
            best_score = score;
            best_move = moves[i];

            // Update PV
            pv.moves[0] = moves[i];
            memcpy(pv.moves + 1, npv.moves, npv.length * sizeof(Move));
            pv.length = npv.length + 1;

            if (score > alpha) {
                alpha = score;

                if (alpha >= beta) {
                    if (move_type(moves[i]) != MoveType::CAPTURE) {
                        ss->killer2 = ss->killer1;
                        ss->killer1 = moves[i];
                    }

#ifndef NDEBUG
                    info.cutoffs[legal_moves - 1]++;
#endif

                    break;
                }
            }
        }
    }

    if (legal_moves == 0) {
        if (in_check == true) {
            return -INF + ss->ply;
        } else {
            return 0;
        }
    }

    uint8_t flag;
    if (best_score <= alpha_original) {
        flag = FLAGS::UPPERBOUND;
    } else if (best_score >= beta) {
        flag = FLAGS::LOWERBOUND;
    } else {
        flag = FLAGS::EXACT;
    }

    info.tt->add(key, depth, eval_to_tt(best_score, ss->ply), best_move, flag);

#ifndef NDEBUG
    Entry test_entry = info.tt->probe(key);
    // assert(test_entry == Entry(key, move, depth, eval, flag));
    assert(test_entry.key() == key);
    assert(test_entry.depth() == depth);
    assert(eval_from_tt(test_entry.eval(), ss->ply) == best_score);
    assert(test_entry.move() == best_move);
    assert(test_entry.flag() == flag);
#endif

    return alpha;
}
