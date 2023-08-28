#include <bits/stdc++.h>
#include <iostream>

using namespace std;
string startSymbol;
set<string> nonTerminalSet, terminalSet;

void removeWhiteSpace(string &token)
{
    size_t startPos = token.find_first_not_of(" \t");
    if (startPos != string::npos)
    {
        token.erase(0, startPos);
    }
    size_t endPos = token.find_last_not_of(" \t");
    if (endPos != string::npos)
    {
        token.erase(endPos + 1);
    }
}

vector<string> getRules(string rule)
{
    vector<string> rules;
    stringstream ss(rule);
    string token;
    removeWhiteSpace(token);
    while (getline(ss, token, '|'))
    {
        removeWhiteSpace(token);
        rules.push_back(token);
    }

    return rules;
}

void solveNonImmediateLR(vector<pair<string, vector<string>>> &nonTerminals, int A, int B)
{
    string nameA = nonTerminals[A].first;
    string nameB = nonTerminals[B].first;

    vector<string> &rulesA = nonTerminals[A].second;
    vector<string> &rulesB = nonTerminals[B].second;
    vector<string> newRulesA;

    for (const string &rule : rulesA)
    {
        if (rule.substr(0, nameB.size()) == nameB)
        {
            for (const string &rule1 : rulesB)
            {
                string newRule = rule.substr(nameB.size());
                removeWhiteSpace(newRule);
                if (rule1 == "ε")
                {
                    newRulesA.push_back(rule1);
                    continue;
                }
                newRulesA.push_back(rule1 + " " + newRule);
            }
        }
        else
        {
            newRulesA.push_back(rule);
        }
    }

    nonTerminals[A].second = newRulesA;
}

void solveImmediateLR(vector<pair<string, vector<string>>> &nonTerminals, int A)
{
    string name = nonTerminals[A].first;
    string newName = name + "'";

    vector<string> &rules = nonTerminals[A].second;
    vector<string> alphas, betas, newRulesA, newRulesA1;

    for (const string &rule : rules)
    {
        if (rule.substr(0, name.size()) == name)
        {
            string newRule = rule.substr(name.size());
            removeWhiteSpace(newRule);
            alphas.push_back(rule.substr(name.size()));
        }
        else
        {
            betas.push_back(rule);
        }
    }

    if (alphas.empty())
    {
        return;
    }

    if (betas.empty())
    {
        newRulesA.push_back(newName);
    }

    for (const string &beta : betas)
    {
        if (beta == "ε")
            continue;
        newRulesA.push_back(beta + " " + newName);
    }

    for (const string &alpha : alphas)
    {
        if (alpha == "ε")
            continue;
        newRulesA1.push_back(alpha + " " + newName);
    }

    nonTerminals[A].second = newRulesA;
    newRulesA1.push_back("ε"); // Adding ε (epsilon)
    terminalSet.insert("ε");
    nonTerminals.push_back({newName, newRulesA1});
    nonTerminalSet.insert(newName);
}

void applyAlgorithm(vector<pair<string, vector<string>>> &nonTerminals)
{
    int size = nonTerminals.size();
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < i; j++)
        {
            solveNonImmediateLR(nonTerminals, i, j);
        }
        solveImmediateLR(nonTerminals, i);
    }
}

int main()
{
    vector<pair<string, vector<string>>> nonTerminals;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
    {
        cout << "Failed to open input file." << endl;
        return 1;
    }

    string nonTerminalsStr, terminalsStr, line;

    getline(inputFile, startSymbol);
    getline(inputFile, nonTerminalsStr);
    getline(inputFile, terminalsStr);

    stringstream ssNonTerminals(nonTerminalsStr), ssTerminals(terminalsStr);
    string nt, t;

    while (getline(ssNonTerminals, nt, ','))
    {
        removeWhiteSpace(nt);
        nonTerminalSet.insert(nt);
    }

    while (getline(ssTerminals, t, ','))
    {
        removeWhiteSpace(t);
        terminalSet.insert(t);
    }

    while (getline(inputFile, line))
    {
        size_t arrowPos = line.find("->");
        if (arrowPos != string::npos)
        {
            string nonTerminal = line.substr(0, arrowPos);
            string rule = line.substr(arrowPos + 2);
            removeWhiteSpace(nonTerminal);
            removeWhiteSpace(rule);

            nonTerminals.push_back({nonTerminal, getRules(rule)});
        }
        else
        {
            cout << "Invalid input format: " << line << endl;
        }
    }

    applyAlgorithm(nonTerminals);

    ofstream outputFile("LeftRecursive.txt");
    if (!outputFile.is_open())
    {
        cout << "Failed to open output file." << endl;
        return 1;
    }
    outputFile << startSymbol << "\n";
    int i = nonTerminalSet.size();
    for (const auto &nt : nonTerminalSet)
    {
        if (i > 1)
            outputFile << nt << ", ";
        else if (i == 1)
            outputFile << nt;
        else
            break;
        i--;
    }
    outputFile << "\n";
    i = terminalSet.size();
    for (const auto &nt : terminalSet)
    {
        if (i > 1)
            outputFile << nt << ", ";
        else if (i == 1)
            outputFile << nt;
        else
            break;
        i--;
    }
    outputFile << "\n";
    for (const auto &nt : nonTerminals)
    {
        outputFile << nt.first << " -> ";
        for (int i = 0; i < nt.second.size(); i++)
        {
            if (i == nt.second.size() - 1)
            {
                outputFile << nt.second[i];
            }
            else
            {
                outputFile << nt.second[i] << " | ";
            }
        }
        outputFile << endl;
    }

    outputFile.close();
    cout << "Output has been written to LeftRecursive.txt" << endl;
    return 0;
}

// A1->A2A3
// A2->A3A1|b
// A3->A1A1|a
// A3->A3A1A3A1|a|bA3A1

// A->Aa|b
// A->bA'
// A'->aA'|e