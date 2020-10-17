#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

using namespace std;
#define DEBUG 0

#define GAME_CODE "GZ2E01"

class Map {
   public:
    vector<string> address;
    vector<string> offset;
    vector<string> virtual_address;
    vector<string> size;
    vector<string> symbol;
    vector<string> archive_file;
    vector<string> object_file;
};

vector<string> splitStrings(string str, char delim) {
    string word = "";

    // to count the number of split strings
    int num = 0;

    // adding delimiter character at the end
    // of 'str'
    str = str + delim;

    // length of 'str'
    int l = str.size();

    // traversing 'str' from left to right
    vector<string> substr_list;
    for (int i = 0; i < l; i++) {
        // if str[i] is not equal to the delimiter
        // character then accumulate it to 'word'
        if (str[i] != delim)
            word = word + str[i];

        else {
            // if 'word' is not an empty string,
            // then add this 'word' to the array
            // 'substr_list[]'
            if ((int)word.size() != 0)
                substr_list.push_back(word);

            // reset 'word'
            word = "";
        }
    }

    // return the splitted strings
    return substr_list;
}

void strToUpper(string& string) {
    transform(string.begin(), string.end(), string.begin(), ::toupper);
}

int main() {
    string map_file = (string)GAME_CODE + ".map";
    string asm_file = (string)GAME_CODE + ".s";
    ifstream file(map_file);
    string str;
    Map map;
    char delimeter = ' ';

    // load the map into class
    while (getline(file, str)) {
        vector<string> result = splitStrings(str, delimeter);

        // if not a section header
        if (result[0].substr(0, 1) != ".") {
            // if not a fill symbol
            if (result[4] != "*fill*") {
                // if not a 0 length symbol
                if (result[1] != "000000" || result[4].substr(0, 5) == "_rest" || result[4].substr(0, 5) == "_save" || result[4] == "TRKAccessFile") {
                    strToUpper(result[0]);
                    strToUpper(result[1]);
                    strToUpper(result[2]);
                    map.offset.push_back(result[0]);
                    map.size.push_back(result[1]);
                    map.address.push_back(result[2]);
                    map.symbol.push_back(result[4]);
#if (DEBUG)
                    cout << "offset: " << result[0] << " ";
                    cout << "size: " << result[1] << " ";
                    cout << "address: " << result[2] << " ";
                    cout << "symbol: " << result[4] << " ";
#endif
                    if (result[5].substr(result[5].length() - 2) == ".a") {
                        map.archive_file.push_back(result[5].substr(1));
                        map.object_file.push_back(result[6]);
#if (DEBUG)
                        cout << "archive: " << result[5].substr(1) << " ";
                        cout << "object: " << result[6] << "\n";
#endif
                    } else if (result[5].substr(result[5].length() - 2) == ".o") {
                        map.object_file.push_back(result[5].substr(1));
                        map.archive_file.push_back("N/A");
#if (DEBUG)
                        cout << "object: " << result[5].substr(1) << "\n";
#endif
                    }
                }
            }
        }
    }

    ifstream file2(asm_file);
    string str2;
    vector<string> result;
    string current_func;
    string current_address;
    string line_check;
    string branch_func_check;
    string branch_func_h_check;
    string branch_func_l_check;
    string branch_lbl_check;
    string branch_lbl_h_check;
    string branch_lbl_l_check;
    ptrdiff_t index;
    std::map<string, string> symbol_map;
    int branch_func_start_position;
    int branch_func_h_start_position;
    int branch_func_l_start_position;
    int branch_lbl_start_position;
    int branch_lbl_h_start_position;
    int branch_lbl_l_start_position;
    bool inFunctionFlag = false;

    int branch_lbl_lr_position;
    string branch_lbl_lr_check;
    int branch_lbl_lr2_position;
    string branch_lbl_lr2_check;
    int branch_lbl_sda_base_position;
    string branch_lbl_sda_base_check;
    int branch_lbl_sda2_base_position;
    string branch_lbl_sda2_base_check;
    int branch_lbl_sda_base_operandr2_position;
    string branch_lbl_sda_base_operandr2_check;
    int branch_lbl_sda_base_operandr13_position;
    string branch_lbl_sda_base_operandr13_check;

    // loop through all lines to build a map of func names and symbols
    while (getline(file2, str2)) {
        if (str2.substr(0, 4) == "func") {
            current_address = str2.substr(5, 8);

            if (auto it = find(map.address.begin(), map.address.end(), current_address); it != map.address.end()) {
                index = distance(map.address.begin(), it);
                symbol_map.insert(std::pair<string, string>(str2.substr(0, 13), map.symbol[index]));
            }
        }
    }

    // run through the file again
    file2.clear();
    file2.seekg(0);

    int iter = 0;

    ofstream outfile;

    // write the lines
    while (getline(file2, str2)) {

        if (str2.substr(0, 4) == "func") {
            current_address = str2.substr(5, 8);
            string filename = (string)GAME_CODE + "/func_" + current_address + ".s";
            outfile.open(filename, ios_base::app);

            if (auto it = find(map.address.begin(), map.address.end(), current_address); it != map.address.end()) {
                index = distance(map.address.begin(), it);

                outfile << ".global " << map.symbol[index] << ":" << endl;
                outfile << map.symbol[index] << ":" << endl;

                inFunctionFlag = true;
                continue;
            } else {
                cout << "not found: " << str2.substr(0, 8) << endl;
                cout << "line: " << iter << endl;
            }
        }

        iter++;
        // if we're printing lines under func_
        if (inFunctionFlag) {
            line_check = str2.substr(0, 2);

            // if it's an instruction line
            if (line_check == "/*") {
                // label line
                if (line_check == "lb") {
                    cout << str2 << endl;
                }

                // instruction line
                else {
                    branch_func_start_position = str2.length() - 13;
                    branch_func_check = str2.substr(branch_func_start_position, 5);
                    branch_func_h_start_position = str2.length() - 16;
                    branch_func_h_check = str2.substr(branch_func_h_start_position, 5);
                    branch_func_l_start_position = str2.length() - 15;
                    branch_func_l_check = str2.substr(branch_func_l_start_position, 5);
                    branch_lbl_start_position = str2.length() - 12;
                    branch_lbl_check = str2.substr(branch_lbl_start_position, 4);
                    branch_lbl_h_start_position = str2.length() - 15;
                    branch_lbl_h_check = str2.substr(branch_lbl_h_start_position, 4);
                    branch_lbl_l_start_position = str2.length() - 14;
                    branch_lbl_l_check = str2.substr(branch_lbl_l_start_position, 4);
                    branch_lbl_lr_position = str2.length() - 18;
                    branch_lbl_lr_check = str2.substr(branch_lbl_lr_position, 4);
                    branch_lbl_lr2_position = str2.length() - 19;
                    branch_lbl_lr2_check = str2.substr(branch_lbl_lr2_position, 4);
                    branch_lbl_sda_base_position = str2.length() - 23;
                    branch_lbl_sda_base_check = str2.substr(branch_lbl_sda_base_position, 4);
                    branch_lbl_sda2_base_position = str2.length() - 24;
                    branch_lbl_sda2_base_check = str2.substr(branch_lbl_sda2_base_position, 4);
                    branch_lbl_sda_base_operandr2_position = str2.length() - 28;
                    branch_lbl_sda_base_operandr2_check = str2.substr(branch_lbl_sda_base_operandr2_position, 4);
                    branch_lbl_sda_base_operandr13_position = str2.length() - 28;
                    branch_lbl_sda_base_operandr13_check = str2.substr(branch_lbl_sda_base_operandr13_position, 4);
                    //cout << "TESTING:" << branch_lbl_sda_base_check << endl;

                    // rename branching instruction to appropriate symbol
                    if (branch_func_check == "func_") {
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_func_start_position - 20) << symbol_map[str2.substr(branch_func_start_position)] << endl;
                    } else if (branch_func_h_check == "func_") {
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_func_h_start_position - 20) << symbol_map[str2.substr(branch_func_h_start_position, 13)] << "@ha" << endl;
                    } else if (branch_func_l_check == "func_") {
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_func_l_start_position - 20) << symbol_map[str2.substr(branch_func_l_start_position, 13)] << "@l" << endl;
                    } else if (branch_lbl_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_start_position - 20) << endl;
                    } else if (branch_lbl_h_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_h_start_position - 20) << endl;
                    } else if (branch_lbl_l_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_l_start_position - 20) << endl;
                    } else if (branch_lbl_sda_base_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_sda_base_position - 20) << endl;
                    } else if (branch_lbl_sda2_base_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_sda2_base_position - 20) << endl;
                    } else if (branch_lbl_sda_base_operandr2_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_sda_base_operandr2_position - 20) << endl;
                    } else if (branch_lbl_sda_base_operandr13_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_sda_base_operandr13_position - 20) << endl;
                    } else if (branch_lbl_lr_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_lr_position - 20) << endl;
                    } else if (branch_lbl_lr2_check == "lbl_") {
                        // need to add these back in after compare
                        outfile << str2.substr(0, 3) << str2.substr(20, branch_lbl_lr2_position - 20) << endl;
                    } else {
                        //cout << str2.substr(0, 3) << str2.substr(20) << endl;
                        outfile << str2.substr(0, 3) << str2.substr(20) << endl;
                    }
                }
            }

            // else if it's a label line
            else if (line_check == "lb") {
                outfile << endl;
            }

            // else it's a blank line
            else {
                //cout << endl;
                outfile << endl;
                inFunctionFlag = false;
                outfile.close();
                outfile.clear(); 
            }
        }
    }
}