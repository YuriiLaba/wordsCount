#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <vector>
#include <algorithm>
#include <set>
#include <mutex>
#include <chrono>
#include <atomic>
using namespace std;
mutex mtx;


//for(x: lm)
//gm[x.first] += x.sp
//

int finder(const vector<string>& myVector, int start, int end, std::map<string, int>& m)
{
    map<string, int> localMp;

    for (int i = start; i < end; i++) {
        {
            lock_guard<mutex> lg(mtx);
            ++localMp[myVector[i]];
        }
    }
    lock_guard<mutex> lg(mtx);

    for(map<string, int> :: iterator i = localMp.begin(); i != localMp.end(); i++){
        m[i->first] += i-> second;

    }

    return 0;

}
bool sortBySecond(const pair<string,int> &a,
                  const pair<string,int> &b)
{
    return (a.second < b.second);
}
string toLowerCase(string s)
{
    string r = s;
    for (size_t i = 0; i < r.size(); i++)
        if (r[i] >= 'A' && r[i] <= 'Z')
            r[i] = (char) tolower(r[i]);
    return r;
}


inline std::chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    std::atomic_thread_fence(memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D& d)
{
    return std::chrono::duration_cast<chrono::microseconds>(d).count();
}

int main() {
    string input_data[4], infile, out_by_a, out_by_n;
    int numOfThreads;
    ifstream myFile;
    myFile.open("data_input.txt");

    for(int i = 0; i <= 4; i++)
        myFile>>input_data[i];
    myFile.close();

    infile = input_data[0].substr(8, 8);
    out_by_a = input_data[1].substr(10, 9);
    out_by_n = input_data[2].substr(10, 9);
    numOfThreads = atoi(input_data[3].substr(8).c_str());
    //clock_t t1, t2;


    //vector<string>cont = {",", "/", ".", ";",":", "_", "!", "?"};
    vector<string> myVector;
    vector<pair<string, int>> VectorOfPair;
    map<string, int> m;
    auto startRead= get_current_time_fenced();
    ifstream myReadFile;
    string data;
    myReadFile.open(infile);
    string output;
    if (!myReadFile.is_open()) {
        cerr << "Error opening file ";
        return -1; // exit(-1);
    }
    while (myReadFile >> output) {//check state
        for (size_t i = 0, len = output.size(); i < len; i++){
            if (ispunct(output[i]))
            {
                output.erase(i--, 1);
                len = output.size();
            }
        }
        myVector.push_back(output);
    }
    myReadFile.close();

    //for (size_t i = 0; i < myVector.size(); i++)
    //{
        //myVector[i]=toLowerCase(myVector[i]);
    //}
    transform(myVector.begin(), myVector.end(), myVector.begin(), toLowerCase);


//=========================================================
    //int  numberOfThreads;
    //cout << "Please enter an number of threads: ";
    //cin >> numberOfThreads;


    thread threads[numOfThreads];
    vector<int> partsOfMyVector;

    int step = int(myVector.size())/numOfThreads;
    int iter = 0;
    int sumOfPart = 0;
    partsOfMyVector.push_back(0);
    while(iter != numOfThreads-1){
        sumOfPart+=step;
        partsOfMyVector.push_back(sumOfPart);
        iter++;
    }
    partsOfMyVector.push_back(int(myVector.size()));

    auto startCount = get_current_time_fenced();

    for (int thrIter=0; thrIter<partsOfMyVector.size()-1; ++thrIter) {
        cout<<partsOfMyVector[thrIter]<<endl;
        cout<<partsOfMyVector[thrIter+1]<<endl;
        threads[thrIter] = thread(finder, cref(myVector), partsOfMyVector[thrIter],
                                  partsOfMyVector[thrIter+1], ref(m));

    }
    for (auto& th : threads) th.join();
    auto finishCount = get_current_time_fenced();

    auto countingTime = finishCount - startCount;

    ofstream myWriteFileBash;

    sort(myVector.begin(), myVector.end());
    for(map<string, int> :: iterator i = m.begin(); i != m.end(); i++){
        VectorOfPair.push_back(make_pair(i-> first, i-> second));

    }

    ofstream myWriteFile;
    myWriteFile.open(out_by_a);
    if (!myWriteFile) {
        cerr << "Could not open file." << endl;
        return 1;
    }
    for ( auto it = VectorOfPair.begin(); it != VectorOfPair.end(); it++ )
    {
        myWriteFile << it->first << " : "<< it->second<<endl;
        // To get hold of the class pointers:
    }
    myWriteFile.close();

    sort(VectorOfPair.begin(), VectorOfPair.end(), sortBySecond);

    ofstream myWriteFile1;
    myWriteFile1.open(out_by_n);
    if (!myWriteFile1) {
        cerr << "Could not open file." << endl;
        return 1;
    }
    for ( auto it = VectorOfPair.begin(); it != VectorOfPair.end(); it++ )
    {
        myWriteFile1 << it->first << " : "<< it->second<<endl;
        // To get hold of the class pointers:
    }
    myWriteFile1.close();

    auto finishRead = get_current_time_fenced();
    auto readingTime = finishRead - startRead;

    cout << "Counting Time: " << to_us(countingTime) << endl;
    cout << "Reading Time: " << to_us(readingTime) << endl;


    myWriteFileBash.open("result.txt");
    if (!myWriteFileBash) {
        cerr << "Could not open file." << endl;
        return 1;
    }else{
        myWriteFileBash<< to_us(countingTime) << endl;
        //myWriteFileBash<< to_us(readingTime) << endl;

    }

    //for(map<string, int> :: iterator i = m.begin(); i != m.end(); i++){
        //cout << i -> first << ": " << i-> second << endl;

    //    }

    //for(auto i = VectorOfPair.begin(); i != VectorOfPair.end(); i++){
    //cout << i -> first << ": " << i-> second << endl;

    //}
    //t2=clock();

    return 0;
}