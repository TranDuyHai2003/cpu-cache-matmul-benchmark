#include<iostream>
#include<vector>
#include<chrono>
#include<iomanip>
#include<algorithm>
#include<fstream>
#include<filesystem>
#include<iomanip>

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



double sumRowMajor(
    const std::vector<double>& matrix,int n
){
    double sum = 0.0;
    for(int row=0;row<n;row++){
        for(int col=0;col<n;col++){
            sum += matrix[row*n+col];
        }
    }
    return sum;
}

double sumColumnMajor(const std::vector<double>& matrix,int n){
    double sum = 0.0;
    for(int col=0;col<n;col++){
        for(int row=0;row<n;row++){
            sum += matrix[row*n+col];
        }
    }
    return sum;
}

volatile double benchmarkSink = 0.0;


double benchmarkRow(
    const std::vector<double>& matrix,
    int n
){
    std::vector<double> times;
    times.reserve(11);
    for (int i = 0; i < 2; i++) {
    benchmarkSink = sumRowMajor(matrix, n);
    }

    for(int i=0;i<11;i++){  
        auto startRow = std::chrono::steady_clock::now();

        benchmarkSink = sumRowMajor(matrix, n);

        auto endRow = std::chrono::steady_clock::now();

        double rowMs = std::chrono::duration<double, std::milli>(endRow-startRow).count();
        times.push_back(rowMs);

        // std::cout << "SumRow: " << sumRow ;

    }
    std::cout <<"Times: ";
    for(int i=0;i<11;i++){
        std::cout << times[i] << " ";
    }
    std::cout <<"\nSorted: ";
    std::sort(times.begin(),times.end());
     for(int i=0;i<11;i++){
        std::cout << times[i] << " ";
    }

    std::cout << "\nMedian: " << times[5] << "\n"; 

    return times[5];
};

double benchmarkCol(
    const std::vector<double>& matrix,
    int n
){
    std::vector<double> times;
    times.reserve(11);
   for (int i = 0; i < 2; i++) {
    benchmarkSink = sumColumnMajor(matrix, n);
    }

    for(int i=0;i<11;i++){  
        auto startRow = std::chrono::steady_clock::now();

        benchmarkSink = sumColumnMajor(matrix, n);

        auto endRow = std::chrono::steady_clock::now();

        double rowMs = std::chrono::duration<double, std::milli>(endRow-startRow).count();
        times.push_back(rowMs);
        // std::cout << "colRow: " << colRow ;
    }
    std::cout <<"Times: ";
    for(int i=0;i<11;i++){
        std::cout << times[i] << " ";
    }
    std::cout <<"\nSorted: ";
    std::sort(times.begin(),times.end());
     for(int i=0;i<11;i++){
        std::cout << times[i] << " ";
    }

    std::cout << "\nMedian: " << times[5] << "\n"; 

    return times[5];
};


int main(){
    int n;

    std::cout << "Enter N: ";
    if(!(std::cin >> n) || n<1 || n>1000){
        std::cout << "Invalid input\n";
        return 1;
    }
    std::cout << "Matrix size: " << n << " x " << n << "\n";

    std::vector<double>matrix(n*n,1.0); 

    auto startRow = std::chrono::steady_clock::now();

    double sumRow = sumRowMajor(matrix,n);

    auto endRow = std::chrono::steady_clock::now();

    double rowMs = std::chrono::duration<double, std::milli>(endRow-startRow).count();


    auto startCol = std::chrono::steady_clock::now();
    
    double sumCol = sumColumnMajor(matrix,n);

    auto endCol = std::chrono::steady_clock::now();

    double colMs = std::chrono::duration<double, std::milli>(endCol - startCol).count();

    double ratio = colMs / rowMs;

    std::cout << "Row-major sum: " << sumRow << "\n";
    std::cout << "Column-major sum: " << sumCol << "\n";

    std::cout << std::boolalpha;
    std::cout << "Results match: " << (sumRow == sumCol) << "\n";

    std::cout << "Row-major time: " << rowMs << " ms" << "\n";
    std::cout << "Column-major time: " << colMs << " ms\n";
    std::cout << "Ratio: " << std::fixed << std::setprecision(3) << ratio << "x\n";
    
    double rowMedian = benchmarkRow(matrix, n);
    double colMedian = benchmarkCol(matrix, n);

    std::cout << std::fixed << std::setprecision(3);

    std::cout << "Row median: " << rowMedian << " ms\n";
    std::cout << "Col median: " << colMedian << " ms\n";

    if (rowMedian > 0.0) {
        std::cout << "Ratio: "
                << colMedian / rowMedian
                << "x\n";
    }
}