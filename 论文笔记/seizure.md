# preprocessing

two-dimensional CNN : convert raw EEG data into a matrix, ( The conversion must be able to keep the most important from the EEG signals).  In this work, we use the short-time Fourier tansform (STFT) to translate raw EEG signal into a two-dimensional matrix composed of frequency and time axes.

# 信号处理

平稳信号：在不同时间得到的采样值的统计特性（比如期望、方差等 ）是相同的，在信号处理中，这个特性通常指频率。

非平稳信号：与平稳信号相反，其特征会随时间变化。

# 短时傅里叶变换

实际上，计算短时傅里叶变换的过程是把一个较长的时间信号分成相同长度的更短的段，在每个更短的段上计算傅里叶变换

# torch

x = torch.rand(4,3)

```python
x = torch.rand(4,3)
x.size(0) -> 4 第一个维度的值
x.size(1) -> 3 第二个维度的值
```



# 卷积核尺寸计算公式

输入尺寸（input）：i

卷积核大小（kernel size）：k

步幅（stride）：s

边界扩充（padding）：p

输出尺寸（output）：o

# 各符号含义

Fp1：左额极		Fp2：右额极

F3：左颞			

C3：

P3

O1



| 符号 | 含义     |
| ---- | -------- |
| Fp1  | 左额极   |
| Fp2  | 右额极   |
| F3   | 左额点   |
| F4   | 右额点   |
| C3   | 左中央点 |
| C4   | 右中央点 |
| P3   | 左顶点   |
| P4   | 右顶点   |
| F7   | 左颞前   |
| F8   | 右颞前   |
| T3   | 左颞点   |
| T4   | 右颞点   |
| T5   | 左颞后   |
| T6   | 右颞后   |
| O1   | 左枕点   |
| O2   | 右枕点   |



# 一个 self-attention 模块包括以下步骤

1. 准备输入

2. 初始化权重

3. 推导key、query 和 value

4. 计算输入 1 的注意力得分
5. 计算 softmax
6. 将分数与值相乘
7. 将权重值相加，得到输出1
8. 对输入 2和输入3重复步骤4-7