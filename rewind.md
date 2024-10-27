# 回望&分析

## 数据结构
以Teams下标TeamId为主，队名索引

## 时间复杂度（主要操作）

`AddTeam()` 常数

`Start()` 快速排序 $O(n\log n)$~$O(n)$

`Submit()` 常数

`Flush()` 快速排序 $O(n\log n)$~$O(n)$

`Scroll()` $O(n\log n)$

`QueryRanking()` 常数

`QuerySubmission` 常数