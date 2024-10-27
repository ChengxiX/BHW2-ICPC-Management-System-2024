#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#pragma GCC optimize(3)

const bool debug = true;

struct Problem {
    int freezed = 0;
    int failed_b4_ac = 0;
    int failed_b4_freezed = 0;
    int last_submit = -1;
    int ac_time = -1;
    int first_ac_time = -1;
    int wa_time = -1;
    int tle_time = -1;
    int re_time = -1;
};

struct Team {
    std::string name;
    int seq = -1;
    int passed = 0;
    int penalty = 0;
    int latest_submit = -1;
    int latest_ac = -1;
    int latest_wa = -1;
    int latest_tle = -1;
    int latest_re = -1;
};

std::vector<Team> Teams;
int problem_count;
std::vector<std::vector<Problem>> Scores;

struct cmp {
    bool operator() (const int a, const int b) const {
        if (a == b) {
            return false;
        }
        if (Teams[a].passed == Teams[b].passed) {
            if (Teams[a].passed == 0) {
                return Teams[a].name.compare(Teams[b].name) < 0;
            }
            if (Teams[a].penalty == Teams[b].penalty) {
                std::vector<int> a_time, b_time;
                a_time.reserve(Teams[a].passed);
                b_time.reserve(Teams[b].passed);
                int a_max = -1, b_max = -1;
                for (int i = 0; i < problem_count; i++) {
                    if (Scores[a][i].first_ac_time != -1 && Scores[a][i].freezed == 0) {
                        a_time.push_back(Scores[a][i].first_ac_time);
                        if (Scores[a][i].first_ac_time > a_max) {
                            a_max = Scores[a][i].first_ac_time;
                        }
                    }
                    if (Scores[b][i].first_ac_time != -1 && Scores[b][i].freezed == 0) {
                        b_time.push_back(Scores[b][i].first_ac_time);
                        if (Scores[b][i].first_ac_time > b_max) {
                            b_max = Scores[b][i].first_ac_time;
                        }
                    }
                }
                if (a_max != b_max) {
                    return a_max < b_max;
                }
                std::sort(a_time.begin(), a_time.end());
                std::sort(b_time.begin(), b_time.end());
                for (int i = Teams[a].passed-2; i >= 0; i--) {
                    if (a_time[i] != b_time[i]) {
                        return a_time[i] < b_time[i];
                    }
                }
                return Teams[a].name.compare(Teams[b].name) < 0;
            }
            return Teams[a].penalty < Teams[b].penalty;
        }
        return Teams[a].passed > Teams[b].passed;
    }
};

struct dic_cmp {
    bool operator() (const int a, const int b) const {
        return Teams[a].name.compare(Teams[b].name) < 0;
    }
};

std::unordered_map<std::string, int> NameToTeam;
std::vector<int> SeqToTeam;
bool started = false;
bool global_freeze = false;
int end_time;

