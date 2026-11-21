#include "uci.hpp"
#include "board.hpp"
#include "core.hpp"
#include "movegen.hpp"
#include "perft.hpp"
#include "search.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

namespace {

// We keep a board atate
Board board;

// We keep the UCI loop on the main thread while using a search thread
std::thread search_thread;

void stop_thread() {
    Search::stop();

    if (search_thread.joinable())
        search_thread.join();
}

void process_go_command(std::istringstream &iss) {

    stop_thread();

    std::string token;

    int depth = 0;
    int movetime = 0;
    int wtime = 0;
    int winc = 0;
    int btime = 0;
    int binc = 0;

    while (iss >> token) {

        if (token == "depth") iss >> depth;
        else if (token == "movetime") iss >> movetime;
        else if (token == "wtime") iss >> wtime;
        else if (token == "winc") iss >> winc;
        else if (token == "btime") iss >> btime;
        else if (token == "binc") iss >> binc;
        else if (token == "perft") {
            iss >> depth;
            perft_divide(board, depth);
            return;
        }
    }

    if (depth <= 0) depth = 100;
    if (depth > 100) depth = 100;

    int time = movetime;
    if (time <= 0) {
        int our_time = board.side_to_move == Color::White ? wtime : btime;
        int our_inc = board.side_to_move == Color::White ? winc : binc;

        time = our_time / 20 + our_inc / 2;
    }

    search_thread = std::thread([time, depth](){
        Search::iterative_deepening(board, depth, time);
    });
}

void process_position_command(std::istringstream &iss) {

    stop_thread();

    std::string token;
    iss >> token;

    if (token == "startpos") {
        board.parse_fen(START_FEN);

        iss >> token;

        if (token != "moves") {
            return;
        }
    } else if (token == "fen"){
        std::string fen;

        while (iss >> token && token != "moves") {
            fen += token + " ";
        }

        if (iss.eof()) {
            return;
        }
    }

    // Parse moves
    while (iss >> token) {

        MoveList moves;
        MoveGenerator gen(moves.data, moves.size);
        gen.generate_moves(board);

        auto opt_move = board.parse_move(token);

        if (!opt_move) {
            return;
        }

        board.make_move(*opt_move);
    }
}

}

void start_uci() {
    board.parse_fen(START_FEN);

    while (true) {

        std::string input;
        
        if (!std::getline(std::cin,input)) {
            break;
        }

        std::istringstream iss(input);

        std::string token;
        iss >> token;

        if (token == "quit") {
            break;
        }

        if (token == "stop") {
            stop_thread();
            continue;
        }

        if (token == "uci") {
            std::cout << "id name Cubeeb\n";
            std::cout << "id author GoobusTheNoobus, picowso\n";
            std::cout << "\nuciok\n";
    
            continue;
        }

        if (token == "isready") {
            std::cout << "readyok\n";

            continue;
        }

        if (token == "go") {
            process_go_command(iss);
            Search::stop();
            continue;
        }

        if (token == "position") {
            process_position_command(iss);
        }
    }
}