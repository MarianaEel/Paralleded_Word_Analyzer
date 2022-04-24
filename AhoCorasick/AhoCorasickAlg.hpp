/** AhoCorasickAlg.hpp
 * @brief       This program is a modification of icerlion's AchoCorasick,
 *              I modify and commented the original code for learning purpose
 *              Counters are added for ranking purpose;
 *              some inline function are not inline anymore to reduce compile size;
 *
 * @copyright   Copyright (c) 2021 Zhang, Chenbo
 * @author      Zhang, Chenbo (zhangcb@bu.edu)
 * @ref         icerlion (icerlion@163.com)
 *
 * @attention   This program can only be used for learning purpose ONLY.
 *              No commercial use of this program is allowed.
 *
 */
// the header guard can also be #pragma once
#ifndef _AHOCORASICK_H_
#define _AHOCORASICK_H_
//==================Here goes the includes======================//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <streambuf>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <set>
#include <map>
/*===================Namespace Declare==========================*/
using namespace std; // for read-friendly
/*=================Here goes Class Defination===================*/
class CAhoTree
{
private:
    /** Here start private properties of CAhoTree
     * @param BREAK_ON_NULLPTR stop when input null pointers.
     * @param RETURN_ON_NULLPTR return when input null pointers.
     *
     * @param SNode the basic node of CAhoTree
     *
     * @param parameter-name description
     */
#define BREAK_ON_NULLPTR(POINTER) \
    if (nullptr == POINTER)       \
    {                             \
        assert(false);            \
        break;                    \
    }
#define RETURN_ON_NULLPTR(POINTER, RETVAL) \
    if (nullptr == POINTER)                \
    {                                      \
        assert(false);                     \
        return RETVAL;                     \
    }

    typedef struct SNode
    /** Reason for using typedef
     * @brief Though Linus is angry about typedef and not recommend it,
     *  Here still use it because the opaque property of S_node,
     *  that the access to be function-only.
     *
     */
    {
        /** The discription of SNode
         * @brief this is the property of nodes in AhoThree
         *
         * @param nId int, the id of the node
         * @param nValue char, the value for matching
         * @param nHeight int, the height of node on the tree
         * @param pParent pointer, point to parent node
         * @param pRedirect pointer, point to the redirect node,
         *                  if the matching fails, go this way for next match
         * @param mapChild map<char, SNode*> link parent to their children
         * @param bEndNode decide if the node is a end of a matching pattern
         * @return
         */
        SNode()
            : nId(),
              nValue(0),
              nHeight(0),
              pParent(nullptr),
              pRedirect(nullptr),
              bEndNode(false)
        {
        }
        int nId;
        char nValue;
        map<char, SNode *> mapChild;
        int nHeight;
        SNode *pParent;
        SNode *pRedirect;
        bool bEndNode;

    } SNode;

    bool this_bCaseSensitive;
    int this_nNextNodeId;
    SNode *this_pRootNode;
    /** Map defination
     * @brief
     * Key: node value
     * Value: set of node which has the corresponding value
     */
    map<char, set<SNode *>> this_mapNode;
    vector<SNode *> this_vecNode;

public:
    CAhoTree(bool bCaseSensitive = false)      // Constructor
        : this_bCaseSensitive(bCaseSensitive), // take the input
          this_nNextNodeId(-1),                // -1 for invalid
          this_pRootNode(nullptr)              // pointer at root node
    {
        this_pRootNode = NewNode(0, nullptr, 0);
        this_pRootNode->pRedirect = this_pRootNode; // Root node redirect to itself
    }
    ~CAhoTree() // Destroyer
    {
        /** Destroyer
         * @brief use C++11 new sytax to traverse vector
         *          and delete every node;
         *          then clear both map and vector, free space
         *
         * @param this_vecNode local vector contains SNode pointers
         */
        for (SNode *pNode : this_vecNode)
        {
            delete pNode;
        }
        this_pRootNode = nullptr;
        this_mapNode.clear();
        this_vecNode.clear();
    }

    /** AddPattern(const string &strValue)
     * @brief add a pattern with input string
     *
     * @param strValue input string
     * @return true if success added
     * @return false if nullptr
     */
    bool AddPattern(const string &strValue)
    {
        int nHeight = 0;
        SNode *pCurNode = this_pRootNode; // link first letter to root node
        for (char chValue : strValue)
        {
            chValue = CaseConvert(chValue);
            ++nHeight;
            BREAK_ON_NULLPTR(pCurNode);
            auto iterFind = pCurNode->mapChild.find(chValue);
            if (iterFind == pCurNode->mapChild.end()) // not find
            {
                pCurNode = NewNode(chValue, pCurNode, nHeight); // add new node and point to it
            }
            else
            {
                pCurNode = iterFind->second;
            }
        }
        RETURN_ON_NULLPTR(pCurNode, false);
        pCurNode->bEndNode = true;
        return true; // success return;
    }

