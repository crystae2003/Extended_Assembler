#include <fstream>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

#define NUM_BITS_INSTRUCTION 32
#define IMMEDIATE_FIELD_SIZE 13

#define ll long long
using namespace std;

map<long long int, string> tAKEN_Ht;       // History table for branches taken
map<long long int, string> N_tAKEN_Ht;     // History table for branches not taken
map<long long int, long long int> address; // Address map for branches
vector<bool> valid;

map<long long int, string> Ht;
// function to convert hexadecimal number into binary
string H2B(string number)
{
    static const unordered_map<char, string> H2B = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"}, {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"}, {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"}, {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"}, {'a', "1010"}, {'b', "1011"}, {'c', "1100"}, {'d', "1101"}, {'e', "1110"}, {'f', "1111"}, {'.', "."}};

    size_t startIndex = (number[1] == 'x' || number[1] == 'X') ? 2 : 0;
    string binary = "";

    for (size_t i = startIndex; i < number.size(); i++)
    {
        char hexDigit = number[i];
        auto it = H2B.find(hexDigit);
        if (it != H2B.end())
        {
            binary += it->second;
        }
        else
        {
            cout << "\nInvalid hexadecimal digit " << hexDigit << endl;
        }
    }

    return binary;
}

long long int ONE_BIt = 0, tWO_BIt = 0, SOME_t = 0, SOME_Nt = 0;
map<long long int, pair<bool, bool>> bit2state;

bool prev_taken = false;
bool curr = false;
bool pre = false;

string two = "";

map<long long int, string> one_bit_Ht;

// to check one_bit branch prediction

void one_bit(long long int pc, bool taken)
{
    if (one_bit_Ht[pc].size() == 0)
    {
        prev_taken = false;
    }
    else if (Ht[pc][Ht[pc].size() - 1] == 't')
    {
        prev_taken = true;
    }
    else
    {
        prev_taken = false;
    }
    if (taken == prev_taken)
    {
        ONE_BIt++;
    }
    if (prev_taken)
    {
        one_bit_Ht[pc] += "t";
    }
    else
    {
        one_bit_Ht[pc] += "N";
    }
    prev_taken = taken;
}

// function when branch is always taken
void ALWAYS_tAKEN(long long int pc, bool taken)
{
    if (taken == true)
    {
        SOME_t++;
    }
    tAKEN_Ht[pc] += "t";
}

// function when branch is not always taken completely
void ALWAYS_NOt_tAKEN(long long int pc, bool taken)
{
    if (taken == false)
    {
        int count = 0;
        count++;
        SOME_Nt++;
    }
    N_tAKEN_Ht[pc] += "N";
}
map<long long int, string> two_bit_Ht;

// function for onebit
void two_bit(long long int pc, bool taken)
{
    bool guess;
    if (two_bit_Ht[pc].size() == 0)
    {
        bit2state[pc] = {false, false};
    }

    if (bit2state[pc].first == bit2state[pc].second)
    {
        guess = bit2state[pc].first;
        bit2state[pc].first = taken;
    }
    else
    {
        guess = bit2state[pc].second;
        if (taken == bit2state[pc].first)
        {
            bit2state[pc].second = taken;
        }
        else
        {
            bit2state[pc].first = taken;
        }
    }
    if (guess)
    {
        two_bit_Ht[pc] += "t";
        two += "t";
    }
    else
    {
        two += "N";
        two_bit_Ht[pc] += "N";
    }
    if (guess == taken)
        tWO_BIt++;
}

// calling main

