#include "seq_estimator.h"
#include "seq_estimator_optimized.h"

#include <vector>
#include <random>
#include <chrono>
#include <iostream>

using Hasaki::SeqType;

int main() {
    // generate fixed 1e8 random number ~ N(5, 2^2)
    std::vector<double> randomNums;

    std::mt19937 gen{19260817};
    std::normal_distribution<> nd{5, 2};
    for (size_t i = 0; i < 1e8; ++ i)
        randomNums.emplace_back(nd(gen));


    // test1: test no optimized version
    auto start_time1 = std::chrono::system_clock::now();
    Hasaki::SeqEstimator seqEst1;
    for (const auto num: randomNums)
        seqEst1.Append(num);
    auto end_time1 = std::chrono::system_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end_time1 - start_time1);

    std::cout << "res1 = " << seqEst1 << std::endl;

    std::cout << "cost "
              << double(duration1.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
              << " second" << std::endl;


    // test2: test optimized version
    auto start_time2 = std::chrono::system_clock::now();
    Hasaki::SeqEstimatorOptimized<SeqType::MEAN | SeqType::VAR | SeqType::MIN | SeqType::MAX> seqEst2;
    for (const auto num: randomNums)
        seqEst2.Append(num);
    auto end_time2 = std::chrono::system_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end_time2 - start_time2);

    std::cout << "res2 = " << seqEst2 << std::endl;

    std::cout << "cost "
              << double(duration2.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
              << " second" << std::endl;


    // test3: test optimized version
    auto start_time3 = std::chrono::system_clock::now();
    Hasaki::SeqEstimatorOptimized<SeqType::MEAN> seqEst3;
    for (const auto num: randomNums)
        seqEst3.Append(num);
    auto end_time3 = std::chrono::system_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end_time3 - start_time3);

    std::cout << "res3 = " << seqEst3 << std::endl;

    std::cout << "cost "
              << double(duration3.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
              << " second" << std::endl;


    // test4: test optimized version
    auto start_time4 = std::chrono::system_clock::now();
    Hasaki::SeqEstimatorOptimized<SeqType::MAX> seqEst4;
    for (const auto num: randomNums)
        seqEst4.Append(num);
    auto end_time4 = std::chrono::system_clock::now();
    auto duration4 = std::chrono::duration_cast<std::chrono::microseconds>(end_time4 - start_time4);

    std::cout << "res4 = " << seqEst4 << std::endl;

    std::cout << "cost "
              << double(duration4.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
              << " second" << std::endl;


//    SeqEstimator seqEst;
//    std::cout << seqEst << std::endl;
//    seqEst.Append (1.145);
//    std::cout << seqEst << std::endl;
//    seqEst.Append (1.4);
//    std::cout << seqEst << std::endl;
//    seqEst.Append (1.919);
//    std::cout << seqEst << std::endl;
//    seqEst.Append (0.8);
//    std::cout << seqEst << std::endl;
//    seqEst.Append (1.0);
//    std::cout << seqEst << std::endl;

    return 0;
}
