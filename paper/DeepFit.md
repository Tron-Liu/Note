# DeepFit: 3D Surface Fitting via Neural Network Weighted Least Squares

1. principal curvature (主曲率)
    * 数学上的定义：过曲面上某个点上具有正无穷个正交曲率，其中存在一条曲线使得该曲线的曲率为极大，这个曲率为极大值 K_max, 垂直于极大曲率的曲率为极小值 K_min。这两个曲率属性为主曲率，他们对应的方向称为主方向。他们代表这法曲率的极值。
    * 法曲率是刻画曲面在某一个方向的弯曲程度的量。对曲面而言，固定一个点，沿着该点不同切方向截出的曲线的曲率，就是曲面沿着这个方向的法曲率。

2. The main contributions of this paper are:
    * A method for per-point weight estimation for weighted least squares fitting using deep neural networks.
    * A scale-free method for robust surface fitting and normal estimation.
    * A method for principal curvature and geometric properties estimation without using ground truth labels.

3. In this paper we propose a novel approach for normal estimation by learning to fit an n-order Jet while predicting informative points' weights. Our approach removes the need of predefined scales and optimal scale selection since the informative points are extracted at any given scale. Our method generalizes the contemporary method proposed by Lenssen et. al. avoids the iterative process, and enables the computation of additional geometric properties.

4. Jets (truncated Taylor expansion)

## 文章结构

1. Abstract
2. Introduction

## TODO

1. 结果可视化是如何实现的，包括 normal vectors 和 principal curvature
