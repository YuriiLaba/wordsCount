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
#include <sstream>

using namespace std;
mutex mtx;


//for(x: lm)
//gm[x.first] += x.sp
//

vector<string> open_read(string path) {
    ifstream myfile;
    vector<string> words;
    string word;
    myfile.open(path);
    if (!myfile.is_open()) {
        cerr << "Error" << endl;
        return words;
    }
    string formated_word;
    while (myfile >> word) {
        for (size_t i = 0, len = word.size(); i < len; i++)
        {
            auto to = begin(word);
            for (auto from : word)
                if (!ispunct(from))
                    *to++ = from;
            word.resize(distance(begin(word), to));
        }

        transform(word.begin(), word.end(), word.begin(), ::tolower);
        words.push_back(word);
    }
    myfile.close();
    return words;
}


int finder(const vector<string>& myVector, int start, int end, std::map<string, int>& m)
{
    map<string, int> localMp;

    for (int i = start; i < end; i++) {
        {
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
void write_to_file(const map<string, int> &m, string path) {
    ofstream myfile;
    myfile.open(path);
    for (auto elem : m) {
        myfile << elem.first << "    " << elem.second << "\n";
    }
    myfile.close();
}

void printMap(const map<string, int> &m) {
    for (auto elem : m) {
        cout << elem.first << " : " << elem.second << "\n";
    }
}

vector<int> SplitVector(const vector<string>& vec, int n) {

    vector<int> outVec;
    int length = int(vec.size())/ n;
    int count = 0;
    int sum = 0;

    outVec.push_back(0);
    while(count != n - 1){
        sum += length;
        outVec.push_back(sum);
        //cout<<outVec[count]<<endl;
        count++;
    }
    outVec.push_back(int(vec.size()));
    return outVec;
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
    int threads_n;
    ifstream myFile;
    myFile.open("data_input.txt");

    for(int i = 0; i < 4; i++)
        myFile >> input_data[i];
    myFile.close();

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < input_data[i].length(); j++) {
            if (input_data[i][j] == '=')
                input_data[i][j] = ' ';
        }
        stringstream ss(input_data[i]);
        string temp;
        int k = 0;
        while (ss >> temp) {
            if (k != 0) {
                stringstream lineStream(temp);
                string s;
                getline(lineStream,s,',');
                s.erase(remove( s.begin(), s.end(), '\"' ), s.end());
                input_data[i] = s;
            }
            k++;
        }
    }

    infile = input_data[0];
    out_by_a = input_data[1];
    out_by_n = input_data[2];
    threads_n = stoi(input_data[3]);

    vector<string> words;
    words = open_read(infile);
    vector<pair<string, int>> VectorOfPair;
    map<string, int> m;
    thread threads[threads_n];
    vector<int> list_of_words_amount = SplitVector(words, threads_n);

    for (int a = 0; a < list_of_words_amount.size()-1; ++a) {

        threads[a] = thread(finder, cref(words), list_of_words_amount[a],
                            list_of_words_amount[a + 1], ref(m));
    }

    for (auto& th : threads) th.join();


    sort(words.begin(), words.end());
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
    myWriteFile.open(out_by_n);
    if (!myWriteFile) {
        cerr << "Could not open file." << endl;
        return 1;
    }
    for ( auto it = VectorOfPair.begin(); it != VectorOfPair.end(); it++ )
    {
        myWriteFile << it->first << " : "<< it->second<<endl;
        // To get hold of the class pointers:
    }
    myWriteFile1.close();
    printMap(m);

    return 0;

}
