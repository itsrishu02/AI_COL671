#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>
#include "board.hpp"
#include "engine.hpp"
using namespace std;



const U8 PM = (PAWN | ROOK | KING | BISHOP);

const int MAX_DEPTH = 3; 



static int evaluate(const Board& b, U8 player) {
    int score1 = 0;
    // int score2 = 0;
    // int score3 = 0;


    int Value1 = 0;
    int Value2 = 0;

    for (int i = 0; i < 64; i++) {
        U8 piece = b.data.board_0[i];
        if (piece == 0) continue; 
        int Value = 0;
        switch (piece & PM) {
            case PAWN: Value = 100+i; break;
            case ROOK: Value = 500; break;
            case KING: Value = 10000; break;
            case BISHOP: Value = 300; break;
        }
        if (piece & player) Value1 += (Value);
        else Value2 += (Value);
    }
    score1 += Value1 - Value2;


    //int my_moves_left = b.get_legal_moves().size();

    // if (b.in_check()) {
    //     if (b.data.player_to_play == player) {
    //         score3 -= 200;
    //     } else {
    //         score3 += 200;
    //     }
    // }


    // int p1 = 0;
    // int p2 = 0;

    // auto legal_moves = b.get_legal_moves();
    // for (auto move : legal_moves){
    //     Board *co = b.copy();
    //     co->do_move(move);
    //     for (int i = 0; i < 64; i++){
    //             U8 piece = co->data.board_0[i];
    //             if (piece == 0)
    //                 continue;
    //             if (piece & player)
    //                 p1++;
    //             else
    //                 p2++;
    //     }
    //     delete co;
    // }
    // score2 = p1 - p2;


    
    int score = score1 ;//+ 100*score2;
    return score ;
}


static int mimx(Board& b, int depth, int alpha, int beta, U8 player, bool maxPl) {

     if (depth == 0) {
        return evaluate(b,player);
    }

    auto legal_moves = b.get_legal_moves();
    int best_score = maxPl ? INT_MIN : INT_MAX;

    for (auto move : legal_moves) {
    
        Board* co;
        co = b.copy(); 
        co->do_move(move);
        int score = mimx(*co, depth - 1, alpha, beta, player, !maxPl);
        delete co;

        if (maxPl) {
            best_score = max(best_score, score);
            alpha = max(alpha, score);
        } else {
            best_score = min(best_score, score);
            beta = min(beta, score);
        }
        if (beta <= alpha) {
            break;
        }
    }
    return best_score;
}

void Engine::find_best_move(const Board& b) {

    U8 player = b.data.player_to_play;

    auto legal_moves = b.get_legal_moves();
    int best_score = INT_MIN;
    U16 best_move = 0;

    for (auto move : legal_moves) {
        Board next_board = b;
        next_board.do_move(move);

        //int max_depth = this->search ? MAX_DEPTH : MAX_DEPTH-1;

        int score = mimx(next_board, MAX_DEPTH, INT_MIN, INT_MAX, player , false);

        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }

    this->best_move = best_move;
}
