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
    while (getline(ss, token, '|'))
    {
        removeWhiteSpace(token);
        rules.push_back(token);
    }

    return rules;
}

string getCommonPrefixes(const vector<string> &rules)
{
    if (rules.empty())
    {
        return "";
    }

    vector<string> tokens;

    for (const string &rule : rules)
    {
        stringstream ss(rule);
        string token;
        while (getline(ss, token, ' '))
        {
            removeWhiteSpace(token);
            if (nonTerminalSet.count(token) || terminalSet.count(token))
            {
                tokens.push_back(token);
            }
            else
            {
                break;
            }
        }
    }
    string commonPrefix = "";
    if (rules.size() == 1)
        return commonPrefix;
    for (int i = 0; i < tokens.size(); i++)
    {
        int count = 0;

        string potentialPrefix = "";
        if (commonPrefix == "")
            potentialPrefix = commonPrefix + tokens[i];
        else
            potentialPrefix = commonPrefix + " " + tokens[i];
        for (int j = 0; j < rules.size(); j++)
        {
            if (rules[j].find(potentialPrefix) == 0)
            {
                count++;
            }
        }
        if (count > 1)
        {
            commonPrefix = potentialPrefix;
        }
    }
    return commonPrefix;
}

void applyLeftFactoring(vector<pair<string, vector<string>>> &nonTerminals)
{
    bool foundCommonPrefix = true;
    while (foundCommonPrefix)
    {
        foundCommonPrefix = false;
        int z = 1;
        for (int i = 0; i < nonTerminals.size();)
        {
            vector<string> &rules = nonTerminals[i].second;
            string commonPrefix = getCommonPrefixes(rules);

            if (commonPrefix != "")
            {
                foundCommonPrefix = true;
                string a = commonPrefix; // Longest common prefix
                string newName;
                if (z != 1)
                    newName = nonTerminals[i].first + to_string(z);
                else
                    newName = nonTerminals[i].first + "'";

                vector<string> newRules;
                vector<string> nonCommonSuffixes;

                for (const string &rule : rules)
                {
                    if (rule.find(a) == 0)
                    {
                        newRules.push_back((rule.substr(a.size()) == "") ? "ε" : (rule.substr(a.size())));
                    }
                    else
                    {
                        nonCommonSuffixes.push_back(rule == "" ? "ε" : rule);
                    }
                }
                nonTerminals[i].second = {a + " " + newName};
                nonTerminalSet.insert(newName);
                for (auto k : nonCommonSuffixes)
                    nonTerminals[i].second.push_back(k);
                nonTerminals.push_back({newName, newRules});
                z++;
            }
            else
            {
                i++;
                z = 1;
            }
        }
    }
}

int main()
{
    vector<pair<string, vector<string>>> nonTerminals;

    ifstream inputFile("LeftRecursive.txt");
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

    applyLeftFactoring(nonTerminals);

    ofstream outputFile("GrammarLL.txt");
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
    cout << "Output has been written to GrammarLL.txt" << endl;

    return 0;
}