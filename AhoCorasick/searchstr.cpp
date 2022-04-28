/** searchstr.cpp
 * @file        searchstr.cpp
 * @author      Zhang, Chenbo (zhangcb@bu.edu)
 * @brief
 * @version     1.0
 * @date        2021-12-06
 *
 * @copyright   Copyright (c) 2021 Zhang, Chenbo
 *
 * @warning     input format:   data has no limitation
 *                              Pattern will read every line a new pattern
 *              output format:  matched count (max to min) ,         its row index in data
 */
#include "AhoCorasickAlg.hpp"

#define SET_ARRAY_ITEMS_ZERO(arrName) memset(arrName, 0, sizeof(arrName))
#define FREE_MALLOC_PTR(ptrName) \
    ;                            \
    if (NULL != ptrName)         \
    {                            \
        free(ptrName);           \
        ptrName = NULL;          \
    }

// first arg data, second Pattern
bool errorhandle(int argc, char *argv[]);

bool cmp(const pair<string, int> &a,
         const pair<string, int> &b)
{
    return a.second > b.second;
}

int main(int argc, char *argv[])
{
    bool bInputCheck;
    string inData;
    string inPattern;
    string outlocation;
    string strdata;
    string Pattern;
    ifstream inFile;
    ofstream outFile;

    map<string, int> mapOutput; // map<int, set<long>, greater<int>> mapOutput;
    vector<pair<string, int>> vecoutput;
    vector<string> vecstrdata;

    /** errorhandle return bInputCheck fail if input invalid,
     *  use default input data and pattern instead
     */
    bInputCheck = errorhandle(argc, argv);
    if (bInputCheck) // input check success
    {
        inData = argv[1];
        inPattern = argv[2];
    }
    else
    {
        inData = "abcnews-date-text";
        inPattern = "pattern";
    }

    // set output location
    outlocation = "MatchedPattern";
    cout << "in data: " << inData << endl;
    cout << "in Pattern: " << inPattern << endl;

    /* here goes code that new a CAhoTree and match Patterns within data using it*/
    CAhoTree oAhoCorasick(false); // true for case sensitive

    /* here openfile and give Pattern input*/
    inFile.open(inPattern);
    while (getline(inFile, Pattern))
    {
        oAhoCorasick.AddPattern(Pattern);
    }
    inFile.close();
    oAhoCorasick.Redirecting();

    /* here openfile and give data input*/
    inFile.open(inData);

    // while (getline(inFile, strdata))
    // {
    //     map<string, int> tempmapOutput = oAhoCorasick.SearchPattern(strdata);
    //     for (auto it : tempmapOutput){
    //         if(mapOutput.find(it.first)==mapOutput.end()){
    //             mapOutput.insert(it);
    //         }
    //         else{
    //             mapOutput[it.first]+=it.second;
    //         }
    //     }
    // }
    while (getline(inFile, strdata)) // traverse every line of input file,
                                     // get pattern match count
                                     // return a in-order map<int patterncount, set<long matched_pattern_in_data_posision>>
    {
        vecstrdata.push_back(strdata);
    }

    int strnum = vecstrdata.size();

    // TIMING LINE 1: Get the starting timestamp.
    std::chrono::time_point<std::chrono::steady_clock> begin_time =
        std::chrono::steady_clock::now();

    for (int i = 0; i < strnum; i++)
    {
        map<string, int> tempmapOutput = oAhoCorasick.SearchPattern(vecstrdata[i]);
        for (auto it : tempmapOutput)
        {
            if (mapOutput.find(it.first) == mapOutput.end())
            {
                mapOutput.insert(it);
            }
            else
            {
                mapOutput[it.first] += it.second;
            }
        }
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

    for (auto &it : mapOutput)
    {
        vecoutput.push_back(it);
    }
    sort(vecoutput.begin(), vecoutput.end(), cmp);
    inFile.close();

    /** generate output file
     * @brief iterate map and count how many times each word appears
     *
     */
    outFile.open(outlocation);

    for (auto it = vecoutput.begin(); it != vecoutput.end(); it++)
    {

        // cout << it->first << '\t';
        outFile << left << it->first << '\t';
        // cout << itvec << '\t';
        outFile << left << it->second << '\n';
    }
    outFile << "Time elapsed " << difference_in_seconds  << endl;

    outFile.close();
    return 0;
}

/** errorhandle(int argc, char *argv[])
 * @brief
 * @return true when arguement number = 3 (1 input data, 1 input pattern)
 * @return false when arguement number != 3
 */
bool errorhandle(int argc, char *argv[])
{
    try
    {
        if (argc == 3)
        {
            return true;
        }
        else
        {
            throw(argc);
        }
    }
    catch (int argc)
    {
        cout << "need 2 input, first data, second pattern" << endl;
        cout << "input automatically set as 'abcnews-date-text.csv' and 'pattern'" << endl;
        // assert(false);
        return false;
    }
    return false;
}