int main()
{

    bool checking = false;
    vector<long long int> pc;
    string add_line;
    vector<long long int> target_address;
    long long int j = 0;
    long long int branch = 0;
    string ans = "";

    // inputting a file
    string input_file = "input.txt";
    ifstream ifile(input_file);

    // outputting if branch is taken or not taken on a file
    ofstream outfile("output.txt");

    if (!ifile.is_open())
    {
        cout << "file is not opening" << endl;
        return 1;
    }

    if (!outfile.is_open())
    {
        cout << "Error opening output file." << endl;
        return 1;
    }
    // reading from the file
    while (getline(ifile, add_line))
    {

        if (add_line.size() == 0)
            continue;

        vector<string> vec;
        string check_str = "";
        int check = 0;
        int k = 0;
        long long int n = 0;

        for (int i = 0; i < add_line.size(); i++)
        {
            if (add_line[i] == ' ')
            {
                if (check_str != "")
                {
                    vec.push_back(check_str);
                    check_str = "";
                }
                continue;
            }
            check_str += add_line[i];
            if (i == add_line.size() - 1)
                vec.push_back(check_str);
        }

        while (k < vec.size())
        {
            if (vec[k] == "core")
            {
                break;
            }
            k++;
        }
        if (k == vec.size())
            continue;
        if (vec.size() < 4 + k || vec[3 + k].substr(0, 3) != "(0x" || vec[2 + k].substr(0, 2) != "0x")
            continue;

        string pc_temp = H2B(vec[2 + k]);

        for (int i = 0; i < 32; i++)
        {
            n += (pc_temp[i] - '0') * powl(2, 31 - i);
        }
        pc.push_back(n);
        if (checking)
        {
            target_address.push_back(pc[target_address.size() + 1] - pc[target_address.size()]);
            checking = false;
        }
        string instruction = H2B(vec[3 + k].substr(1, 10));
        string op = instruction.substr(32 - 7, 7);
        // matching opcode with branch instruction
        string immediate(13, '0');
        if (op == "1100011")
        {

            immediate[0] = instruction[0];
            immediate[1] = instruction[24];
            for (int i = 6, j = 7; i >= 1, j >= 2; i--, j--)
            {
                immediate[j] = instruction[i];
            }
            for (int i = 23, j = 11; i >= 20, j >= 8; i--, j--)
            {
                immediate[j] = instruction[i];
            }
            long long int n = stoi(immediate, nullptr, 2);
            if (immediate[0] == '1')
            {
                n = n - pow(2, 13);
            }
            target_address.push_back(n);
        }
        // matching opcode if it is jal instruction// matching opcode if it is jalr instruction respectively

        else if (op == "1101111" || op == "1100111")
            checking = true;
        else
            target_address.push_back(0);
    }

    for (int i = 0; i < pc.size() - 1; i++)
    {
        if (pc[i + 1] - pc[i] == target_address[i])
        {
            valid.push_back(1);
        }
        else
        {
            valid.push_back(0);
        }
    }

    for (long long int i = 0; i < target_address.size(); i++)
    {
        if (target_address[i])
        {
            address[pc[i]] = target_address[i];

            ALWAYS_tAKEN(pc[i], (bool)valid[i]);
            ALWAYS_NOt_tAKEN(pc[i], (bool)valid[i]);
            one_bit(pc[i], (bool)valid[i]);
            two_bit(pc[i], (bool)valid[i]);

            if (valid[i])
            {

                ans += 't';
            }
            else
            {
                Ht[pc[i]].push_back('N');
                ans += 'N';
            }
            branch++;
        }
    }
    // making table to calculate accuracyfor one_bit and two bit branch prediction,if it has taken or not
    for (auto const &pair : Ht)
    {
        outfile << "instructionruction at PC " << pair.first << ": " << pair.second << endl;
    }

    cout << setw(25) << left << "Branch guessor type" << setw(20) << "Accuracy (%)";
    cout << endl;

    cout << setw(25) << left << "Single Bit guessor" << setw(20) << fixed << setprecision(2) << ((float)ONE_BIt / branch) * 100;
    cout << endl;

    cout << setw(25) << left << "two Bit guessor" << setw(20) << fixed << setprecision(2) << ((float)tWO_BIt / branch) * 100;
    cout << endl;

    cout << setw(25) << left << "Always Not taken" << setw(20) << fixed << setprecision(2) << ((float)SOME_Nt / branch) * 100;
    cout << endl;

    cout << setw(25) << left << "Always taken" << setw(20) << fixed << setprecision(2) << ((float)SOME_t / branch) * 100;
    cout << endl;

    ifile.close();
    outfile.close();

    return 0;
}
// end code
