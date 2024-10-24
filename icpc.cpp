#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#pragma GCC optimize(3)

const bool debug = true;

struct Problem {
    int freezed = 0;
    int failed_b4_ac = 0;
    int failed_b4_freezed = 0;
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
    printf("[Info]Add successfully.\n");
}

void Start(const int duration_time, const int count) {
    if (started) {
        printf("[Error]Start failed: competition has started.\n");
        return;
    }
    end_time = duration_time;
    problem_count = count;
    Scores.resize(Teams.size(), std::vector<Problem>(problem_count));
    SeqToTeam.resize(Teams.size());
    int i = 0;
    for (auto it = RealSequence.begin(); it != RealSequence.end(); it++, i++) {
        Teams[*it].seq = i;
        SeqToTeam[i] = *it;
    }
    started = true;
    printf("[Info]Competition starts.\n");
}

inline int QuestionNameToIndex(const char problem_name) {
    return problem_name - 'A';
}

void Submit(const char problem_name, const std::string &team_name, const int submit_status, const int time) {
    int team_id = NameToTeam[team_name];
    int problem_id = QuestionNameToIndex(problem_name);
    if (submit_status == 0) {
        if (global_freeze && !(Scores[team_id][problem_id].freezed == 0 && Scores[team_id][problem_id].ac_time != -1)) {
            Scores[team_id][problem_id].freezed++;
        }
        if (Scores[team_id][problem_id].ac_time == -1 && !global_freeze) {
            Teams[team_id].passed++;
            Teams[team_id].penalty += time + Scores[team_id][problem_id].failed_b4_ac * 20;
            RealSequence.erase(team_id);
            RealSequence.insert(team_id);
        }
        Scores[team_id][problem_id].ac_time = time;
    }
    else if (submit_status == 1) {
        if (global_freeze && !(Scores[team_id][problem_id].freezed == 0 && Scores[team_id][problem_id].ac_time != -1)) {
            Scores[team_id][problem_id].freezed++;
        }
        if (Scores[team_id][problem_id].ac_time == -1) {
            Scores[team_id][problem_id].failed_b4_ac++;
            if (!global_freeze) {
                Scores[team_id][problem_id].failed_b4_freezed++;
            }
        }
        Scores[team_id][problem_id].wa_time = time;
    }
    else if (submit_status == 2) {
        if (global_freeze && !(Scores[team_id][problem_id].freezed == 0 && Scores[team_id][problem_id].ac_time != -1)) {
            Scores[team_id][problem_id].freezed++;
        }
        if (Scores[team_id][problem_id].ac_time == -1) {
            Scores[team_id][problem_id].failed_b4_ac++;
            if (!global_freeze) {
                Scores[team_id][problem_id].failed_b4_freezed++;
            }
        }
        Scores[team_id][problem_id].tle_time = time;
    }
    else if (submit_status == 3) {
        if (global_freeze && !(Scores[team_id][problem_id].freezed == 0 && Scores[team_id][problem_id].ac_time != -1)) {
            Scores[team_id][problem_id].freezed++;
        }
        if (Scores[team_id][problem_id].ac_time == -1) {
            Scores[team_id][problem_id].failed_b4_ac++;
            if (!global_freeze) {
                Scores[team_id][problem_id].failed_b4_freezed++;
            }
        }
        Scores[team_id][problem_id].re_time = time;
    }
}

void Flush() {
    int i = 0;
    for (auto it = RealSequence.begin(); it != RealSequence.end(); it++, i++) {
        Teams[*it].seq = i;
        SeqToTeam[i] = *it;
    }
    printf("[Info]Freeze scoreboard.\n");
}

