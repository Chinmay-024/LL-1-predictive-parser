#include <bits/stdc++.h>
using namespace std;

// Map to store the parsing table
map<string, map<string, string>> parsingTable;
string startSymbol;

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

set<string> computeFirstSet(const string &symbol, const map<string, vector<string>> &productionRules,
                            map<string, set<string>> &firstSets, const set<string> &terminals)
{
    // If the FIRST set for the symbol is already computed, return it
    if (firstSets.find(symbol) != firstSets.end())
    {
        return firstSets[symbol];
    }

    set<string> first;

    // Rule 1: If X is a terminal, then FIRST(X) = {X}
    if (terminals.count(symbol))
    {
        first.insert(symbol);
    }
    else
    {
        // Rule 2: If X is a nonterminal
        if (productionRules.find(symbol) != productionRules.end())
        {
            int truth = 0;
            for (const string &rule : productionRules.at(symbol))
            {
                stringstream ss(rule);
                string nextSymbol;
                int f = 0;
                while (getline(ss, nextSymbol, ' '))
                {
                    set<string> symbolFirst = computeFirstSet(nextSymbol, productionRules, firstSets, terminals);
                    first.insert(symbolFirst.begin(), symbolFirst.end());
                    if (symbolFirst.find("ε") == symbolFirst.end())
                    {
                        f = 1;
                        break;
                    }
                }
                if (f == 1)
                    first.erase("ε");
                if (f == 0)
                    truth = 1;
            }
            if (truth)
                first.insert("ε");
            // // Rule 3: If X -> € is a production, then add € to FIRST(X)
            for (auto i : productionRules.at(symbol))
            {
                if (i == "ε")
                    first.insert("ε");
            }
        }
    }

    // Cache the computed FIRST set for the symbol and return it
    firstSets[symbol] = first;
    return first;
}

set<string> computeFollowSet(const string &symbol, const map<string, vector<string>> &productionRules,
                             map<string, set<string>> &firstSets, map<string, set<string>> &followSets,
                             const set<string> &terminals) // Add terminals parameter here
{
    // If the FOLLOW set for the symbol is already computed, return it
    if (followSets.find(symbol) != followSets.end())
    {
        return followSets[symbol];
    }

    set<string> follow;

    // Rule 1: Add $ to FOLLOW(S) where S is the starting non-terminal
    if (symbol == startSymbol)
    {
        follow.insert("$");
    }

    // Traverse through the production rules to find where the symbol appears
    for (const auto &entry : productionRules)
    {
        const string &nonTerminal = entry.first;
        for (const string &rule : entry.second)
        {
            string rule3 = rule;
            size_t pos = rule3.find(symbol);

            while (pos != string::npos)
            {
                if ((pos + symbol.length() == rule3.length()) || (rule3[pos + symbol.length()] == ' '))
                {
                    string remaining = rule3.substr(pos + symbol.length());
                    removeWhiteSpace(remaining);
                    stringstream ss(remaining);
                    string token;
                    while (getline(ss, token, ' '))
                    {
                        removeWhiteSpace(token);
                        if (terminals.find(token) != terminals.end())
                        {
                            follow.insert(token);
                            break;
                        }
                        bool flag = false;
                        for (auto el : computeFirstSet(token, productionRules, firstSets, terminals))
                        {
                            if (el == "ε")
                                flag = true;
                            else
                            {
                                follow.insert(el);
                            }
                        }
                        if (!flag)
                            break;
                        pos += symbol.length();
                    }
                }
                else
                    pos++;
                pos = rule3.find(symbol + " ", pos);
                if (pos != string::npos)
                    break;
            }
        }
    }

    // Cache the computed FOLLOW set for the symbol and return it
    followSets[symbol] = follow;
    return follow;
}

