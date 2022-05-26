# PoinTr: Diverse Point Cloud Completion with Geometry-Aware Transformers

1. 使用 encoder-decoder 结构生成点云数据的瓶颈在 encoding 领域的 max-pooling 操作，此操作会导致细粒度信息 (fine-grained) 的丢失，并很难在 decoding 领域恢复该信息。
2. However, the bottleneck of such methods lies in the max-pooling operation in the encoding phase, where fine-grained information is lost and can hardly be recovered in the decoding phase.
3. Encoder-Decoder Architecture: 我们使用 Encoder-Decoder 结构来将点云补全转化为一个集合到集合的翻译问题。
4. Encoder-Decoder Architecture: We adopt the encoder-decoder architecture to convert point cloud completion as
a set-to-set translation problem.
5. 点代理：
6. Point Proxy: We represent the set of point clouds in a local region as a feature vector called Point Proxy.

## FPS 最远点采样

[知乎链接](https://zhuanlan.zhihu.com/p/209251414)

1. 从 N 个点中，采样出 M 个点 (M < N)。那么我们可以设定两个集合A，B。A 表示选中的点形成的集合，B表示未选中的点构成的集合。顾名思义，FPS做的事情就是：每次从集合 B 里面选一个到集合 A 里面的点距离最大的点
2. 优点：尽可能覆盖空间中所有点

### 实现逻辑

1. **选前两个点**：逻辑较为简单，不做赘述
2. **选剩下的 M - 2 个点**：此时要确定的是集合 A 和 B 之间的距离，最远点采样，根本思想就是**每次要得到的采样点都是原点集 B 到采样点集 A 最远距离的点**。所以接下来的采样，需要遍历原点集 B 中的每一个点，现在任取一个点 B1，计算 B1 到集合 A 的距离为：
    - 分别计算 B1 到集合 A 中每一个点的距离
    - 取最小的距离最为点 B1 到集合 A 的距离

对于集合 B 中的每一个点，都进行一次计算，求得每一个点到集合 A 的最小距离，然后取这些距离中最大的距离，作为当前点集 B 与 采样点集 A 之间的最远距离，最大距离所对应的点，就是 A 在 B 中距离最远的点，也就是这次计算需要采样的点，放入 A 集合中。后续点的计算重复此过程即可。

## MLP

多层感知机

## position embedding

把位置信息映射为位置空间的一个点，也就是一个 vector

## A simple MLP and FodingNet

根据 Transformer 预测的点代理，补全缺失的点云

## FoldingNet

## xyz

点云的坐标系参数

coor：坐标
