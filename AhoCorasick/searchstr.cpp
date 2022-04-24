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

char *ReadSpeacialLine(char *filename, long whichLine);
// first arg data, second Pattern
bool errorhandle(int argc, char *argv[]);
int main(int argc, char *argv[])
{
    bool bInputCheck;
    string inData;
    string inPattern;
    string outlocation;
    string strdata;
    int nPatternnum;
    string Pattern;
    ifstream inFile;
    ofstream outFile;
    long linepos;
    map<int, set<long>, greater<int>> mapOutput;
    char *sdf = NULL;
    FILE *fp;

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
        inPattern = "Pattern";
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
    linepos = 1;
    while (getline(inFile, strdata)) // traverse every line of input file,
                                     // get pattern match count
                                     // return a in-order map<int patterncount, set<long matched_pattern_in_data_posision>>
    {
        nPatternnum = oAhoCorasick.SearchCount(strdata);
        auto iterFind = mapOutput.find(nPatternnum);
        // element not found, then insert it as new set
        if (iterFind == mapOutput.end())
        {
            set<long> veclinepos;
            veclinepos.insert(linepos);
            mapOutput.insert(make_pair(nPatternnum, veclinepos));
        }
        else // element found, add it in set
        {
            set<long> &veclinepos = iterFind->second;
            veclinepos.insert(linepos);
        }
        linepos++;
    }
    inFile.close();

    /** generate output file
     * @brief iterate map then map->linepos set for top 20 hits
     *
     */
    outFile.open(outlocation);
    int i = 1;
    for (auto it = mapOutput.begin(); it != mapOutput.end(); it++)
    {
        for (auto itvec : it->second)
        {
            cout << it->first << '\t';
            outFile << left << it->first << '\t';
            cout << itvec << '\t';
            outFile << left << itvec << '\t' ;
            // str to char*
            char *cstr = new char[inData.length() + 1];
            strcpy(cstr, inData.c_str());

            sdf = ReadSpeacialLine(cstr, itvec);
            if (sdf)
            {
                cout << sdf << endl;
                outFile << left << sdf << endl;
            }
            // do stuff
            delete[] cstr;
            FREE_MALLOC_PTR(sdf); //此处必须有,因为在RSL函数里面分配了堆内存

            i++;
        }
        if (i > 20)
            break;
    }
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

char *ReadSpeacialLine(char *filename, long whichLine)
{
    if (whichLine < 0 || NULL == filename)
    {
        return NULL;
    }
    FILE *fp = fopen(filename, "r");
    if (NULL == fp)
    {
        return NULL;
    }
    int reachWhichLine = 0;
    int curLine = 1;
#define LINE_SIZE 256
    char *data = NULL;
    data = (char *)malloc(LINE_SIZE);
    while (!feof(fp)) //文件未结束
    {
        memset(data, 0, LINE_SIZE);
        fgets(data, LINE_SIZE - 1, fp);
        curLine++;
        if (curLine > whichLine)
        {
            reachWhichLine = 1; //已经读取到whichLine行
            break;
        }
    }
    fclose(fp);

    return 0 == reachWhichLine ? NULL : data;
}
