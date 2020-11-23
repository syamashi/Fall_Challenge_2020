# Fall_Challenge_2020

https://www.codingame.com/contests/fall-challenge-2020

- Result

Global Rank 81/7035

Gold League 1/471

こどげまとめ

・初手8ターンはLEARN

・actionid >= 34のLEARNがid = 1にあれば先取り(actionid = 41に近いほど上昇上限が10に近い）

https://docs.google.com/spreadsheets/d/1pP_hTWFScC0pGb3KnLpXAurs0MdWyG8wZ_qFKjeRJhk/edit#gid=1292434848

・loop = 5でDFS全探索（約80000nodeまで探索可）

・探索の初手(loop = 1)だけLearn考慮

・evalは4要素。inv, potion, rest, learn

