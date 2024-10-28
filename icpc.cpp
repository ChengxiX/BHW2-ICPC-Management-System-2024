#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
//#pragma GCC optimize(3)

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
        if ((a==7&&b==13)||(a==13&&b==7)) {
            int x = 1;
        }
        if (a == b) {
            return false;
        }
        auto &a_team = Teams[a];
        auto &b_team = Teams[b];
        if (a_team.passed == b_team.passed) {
            if (a_team.passed == 0) {
                return a_team.name.compare(b_team.name) < 0;
            }
            if (a_team.penalty == b_team.penalty) {
                std::vector<int> a_time, b_time;
                a_time.reserve(a_team.passed);
                b_time.reserve(b_team.passed);
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
                if (a_team.passed>1) {
                    std::sort(a_time.begin(), a_time.end());
                    std::sort(b_time.begin(), b_time.end());
                    for (int i = a_team.passed-2; i >= 0; i--) {
                        if (a_time[i] != b_time[i]) {
                            return a_time[i] < b_time[i];
                        }
                    }
                }
                return a_team.name.compare(b_team.name) < 0;
            }
            return a_team.penalty < b_team.penalty;
        }
        return a_team.passed > b_team.passed;
    }
};

std::unordered_map<std::string, int> NameToTeam;
std::vector<int> SeqToTeam;
bool started = false;
bool global_freeze = false;
int end_time;
std::set<int, cmp> RealSequence;

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
    for (auto it = RealSequence.begin(); i < Teams.size(); i++, it++) {
        int id = *it;
        Teams[id].seq = i;
        SeqToTeam[i] = id;
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
    auto &p_struct = Scores[team_id][problem_id];
    switch (submit_status) {
        case 0:
            if (global_freeze && !(p_struct.freezed == 0 && p_struct.ac_time != -1)) {
                p_struct.freezed++;
            }
            if (p_struct.ac_time == -1 && !global_freeze) {
                RealSequence.erase(team_id);
                if (p_struct.first_ac_time == -1) {
                    p_struct.first_ac_time = time;
                }
                Teams[team_id].passed++;
                Teams[team_id].penalty += time + p_struct.failed_b4_ac * 20;
                RealSequence.insert(team_id);
            }
            if (p_struct.first_ac_time == -1) {
                p_struct.first_ac_time = time;
            }
            p_struct.ac_time = time;
            p_struct.last_submit = 0;
            Teams[team_id].latest_ac = problem_id;
            break;
        case 1:
            if (global_freeze && !(p_struct.freezed == 0 && p_struct.ac_time != -1)) {
                p_struct.freezed++;
            }
            if (p_struct.ac_time == -1) {
                p_struct.failed_b4_ac++;
                if (!global_freeze) {
                    p_struct.failed_b4_freezed++;
                }
            }
            p_struct.wa_time = time;
            p_struct.last_submit = 1;
            Teams[team_id].latest_wa = problem_id;
            break;
        case 2:
            if (global_freeze && !(p_struct.freezed == 0 && p_struct.ac_time != -1)) {
                p_struct.freezed++;
            }
            if (p_struct.ac_time == -1) {
                p_struct.failed_b4_ac++;
                if (!global_freeze) {
                    p_struct.failed_b4_freezed++;
                }
            }
            p_struct.tle_time = time;
            p_struct.last_submit = 2;
            Teams[team_id].latest_tle = problem_id;
            break;
        case 3:
            if (global_freeze && !(p_struct.freezed == 0 && p_struct.ac_time != -1)) {
                p_struct.freezed++;
            }
            if (p_struct.ac_time == -1) {
                p_struct.failed_b4_ac++;
                if (!global_freeze) {
                    p_struct.failed_b4_freezed++;
                }
            }
            p_struct.re_time = time;
            p_struct.last_submit = 3;
            Teams[team_id].latest_re = problem_id;
            break;
    }
}

void Flush() {
    int i = 0;
    for (auto it = RealSequence.begin(); i < Teams.size(); i++, it++) {
        int id = *it;
        Teams[id].seq = i;
        SeqToTeam[i] = id;
    }
    printf("[Info]Flush scoreboard.\n");
}

