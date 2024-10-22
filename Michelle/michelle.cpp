#include <cstdio>
#include <utility>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>
#include <string>
#include <cstring>
#include <array>

struct Student {
    char name[31];
    char gender;
    int class_;
};

using Scores = std::array<int, 10>; // Last one is average

std::vector<Student> Students_List;
std::vector<Scores> Scores_List;
int StudentAmount;

struct cmp {
    bool operator()(int a, int b) const {
        if (a == b) {
            return false;
        }
        if (Scores_List[a][9] == Scores_List[b][9]) {
            for (int i = 0; i < 9; i++) {
                if (Scores_List[a][i] != Scores_List[b][i]) {
                    return Scores_List[a][i] > Scores_List[b][i];
                }
            }
            return strcmp(Students_List[a].name, Students_List[b].name) < 0;
        }
        else {
            return Scores_List[a][9] > Scores_List[b][9];
        }
    }
};

std::unordered_map<std::string, int> Students;
std::set<int, cmp> CurrentRanking;
std::vector<int> RankingResult;
std::vector<int> StudentRanking;

int main () {
    char command[16];
    int status = 0; // 0 for not started, 1 for start
    while (scanf("%s", command), strcmp(command, "END") != 0) {
        if (strcmp(command, "ADD") == 0) {
            if (status!=0) {
                printf("[Error]Cannot add student now.\n");
                std::string none;
                std::getline(std::cin, none);
                continue;
            }
            
            char name[31];
            scanf("%s", name);
            if (Students.count(name) != 0) {
                printf("[Error]Add failed.\n");
                std::string none;
                std::getline(std::cin, none);
                continue;
            }
            char gender;
            int class_;
            Scores_List.emplace_back();
            Students_List.emplace_back();
            Scores &score = Scores_List[Scores_List.size()-1];
            scanf("%s %d %d %d %d %d %d %d %d %d %d", &Students_List[Students_List.size()-1].gender, &Students_List[Students_List.size()-1].class_,
            &score[0], &score[1], &score[2], &score[3], &score[4], &score[5], &score[6], &score[7], &score[8]);
            score[9] = (score[0] + score[1] + score[2] + score[3] + score[4] + score[5] + score[6] + score[7] + score[8]) / 9;
            Students.insert(std::make_pair(name, Scores_List.size()-1));
            strcpy(Students_List[Students_List.size()-1].name, name);
        }
        else if (strcmp(command, "START") == 0) {
            status = 1;
            StudentAmount = Students_List.size();
            RankingResult.resize(StudentAmount);
            StudentRanking.resize(StudentAmount);
            for (int i = 0; i < StudentAmount; i++) {
                CurrentRanking.insert(i);
            }
            int j = 0;
            for (auto it = CurrentRanking.begin(); j < StudentAmount; j++, it++) {
                RankingResult[j] = *it;
                StudentRanking[*it] = j;
            }
        }
        else if (strcmp(command, "FLUSH") == 0) {
            int i = 0;
            for (auto it = CurrentRanking.begin(); i < StudentAmount; i++, it++) {
                RankingResult[i] = *it;
                StudentRanking[*it] = i;
            }
        }
        else if (strcmp(command, "PRINTLIST") == 0) {
            for (int i = 0; i < StudentAmount; i++) {
                printf("%d %s %s %d %d\n", i + 1, Students_List[RankingResult[i]].name,
                Students_List[RankingResult[i]].gender == 'M' ? "male" : "female",
                Students_List[RankingResult[i]].class_, Scores_List[RankingResult[i]][9]);
            }
        }
        else if (strcmp(command, "QUERY") == 0) {
            char name[31];
            scanf("%s", name);
            auto it = Students.find(name);
            if (it == Students.end()) {
                printf("[Error]Query failed.\n");
                continue;
            }
            printf("STUDENT %s NOW AT RANKING %d\n", name, StudentRanking[it->second] + 1);
        }
        else if (strcmp(command, "UPDATE") == 0) {
            char name[31];
            int code, score;
            scanf("%s %d %d", name, &code, &score);
            auto it = Students.find(name);
            if (it == Students.end()) {
                printf("[Error]Update failed.\n");
                continue;
            }
            auto it2 = CurrentRanking.find(it->second);
            Scores &s = Scores_List[it->second];
            s[code] = score;
            s[9] = (s[0] + s[1] + s[2] + s[3] + s[4] + s[5] + s[6] + s[7] + s[8]) / 9;
            CurrentRanking.erase(it2);
            CurrentRanking.insert(it->second);
        }
    }
}