#include <array>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct Problem {
    bool freezed;
    int failed = 0;
    int ac_time = -1;
    int wa_time = -1;
    int tle_time = -1;
    int re_time = -1;
};

std::unordered_map<std::string, int> NameToTeam;
std::vector<std::vector<Problem>> Scores;
std::set<int> RealSequence;
std::vector<int> SeqToTeam;
std::vector<int> TeamToSeq;

void AddTeam(const std::string &team_name);

void Start(const int duration_time, const int problem_count);

void Submit(const std::string &problem_name, const std::string &team_name, const std::string &submit_status, const int time) {

}

void Flush() {

}



int main() {
    std::ios::sync_with_stdio(false);
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "ADDTEAM") {
            std::string team_name;
            std::cin >> team_name;
            AddTeam(team_name);
        }
        else if (cmd == "START") {
            std::string none;
            int duration_time, problem_count;
            std::cin >> none >> duration_time >> none >> problem_count;
            Start(duration_time, problem_count);
        }
        else if (cmd == "SUBMIT") {
            std::string none, problem_name, team_name, submit_status;
            int time;
            std::cin >> problem_name >> none >> team_name >> none >> submit_status >> none >> time;
            Submit(problem_name, team_name, submit_status, time);
        }
        else if (cmd == "FLUSH") {
            Flush();
        }
        else if (cmd == "SCROLL") {

        }
        else if (cmd == "QUERY_RANKING") {

        }
        else if (cmd == "QUERY_SUBMISSION") {

        }
        else if (cmd == "END") {
            return 0;
        }
        else {
            std::cerr << cmd;
        }
    }
}