void Scroll() {
    if (!global_freeze) {
        printf("[Error]Scroll failed: scoreboard has not been frozen.\n");
        return;
    }
    printf("[Info]Scroll scoreboard.\n");
    Flush();
    for (int i = 0; i < Teams.size(); i++) {
        printf("%s %d %d %d", Teams[SeqToTeam[i]].name.c_str(), i+1, Teams[SeqToTeam[i]].passed, Teams[SeqToTeam[i]].penalty);
        for (int j = 0; j < problem_count; j++) {
            if (Scores[SeqToTeam[i]][j].freezed == 0) {
                if (Scores[SeqToTeam[i]][j].ac_time != -1) {
                    if (Scores[SeqToTeam[i]][j].failed_b4_ac == 0) {
                        printf(" +");
                    }
                    else {
                        printf(" +%d", Scores[SeqToTeam[i]][j].failed_b4_ac);
                    }
                }
                else {
                    if (Scores[SeqToTeam[i]][j].failed_b4_ac == 0) {
                        printf(" .");
                    }
                    else {
                        printf(" -%d", Scores[SeqToTeam[i]][j].failed_b4_ac);
                    }
                }
            }
            else {
                if (Scores[SeqToTeam[i]][j].failed_b4_freezed == 0) {
                    printf(" 0/%d", Scores[SeqToTeam[i]][j].freezed);
                }
                else {
                    printf(" -%d/%d", Scores[SeqToTeam[i]][j].failed_b4_freezed, Scores[SeqToTeam[i]][j].failed_b4_freezed);
                }
            }
        }
        printf("\n");
    }
    bool exit = false;
    while (!exit) {
        exit = true;
        for (auto s_it = RealSequence.rbegin(); s_it != RealSequence.rend(); s_it++) {
            for (int j = 0; j < problem_count; j++) {
                if (Scores[*s_it][j].freezed != 0) {
                    if (Scores[*s_it][j].ac_time != -1) {
                        Scores[*s_it][j].freezed = 0;
                        Scores[*s_it][j].failed_b4_freezed = 0;
                        Teams[*s_it].passed++;
                        Teams[*s_it].penalty += Scores[*s_it][j].ac_time + Scores[*s_it][j].failed_b4_ac * 20;
                        RealSequence.erase(*s_it);
                        auto res = RealSequence.insert(*s_it);
                        auto it = res.first;
                        it++;
                        printf("%s %s %d %d\n", Teams[*res.first].name.c_str(), Teams[*it].name.c_str(), Teams[*res.first].passed, Teams[*res.first].penalty);
                        exit = false;
                        break;
                    }
                    else {
                        Scores[*s_it][j].freezed = 0;
                        Scores[*s_it][j].failed_b4_freezed = 0;
                    }
                }
            }
        }
    }
    Flush();
    for (int i = 0; i < Teams.size(); i++) {
        printf("%s %d %d %d", Teams[SeqToTeam[i]].name.c_str(), i+1, Teams[SeqToTeam[i]].passed, Teams[SeqToTeam[i]].penalty);
        for (int j = 0; j < problem_count; j++) {
            if (Scores[SeqToTeam[i]][j].ac_time != -1) {
                if (Scores[SeqToTeam[i]][j].failed_b4_ac == 0) {
                    printf(" +");
                }
                else {
                    printf(" +%d", Scores[SeqToTeam[i]][j].failed_b4_ac);
                }
            }
            else {
                if (Scores[SeqToTeam[i]][j].failed_b4_ac == 0) {
                    printf(" .");
                }
                else {
                    printf(" -%d", Scores[SeqToTeam[i]][j].failed_b4_ac);
                }
            }
        }
        printf("\n");
    }
    global_freeze = false;
}

void QueryRanking(const std::string &team_name) {
    auto t_it = NameToTeam.find(team_name);
    if (t_it == NameToTeam.end()) {
        printf("[Error]Query ranking failed: cannot find the team.\n");
        return;
    }
    printf("[Info]Complete query ranking.\n");
    if (global_freeze) {
        printf("[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled.\n");
    }
    printf("%s NOW AT RANKING %d\n", team_name.c_str(), Teams[t_it->second].seq);
}

