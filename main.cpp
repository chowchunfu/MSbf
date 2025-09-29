#include <chrono>
#include <ctime>
#include "include/pcal.cpp"
#include "include/bfa.cpp"

std::chrono::system_clock::time_point t1;
std::chrono::system_clock::time_point t2;
double elapsed;

Board board;
ProbabilityCalculator pcal;
BruteForceAnalyzer bfa;

void writeBFResult() {
    std::ofstream f("bfinfo.csv");
    char datebuffer[80];
    time_t now = std::chrono::system_clock::to_time_t(t2);
    struct tm *local_tm = localtime(&now);
    strftime(datebuffer, 80, "%Y-%m-%d %H:%M:%S", local_tm);

    f << "cachebuild," << bfa.stat.cachebuild << std::endl;
    f << "cachehit," << bfa.stat.cachehit << std::endl;
    f << "iteration," << bfa.stat.iteration << std::endl;
    f << "node," << bfa.stat.node << std::endl;
    f << "collision," << hashTable.stat_collision << std::endl;
    f << "density," <<  std::fixed << std::setprecision(2) << pcal.density*100 << "%" << std::endl;
    f << "confidence," << std::fixed << std::setprecision(14) << (double) 1 - hashTable.stat_collision / 1.8446744073709551616 / 1e19  << std::endl;
    f << "elapsed," << std::fixed << std::setprecision(6) << elapsed << std::endl;
    f << "date," << datebuffer << std::endl;


    f << "length," << bfa.length << std::endl;
    f << "width," << bfa.width << std::endl;

    f.close();

    f = std::ofstream("bfresult.csv");
    if (!f.is_open()) f = std::ofstream("bfresult2.csv");
    f << "coord,wins, winprob\n";
    for (int i = 0; i < board.size; ++i) {
        if (board.contents[i] != HIDDEN) continue;
        int x = i % board.width;
        int y = i / board.width;
        int globalIndex = pcal.getCp(i, 9);
        f << "(" << x << " " << y << "),";
        f << bfa.W[globalIndex] << ",";
        f << std::fixed << std::setprecision(2) << bfa.winningProbabilities[globalIndex]*100 << "%" ;
        f << std::endl;
    }
    f.close();
    std::cout << "saved result as bfresult.csv" << std::endl;
}

extern "C" void run(const char* filename) {
    t1 = std::chrono::system_clock::now(); // start clock
    board.from_file(filename);
    pcal = ProbabilityCalculator(&board);
    cArray2d candidates = pcal.permutateCandidateSolutions();

    bfa = BruteForceAnalyzer(&candidates);
    t2 = std::chrono::system_clock::now(); // end clock

    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1e6;

    writeBFResult();

}

extern "C" int getRemainingMines() {
    return pcal.remainingMine;
}

extern "C" int getContent(int index) {
    return board.contents[index];
}

extern "C" double getWinningProbability(int index) {
    if (board.contents[index] != HIDDEN) return -1;
    int globalIndex = pcal.getCp(index, 9);
    double wp = bfa.winningProbabilities[globalIndex];
    return wp;
}


int main() {
    run("mbf/bfa_3x3x2.txt");


    return 0;
}
