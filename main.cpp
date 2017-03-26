#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <vector>
#include <algorithm>
#include <set>
#include <mutex>

using namespace std;
mutex mtx;

int finder(const std::vector<string>& myVector, int start, int end, std::map<std::string, int>& m)
{   mtx.lock();
    for (int i = start; i < end; i++) {

        ++m[myVector[i]];
    }
    mtx.unlock();
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
    for (int i = 0; i < r.size(); i++)
        if (r[i] >= 'A' && r[i] <= 'Z')
            r[i] = tolower(r[i]);
    return r;
}

int main() {
    clock_t t1, t2;


    //vector<string>cont = {",", "/", ".", ";",":", "_", "!", "?"};
    vector<string> myVector;
    vector<pair<string, int>> VectorOfPair;
    map<std::string, int> m;

    ifstream myReadFile;
    string data;
    myReadFile.open("text.txt");
    string output;
    if (!myReadFile.is_open()) {
        cerr << "Error opening file ";
        return -1; // exit(-1);
    }
    while (myReadFile >> output) {//check state
        myVector.push_back(output);
    }
    myReadFile.close();

    for (unsigned int i = 0; i < myVector.size(); i++)
    {
        myVector[i]=toLowerCase(myVector[i]);
    }


//=========================================================
    int  numberOfThreads;
    cout << "Please enter an number of threads: ";
    cin >> numberOfThreads;


    thread threads[numberOfThreads];
    vector<int> partsOfMyVector;
    int step = int(myVector.size())/numberOfThreads;
    int iter = 0;
    int sumOfPart = 0;
    partsOfMyVector.push_back(0);
    while(iter != numberOfThreads-1){
        sumOfPart+=step;
        partsOfMyVector.push_back(sumOfPart);
        iter++;
    }
    partsOfMyVector.push_back(int(myVector.size()));

    t1 = clock();
    for (int thrIter=0; thrIter<partsOfMyVector.size()-1; ++thrIter) {

        threads[thrIter] = thread(finder, cref(myVector), partsOfMyVector[thrIter],
                                  partsOfMyVector[thrIter+1], ref(m));

    }
    for (auto& th : threads) th.join();
    t2=clock();

    sort(myVector.begin(), myVector.end());
    for(map<string, int> :: iterator i = m.begin(); i != m.end(); i++){
        VectorOfPair.push_back(make_pair(i-> first, i-> second));

    }

    ofstream myWriteFile;
    myWriteFile.open("Alphabet.txt");
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
    myWriteFile.open("Increasing.txt");
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

    for(map<string, int> :: iterator i = m.begin(); i != m.end(); i++){
        cout << i -> first << ": " << i-> second << endl;

    }

    //for(auto i = VectorOfPair.begin(); i != VectorOfPair.end(); i++){
    //cout << i -> first << ": " << i-> second << endl;

    //}
    //t2=clock();
    float diff ((float)t2-(float)t1);
    float milSeconds = (diff) / (CLOCKS_PER_SEC / 1000);
    cout<<"Time in milliseconds: "<<milSeconds<<endl;
    return 0;
}