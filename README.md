
# Syntax Analysis/Parsing using LL(1) Predictive Parser

This README provides instructions and information about Lab Assignment 5, which involves creating a predictive parser for LL(1) grammars.

## Introduction
In this assignment, you will create a predictive parser for LL(1) grammars. The assignment involves three main steps:

1. Transforming the given grammar to LL(1) form.
2. Computing the FIRST() and FOLLOW() sets for the non-terminals.
3. Implementing a table-driven predictive parser using the LL(1) grammar and the computed sets.

## Program 1: Remove Left-Recursion
### Description
This is a C++ program that implements an algorithm to remove left recursion from a context-free grammar. Left recursion can cause parsing conflicts and ambiguity in parsing techniques. The program reads input from a file named "input.txt," where each line represents a production rule in the form NonTerminal -> Production, and applies the left recursion removal algorithm to transform the grammar.

## How the Algorithm Works

The algorithm consists of the following steps:

1. **Read Input File**: The program reads the input grammar rules from the "input.txt" file, where each line represents a production rule for a non-terminal symbol.

2. **Preprocessing Functions**:

   - `removeWhiteSpace()`: Removes leading and trailing whitespaces from a string.
   - `getRules()`: Splits a rule into multiple sub-rules separated by the '|' symbol.

3. **Immediate Left Recursion Removal**:

   - If a production rule starts with the non-terminal symbol, it is considered left-recursive. The program separates such rules into two parts: rules that start with the non-terminal (alphas) and rules that do not (betas).
   - New non-terminal symbols are introduced for the immediate left-recursive rules. The original non-terminal is replaced with a non-terminal with a prime symbol (e.g., A' for A).
   - The new production rules are generated for the non-left-recursive rules by appending the new non-terminal with prime.
   - If there are no non-left-recursive rules (only immediate left-recursive rules), a new rule is added to generate ε (epsilon).

4. **Non-Immediate Left Recursion Removal**:

   - For each pair of non-terminals (A and B), the algorithm identifies whether A can derive B.
   - If a rule in A starts with B, the algorithm replaces that rule by generating rules with all possible combinations of B's rules and A's remaining part.

5. **Apply Algorithm**:

   - The algorithm is applied to all non-terminals. First, non-immediate left recursion is handled for each pair of non-terminals, and then immediate left recursion is resolved for each non-terminal.

6. **Print Results**:

   - The modified grammar rules are printed to the console.

## Usage

1. Create an "input.txt" file with the grammar rules following the format `NonTerminal -> Production | Production | ...`.
2. Compile and run the C++ program.
3. The modified grammar rules will be displayed on the console.

### Example
Input : 
```javascript
E->E+T|T
T->T*F|F
F->(E)|id
```

Output : 
```javascript
E -> T E'
T -> F T'
F -> (E) | id
E' -> +T E' | ε
T' -> *F T' | ε
```

# Program 2: Left Factoring Algorithm

This Markdown README provides an explanation for the C++ code that implements the left factoring algorithm. The code reads a set of context-free grammar rules from an "input.txt" file and applies the left factoring technique to remove ambiguity in parsing.

## Algorithm Overview

The left factoring algorithm is used to remove ambiguity that arises when multiple production rules for a non-terminal have the same prefix. The algorithm works as follows:

1. **Read Input File**: The program reads the grammar rules from the "input.txt" file. Each line represents a production rule for a non-terminal symbol.

2. **Preprocessing Functions**:
   - `removeWhiteSpace()`: Removes leading and trailing whitespaces from a string.
   - `getRules()`: Splits a rule into multiple sub-rules separated by the '|' symbol.

3. **Get Common Prefixes**:
   - `getCommonPrefixes()`: Finds the common prefix among multiple rules for a non-terminal.

4. **Apply Left Factoring**:
   - The algorithm iteratively searches for common prefixes among the rules for each non-terminal.
   - If a common prefix is found, the algorithm replaces the common prefix with a new non-terminal and generates new rules with the remaining suffixes.
   - The new non-terminal is named by adding a prime symbol (e.g., A' for A).

5. **Print Results**:
   - The modified grammar rules are printed to the console.

## Usage

1. Create an "input.txt" file with the grammar rules in the format `NonTerminal -> Production | Production | ...`.
2. Compile and run the C++ program.
3. The modified grammar rules will be displayed on the console.

Please ensure that you have a valid input file and that your grammar conforms to the expected format. The left factoring algorithm helps in eliminating ambiguity in parsing by transforming the grammar to a form that is easier to work with.

### Example
Input : 
```javascript
A->aAB|aBc|aAc
```

Output : 
```javascript
A -> aA'
A' -> AA'' | Bc
A'' -> B | c
```

# LL(1) Parsing Algorithm using First and Follow Sets

## Introduction

This README provides an overview of the LL(1) parsing algorithm using the First and Follow sets. The algorithm aims to create a parsing table that guides the parsing process based on the First and Follow sets of non-terminals. Additionally, the rules to calculate the First and Follow sets are outlined.

## Algorithm

1. **Input**: Given a grammar G in augmented form, sets of terminals (T) and non-terminals (N), production rules (P), start symbol (S), and computed FIRST and FOLLOW sets.

2. **Compute FIRST and FOLLOW sets**: Calculate the FIRST and FOLLOW sets for each non-terminal symbol using the provided rules.

3. **Construct Parsing Table**:
   - For each production A → α, where A is a non-terminal and α is a string of terminals and non-terminals:
     - If ε is in FIRST(α), add all terminals in FOLLOW(A) to the parsing table entries for A.
     - For each terminal a in FIRST(α), add the production rule A → α to the parsing table entry M[A, a].
   - For each production A → ε, add all terminals in FOLLOW(A) to the parsing table entries for A.

4. **Parsing**:
   - Initialize a stack to store symbols and a pointer to the input string.
   - Push the start symbol onto the stack.
   - While the stack is not empty:
     - If the top of the stack is a terminal:
       - If it matches the current input symbol, pop the stack and move to the next input symbol.
       - Otherwise, report an error.
     - If the top of the stack is a non-terminal A:
       - If M[A, a] is a production A → α, pop A from the stack and push the symbols of α onto the stack in reverse order.
       - Otherwise, report an error.

5. **Output**:
   - If parsing is successful and the stack is empty, the input string is accepted.
   - If parsing encounters an error, report a syntax error.

## Rules to Find FIRST and FOLLOW Sets

### FIRST Set Rules

1. If X is a terminal, FIRST(X) = {X}.
2. If X is a non-terminal and X → ε is a production, add ε to FIRST(X).
3. If X is a non-terminal and X → Y1Y2...Yk is a production, for each Yi:
   - If ε is in FIRST(Yi), add all symbols in FIRST(Yi) except ε to FIRST(X).
   - If ε is not in FIRST(Yi), add all symbols in FIRST(Yi) to FIRST(X).

### FOLLOW Set Rules

1. Initialize FOLLOW(S) = {$}, where S is the start symbol.
2. For each production A → αBβ, where B is a non-terminal:
   - Add FIRST(β) to FOLLOW(B) if ε is not in FIRST(β).
   - Add FOLLOW(A) to FOLLOW(B) if ε is in FIRST(β) or FIRST(β) is ε.
3. For each production A → αB or A → αBβ where ε is in FIRST(β):
   - Add FOLLOW(A) to FOLLOW(B).

 ## Code Overview

The provided C++ code implements an LL(1) parser using FIRST and FOLLOW sets. Here's a summary of the main components of the code:

1. **Parsing Table Construction**: The code constructs a parsing table using FIRST and FOLLOW sets. It uses the provided grammar rules and generates the FIRST and FOLLOW sets for each non-terminal symbol.

2. **Parsing Process**: The LL(1) parsing process involves reading input from a file (`example/ex.txt`), validating the input string, and constructing a parse tree.

3. **Error Handling**: The code provides error handling to detect syntax errors during the parsing process.

4. **Graph Visualization**: The code generates a graph visualization of the constructed parse tree using the Graphviz tool. The graph is saved as `graph.png`.

5. **Output Generation**: The code generates several output files:
   - `First-Follow.txt`: Contains the computed FIRST and FOLLOW sets for each non-terminal symbol.
   - `Leftmost-Derivation.txt`: Contains the leftmost derivation of the input string.
   - `parse_table.txt`: Contains the parsing table entries in a formatted text file.
   - `parse_table.csv`: Contains the parsing table entries in CSV format.

## Execution Steps

1. The code compiles two additional C++ files (`LeftRecursion.cpp` and `LeftFactoring.cpp`) using `g++` to handle left recursion and left factoring.
2. It reads the grammar rules from the `GrammarLL.txt` file and extracts terminals, non-terminals, and production rules.
3. The FIRST and FOLLOW sets are computed for each non-terminal.
4. The code constructs the parsing table using the computed FIRST and FOLLOW sets.
5. Input keywords are read from `example/keywords.txt`, and the LL(1) parsing process is performed.
6. The constructed parse tree is visualized as a graph and saved as `graph.png`.

## Output Files

- `First-Follow.txt`: Contains the computed FIRST and FOLLOW sets for each non-terminal.
- `Leftmost-Derivation.txt`: Contains the leftmost derivation of the input string.
- `parse_table.txt`: Contains the LL(1) parsing table entries.
- `parse_table.csv`: Contains the LL(1) parsing table entries in CSV format.

## Prerequisites

- The code uses the `system()` function to execute shell commands, so a Unix-like shell environment is required.
- The Graphviz tool (`dot`) is required for generating the parse tree visualization.

## Usage

1. Compile the provided C++ code: `g++ LLParser.cpp -o parser`.
2. Run the compiled code: `./parser`.
3. The LL(1) parsing process will be performed based on the provided grammar and input keywords.
4. The parsing results, parse tree visualization, and output files will be generated.


## Screenshots
example : 
Grammar.LL

```javascript
P
AE, AE', BE, BE', D, DL, E, E', ES, F, IOS, IS, IS', NE, P, PE, RE, RE', S, SL, T, T', TY, VL, VL', WS
(, ), +, -, /, :=, ;, <, =, >, and, do, else, end, fc, float, ic, id, if, int, not, or, print, prog, scan, str, then, while, {, }, ε, ∗
P -> prog DL SL end
DL -> D DL | ε
D -> TY VL ;
TY -> int | float
VL -> id VL'
SL -> S SL | ε
S -> ES | IS | WS | IOS
ES -> id := E ;
IS -> if BE then SL IS'
WS -> while BE do SL end
IOS -> print PE | scan id
PE -> E | str
BE -> AE BE'
AE -> NE AE'
NE -> not NE | { BE } | RE
RE -> E RE'
E -> T E'
T -> F T'
F -> ( E ) | id | ic | fc
BE' -> or AE BE' | ε
AE' -> and NE AE' | ε
E' -> + T E' | - T E' | ε
T' -> ∗ F T' | / F T' | ε
VL' ->  VL | ε
IS' ->  end |  else SL end
RE' ->  = E |  < E |  > E

```

sample input

```javascript
prog 
    int a ; 
    float tm;
    int sum;
    int count;
    sum:=5;
    scan count
    scan tm
    print sum
    if count=3 and tm=5.8
    then sum:=18;
    else 
    tm := 4.5;
    end
    while sum>3 or count<1000 do
        sum:=sum-1;
        count:=a+count;
    end
end
```

Parse Tree

![App Screenshot](https://drive.google.com/uc?export=view&id=1Tbd9cMIuyp74jK2J0JhN0w1Tewkvu00S)