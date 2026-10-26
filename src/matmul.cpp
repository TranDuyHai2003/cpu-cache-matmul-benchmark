#include<iostream>
#include<vector>
#include<chrono>
#include<algorithm>
#include<iomanip>
#include <numeric>
#include<cmath>
#include<fstream>
#include<filesystem>

volatile double benchmarkSink = 0.0;


void saveCsv(int n,double ijkMedian, double ikjMedian){
    std::filesystem::create_directories("results");

    std::string path = "results/matmul_results.csv";

    bool needHeader = !std::filesystem::exists(path) || std::filesystem::file_size(path) == 0;

    std::ofstream file(path, std::ios::app);

    if(!file){
        std::cerr << "Cannot open CSV file\n";
        return;
    }

    if(needHeader){
        file << "n,ijk_ms,ikj_ms,speedup\n";
    }

    double speedup = ijkMedian / ikjMedian;

    file << std::fixed << std::setprecision(6);

    file << n << "," << ijkMedian << "," << ikjMedian << "," << speedup << "\n";
}


bool matrixEqual(const std::vector<double> &A, const std::vector<double> &B,double absTol = 1e-9 , double relTol=1e-8){
    if (A.size() != B.size()){
        return false;
    }

    for(std::size_t i=0;i<A.size();i++){
        double a = A[i];
        double b = B[i];

        if(!std::isfinite(a) || !std::isfinite(b)){
            return false;
        }
        double tolerance = absTol + relTol * std::max(std::abs(a), std::abs(b));

        if(std::abs(a-b) > tolerance){
            return false;
        }
    }
    return true;
}

std::vector<double> matmul(const std::vector<double>& A,const std::vector<double>& B,int n){
    std::vector<double> C(n*n, 0.0);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int k=0;k<n;k++){
                C[i*n + j] += A[i*n+k] * B[k*n+j];
            }
        }
    }
    return C;

}


std::vector<double> matmulIKJ(
    const std::vector<double>& A,
    const std::vector<double>& B,
    int n
) {
    std::vector<double> C(n*n, 0.0);

    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            for (int j = 0; j < n; j++) {
                C[i*n + j] += A[i*n+k] * B[k*n+j];
            }
        }
    }

    return C;
}


void matmulIJKInto(
    const std::vector<double>& A,
    const std::vector<double>& B,
    std::vector<double>& C,
    int n
) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                C[i*n+j] += A[i*n+k] * B[k*n+j];
            }
        }
    }
}

void matmulIKJInto(
    const std::vector<double>& A,
    const std::vector<double>& B,
    std::vector<double>& C,
    int n
) {
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            for (int j = 0; j < n; j++) {
                C[i*n+j] += A[i*n+k] * B[k*n+j];
            }
        }
    }
}



double benchMarkIJK(const std::vector<double>& A,const std::vector<double>& B,int n){
    std::vector<double> C(n*n,0.0);
    std::vector<double> times;
    times.reserve(11);
    for(int i=0;i<2;i++){
        std::fill(C.begin(), C.end(), 0.0);
        matmulIJKInto(A, B, C, n);
        benchmarkSink = std::accumulate(C.begin(), C.end(), 0.0);
    }
    for(int i=0;i<11;i++){
        std::fill(C.begin(), C.end(), 0.0);

        auto start = std::chrono::steady_clock::now();

        matmulIJKInto(A, B, C, n);

        auto end = std::chrono::steady_clock::now();

        benchmarkSink = std::accumulate(C.begin(), C.end(), 0.0);

    double Ms =
    std::chrono::duration<double, std::milli>(
        end - start
    ).count();
        times.push_back(Ms);
    }

    std::sort(times.begin(),times.end());

    return times[5];
}


double benchMarkIKJ(const std::vector<double>& A,const std::vector<double>& B,int n){
    std::vector<double> C(n*n,0.0);
    std::vector<double> times;
    times.reserve(11);
    for(int i=0;i<2;i++){
        std::fill(C.begin(), C.end(), 0.0);
        matmulIKJInto(A, B,C, n);
        benchmarkSink = std::accumulate(C.begin(), C.end(), 0.0);
    }
    for(int i=0;i<11;i++){
        std::fill(C.begin(), C.end(), 0.0);

        auto start = std::chrono::steady_clock::now();

        matmulIKJInto(A, B,C, n);

        auto end = std::chrono::steady_clock::now();

        benchmarkSink = std::accumulate(C.begin(), C.end(), 0.0);
        double Ms = std::chrono::duration<double,std::milli>(end - start).count();
        times.push_back(Ms);
    }

    std::sort(times.begin(),times.end());

    return times[5];
}



