#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <string>

#include "board.h"
#include "cluster.h"
#include "config.h"
#include "enumerator.h"
#include "solver.h"

using namespace std;
using namespace std::chrono;

// Define runtime globals
int BoardSize = 5;
int PrimaryRow = 2;
uint64_t MaxID = 268108;
int NumWorkers = 4;
int BoardSize2 = 25;
int Target = 13;
int V = 5;
std::array<bb, 7> RowMasks;
std::array<bb, 7> ColumnMasks;
bb TopRow;
bb BottomRow;
bb LeftColumn;
bb RightColumn;

typedef std::function<void(const Cluster &)> CallbackFunc;

void worker(const int wi, const int wn, CallbackFunc func) {
    Enumerator enumerator;
    enumerator.Enumerate([&](uint64_t id, const Board &board) {
        if (id % wn != wi) {
            return;
        }
        Cluster cluster(id, board);
        func(cluster);
    });
}

void initializeBoardConfig() {
    BoardSize2 = BoardSize * BoardSize;
    Target = PrimaryRow * BoardSize + BoardSize - PrimarySize;
    V = BoardSize;
    
    // Initialize row masks
    for (int y = 0; y < BoardSize; y++) {
        bb mask = 0;
        for (int x = 0; x < BoardSize; x++) {
            const int i = y * BoardSize + x;
            mask |= (bb)1 << i;
        }
        RowMasks[y] = mask;
    }
    
    // Initialize column masks
    for (int x = 0; x < BoardSize; x++) {
        bb mask = 0;
        for (int y = 0; y < BoardSize; y++) {
            const int i = y * BoardSize + x;
            mask |= (bb)1 << i;
        }
        ColumnMasks[x] = mask;
    }
    
    TopRow = RowMasks[0];
    BottomRow = RowMasks[BoardSize - 1];
    LeftColumn = ColumnMasks[0];
    RightColumn = ColumnMasks[BoardSize - 1];
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "5x5") {
            BoardSize = 5;
            PrimaryRow = 2;
            MaxID = 268108;
        } else if (arg == "6x6") {
            BoardSize = 6;
            PrimaryRow = 2;
            MaxID = 243502785;
        } else if (arg == "7x7") {
            BoardSize = 7;
            PrimaryRow = 3;  // Exit row 3 for 7x7
            MaxID = 561276504436;
        } else if (arg == "--threads" && i + 1 < argc) {
            NumWorkers = std::stoi(argv[++i]);
        } else {
            cerr << "Usage: " << argv[0] << " [5x5|6x6|7x7] [--threads N]" << endl;
            return 1;
        }
    }
    
    // Initialize board configuration
    initializeBoardConfig();
    
    cerr << "Board size: " << BoardSize << "x" << BoardSize << endl;
    cerr << "Primary row: " << PrimaryRow << endl;
    cerr << "Threads: " << NumWorkers << endl;
    cerr << "MaxID: " << MaxID << endl;

    mutex m;

    uint64_t maxSeenID = 0;
    uint64_t numIn = 0;
    uint64_t numCanonical = 0;
    uint64_t numSolvable = 0;
    uint64_t numMinimal = 0;

    auto start = steady_clock::now();

    auto callback = [&](const Cluster &c) {
        lock_guard<mutex> lock(m);

        numIn++;
        if (c.Canonical()) numCanonical++;
        if (c.Solvable()) numSolvable++;
        if (c.Minimal()) numMinimal++;
        if (!c.Canonical() || !c.Solvable() || !c.Minimal()) {
            return;
        }

        maxSeenID = std::max(maxSeenID, c.ID());
        const Board &unsolved = c.Unsolved();
        const double pct = (double)maxSeenID / (double)MaxID;
        const double hrs = duration<double>(steady_clock::now() - start).count() / 3600;
        const double est = pct > 0 ? hrs / pct : 0;

        // print results to stdout
        cout
            << setfill('0')
            << setw(2) << c.NumMoves() << " "
            << unsolved << " "
            << c.NumStates() << " ";
        for (int i = 0; i < c.DistanceCounts().size(); i++) {
            if (i != 0) {
                cout << ",";
            }
            cout << c.DistanceCounts()[i];
        }
        cout << endl;

        // print progress info to stderr
        cerr
            << fixed
            << pct << " pct "
            << hrs << " hrs "
            << est << " est - "
            << numIn << " inp "
            << numCanonical << " can "
            << numSolvable << " slv "
            << numMinimal << " min"
            << endl;
    };

    std::vector<std::thread> threads;
    for (int wi = 0; wi < NumWorkers; wi++) {
        threads.push_back(std::thread(worker, wi, NumWorkers, callback));
    }
    for (int wi = 0; wi < NumWorkers; wi++) {
        threads[wi].join();
    }

    // print final stats to stderr
    const double pct = (double)maxSeenID / (double)MaxID;
    const double hrs = duration<double>(steady_clock::now() - start).count() / 3600;
    const double est = pct > 0 ? hrs / pct : 0;
    cerr
        << fixed
        << 1.0 << " pct "
        << hrs << " hrs "
        << est << " est - "
        << numIn << " inp "
        << numCanonical << " can "
        << numSolvable << " slv "
        << numMinimal << " min"
        << endl;
    return 0;
}

int main2() {
    // // 51 83 13 BCDDE.BCF.EGB.FAAGHHHI.G..JIKKLLJMM. 4780
    Board board("BCDDE.BCF.EGB.FAAGHHHI.G..JIKKLLJMM.");

    Solver solver;
    for (int i = 0; i < 100; i++) {
        solver.Solve(board);
    }
    // Solver solver(board);
    // const int numMoves = solver.Solve();
    // cout << numMoves << endl;

    // // 15 32 12 BB.C...D.CEE.DAAFGH.IIFGH.JKK.LLJ... 541934
    // Board board("BB.C...D.CEE.DAAFGH.IIFGH.JKK.LLJ...");

    // // 24 43 13 B..CDDBEEC.F.G.AAF.GHHIJKKL.IJ..L.MM 278666
    // // Board board("B..CDDBEEC.F.G.AAF.GHHIJKKL.IJ..L.MM");

    // Cluster cluster(board);

    // cout << "canonical: " << cluster.Canonical() << endl;
    // cout << "solvable:  " << cluster.Solvable() << endl;
    // cout << "states:    " << cluster.NumStates() << endl;
    // cout << "moves:     " << cluster.NumMoves() << endl;
    // cout << "counts:    ";

    // for (int count : cluster.DistanceCounts()) {
    //     cout << count << ",";
    // }
    // cout << endl;
    // cout << endl;

    // cout << "unsolved:" << endl;
    // cout << cluster.Unsolved().String2D() << endl;
    // cout << "solved:" << endl;
    // cout << cluster.Solved().String2D() << endl;

    return 0;
}