void QuerySubmission(const std::string &team_name, const char problem_name, const int status) {
    auto t_it = NameToTeam.find(team_name);
    if (t_it == NameToTeam.end()) {
        printf("[Error]Query submission failed: cannot find the team.\n");
        return;
    }
    printf("[Info]Complete query submission.\n");
    int question_id = QuestionNameToIndex(problem_name);
    const auto &score = Scores[t_it->second];
    int latest_time = 0;
    int latest_problem = question_id;
    int latest_status = status;
    if (question_id == -1) {
        if (status == -1) {
            for (int i = 0; i < problem_count; i++) {
                if (latest_time < score[i].ac_time) {
                    latest_time = score[i].ac_time;
                    latest_problem = i;
                    latest_status = 0;
                }
                if (latest_time < score[i].wa_time) {
                    latest_time = score[i].wa_time;
                    latest_problem = i;
                    latest_status = 1;
                }
                if (latest_time < score[i].tle_time) {
                    latest_time = score[i].tle_time;
                    latest_problem = i;
                    latest_status = 2;
                }
                if (latest_time < score[i].re_time) {
                    latest_time = score[i].re_time;
                    latest_problem = i;
                    latest_status = 3;
                }
            }
        }
        else if (status == 0) {
            for (int i = 0; i < problem_count; i++) {
                if (latest_time < score[i].ac_time) {
                    latest_time = score[i].ac_time;
                    latest_problem = i;
                }
            }
        }
        else if (status == 1) {
            for (int i = 0; i < problem_count; i++) {
                if (latest_time < score[i].wa_time) {
                    latest_time = score[i].wa_time;
                    latest_problem = i;
                }
            }
        }
        else if (status == 2) {
            for (int i = 0; i < problem_count; i++) {
                if (latest_time < score[i].tle_time) {
                    latest_time = score[i].tle_time;
                    latest_problem = i;
                }
            }
        }
        else if (status == 3) {
            for (int i = 0; i < problem_count; i++) {
                if (latest_time < score[i].re_time) {
                    latest_time = score[i].re_time;
                    latest_problem = i;
                }
            }
        }
    }
    else {
        if (status == -1) {
            if (latest_time < score[question_id].ac_time) {
                latest_time = score[question_id].ac_time;
                latest_status = 0;
            }
            if (latest_time < score[question_id].wa_time) {
                latest_time = score[question_id].wa_time;
                latest_status = 1;
            }
            if (latest_time < score[question_id].tle_time) {
                latest_time = score[question_id].tle_time;
                latest_status = 2;
            }
            if (latest_time < score[question_id].re_time) {
                latest_time = score[question_id].re_time;
                latest_status = 3;
            }
        }
        else if (status == 0) {
            if (latest_time < score[question_id].ac_time) {
                latest_time = score[question_id].ac_time;
            }
        }
        else if (status == 1) {
            if (latest_time < score[question_id].wa_time) {
                latest_time = score[question_id].wa_time;
            }
        }
        else if (status == 2) {
            if (latest_time < score[question_id].tle_time) {
                latest_time = score[question_id].tle_time;
            }
        }
        else if (status == 3) {
            if (latest_time < score[question_id].re_time) {
                latest_time = score[question_id].re_time;
            }
        }
    }
    if (latest_time == 0) {
        printf("Cannot find any submission.\n");
    }
    else {
        printf("%s %c %s %d", team_name.c_str(), char(latest_problem + 'A'),
        latest_status == 0 ? "Accepted" : latest_status == 1 ? "Wrong_Answer" : latest_status == 2 ? "Time_Limit_Exceed" : "Runtime_Error", latest_time);
    }
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
            int status;
            if (submit_status[0]=='A') {
                status = 0;
            }
            else if (submit_status[0]=='W') {
                status = 1;
            }
            else if (submit_status[0]=='T') {
                status = 2;
            }
            else {
                status = 3;
            }
            Submit(problem_name, team_name, status, time);
        }
        else if (cmd == "FLUSH") {
            Flush();
        }
        else if (cmd == "FREEZE") {
            if (global_freeze) {
                printf("[Error]Freeze failed: scoreboard has been frozen.\n");
            }
            else {
                global_freeze = true;
                printf("[Info]Freeze scoreboard.\n");
            }
        }
        else if (cmd == "SCROLL") {
            Scroll();
        }
        else if (cmd == "QUERY_RANKING") {
            std::string team_name;
            QueryRanking(team_name);
        }
        else if (cmd == "QUERY_SUBMISSION") {
            std::string none, team_name, submit_status, problem_name;
            std::cin >> team_name >> none >> problem_name >> none >> submit_status;
            char problem = problem_name.back();
            if (problem_name[problem_name.size()-2]=='L') {
                problem = 'A' - 1;
            }
            int status;
            if (submit_status[7]=='A') {
                if (submit_status[8]=='L') {
                    status = -1;
                }
                else {
                    status = 0;
                }
            }
            else if (submit_status[7]=='W') {
                status = 1;
            }
            else if (submit_status[7]=='T') {
                status = 2;
            }
            else {
                status = 3;
            }
            QuerySubmission(team_name, problem, status);
        }
        else if (cmd == "END") {
            return 0;
        }
        else if (debug) {
            std::cerr << cmd;
        }
    }
    printf("[Info]Competition ends.\n");
}