void Scroll() {
    if (!global_freeze) {
        printf("[Error]Scroll failed: scoreboard has not been frozen.\n");
        return;
    }
    printf("[Info]Scroll scoreboard.\n");
    int i = 0;
    for (auto it = RealSequence.begin(); i < Teams.size(); i++, it++) {
        int id = *it;
        Teams[id].seq = i;
        SeqToTeam[i] = id;
        printf("%s %d %d %d", Teams[id].name.c_str(), i+1, Teams[id].passed, Teams[id].penalty);
        for (int j = 0; j < problem_count; j++) {
            if (Scores[id][j].freezed == 0) {
                if (Scores[id][j].ac_time != -1) {
                    if (Scores[id][j].failed_b4_ac == 0) {
                        printf(" +");
                    }
                    else {
                        printf(" +%d", Scores[id][j].failed_b4_ac);
                    }
                }
                else {
                    if (Scores[id][j].failed_b4_ac == 0) {
                        printf(" .");
                    }
                    else {
                        printf(" -%d", Scores[id][j].failed_b4_ac);
                    }
                }
            }
            else {
                if (Scores[id][j].failed_b4_freezed == 0) {
                    printf(" 0/%d", Scores[id][j].freezed);
                }
                else {
                    printf(" -%d/%d", Scores[id][j].failed_b4_freezed, Scores[id][j].freezed);
                }
            }
        }
        printf("\n");
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
    i = 0;
    for (auto it = RealSequence.begin(); i < Teams.size(); i++, it++) {
        int t_id = *it;
        Teams[t_id].seq = i;
        SeqToTeam[i] = t_id;
        printf("%s %d %d %d", Teams[t_id].name.c_str(), i+1, Teams[t_id].passed, Teams[t_id].penalty);
        for (int j = 0; j < problem_count; j++) {
            if (Scores[t_id][j].ac_time != -1) {
                if (Scores[t_id][j].failed_b4_ac == 0) {
                    printf(" +");
                }
                else {
                    printf(" +%d", Scores[t_id][j].failed_b4_ac);
                }
            }
            else {
                if (Scores[t_id][j].failed_b4_ac == 0) {
                    printf(" .");
                }
                else {
                    printf(" -%d", Scores[t_id][j].failed_b4_ac);
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
    int t_id = t_it->second;
    const auto &score = Scores[t_id];
    int latest_time = 0;
    int latest_problem = question_id;
    int latest_status = status;
    if (question_id == -1) {
        switch (status) {
            case -1:
                switch (score[Teams[t_id].latest_submit].last_submit) {
                    case 0:
                        latest_time = score[Teams[t_id].latest_submit].ac_time;
                        latest_problem = Teams[t_id].latest_submit;
                        latest_status = 0;
                        break;
                    case 1:
                        latest_time = score[Teams[t_id].latest_submit].wa_time;
                        latest_problem = Teams[t_id].latest_submit;
                        latest_status = 1;
                        break;
                    case 2:
                        latest_time = score[Teams[t_id].latest_submit].tle_time;
                        latest_problem = Teams[t_id].latest_submit;
                        latest_status = 2;
                        break;
                    case 3:
                        latest_time = score[Teams[t_id].latest_submit].re_time;
                        latest_problem = Teams[t_id].latest_submit;
                        latest_status = 3;
                        break;
                }
                break;
            case 0:
                if (Teams[t_id].latest_ac != -1) {
                    latest_time = score[Teams[t_id].latest_ac].ac_time;
                    latest_problem = Teams[t_id].latest_ac;
                }
                break;
            case 1:
                if (Teams[t_id].latest_wa != -1) {
                    latest_time = score[Teams[t_id].latest_wa].wa_time;
                    latest_problem = Teams[t_id].latest_wa;
                }
                break;
            case 2:
                if (Teams[t_id].latest_tle != -1) {
                    latest_time = score[Teams[t_id].latest_tle].tle_time;
                    latest_problem = Teams[t_id].latest_tle;
                }
                break;
            case 3:
                if (Teams[t_id].latest_re != -1) {
                    latest_time = score[Teams[t_id].latest_re].re_time;
                    latest_problem = Teams[t_id].latest_re;
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
    std::string none;
    std::string team_name;
    int duration_time, count;
    while (std::cin >> cmd) {
        switch (cmd[1]) {
            case 'D':
                std::cin >> team_name;
                AddTeam(team_name);
                break;
            case 'T':
                std::cin >> none >> duration_time >> none >> count;
                Start(duration_time, count);
                break;
            case 'U':
                if (cmd == "SUBMIT") {
                    char problem_name;
                    std::string none, team_name, submit_status;
                    int time;
                    std::cin >> problem_name >> none >> team_name >> none >> submit_status >> none >> time;
                    int status;
                    switch (submit_status[0]) {
                        case 'A':
                            status = 0;
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
                    Submit(problem_name, team_name, status, time);
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
                break;
            case 'L':
                Flush();
                break;
            case 'R':
                if (global_freeze) {
                    printf("[Error]Freeze failed: scoreboard has been frozen.\n");
                }
                else {
                    global_freeze = true;
                    printf("[Info]Freeze scoreboard.\n");
                }
                break;
            case 'C':
                Scroll();
                break;
            case 'N':
                printf("[Info]Competition ends.\n");
                return 0;
        }
    }
}