bool runCorrectnessTests(){
    bool allPassed = true;


    // Test 1
    std::vector<double> A = {1, 2, 3, 4};
    std::vector<double> B = {5, 6, 7, 8};
    std::vector<double> expected = {19, 22, 43, 50};

    auto C1 = matmul(A, B, 2);
    auto C2 = matmulIKJ(A, B, 2);

    bool passed = matrixEqual(C1, expected)
            && matrixEqual(C2, expected);

    std::cout << "Test 1 - Known result "
          << (passed ? "PASS" : "FAIL") << "\n";


    // Test 2
    A = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<double> I = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    C1 = matmul(A, I, 3);
    C2 = matmulIKJ(A, I, 3);

    bool passed2 = matrixEqual(C1, A)
            && matrixEqual(C2, A);

    std::cout << "Test 2 - Identity: "
          << (passed2 ? "PASS" : "FAIL") << "\n";

    // Test 3
    A = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<double> zero(9, 0.0);

    C1 = matmul(A, zero, 3);
    C2 = matmulIKJ(A, zero, 3);
   

    bool passed3 = matrixEqual(C1,zero)
            && matrixEqual(C2,zero);

    std::cout << "Test 3 - Zero: "
          << (passed3 ? "PASS" : "FAIL") << "\n";
    
    allPassed = passed && passed2 && passed3;

    return allPassed;
}

int main(){
    if (!runCorrectnessTests()) {
    std::cout << "Correctness tests FAILED\n";
    return 1;
    }
    std::cout << "All correctness tests PASSED\n";

    int n;

    std::cout << "N = ";
    if (!(std::cin >> n) || n < 1 || n > 256) {
        std::cout << "Invalid input\n";
        return 1;
    }

    std::vector<double> A(n * n);
    std::vector<double> B(n * n);

    for (int i = 0; i < n * n; i++) {
        A[i] = (i % 7) + 1;
        B[i] = (i % 11) + 1;
    }

    std::vector<double> Cijk(n * n, 0.0);
    std::vector<double> Cikj(n * n, 0.0);

    matmulIJKInto(A, B, Cijk, n);
    matmulIKJInto(A, B, Cikj, n);

    std::vector<double> expected = matmul(A, B, n);

    bool passed = matrixEqual(Cijk, expected)
            && matrixEqual(Cikj, expected);

    std::cout << "Into functions match: "
            << std::boolalpha << passed << "\n";

    if (!passed) {
        return 1;
    }



    auto C1 = matmul(A, B, n);
    auto C2 = matmulIKJ(A, B, n);

    if (!matrixEqual(C1, C2)) {
        std::cerr << "Results mismatch!\n";
        return 1;
    }

    std::cout << "Results match: true\n";



    std::vector<double> ijkResults;
    std::vector<double> ikjResults;

    ijkResults.reserve(4);
    ikjResults.reserve(4);

    std::cout << std::fixed << std::setprecision(3);

    for (int trial = 0; trial < 4; trial++) {
        double ijkMs, ikjMs;

        if (trial % 2 == 0) {
            ijkMs = benchMarkIJK(A, B, n);
            ikjMs = benchMarkIKJ(A, B, n);
        } else {
            ikjMs = benchMarkIKJ(A, B, n);
            ijkMs = benchMarkIJK(A, B, n);
        }

        ijkResults.push_back(ijkMs);
        ikjResults.push_back(ikjMs);

        std::cout << "Trial " << trial + 1
                << ": IJK " << ijkMs
                << " ms | IKJ " << ikjMs
                << " ms\n";
    }

    std::sort(ijkResults.begin(), ijkResults.end());
    std::sort(ikjResults.begin(), ikjResults.end());

    double ijkMedian = (ijkResults[1] + ijkResults[2]) / 2.0;
    double ikjMedian = (ikjResults[1] + ikjResults[2]) / 2.0;

    if (ikjMedian > 0.0) {
    saveCsv(n, ijkMedian, ikjMedian);
    }

    std::cout << "\nFinal IJK median: "
            << ijkMedian << " ms\n";

    std::cout << "Final IKJ median: "
            << ikjMedian << " ms\n";

    if (ikjMedian > 0.0) {
        std::cout << "Final speedup: "
                << ijkMedian / ikjMedian
                << "x\n";
    }

   

}