    /** void Redirecting()
     * @brief   redirecting nodes to each others, core function of aho corasick
     *          scanning all pairs of nodes in vecNode then call ConnectRedirectNode
     */
    void Redirecting()
    {
        for (auto &kvp : this_mapNode) // traverse the whole map of chValue to nodes
        {
            const set<SNode *> &setSNode = kvp.second; // for every chVal set
            for (SNode *pBaseNode : setSNode)
            {
                for (SNode *pRedirect : setSNode)
                {
                    ConnectRedirectNode(pBaseNode, pRedirect);
                }
            }
        }
    }

    /** bool MatchPattern(const string &strInput) const
     * @brief   if any pattern matches return true
     *          faster then the two search method
     * 
     * @param   strInput input data
     * @return  true if any patern matched
     * @return  false non matched
     */
    bool MatchPattern(const string &strInput) const
    {
        bool bResult = false;
        SNode *pCurNode = this_pRootNode;
        for (char chValue : strInput)
        {
            chValue = CaseConvert(chValue);
            while (true)
            {
                BREAK_ON_NULLPTR(pCurNode);
                auto iterFind = pCurNode->mapChild.find(chValue);
                if (iterFind == pCurNode->mapChild.end()) // matching failed
                {
                    if (pCurNode == this_pRootNode) // break if reach root
                    {
                        break;
                    }
                    BREAK_ON_NULLPTR(pCurNode);
                    pCurNode = pCurNode->pRedirect; // go redirect route
                }
                else
                {
                    pCurNode = iterFind->second; // get the node
                    BREAK_ON_NULLPTR(pCurNode);
                    if (pCurNode->bEndNode) // if reach end node, pattern found
                    {
                        bResult = true;
                    }
                    break;
                }
            }
            if (bResult) // dont continue if matched
            {
                break;
            }
        }
        return bResult;
    }

    /** inline map<string, int> SearchPattern(const string &strInput) const
     * @brief   return a map, 
     *              1st element is matched pattern,
     *              2nd element is matched times
     * 
     * @param   strInput input data
     * @return  map<string, int> 
     */
    inline map<string, int> SearchPattern(const string &strInput) const
    {
        map<string, int> mapOutput;
        string strOutput;
        SNode *pCurNode = this_pRootNode; // start from root
        for (char chValue : strInput)
        {
            chValue = CaseConvert(chValue);
            while (true)
            {
                BREAK_ON_NULLPTR(pCurNode);
                auto iterFind = pCurNode->mapChild.find(chValue); // search child
                if (iterFind == pCurNode->mapChild.end())         // no match
                {
                    BREAK_ON_NULLPTR(pCurNode);
                    if (pCurNode == this_pRootNode)
                    {
                        break;
                    }
                    pCurNode = pCurNode->pRedirect; // goto redirect
                }
                else // match
                {
                    pCurNode = iterFind->second; // go for node
                    BREAK_ON_NULLPTR(pCurNode);
                    if (pCurNode->bEndNode)
                    {
                        strOutput = GenerateOutput(pCurNode);
                        auto iterMap = mapOutput.find(strOutput);
                        if (iterMap == mapOutput.end()) // first find
                        {
                            mapOutput.insert(make_pair(strOutput, 1));
                        }
                        else
                        {
                            iterMap->second++; // counter ++
                        }
                    }
                    break;
                }
            }
        }
        return mapOutput;
    }
    
