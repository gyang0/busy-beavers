#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

const int STATES = 2; // Number of internal states
const int MAX_TM_ITER = 10; // Max TM iteration steps
const int TAPE_LEN = MAX_TM_ITER*2 + 3; // Tape length for TM

/**
 * Transitions for internal state & value read from tape (0 or 1)
 * trans[state][value] = transition rule
 * e.g. trans['B'-'A'][1] = "1RB", for 'B' and 1.
 */
string trans[STATES][2];
vector<string> all_trans; // All possible transitions
vector<int> tape; // Tape (0s and 1s)

int max_BB = 0; // Maximum BB(n) found (# of steps)
int max_sigma = 0; // Maximum Σ(n) found (# of 1's written)

vector<vector<string>> BB_solns; // transition states for max BB(n)
vector<vector<string>> sigma_solns; // transition states for max Σ(n)

// Prints string vector
void display(vector<string> v){
    for(string a : v)
        cout << a << " ";
    cout << "\n";
}

// Sets tape to all zeros
void reset_tape(){
    for(int i = 0; i < tape.size(); i++){
        tape[i] = 0;
    }
}

// Counts number of 1's in tape
int num_ones(){
    int ans = 0;
    for(int a : tape)
        if(a == 1) ans++;
    return ans;
}

// Simulates a BB game, starting from the middle of the tape
void simulate(){
    int pos = TAPE_LEN/2;
    char state = 'A'; // internal state

    int steps = 0;

    while(true){
        string rule = trans[state - 'A'][tape[pos]];
        
        if(rule == "H"){
            steps++;

            // Record max BB
            if(steps > max_BB){
                BB_solns = {};
            }
            if(steps >= max_BB){
                max_BB = steps;

                vector<string> v;
                for(int i = 0; i < STATES; i++){
                    for(int j = 0; j < 2; j++)
                        v.push_back(trans[i][j]);
                }
                BB_solns.push_back(v);
            }

            // Record max Σ
            if(num_ones() > max_sigma){
                sigma_solns = {};
            }
            if(num_ones() >= max_sigma){
                max_sigma = num_ones();

                vector<string> v;
                for(int i = 0; i < STATES; i++){
                    for(int j = 0; j < 2; j++)
                        v.push_back(trans[i][j]);
                }
                sigma_solns.push_back(v);
            }

            break;
        }
        if(steps > MAX_TM_ITER){
            break;
        }

        tape[pos] = rule[0] - '0';
        if(rule[1] == 'R') pos++;
        else pos--;
        state = rule[2];
        
        steps++;
    }

    reset_tape();
}

// Add new transition rules
void add_rules(vector<string> temp){
    for(int i = 0; i < STATES; i++){
        for(int j = 0; j < 2; j++){
            trans[i][j] = temp[2*i + j];
        }
    }
}

// Generate all possible transition rules
void generateTrans(){
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            for(int k = 0; k < STATES; k++){
                string t = to_string(i) + (j == 0 ? 'L' : 'R');
                t.push_back('A' + k);

                all_trans.push_back(t);
            }
        }
    }
}

int main() {
    // Init tape
    for(int i = 0; i < TAPE_LEN; i++){
        tape.push_back(0);
    }

    generateTrans(); // generate all transitions

    map<vector<string>, bool> cache; // cache of transition orders

    string init = "";

    do {
        // If the first seq. starts w/ a rule that has an R, we ignore that
        // works since by switching all the R's for L's and vice versa, we get the same TM
        // cuts down search space by 2.
        if(all_trans[0][1] == 'R')
            continue;

        // Get first 2n - 1 states
        vector<string> temp;
        for(int i = 0; i < 2*STATES - 1; i++){
            temp.push_back(all_trans[i]);
        }

        // Seen before
        if(cache[temp]) continue;
        cache[temp] = true;

        // Insert a halt somewhere
        // But not at first pos since that corresponds to trans['A'][0] in array, our starting point.
        for(int hpos = 1; hpos <= temp.size(); hpos++){
            temp.insert(temp.begin() + hpos, "H");
            add_rules(temp);
            simulate();
            temp.erase(temp.begin() + hpos);
        }

        if(init != temp[0]){
            cout << temp[0] << "\n";
            init = temp[0];
        }

    } while (next_permutation(all_trans.begin(), all_trans.end()));

    cout << "\n\n----------------------------\n";
    cout << "Max BB(" << STATES << "): " << max_BB << "\n";
    for(vector<string> v : BB_solns){
        display(v);
    }

    cout << "----------------------------\n";

    cout << "Max sigma(" << STATES << "): " << max_sigma << "\n";
    for(vector<string> v : sigma_solns){
        display(v);
    }

    cout << "----------------------------\n";
    cout << "Programs that are both BB(" << STATES << ") and sigma(" << STATES << "):\n";
    for(vector<string> v : BB_solns){
        if(find(sigma_solns.begin(), sigma_solns.end(), v) != sigma_solns.end()){
            display(v);
        }
    }

    return 0;
}
