#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*
    DFA being simulated
    --------------------
    States   : q0 (start), q1, q2 (final)
    Alphabet : {0, 1}
    Language : all binary strings that contain "01" as a substring

    Transitions:
        q0 --0--> q1
        q0 --1--> q0
        q1 --0--> q1
        q1 --1--> q2
        q2 --0--> q2
        q2 --1--> q2
*/

// One row of the transition table. Kept as a simple struct + linear
// scan (instead of nested if/else or a map) so new states/symbols can
// be added just by adding a row.
struct Transition
{
    string from;
    char symbol;
    string to;
};

const vector<Transition> TRANSITIONS = {
    {"q0", '0', "q1"},
    {"q0", '1', "q0"},
    {"q1", '0', "q1"},
    {"q1", '1', "q2"},
    {"q2", '0', "q2"},
    {"q2", '1', "q2"},
};

const string START_STATE = "q0";
const string FINAL_STATE = "q2";

// Waits for the user to press ENTER before continuing.
void pauseForEnter(const string &message = "\nPress ENTER to continue...")
{
    cout << message;
    cin.get();
}

string getNextState(const string &state, char input)
{
    for (const Transition &t : TRANSITIONS)
    {
        if (t.from == state && t.symbol == input)
            return t.to;
    }
    return state; // Should never happen for a well-formed DFA.
}

// Draws the DFA. If currentState is non-empty, the state currently
// being visited is marked directly on the diagram.
void drawDFA(const string &currentState = "")
{
    auto marker = [&](const string &state) {
        return state == currentState ? "  <-- HERE" : "";
    };

    cout << "\n";
    cout << "==============================================================\n";
    cout << "                     DFA VISUALIZATION\n";
    cout << "==============================================================\n\n";

    cout << "        (1)                  (0)                  (1)\n";
    cout << "         ^                    ^                    ^\n";
    cout << "         |                    |                    |\n";
    cout << "    +----------+   0     +----------+   1     +==========+\n";
    cout << "--->|    q0    |-------->|    q1    |-------->||   q2   ||" << marker(currentState) << "\n";
    cout << "    +----------+         +----------+         +==========+\n";
    cout << "      START                                      FINAL\n";
    cout << "\n";
    cout << "(A loop like \"(1)\" above a state means that symbol keeps the\n";
    cout << " DFA in that same state.)\n";

    cout << "\n==============================================================\n";
    cout << "START STATE : " << START_STATE << "\n";
    cout << "FINAL STATE : " << FINAL_STATE << "\n";
    cout << "\nTransitions:\n";
    for (const Transition &t : TRANSITIONS)
        cout << t.from << " --" << t.symbol << "--> " << t.to << "\n";
    cout << "==============================================================\n";
}

// Reads a binary string from the user, re-prompting on invalid input
// instead of exiting the program.
string readBinaryString()
{
    string input;

    while (true)
    {
        cout << "\nEnter a binary string: ";
        cin >> input;

        bool valid = !input.empty();
        for (char c : input)
        {
            if (c != '0' && c != '1')
            {
                valid = false;
                break;
            }
        }

        if (valid)
            return input;

        cout << "Invalid input! Only non-empty strings of 0 and 1 are allowed.\n";
    }
}

// Prints the states visited so far as "q0 -> q1 -> q2" with a caret
// pointing at the most recently entered state.
void printPath(const vector<string> &path)
{
    string line;
    size_t lastStart = 0;

    for (size_t i = 0; i < path.size(); i++)
    {
        if (i > 0)
            line += " -> ";
        lastStart = line.size();
        line += path[i];
    }

    cout << "\nPath so far : " << line << "\n";
    cout << string(14 + lastStart, ' ') << string(path.back().size(), '^') << "\n";
}

int main()
{
    drawDFA();

    string input = readBinaryString();

    cout << "\nStarting DFA execution...\n";
    pauseForEnter();

    string state = START_STATE;
    vector<string> path = {state};

    for (size_t i = 0; i < input.length(); i++)
    {
        string currentState = state;
        string nextState = getNextState(state, input[i]);

        drawDFA(currentState);

        cout << "\n==============================================================\n";
        cout << "                    DFA EXECUTION\n";
        cout << "==============================================================\n";

        cout << "\nInput string  : " << input;
        cout << "\nReading       : '" << input[i] << "' (position " << i << ")";
        cout << "\nTransition    : " << currentState << " --" << input[i] << "--> " << nextState << "\n";

        printPath(path);

        state = nextState;
        path.push_back(state);

        pauseForEnter("\nPress ENTER for next transition...");
    }

    drawDFA(state);

    cout << "\n==============================================================\n";
    cout << "                       RESULT\n";
    cout << "==============================================================\n";

    cout << "\nInput string : " << input;
    cout << "\nFinal state  : " << state;
    printPath(path);

    if (state == FINAL_STATE)
    {
        cout << "\nRESULT : ACCEPTED";
        cout << "\nThe DFA reached the final state " << FINAL_STATE
             << " (the string contains \"01\").\n";
    }
    else
    {
        cout << "\nRESULT : REJECTED";
        cout << "\nThe DFA did not reach the final state " << FINAL_STATE
             << " (the string never contains \"01\").\n";
    }

    cout << "\n==============================================================\n";

    return 0;
}
