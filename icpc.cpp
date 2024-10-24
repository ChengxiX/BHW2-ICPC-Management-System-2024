#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct Problem {
    int freezed = 0; // 0 for not freezed, positive number for freezed and the times 
    int failed = 0;
    int ac_time = -1;
    int wa_time = -1;
    int tle_time = -1;
    int re_time = -1;
};

struct Team {
    std::string name;
    int seq = -1;
    int passed = 0;
    int penalty = 0;
};

std::vector<Team> Teams;

struct cmp {
    bool operator() (const int a, const int b) {
        if (a == b) {
            return false;
        }
        if (Teams[a].passed == Teams[b].passed) {
            if (Teams[a].penalty == Teams[b].penalty) {
                return Teams[a].name.compare(Teams[b].name) < 0;
            }
            return Teams[a].penalty < Teams[b].penalty;
        }
        return Teams[a].passed > Teams[b].passed;
    }
};

std::unordered_map<std::string, int> NameToTeam;
std::vector<std::vector<Problem>> Scores;
std::set<int, cmp> RealSequence;
std::vector<int> SeqToTeam;
bool started = false;
bool global_freeze = false;
int problem_count;
int end_time;


void AddTeam(const std::string &team_name) {
    if (started) {
        printf("[Error]Add failed: competition has started.\n");
        return;
    }
    if (NameToTeam.count(team_name) != 0) {
        printf("[Error]Add failed: duplicated team name.\n");
        return;
    }
    int team_id = NameToTeam.size();
    NameToTeam.insert({team_name, team_id});
    Teams.push_back({team_name});
    RealSequence.insert(team_id);
}

void Start(const int duration_time, const int problem_count) {
    if (started) {
        printf("[Error]Start failed: competition has started.\n");
        return;
    }
    end_time = duration_time;
    Scores.resize(Teams.size(), std::vector<Problem>(problem_count));
    SeqToTeam.resize(Teams.size());
    int i = 0;
    for (auto it = RealSequence.begin(); it != RealSequence.end(); ++it, ++i) {
        Teams[*it].seq = i;
        SeqToTeam[i] = *it;
    }
    started = true;
}

inline int QuestionNameToIndex(const char problem_name) {
    return problem_name - 'A';
}

void Submit(const char problem_name, const std::string &team_name, const std::string &submit_status, const int time) {
    int team_id = NameToTeam[team_name];
    int problem_id = QuestionNameToIndex(problem_name);
    if (submit_status == "Accepted") {
        if (global_freeze) {
            Scores[team_id][problem_id].freezed++;
        }
        Scores[team_id][problem_id].ac_time = time;
    }
    else if (submit_status == "Wrong_Answer") {
        if (global_freeze) {
            Scores[team_id][problem_id].freezed++;
        }
        Scores[team_id][problem_id].failed++;
        Scores[team_id][problem_id].wa_time = time;
    }
    else if (submit_status == "Time_Limit_Exceed") {
        if (global_freeze) {
            Scores[team_id][problem_id].freezed++;
        }
        Scores[team_id][problem_id].failed++;
        Scores[team_id][problem_id].tle_time = time;
    }
    else if (submit_status == "Runtime_Error") {
        if (global_freeze) {
            Scores[team_id][problem_id].freezed++;
        }
        Scores[team_id][problem_id].failed++;
        Scores[team_id][problem_id].re_time = time;
    }
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
            int duration_time, count;
            std::cin >> none >> duration_time >> none >> count;
            Start(duration_time, count);
        }
        else if (cmd == "SUBMIT") {
            char problem_name;
            std::string none, team_name, submit_status;
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