void AddTeam(const std::string &team_name) {
    if (started) {
        printf("[Error]Add failed: competition has started.\n");
        return;
    }
    int team_id = Teams.size();
    auto res = NameToTeam.insert({team_name, team_id});
    if (!res.second) {
        printf("[Error]Add failed: duplicated team name.\n");
        return;
    }
    Teams.push_back({team_name});
    SeqToTeam.push_back(team_id);
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
    std::sort(SeqToTeam.begin(), SeqToTeam.end(), dic_cmp());
    for (int i = 0; i < Teams.size(); i++) {
        Teams[SeqToTeam[i]].seq = i;
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
    Teams[team_id].latest_submit = problem_id;
    switch (submit_status) {
        case 0:
            if (global_freeze && !(Scores[team_id][problem_id].freezed == 0 && Scores[team_id][problem_id].ac_time != -1)) {
                Scores[team_id][problem_id].freezed++;
            }
            if (Scores[team_id][problem_id].ac_time == -1 && !global_freeze) {
                Teams[team_id].passed++;
                Teams[team_id].penalty += time + Scores[team_id][problem_id].failed_b4_ac * 20;
            }
            if (Scores[team_id][problem_id].first_ac_time == -1) {
                Scores[team_id][problem_id].first_ac_time = time;
            }
            Scores[team_id][problem_id].ac_time = time;
            Scores[team_id][problem_id].last_submit = 0;
            Teams[team_id].latest_ac = problem_id;
            break;
        case 1:
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
            Scores[team_id][problem_id].last_submit = 1;
            Teams[team_id].latest_wa = problem_id;
            break;
        case 2:
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
            Scores[team_id][problem_id].last_submit = 2;
            Teams[team_id].latest_tle = problem_id;
            break;
        case 3:
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
            Scores[team_id][problem_id].last_submit = 3;
            Teams[team_id].latest_re = problem_id;
            break;
    }
}

void Flush() {
    std::sort(SeqToTeam.begin(), SeqToTeam.end(), cmp());
    for (int i = 0; i < Teams.size(); i++) {
        Teams[SeqToTeam[i]].seq = i;
    }
    printf("[Info]Flush scoreboard.\n");
}

void Scroll() {
    if (!global_freeze) {
        printf("[Error]Scroll failed: scoreboard has not been frozen.\n");
        return;
    }
    printf("[Info]Scroll scoreboard.\n");
    std::sort(SeqToTeam.begin(), SeqToTeam.end(), cmp());
    for (int i = 0; i < Teams.size(); i++) {
        Teams[SeqToTeam[i]].seq = i;
    }
    std::set<int, cmp> RealSequence;
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
                    printf(" -%d/%d", Scores[SeqToTeam[i]][j].failed_b4_freezed, Scores[SeqToTeam[i]][j].freezed);
                }
            }
        }
        printf("\n");
        RealSequence.insert(RealSequence.end(), SeqToTeam[i]);
    }
    bool exit = false;
    bool break_twice = false;
    while (!exit) {
        exit = true;
        int last_id = -1;
        for (auto s_it = RealSequence.end(); s_it != RealSequence.begin();) {
            s_it--;
            int t_id = *s_it;
            for (int j = 0; j < problem_count; j++) {
                if (Scores[t_id][j].freezed != 0) {
                    if (Scores[t_id][j].ac_time != -1) {
                        Scores[t_id][j].freezed = 0;
                        Scores[t_id][j].failed_b4_freezed = 0;
                        auto r = RealSequence.erase(s_it);
                        Teams[t_id].passed++;
                        Teams[t_id].penalty += Scores[t_id][j].first_ac_time + Scores[t_id][j].failed_b4_ac * 20;
                        auto replaced = RealSequence.upper_bound(t_id);
                        int replaced_id;
                        if (replaced == RealSequence.end()) {
                            replaced_id = -1;
                        }
                        else {
                            replaced_id = *replaced;
                        }
                        auto res = RealSequence.insert(replaced, t_id);
                        if (replaced_id != last_id) { 
                            printf("%s %s %d %d\n", Teams[t_id].name.c_str(), Teams[replaced_id].name.c_str(), Teams[t_id].passed, Teams[t_id].penalty);
                        }
                        exit = false;
                        break_twice =true;
                        break;
                    }
                    else {
                        Scores[*s_it][j].freezed = 0;
                        Scores[*s_it][j].failed_b4_freezed = Scores[*s_it][j].failed_b4_ac;
                    }
                }
            }
            if (break_twice) {
                break_twice = false;
                break;
            }
            last_id = t_id;
        }
    }
    int i = 0;
    for (auto it = RealSequence.begin(); i < Teams.size(); i++, it++) {
        Teams[*it].seq = i;
        SeqToTeam[i] = *it;
        printf("%s %d %d %d", Teams[*it].name.c_str(), i+1, Teams[*it].passed, Teams[SeqToTeam[i]].penalty);
        for (int j = 0; j < problem_count; j++) {
            if (Scores[*it][j].ac_time != -1) {
                if (Scores[*it][j].failed_b4_ac == 0) {
                    printf(" +");
                }
                else {
                    printf(" +%d", Scores[*it][j].failed_b4_ac);
                }
            }
            else {
                if (Scores[*it][j].failed_b4_ac == 0) {
                    printf(" .");
                }
                else {
                    printf(" -%d", Scores[*it][j].failed_b4_ac);
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
    printf("%s NOW AT RANKING %d\n", team_name.c_str(), Teams[t_it->second].seq + 1);
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
        switch (status) {
            case -1:
                switch (score[Teams[t_it->second].latest_submit].last_submit) {
                    case 0:
                        latest_time = score[Teams[t_it->second].latest_submit].ac_time;
                        latest_problem = Teams[t_it->second].latest_submit;
                        latest_status = 0;
                        break;
                    case 1:
                        latest_time = score[Teams[t_it->second].latest_submit].wa_time;
                        latest_problem = Teams[t_it->second].latest_submit;
                        latest_status = 1;
                        break;
                    case 2:
                        latest_time = score[Teams[t_it->second].latest_submit].tle_time;
                        latest_problem = Teams[t_it->second].latest_submit;
                        latest_status = 2;
                        break;
                    case 3:
                        latest_time = score[Teams[t_it->second].latest_submit].re_time;
                        latest_problem = Teams[t_it->second].latest_submit;
                        latest_status = 3;
                        break;
                }
                break;
            case 0:
                if (Teams[t_it->second].latest_ac != -1) {
                    latest_time = score[Teams[t_it->second].latest_ac].ac_time;
                    latest_problem = Teams[t_it->second].latest_ac;
                }
                break;
            case 1:
                if (Teams[t_it->second].latest_wa != -1) {
                    latest_time = score[Teams[t_it->second].latest_wa].wa_time;
                    latest_problem = Teams[t_it->second].latest_wa;
                }
                break;
            case 2:
                if (Teams[t_it->second].latest_tle != -1) {
                    latest_time = score[Teams[t_it->second].latest_tle].tle_time;
                    latest_problem = Teams[t_it->second].latest_tle;
                }
                break;
            case 3:
                if (Teams[t_it->second].latest_re != -1) {
                    latest_time = score[Teams[t_it->second].latest_re].re_time;
                    latest_problem = Teams[t_it->second].latest_re;
                }
                break;
        }
    }
    else {
        switch (status) {
            case -1:
                switch (score[question_id].last_submit) {
                    case 0:
                        latest_time = score[question_id].ac_time;
                        latest_status = 0;
                        break;
                    case 1:
                        latest_time = score[question_id].wa_time;
                        latest_status = 1;
                        break;
                    case 2:
                        latest_time = score[question_id].tle_time;
                        latest_status = 2;
                        break;
                    case 3:
                        latest_time = score[question_id].re_time;
                        latest_status = 3;
                        break;
                }
                break;
            case 0:
                if (latest_time < score[question_id].ac_time) {
                    latest_time = score[question_id].ac_time;
                }
                break;
            case 1:
                if (latest_time < score[question_id].wa_time) {
                    latest_time = score[question_id].wa_time;
                }
                break;
            case 2:
                if (latest_time < score[question_id].tle_time) {
                    latest_time = score[question_id].tle_time;
                }
                break;
            case 3:
                if (latest_time < score[question_id].re_time) {
                    latest_time = score[question_id].re_time;
                }
                break;
        }
    }
    if (latest_time == 0) {
        printf("Cannot find any submission.\n");
    }
    else {
        printf("%s %c %s %d\n", team_name.c_str(), char(latest_problem + 'A'),
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
            std::cin >> team_name;
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
            switch (submit_status[7]) {
                case 'A':
                    if (submit_status[8]=='L') {
                        status = -1;
                    }
                    else {
                        status = 0;
                    }
                    break;
                case 'W':
                    status = 1;
                    break;
                case 'T':
                    status = 2;
                    break;
                default:
                    status = 3;
                    break;
            }
            QuerySubmission(team_name, problem, status);
        }
        else if (cmd == "END") {
            printf("[Info]Competition ends.\n");
            return 0;
        }
        else if (debug) {
            std::cerr << cmd;
        }
    }
}