int followRule3(const string &symbol, const map<string, vector<string>> &productionRules,
                map<string, set<string>> &firstSets, map<string, set<string>> &followSets,
                const set<string> &terminals)
{
    int ans = 0;
    for (const auto &entry : productionRules)
    {
        const string &nonTerminal = entry.first;
        for (const string &rule : entry.second)
        {
            string rule2 = rule;
            int f = 0;

            istringstream iss(rule2);
            std::vector<std::string> tokens;
            std::string token1;
            while (iss >> token1)
            {
                tokens.push_back(token1);
            }
            for (auto it = tokens.rbegin(); it != tokens.rend(); ++it)
            {
                string token = *it;
                removeWhiteSpace(token);
                if (token == symbol)
                {
                    f = 1;
                    break;
                }
                bool flag = false;
                for (auto el : computeFirstSet(token, productionRules, firstSets, terminals))
                {
                    if (el == "ε")
                        flag = true;
                }
                if (!flag)
                    break;
            }

            if (f == 1)
            {
                if (nonTerminal != symbol)
                {
                    computeFollowSet(nonTerminal, productionRules, firstSets, followSets, terminals);
                    for (auto el : followSets[nonTerminal])
                    {
                        if (!followSets[symbol].count(el))
                        {
                            followSets[symbol].insert(el);
                            ans++;
                        }
                    }
                }
            }
        }
    }
    return ans;
}

int nodeCounter = 1; // Initialize the node counter

class Node
{
public:
    int ind;
    string label;
    vector<Node *> links;
    int vis;

    Node(string l)
    {
        ind = nodeCounter++;
        label = l;
        vis = 0;
    }
};

void drawTree(Node *root)
{
    if (root == nullptr)
        return;

    ofstream dot_file("graph.dot");

    dot_file << "digraph G {\n";
    dot_file << "node [shape=\"circle\"]\n";

    queue<Node *> bfs;
    dot_file << "\t" << root->ind << " [label=\"" << root->label << "\"]\n";
    bfs.push(root);

    while (!bfs.empty())
    {
        Node *curr = bfs.front();
        bfs.pop();
        for (int i = 0; i < curr->links.size(); i++)
        {
            dot_file << "\t" << curr->links[i]->ind << " [label=\"" << curr->links[i]->label << "\"]\n";
            dot_file << "\t" << curr->ind << " -> " << curr->links[i]->ind << "\n";
            bfs.push(curr->links[i]);
        }
    }
    dot_file << "}\n";
    dot_file.close();
}

void printLeftmostDerivation(Node *node, ofstream &derivationFile)
{
    if (node == nullptr)
        return;

    derivationFile << node->label << " -> ";
    for (int i = 0; i < node->links.size(); i++)
    {
        derivationFile << node->links[i]->label;
        if (i < node->links.size() - 1)
            derivationFile << " ";
    }
    derivationFile << endl;

    for (Node *child : node->links)
    {
        printLeftmostDerivation(child, derivationFile);
    }
}

void printLeftmostDerivationBFS(Node *root, ofstream &derivationFile)
{
    deque<Node *> bfsQueue;
    bfsQueue.push_back(root);
    string previousDerivation = root->label;
    derivationFile << previousDerivation << "\n";

    while (!bfsQueue.empty())
    {
        Node *currentNode = bfsQueue.front();
        bfsQueue.pop_front();

        string nextSymbol = "";
        string der = "";
        stringstream ss(previousDerivation);
        vector<Node *> a;
        while (getline(ss, nextSymbol, ' '))
        {
            if (nextSymbol == currentNode->label && currentNode->vis == 0)
            {
                currentNode->vis = 1;
                if (currentNode->links.size() == 0)
                {
                    derivationFile << " " << nextSymbol;
                    der += (" " + nextSymbol);
                }
                for (int i = 0; i < currentNode->links.size(); i++)
                {
                    derivationFile << " " << currentNode->links[i]->label;
                    der += (" " + currentNode->links[i]->label);
                    a.push_back(currentNode->links[i]);
                }
            }
            else
            {
                derivationFile << " " << nextSymbol;
                der += (" " + nextSymbol);
            }
        }
        for (int i = a.size() - 1; i >= 0; i--)
            bfsQueue.push_front(a[i]);
        removeWhiteSpace(der);
        previousDerivation = der;
        derivationFile << endl;
    }
}

