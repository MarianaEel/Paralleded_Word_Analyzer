/** searchstr.cpp
 * @file        searchstr.cpp
 * @author      Zhang, Chenbo (zhangcb@bu.edu)
 * @brief
 * @version     1.0
 * @date        2022-04-27
 *
 * @copyright   Copyright (c) 2021 Zhang, Chenbo
 *
 * @warning     input format:   data has no limitation
 *                              Pattern will read every line a new pattern
 *              output format:  matched count (max to min) ,         its row index in data
 */
#include "AhoCorasickAlg_OMP.hpp"

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
    bool bInputCheck;
    string inData;
    string inPattern;
    string outlocation;
    string strdata;
    string Pattern;
    ifstream inFile;
    ofstream outFile;

    map<string, int> mapOutput; // map<int, set<long>, greater<int>> mapOutput;
    vector<string> vecstrdata;
    vector<pair<string, int>> vecoutput;


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
    outlocation = "MatchedPattern_OMP";
    cout << "in data: " << inData << endl;
    cout << "in pattern: " << inPattern << endl;

    /* here goes code that new a CAhoTree and match Patterns within data using it*/
    CAhoTree oAhoCorasick(false); // true for case sensitive

    /* here openfile and give Pattern input*/
    inFile.open(inPattern);
    while (getline(inFile, Pattern))
    {
        oAhoCorasick.AddPattern(Pattern);
        mapOutput[Pattern] = 0;
    }
    inFile.close();
    oAhoCorasick.Redirecting();

    /* here openfile and give data input*/
    inFile.open(inData);

    while (getline(inFile, strdata)) // traverse every line of input file,
                                     // get pattern match count
                                     // return a in-order map<int patterncount, set<long matched_pattern_in_data_posision>>
    {
        vecstrdata.push_back(strdata);
    }

    int strnum = vecstrdata.size();
    vector<map<string, int>> vectempmapOutput(strnum);
// TIMING LINE 1: Get the starting timestamp.
std::chrono::time_point<std::chrono::steady_clock> begin_time =
    std::chrono::steady_clock::now();

#pragma omp parallel for
    for (int i = 0; i < strnum; i++)
    {
        vectempmapOutput[i] = oAhoCorasick.SearchPattern(vecstrdata[i]);
        // map<string, int> tempmapOutput =tempmapOutput;
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

    for (auto vecit : vectempmapOutput)
    {
        for (auto it : vecit)
        {
            mapOutput[it.first] += it.second;
        }
    }

    for (auto &it : mapOutput)
    {
        if (it.second != 0)
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

    outFile << "Using " << nthreads << " threads." << endl;
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
    // return false;
}
