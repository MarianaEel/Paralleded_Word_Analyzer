/** searchstr.cpp
 * @file        searchstr_map.cpp
 * @author      Zhang, Chenbo (zhangcb@bu.edu)
 * @brief
 * @version     1.0
 * @date        2022-04-27
 *
 * @copyright   Copyright (c) 2021 Zhang, Chenbo
 *
 * @warning     input format:   data is no limitation text file
 *                              Pattern will read every line a new pattern
 *              output format:  which matched and matched count (max to min) ,
 */

//==================Here goes the includes======================//
#include <chrono>
#include <omp.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <sstream>
#include <cctype>
#include <algorithm>
/*===================Namespace Declare==========================*/
using namespace std; // for read-friendly

// first arg data, second Pattern
void errorhandle(int argc, char *argv[], string &inData, string &inPattern);
bool isNotAlnum(char c);

int main(int argc, char *argv[])
{
    int nthreads = -1;
    int thread_id = -1;

#pragma omp parallel shared(nthreads) private(thread_id)
    {
        thread_id = omp_get_thread_num();
        if (thread_id == 0)
            nthreads = omp_get_num_threads();

// You need this to make sure every thread learns how many
// threads there are in total before printing.
#pragma omp barrier

        printf("Hello world from thread %d of %d!\n", thread_id, nthreads);
    }

    string inData;
    string inPattern;
    string outlocation;
    string strdata;
    vector<string> vecstrdata;
    string Pattern;
    ifstream inFile;
    ofstream outFile;

    unordered_map<string, int> umap;

    // check if data and pattern setted, if not, set them default
    errorhandle(argc, argv, inData, inPattern);
    // set output location
    outlocation = "MatchedPattern_map_OMP";
    // printf("input data: %s\n",inData);
    // printf("input pattern: %s\n",inPattern);
    // printf("outlocation: %s\n",outlocation);

    inFile.open(inPattern);
    while (getline(inFile, Pattern))
    {
        umap[Pattern] = 0;
    }
    inFile.close();

    inFile.open(inData);
    while (inFile >> strdata) // traverse every line of input file,
                              // get pattern match count
                              // return a in-order map<int patterncount, set<long matched_pattern_in_data_posision>>
    {
        strdata.erase(remove_if(strdata.begin(), strdata.end(), isNotAlnum), strdata.end());
        vecstrdata.push_back(strdata);
    }
    inFile.close();

    // // test
    // for (auto it :vecstrdata){
    //     cout<<it<<endl;
    // }
    // // test

    /*===================Here Start finding words in map==========================*/
    int strnum = vecstrdata.size();

    // TIMING LINE 1: Get the starting timestamp.
    std::chrono::time_point<std::chrono::steady_clock> begin_time =
        std::chrono::steady_clock::now();

#pragma omp parallel for
    for (int i = 0; i < strnum; i++)
    {
        if (umap.find(vecstrdata[i]) != umap.end())
            umap[vecstrdata[i]]++;
    }

    // TIMING LINE 2: Get the ending timestamp.
    std::chrono::time_point<std::chrono::steady_clock> end_time =
        std::chrono::steady_clock::now();

    // TIMING LINE 3: Compute the difference.
    std::chrono::duration<double> difference_in_time = end_time - begin_time;

    // TIMING LINE 4: Get the difference in seconds.
    double difference_in_seconds = difference_in_time.count();
    // Print the time.
    printf("Took %.10f seconds.\n", difference_in_seconds);

    outFile.open(outlocation);
    outFile << "Using " << nthreads << " threads." << endl;
    outFile << "Time elapsed " << difference_in_seconds << endl;
    for (auto it = umap.begin(); it != umap.end(); it++)
    {
        // cout << it->first << '\t';
        outFile << left << it->first << '\t';
        // cout << itvec << '\t';
        outFile << left << it->second << '\n';
    }
    outFile.close();

    return 0;
}

/** errorhandle(int argc, char *argv[])
 * @brief
 * @return true when arguement number = 3 (1 input data, 1 input pattern)
 * @return false when arguement number != 3
 */
void errorhandle(int argc, char *argv[], string &inData, string &inPattern)
{
    try
    {
        if (argc == 3)
        {
            inData = argv[1];
            inPattern = argv[2];
        }
        else
        {
            throw(argc);
        }
    }
    catch (int argc)
    {
        // cout << "need 2 input, first data, second pattern" << endl;
        printf("need 2 input, first data, second pattern\n");
        // inData = "abcnews-date-text";
        inData = "abcnews-date-text";
        inPattern = "pattern";
        // cout << "input automatically set as 'abcnews-date-text.csv' and 'pattern'" << endl;
        printf("input automatically set as 'abcnews-date-text.csv' and 'pattern'\n");
        // assert(false);
    }
}

/** bool isNotAlnum(char c)
 * @brief
 * @return true when char is alnum
 * @return false if not
 */
bool isNotAlnum(char c)
{
    return isalnum(c) == 0;
}