    /** inline int SearchCount(const string &strInput) const
     * @brief   return the sum of each matched pattern count
     *          that is, all pattern matched times
     * 
     * @param   strInput Pattern
     * @return  int  all pattern matched times
     */
    inline int SearchCount(const string &strInput) const
    {
        map<string, int> mapOutput;
        int nOutCounter = 0;
        string strOutput;
        SNode *pCurNode = this_pRootNode; // start from root
        for (char chValue : strInput)
        {
            chValue = CaseConvert(chValue);
            while (true)
            {
                BREAK_ON_NULLPTR(pCurNode);
                auto iterFind = pCurNode->mapChild.find(chValue); // search child
                if (iterFind == pCurNode->mapChild.end())         // no match
                {
                    BREAK_ON_NULLPTR(pCurNode);
                    if (pCurNode == this_pRootNode)
                    {
                        break;
                    }
                    pCurNode = pCurNode->pRedirect; // goto redirect
                }
                else // match
                {
                    pCurNode = iterFind->second; // go for node
                    BREAK_ON_NULLPTR(pCurNode);
                    if (pCurNode->bEndNode)
                    {
                        strOutput = GenerateOutput(pCurNode);
                        auto iterMap = mapOutput.find(strOutput);
                        if (iterMap == mapOutput.end()) // first find
                        {
                            mapOutput.insert(make_pair(strOutput, 1));
                            nOutCounter++;
                        }
                        else
                        {
                            iterMap->second++;
                            nOutCounter++;
                        }
                    }
                    break;
                }
            }
        }
        return nOutCounter;
    }

private:
    /** inline SNode* NewNode(char chValue, SNode *pParent, int nHeight)
     * @brief create a new node with nValue chValue, parent pParent, nheight nheight
     *
     * @param chValue
     * @param pParent
     * @param nHeight
     * @return SNode* return the pointer to new node
     */
    inline SNode *NewNode(char chValue, SNode *pParent, int nHeight)
    {
        SNode *pNode = new SNode();
        pNode->nId = ++this_nNextNodeId;
        pNode->nValue = chValue;
        pNode->nHeight = nHeight;
        pNode->pParent = pParent;
        pNode->pRedirect = this_pRootNode;
        pNode->bEndNode = false;
        if (pParent != nullptr) // has a parent then be its child
        {
            pParent->mapChild.insert(make_pair(chValue, pNode));
        }
        auto iterFind = this_mapNode.find(chValue);
        // element not found, then insert it as new set
        if (iterFind == this_mapNode.end())
        {
            set<SNode *> setInner;
            setInner.insert(pNode);
            this_mapNode.insert(make_pair(chValue, setInner));
        }
        // element found, add this node to original set
        else
        {
            set<SNode *> &setInner = iterFind->second; // second is the nodeset
            setInner.insert(pNode);
        }
        this_vecNode.push_back(pNode);
        return pNode;
    }

    /** inline bool ConnectRedirectNode(SNode *pBaseNode, SNode *pRedirect)
     * @brief   pass 2 node pointer in, scan along the node route;
     *          connect if all nodes on both route successfully match
     *
     * @param pBaseNode node to connected to redirect node
     * @param pRedirect redirect desitination
     * @return true if success connected
     * @return false if fail happend (etc. different chValue, height problem)
     */
    inline bool ConnectRedirectNode(SNode *pBaseNode, SNode *pRedirect)
    {
        RETURN_ON_NULLPTR(pBaseNode, false);
        RETURN_ON_NULLPTR(pRedirect, false);
        /** The redirect only occurs for heigher nodes to lower nodes
         * as a redirect means partial matching.
         * so pBaseNode->nHeight shall greater or equal than pRedirect->nHeight.
         *
         * Also, when the height of pBaseNode's redirected node
         * pBaseNode->pRedirect->nHeight
         * is the not smaller than pRedirect->nHeight
         *
         * And if a redirect node is assigned to a higher node,
         * do not re-assign it to lower one;
         */
        if (pBaseNode->nHeight <= pRedirect->nHeight)
        {
            return false;
        }
        if (nullptr != pBaseNode->pRedirect && pBaseNode->pRedirect->nHeight >= pRedirect->nHeight)
        {
            return false;
        }
        bool bResult = true;
        SNode *pBaseCursor = pBaseNode;
        SNode *pRedirectCursor = pRedirect;
        while (pRedirectCursor != this_pRootNode)
        // scan along all base route and redirect route
        {
            BREAK_ON_NULLPTR(pBaseCursor);
            BREAK_ON_NULLPTR(pRedirectCursor);
            if (CaseConvert(pBaseCursor->nValue) != CaseConvert(pRedirectCursor->nValue))
            { // not same value, skip and return false
                bResult = false;
                break;
            }
            pBaseCursor = pBaseCursor->pParent;
            pRedirectCursor = pRedirectCursor->pParent;
        }
        if (bResult) // if after scan all nodes on route matches, assign new redirection
        {
            pBaseNode->pRedirect = pRedirect;
        }
        return bResult;
    }

    /** inline char CaseConvert(char chValue) const
     * @brief const for not changing original input data
     *          this code return uppercase letter to lower case letter
     *          when this_bCaseSensitive == 0
     * @param chValue input char value
     * @return char
     */
    inline char CaseConvert(char chValue) const
    {
        if (!this_bCaseSensitive) // if not casesensitive
        {
            if (chValue >= 'A' && chValue <= 'Z') // if chValue is a letter
            {
                return chValue + ('a' - 'A'); // Upper to lower
            }
        }
        return chValue;
    }

    /** GenerateOutput(SNode *pNode) const
     * @brief read a matched end node and read from it to root, return the matched word
     * 
     * @param pNode input node
     * @return string matched word
     */
    string GenerateOutput(SNode *pNode) const
    {
        string strResult;
        RETURN_ON_NULLPTR(pNode, strResult);
        strResult.reserve(pNode->nHeight); 
        while (nullptr != pNode && pNode->pParent != nullptr)
        {
            strResult.push_back(pNode->nValue);
            pNode = pNode->pParent;
            BREAK_ON_NULLPTR(pNode);
        }
        std::reverse(strResult.begin(), strResult.end());
        return strResult;
    }
};
#endif