void printParseTableToFile(const std::map<std::string, std::map<std::string, std::string>> &parsingTable, const std::set<std::string> &nonTerminals, const std::set<std::string> &terminals, const std::string &filename)
{
    std::ofstream outputFile(filename);
    if (!outputFile.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    size_t maxCellWidth = 20; // Set a fixed maximum cell width

    // Print column headings (terminals)
    outputFile << std::setw(maxCellWidth) << "|";
    for (const std::string &terminal : terminals)
    {
        if (terminal != "ε")
        { // Skip the column for "ε"
            outputFile << std::setw(maxCellWidth - 1) << terminal << "|";
        }
    }
    outputFile << std::endl;

    for (const std::string &terminal : terminals)
    {
        if (terminal != "ε")
        {
            outputFile << std::string(maxCellWidth - 1, '-') << "+";
        }
    }
    outputFile << '-' << std::string(maxCellWidth - 1, '-') << "+";
    outputFile << std::endl;

    // Print rows with non-terminal headings and parsing table entries
    for (const std::string &nonTerminal : nonTerminals)
    {
        if (parsingTable.count(nonTerminal))
        {
            outputFile << std::setw(maxCellWidth - 1) << nonTerminal << "|";
            for (const std::string &terminal : terminals)
            {
                if (terminal != "ε" && parsingTable.at(nonTerminal).count(terminal))
                {
                    std::string cellContent = parsingTable.at(nonTerminal).at(terminal);
                    if (cellContent == "ε")
                        outputFile << std::setw(maxCellWidth) << "ε"
                                   << "|";
                    else
                        outputFile << std::setw(maxCellWidth - 1) << cellContent << "|";
                }
                else if (terminal == "ε")
                {
                    continue;
                }
                else
                {
                    outputFile << std::setw(maxCellWidth) << "|";
                }
            }
            outputFile << std::endl;

            for (const std::string &terminal : terminals)
            {
                if (terminal != "ε")
                {
                    outputFile << std::string(maxCellWidth - 1, '-') << "+";
                }
            }
            outputFile << '-' << std::string(maxCellWidth - 1, '-') << "+";
            outputFile << std::endl;
        }
    }

    outputFile.close();
    std::cout << "Parse table has been written to " << filename << std::endl;
}

void printParseTableToCSV(const std::map<std::string, std::map<std::string, std::string>> &parsingTable, const std::set<std::string> &nonTerminals, const std::set<std::string> &terminals, const std::string &filename)
{
    std::ofstream outputFile(filename);
    if (!outputFile.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    outputFile << ",";
    for (const std::string &terminal : terminals)
    {
        if (terminal != "ε")
        {
            outputFile << terminal << ",";
        }
    }
    outputFile << std::endl;

    for (const std::string &nonTerminal : nonTerminals)
    {
        if (parsingTable.count(nonTerminal))
        {
            outputFile << nonTerminal << ",";
            for (const std::string &terminal : terminals)
            {
                if (terminal != "ε" && parsingTable.at(nonTerminal).count(terminal))
                {
                    std::string cellContent = parsingTable.at(nonTerminal).at(terminal);
                    if (cellContent == "ε")
                    {
                        cellContent = ""; // Remove "ε"
                    }
                    outputFile << cellContent << ",";
                }
                else
                {
                    outputFile << ",";
                }
            }
            outputFile << std::endl;
        }
    }

    outputFile.close();
    std::cout << "Parse table in CSV format has been written to " << filename << std::endl;
}

int main()
{
    system("g++ LeftRecursion.cpp -o 1 && ./1");
    system("g++ LeftFactoring.cpp -o 2 && ./2");

    ifstream inputFile("GrammarLL.txt");
    if (!inputFile.is_open())
    {
        cout << "Failed to open input file." << endl;
        return 1;
    }

    set<string> terminals, nonTerminals;
    map<string, vector<string>> productionRules;

    string line;

    getline(inputFile, startSymbol);
    getline(inputFile, line);
    stringstream nonTermStream(line);
    string nonTerm;
    while (getline(nonTermStream, nonTerm, ','))
    {
        removeWhiteSpace(nonTerm);
        nonTerminals.insert(nonTerm);
    }

    getline(inputFile, line);
    stringstream termStream(line);
    string term;
    while (getline(termStream, term, ','))
    {
        removeWhiteSpace(term);
        terminals.insert(term);
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
            productionRules[nonTerminal] = getRules(rule);
        }
    }
    inputFile.close();

    // Map to store computed FIRST sets
    map<string, set<string>> firstSets;

    // Compute FIRST sets for all symbols
    for (const string &symbol : terminals)
    {
        computeFirstSet(symbol, productionRules, firstSets, terminals);
    }

    // Map to store computed FOLLOW sets
    map<string, set<string>> followSets;

    // // Compute FOLLOW sets for all non-terminals
    for (const string &nonTerminal : nonTerminals)
    {
        computeFollowSet(nonTerminal, productionRules, firstSets, followSets, terminals); // Pass terminals here
    }
    int q = 1;
    while (q != 0)
    {
        q = 0;
        for (const string &nonTerminal : nonTerminals)
        {
            q += followRule3(nonTerminal, productionRules, firstSets, followSets, terminals); // Pass terminals here
        }
    }

    // Save the standard output buffer
    streambuf *coutbuf = cout.rdbuf();

    // Redirect output to "First-Follow.txt"
    ofstream output("First-Follow.txt");
    cout.rdbuf(output.rdbuf());

    // Print the computed FIRST sets
    for (const auto &entry : firstSets)
    {
        cout << "FIRST(" << entry.first << ") = { ";
        for (const string &symbol : entry.second)
        {
            cout << symbol << " ";
        }
        cout << "}" << endl;
    }

    // // Print the computed FOLLOW sets
    for (const auto &entry : followSets)
    {
        cout << "FOLLOW(" << entry.first << ") = { ";
        for (const string &symbol : entry.second)
        {
            cout << symbol << " ";
        }
        cout << "}" << endl;
    }

    // Restore the standard output buffer
    cout.rdbuf(coutbuf);

    terminals.insert("$");

    // Construct the parsing table using FIRST and FOLLOW sets
    for (const auto &entry : productionRules)
    {
        const string &nonTerminal = entry.first;
        for (const string &rule : entry.second)
        {
            stringstream ss(rule);
            string nextSymbol;
            set<string> firstOfRule;
            while (getline(ss, nextSymbol, ' '))
            {
                set<string> symbolFirst = computeFirstSet(nextSymbol, productionRules, firstSets, terminals);
                firstOfRule.insert(symbolFirst.begin(), symbolFirst.end());
                break;
            }
            for (const string &symbol : firstOfRule)
            {
                if (symbol != "ε")
                {
                    if (parsingTable.count(nonTerminal) && parsingTable[nonTerminal].count(symbol))
                    {
                        cout << "WRONG GRAMMAR";
                        return 0;
                    }
                    parsingTable[nonTerminal][symbol] = rule;
                }
                else
                {
                    set<string> followOfNonTerminal = followSets[nonTerminal];
                    for (const string &followSymbol : followOfNonTerminal)
                    {
                        if (parsingTable.count(nonTerminal) && parsingTable[nonTerminal].count(followSymbol))
                        {
                            cout << "WRONG GRAMMAR";
                            return 0;
                        }
                        parsingTable[nonTerminal][followSymbol] = rule;
                    }
                }
            }
        }
    }

    // Redirect output to "Parsing-Table.txt"
    ofstream parsingTableOutput("Parsing-Table.txt");
    cout.rdbuf(parsingTableOutput.rdbuf());

    // Print the parsing table
    for (const auto &entry : parsingTable)
    {
        const string &nonTerminal = entry.first;
        for (const auto &symbolEntry : entry.second)
        {
            const string &symbol = symbolEntry.first;
            const string &rules = symbolEntry.second;
            cout << "M[" << nonTerminal << ", " << symbol << "] = ";
            cout << rules << " | ";
            cout << endl;
        }
    }

    // Restore the standard output buffer
    cout.rdbuf(coutbuf);

    vector<string> input;
    system("lex -o tokenizer/lex.yy.c tokenizer/lexa.l && gcc tokenizer/lex.yy.c -o lexa && ./lexa < example/ex.txt > example/keywords.txt");
    // system("lex -o tokenizer/lex.yy.c tokenizer/lexa.l && gcc tokenizer/lex.yy.c -o lexa && ./lexa < example/exFail.txt > example/keywords.txt");
    // system("lex -o tokenizer/lex.yy.c tokenizer/lexa.l && gcc tokenizer/lex.yy.c -o lexa && ./lexa < example/ex2.txt > example/keywords.txt");
    // system("lex -o tokenizer/lex.yy.c tokenizer/lexa.l && gcc tokenizer/lex.yy.c -o lexa && ./lexa < example/ex2Fail.txt > example/keywords.txt");
    // system("lex -o tokenizer/lex.yy.c tokenizer/lexa1.l && gcc tokenizer/lex.yy.c -o lexa && ./lexa < example/ex3.txt > example/keywords.txt");

    string filename = "example/keywords.txt";
    std::ifstream inputFile1(filename);

    if (!inputFile1.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    // Read keywords line by line and store in the vector
    std::string keyword;
    while (std::getline(inputFile1, keyword))
    {
        input.push_back(keyword);
    }
    input.push_back("$");

    // Close the file
    inputFile1.close();

    stack<string> parseStack;
    stack<Node *> treeStack; // Stack to build the parse tree
    parseStack.push("$");
    parseStack.push(startSymbol); // Starting symbol

    size_t ip = 0;
    string X = parseStack.top();

    Node *root = new Node(startSymbol); // Create the root node of the parse tree
    Node *dol = new Node("$");
    treeStack.push(dol);
    treeStack.push(root);

    filename = "parse_table.txt";
    printParseTableToFile(parsingTable, nonTerminals, terminals, filename);

    filename = "parse_table.csv";
    printParseTableToCSV(parsingTable, nonTerminals, terminals, filename);

    while (!parseStack.empty())
    {
        X = parseStack.top();
        Node *temp = treeStack.top();
        if (X == "$" && ip == input.size() - 1)
        {
            cout << "Input string accepted!" << endl;
            drawTree(root); // Generate the parse tree graph
            system("dot -Tpng graph.dot -o graph.png");
            cout << "Parse tree generated and saved as graph.png" << endl;
            system("feh -F graph.png");
            break;
        }
        if (X == "ε" || X == " " || X == "\t")
        {
            parseStack.pop();
            treeStack.pop();
            continue;
        }

        if (X == input[ip])
        {
            parseStack.pop();
            treeStack.pop();
            ip++;
        }
        else if (parsingTable.find(X) != parsingTable.end() && parsingTable[X].find(input[ip]) != parsingTable[X].end())
        {
            string rule = parsingTable[X][input[ip]];
            parseStack.pop();
            treeStack.pop();
            stringstream ss(rule);
            string nextSymbol;
            vector<string> tok;
            vector<Node *> tok1;
            while (getline(ss, nextSymbol, ' '))
            {
                removeWhiteSpace(nextSymbol);
                tok.push_back(nextSymbol);
                Node *a = new Node(nextSymbol);
                tok1.push_back(a);
                temp->links.push_back(a);
            }
            for (int i = tok.size() - 1; i >= 0; i--)
            {
                parseStack.push(tok[i]);
            }
            for (int i = tok1.size() - 1; i >= 0; i--)
            {
                treeStack.push(tok1[i]);
            }
        }
        else
        {
            cout << "Error: Parsing failed at stack value: "
                 << "\033[31m" << X << "\033[0m"
                 << " and input value: "
                 << "\033[31m" << input[ip] << "\033[0m" << endl;
            return 0;
        }
    }
    ofstream derivationFile("Leftmost-Derivation.txt");
    printLeftmostDerivation(root, derivationFile);
    derivationFile.close();

    ofstream derivationFile1("Leftmost-Derivation1.txt");
    printLeftmostDerivationBFS(root, derivationFile1);
    derivationFile1.close();

    cout << "Leftmost derivation saved as Leftmost-Derivation.txt" << endl;

    return 0;
}

// S, A, B, C
// a, d, g, h, ε, b
// S -> A C B | C b B | B a
// A -> d a | B C
// B -> g | ε
// C -> h | ε

// Goal, Expr, Expr', Term, Term', Factor
// number, id, - ,+ ,\, *
// Goal -> Expr
// Expr -> Term Expr'
// Expr' -> +Term Expr' | – Term Expr' | ε
// Term -> Factor Term'
// Term' -> *Factor Term' | / Factor Term' | ε
// Factor -> number | id

// E, T, E', T',F
// id, (,), ε, *, +
// E -> T E'
// E' -> + T E' | ε
// T -> F T'
// T' -> * F T' | ε
// F -> id | ( E )