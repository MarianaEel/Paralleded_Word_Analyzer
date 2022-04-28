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
#include <iostream>
#include <chrono>
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
    string inData;
    string inPattern;
    string outlocation;
    string strdata;
    vector<string> vecstrdata;
    string Pattern;
    ifstream inFile;
    ofstream outFile;

    unordered_map<string, int> umap;

    // #pragma acc init

    // check if data and pattern setted, if not, set them default
    errorhandle(argc, argv, inData, inPattern);
    // set output location
    outlocation = "MatchedPattern_map";
    cout << "in data: " << inData << endl;
    cout << "in pattern: " << inPattern << endl;
    cout << "outlocation: " << outlocation << endl;

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

    outFile.open(outlocation);
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
        cout << "need 2 input, first data, second pattern" << endl;
        // inData = "abcnews-date-text";
        inData = "abcnews-date-text";
        inPattern = "pattern";
        cout << "input automatically set as 'abcnews-date-text.csv' and 'pattern'" << endl;
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
