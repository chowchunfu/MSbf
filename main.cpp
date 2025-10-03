#include <chrono>
#include <ctime>
#include "include/pcal.cpp"
#include "include/bfa.cpp"


Board board;
ProbabilityCalculator pcal;
BruteForceAnalyzer bfa;

void writeBFResult() {
    std::ofstream f("bfinfo.txt");
    char datebuffer[80];
    time_t now = std::chrono::system_clock::to_time_t(t2);
    struct tm *local_tm = localtime(&now);
    strftime(datebuffer, 80, "%Y-%m-%d %H:%M:%S", local_tm);

    f << "Iterations: " << bfa.stat.iteration << std::endl;
    f << "Cache builds: " << bfa.stat.cachebuild << std::endl;
    f << "Cache hits: " << bfa.stat.cachehit << std::endl << std::endl;
    //f << "collision," << hashTable.stat_collision << std::endl;
    
    f << "Candidates: " << pcal.globalSolutionCount << std::endl;
    f << "Hidden cells: " << pcal.hiddenCount << std::endl;
    f << "Density: " <<  std::fixed << std::setprecision(2) << pcal.density*100 << "%" << std::endl << std::endl;

    

    f << "Elapsed: " << std::fixed << std::setprecision(6) << bfa.stat.elapsed << " s" << std::endl;
    f << "Nodes explored: " << bfa.stat.node << std::endl;
    f << "Confidence: " << std::fixed << std::setprecision(14) << (double) 1 - bfa.stat.node / 1.8446744073709551616 / 1e19  << std::endl;
    f << "Date: " << datebuffer << std::endl;

    f.close();

    f = std::ofstream("bfresult.csv");
    if (!f.is_open()) f = std::ofstream("bfresult2.csv");
    f << "coord,wins, winprob\n";
    for (int i = 0; i < board.size; ++i) {
        if (board.contents[i] != HIDDEN) continue;
        int x = i % board.width;
        int y = i / board.width;
        int globalIndex = pcal.getCp(i, 9);
        f <<   "\"(" << x << "," << y << ")\",";
        f << bfa.W[globalIndex] << ",";
        f << std::fixed << std::setprecision(2) << bfa.winningProbabilities[globalIndex]*100 << "%" ;
        f << std::endl;
    }
    f.close();
    std::cout << "saved result as bfresult.csv" << std::endl;
}

extern "C" void no_bfa(const char* filename) {
    board.from_file(filename);
    pcal = ProbabilityCalculator(&board);

    std::ofstream f("prepare.txt");
    f << "Candidates: " << pcal.globalSolutionCount << std::endl;
    f << "Hidden cells: " << pcal.hiddenCount << std::endl;
    f << "Density: " <<  std::fixed << std::setprecision(2) << pcal.density*100 << "%" << std::endl << std::endl;
    f.close();
}



extern "C" void run(const char* filename) {
    board.from_file(filename);
    pcal = ProbabilityCalculator(&board);
    cArray2d candidates = pcal.permutateCandidateSolutions();
    //candidates.to_file("abcd.txt");
    bfa = BruteForceAnalyzer(&candidates);

    bfa.stat.print();
    writeBFResult();

}


int main() {
    run("bfboard.txt");


    return 0;
}
