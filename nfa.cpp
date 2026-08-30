#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>

using namespace std;

/*
    NFA being simulated
    --------------------
    States   : q0 (start), q1, q2 (final)
    Alphabet : {0, 1}

    Transitions:
        q0 --0--> q0
        q0 --0--> q1
        q0 --1--> q2
        q1 --1--> q2
        q2 --0--> q2
        q2 --1--> q2

    Unlike a DFA, q0 has TWO destinations on symbol '0' ({q0, q1}), so the
    machine tracks a *set* of currently-active states rather than a single
    state. A string is accepted if q2 is in the active set after the last
    symbol is read.
*/

const string START_STATE = "q0";
const string FINAL_STATE = "q2";

// transitions[state][symbol] = set of possible destination states.
const map<string, map<char, set<string>>> TRANSITIONS = {
    {"q0", {{'0', {"q0", "q1"}}, {'1', {"q2"}}}},
    {"q1", {{'1', {"q2"}}}},
    {"q2", {{'0', {"q2"}}, {'1', {"q2"}}}},
};


void pauseForEnter(const string &message = "\nPress ENTER to continue...")
{
    cout << message;
    cin.get();
}

string setToString(const set<string> &states)
{
    if (states.empty())
        return "{}";

    string result = "{";
    for (auto it = states.begin(); it != states.end(); ++it)
    {
        if (it != states.begin())
            result += ",";
        result += *it;
    }
    result += "}";
    return result;
}

// Union of every destination reachable from any state in `active` on `symbol`.
set<string> step(const set<string> &active, char symbol)
{
    set<string> next;

    for (const string &state : active)
    {
        auto stateIt = TRANSITIONS.find(state);
        if (stateIt == TRANSITIONS.end())
            continue;

        auto symIt = stateIt->second.find(symbol);
        if (symIt == stateIt->second.end())
            continue;

        next.insert(symIt->second.begin(), symIt->second.end());
    }

    return next;
}

void drawNFA(const set<string> &activeStates = {})
{
    auto marker = [&](const string &state) {
        return activeStates.count(state) ? "  <-- ACTIVE" : "";
    };

    cout << "\n";
    cout << "==============================================================\n";
    cout << "                     NFA VISUALIZATION\n";
    cout << "==============================================================\n\n";

    cout << "        (0)\n";
    cout << "         ^\n";
    cout << "         |\n";
    cout << "    +----------+                            +==========+\n";
    cout << "--->|    q0    |------------0-------------->||   q1   ||" << marker("q1") << "\n";
    cout << "    +----------+ \\                          +==========+\n";
    cout << "     START        \\ 1                             \\\n";
    cout << "                    \\                               1\n";
    cout << "                     \\        +==========+          \\\n";
    cout << "                      \\------>||   q2   ||<----------+\n";
    cout << "                              +==========+" << marker("q2") << "\n";
    cout << "                               ^  FINAL\n";
    cout << "                               |\n";
    cout << "                             (0,1)\n";

    cout << "\n==============================================================\n";
    cout << "START STATE : " << START_STATE << "\n";
    cout << "FINAL STATE : " << FINAL_STATE << "\n";
    cout << "TYPE        : NON-DETERMINISTIC FINITE AUTOMATON\n";
    cout << "==============================================================\n";

    cout << "\nTransitions:\n";
    for (const auto &fromEntry : TRANSITIONS)
        for (const auto &symEntry : fromEntry.second)
            for (const string &to : symEntry.second)
                cout << fromEntry.first << " --" << symEntry.first << "--> " << to << "\n";

    cout << "\n==============================================================\n";
}

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

void printPath(const vector<set<string>> &path)
{
    cout << "\nActive-state history:\n";
    for (size_t i = 0; i < path.size(); i++)
    {
        cout << "  step " << i << " : " << setToString(path[i]) << "\n";
    }
}

int main()
{
    drawNFA();

    string input = readBinaryString();

    cout << "\nStarting NFA execution...\n";
    cout << "At every step the NFA can be in SEVERAL states at once.\n";
    pauseForEnter();

    set<string> active = {START_STATE};
    vector<set<string>> path = {active};

    for (size_t i = 0; i < input.length(); i++)
    {
        set<string> currentActive = active;
        set<string> nextActive = step(currentActive, input[i]);

        drawNFA(currentActive);

        cout << "\n==============================================================\n";
        cout << "                    NFA EXECUTION\n";
        cout << "==============================================================\n";

        cout << "\nInput string   : " << input;
        cout << "\nReading        : '" << input[i] << "' (position " << i << ")";
        cout << "\nActive states  : " << setToString(currentActive);
        cout << "\nTransition     : " << setToString(currentActive) << " --" << input[i]
             << "--> " << setToString(nextActive) << "\n";

        active = nextActive;
        path.push_back(active);
        printPath(path);

        if (active.empty())
        {
            cout << "\nNo transitions are possible from here - every branch has died.\n";
        }

        pauseForEnter("\nPress ENTER for next transition...");

        if (active.empty())
            break;
    }

    drawNFA(active);

    cout << "\n==============================================================\n";
    cout << "                       RESULT\n";
    cout << "==============================================================\n";

    cout << "\nInput string        : " << input;
    cout << "\nFinal active states : " << setToString(active);
    printPath(path);

    bool accepted = active.count(FINAL_STATE) > 0;

    if (accepted)
    {
        cout << "\nRESULT : ACCEPTED";
        cout << "\n" << FINAL_STATE << " is among the final active states.\n";
    }
    else
    {
        cout << "\nRESULT : REJECTED";
        cout << "\n" << FINAL_STATE << " was never reached by any branch.\n";
    }

    cout << "\n==============================================================\n";

    return 0;
}
