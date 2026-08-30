#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <queue>

using namespace std;

/*
    NFA being converted (subset construction / powerset construction)
    -------------------------------------------------------------------
    States   : q0 (start), q1, q2 (final)
    Alphabet : {0, 1}

    Transitions:
        q0 --0--> q0
        q0 --0--> q1
        q0 --1--> q2
        q1 --1--> q2
        q2 --0--> q2
        q2 --1--> q2
*/

// nfa[state][symbol] = set of destination states.
map<int, map<char, set<int>>> buildNFA()
{
    map<int, map<char, set<int>>> nfa;

    nfa[0]['0'].insert(0);
    nfa[0]['0'].insert(1);
    nfa[0]['1'].insert(2);

    nfa[1]['1'].insert(2);

    nfa[2]['0'].insert(2);
    nfa[2]['1'].insert(2);

    return nfa;
}

const int NFA_FINAL_STATE = 2;

void pauseForEnter(const string &message = "\nPress ENTER to continue...")
{
    cout << message;
    cin.get();
}

// Renders a set of NFA state numbers as "{q0,q1}". Uses to_string
// instead of char('0' + n), so it stays correct for states >= 10.
string setToString(const set<int> &states)
{
    if (states.empty())
        return "{}";

    string result = "{";
    for (auto it = states.begin(); it != states.end(); ++it)
    {
        if (it != states.begin())
            result += ",";
        result += "q" + to_string(*it);
    }
    result += "}";
    return result;
}

// Every destination reachable from any NFA state in `subset` on `symbol`.
set<int> move(const map<int, map<char, set<int>>> &nfa, const set<int> &subset, char symbol)
{
    set<int> destination;

    for (int nfaState : subset)
    {
        auto stateIt = nfa.find(nfaState);
        if (stateIt == nfa.end())
            continue;

        auto symIt = stateIt->second.find(symbol);
        if (symIt == stateIt->second.end())
            continue;

        destination.insert(symIt->second.begin(), symIt->second.end());
    }

    return destination;
}

int main()
{
    map<int, map<char, set<int>>> nfa = buildNFA();

    set<int> start = {0};

    queue<set<int>> pending;
    vector<set<int>> dfaStates;
    map<set<int>, int> stateNumber;

    // dfaTransitions[i][symbol] = index of the DFA state reached from
    // dfaStates[i] on that symbol, or -1 if there is no transition.
    vector<map<char, int>> dfaTransitions;

    pending.push(start);
    stateNumber[start] = 0;
    dfaStates.push_back(start);
    dfaTransitions.push_back({});

    cout << "\n";
    cout << "==============================================================\n";
    cout << "              NFA TO DFA CONVERSION\n";
    cout << "                 SUBSET CONSTRUCTION\n";
    cout << "==============================================================\n\n";

    cout << "NFA STATES : q0, q1, q2\n";
    cout << "ALPHABET   : {0, 1}\n";
    cout << "START      : q0\n";
    cout << "FINAL      : q2\n\n";

    cout << "NFA TRANSITIONS\n";
    cout << "--------------------------------------------------------------\n";
    for (const auto &fromEntry : nfa)
    {
        for (char symbol : {'0', '1'})
        {
            auto symIt = fromEntry.second.find(symbol);
            set<int> dest = (symIt != fromEntry.second.end()) ? symIt->second : set<int>();
            cout << "q" << fromEntry.first << " --" << symbol << "--> " << setToString(dest) << "\n";
        }
    }
    cout << "--------------------------------------------------------------\n";

    pauseForEnter("\n\nPress ENTER to begin conversion...");

    while (!pending.empty())
    {
        set<int> current = pending.front();
        pending.pop();

        int currentNumber = stateNumber[current];

        cout << "\n";
        cout << "==============================================================\n";
        cout << "              NFA TO DFA CONVERSION\n";
        cout << "==============================================================\n\n";

        cout << "Currently processing DFA state:\n\n";
        cout << "D" << currentNumber << " = " << setToString(current);

        bool currentFinal = current.count(NFA_FINAL_STATE) > 0;
        if (currentFinal)
            cout << "   <-- FINAL STATE";

        cout << "\n\nFinding transitions...\n\n";

        for (char symbol : {'0', '1'})
        {
            set<int> destination = move(nfa, current, symbol);

            cout << "D" << currentNumber << " --" << symbol << "--> ";

            if (destination.empty())
            {
                cout << "{}\n";
                dfaTransitions[currentNumber][symbol] = -1;
                continue;
            }

            cout << setToString(destination);

            if (stateNumber.find(destination) == stateNumber.end())
            {
                int newNumber = static_cast<int>(dfaStates.size());
                stateNumber[destination] = newNumber;
                dfaStates.push_back(destination);
                dfaTransitions.push_back({});
                pending.push(destination);

                cout << "   [NEW DFA STATE D" << newNumber << "]";
            }

            dfaTransitions[currentNumber][symbol] = stateNumber[destination];
            cout << "\n";
        }

        pauseForEnter("\nPress ENTER for next DFA state...");
    }


    cout << "\n";
    cout << "==============================================================\n";
    cout << "                DFA CONVERSION COMPLETE\n";
    cout << "==============================================================\n\n";

    cout << "DFA STATES GENERATED:\n\n";

    for (size_t i = 0; i < dfaStates.size(); i++)
    {
        cout << "D" << i << " = " << setToString(dfaStates[i]);

        if (dfaStates[i].count(NFA_FINAL_STATE) > 0)
            cout << "   <-- FINAL";
        if (i == 0)
            cout << "   <-- START";

        cout << "\n";
    }

    cout << "\n\n";
    cout << "==============================================================\n";
    cout << "                  DFA TRANSITION TABLE\n";
    cout << "==============================================================\n\n";

    cout << "State       0              1\n";
    cout << "--------------------------------------------------------------\n";

    for (size_t i = 0; i < dfaStates.size(); i++)
    {
        cout << "D" << i << "          ";

        for (char symbol : {'0', '1'})
        {
            int dest = dfaTransitions[i][symbol];
            if (dest == -1)
                cout << "{}";
            else
                cout << "D" << dest;

            cout << (symbol == '0' ? "             " : "");
        }

        cout << "\n";
    }

    // The diagram below is generated from the DFA that was actually
    // computed above, so it stays correct even if the NFA definition
    // at the top of this file is changed (unlike a fixed ASCII picture,
    // which would silently become wrong).
    cout << "\n==============================================================\n";
    cout << "              DFA STATE DIAGRAM (generated)\n";
    cout << "==============================================================\n\n";

    for (size_t i = 0; i < dfaStates.size(); i++)
    {
        cout << "D" << i << " = " << setToString(dfaStates[i]);
        if (i == 0)
            cout << " [START]";
        if (dfaStates[i].count(NFA_FINAL_STATE) > 0)
            cout << " [FINAL]";
        cout << "\n";

        for (char symbol : {'0', '1'})
        {
            int dest = dfaTransitions[i][symbol];
            cout << "    --" << symbol << "--> ";
            cout << (dest == -1 ? "(no transition)" : ("D" + to_string(dest))) << "\n";
        }
        cout << "\n";
    }

    cout << "==============================================================\n";
    cout << "              NFA -> DFA SUCCESSFULLY COMPLETED\n";
    cout << "==============================================================\n";

    return 0;
}
