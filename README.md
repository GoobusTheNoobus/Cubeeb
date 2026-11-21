# Cubeeb
Authors: GoobusTheNoobus, picowso

Cubeeb is a UCI chess engine written in C++20. It uses bitboards, negamax search algorithm, and applies
various search techniques to speed up search. It is WIP.

## How to build
You need make and g++ to build.    
Use the Makefile at root project directory. It supports basic commands:  
- `make` or `make release` - builds a release executable `build/cubeeb`  
- `make debug` - builds a debug executable `build-debug/cubeeb`  
- `make clean` - deletes the build directories  

## SPRT
Every patch that could potentially change engine performance should be tested with 
[SPRT](https://chessprogramming.org/Sequential_Probability_Ratio_Test). 
There is a script, `./test/sprt.py` for this. It clones the current Github source code
into a temporary folder, builds it, and moves it to the sprt/ folder. It then builds 
the current local version as the new version. It will then run Fastchess (Cutechess but better) 
between the 2 engines. It is written in python though, so you will need that too.

## UCI
Cubeeb supports the [Universal Chess Interface](https://official-stockfish.github.io/docs/stockfish-wiki/UCI-Protocol-and-Stockfish-Commands.html), 
which is the modern standard communcation
protocol between the engine and the GUI. Here are the commands it support:
* `uci` - confirm UCI. Cubeeb will print some information, then print `uciok`
* `isready` - checks if the engine is still alive. Will print `readyok` if it is
* `ucinewgame` - clears internal cache, prepares Cubeeb for a new game
* `position` - sets up the position
  * `position startpos moves <move1>...<moven>` - sets up starting position, then play the following moves 
  until no more is present or encountered an illegal move
  * `position fen [fenstring] moves <move1>...<moven>` - sets up the position in the Forsyth-Edward Notation 
  string, then play the following moves until no mor is present or encountered an illegal move
* `go <params>...` - searches the current position set up by the `position` command. 
**Note: Running this does not block the input-output loop, as it searches on a seperate thread.**
  * `go` - with no parameters, searches to depth 100
  * `go depth [depth]` - iteratively deepens to depth `[depth]`
  * `go movetime [movetime]` - searches for `[movetime]` milliseconds
  * `go wtime [wtime] btime [btime] winc [winc] binc [binc]` - lets the engine decide how long to spend 
  given the current time left for each side and the increment per side, all in milliseconds
  * `go perft [depth]` - has higher precedence then every other parameter - does perft to `[depth]` depth. 
  Blocks main thread, so you can't exit out unless you terminate the program

There are also unofficial commands for debugging purposes:
* `board` - prints the current board set up by the `position` command
* `rep` - prints the zobrist stack of the board history

## Features
* Bitboards + mailbox board representation
* Bitboards base move/attack generation
* Alpha-Beta Negamax 
* Iterative Deepening
* Large set of UCI commands + extra debug commands (mentioned above)
* Basic MVV->LVA Move Ordering (+303.43 Elo)