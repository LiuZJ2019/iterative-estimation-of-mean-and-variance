[toc]



## 概述

用于计算一个序列的均值、方差、最小值、最大值，采用迭代的方式计算，**因此计算开销很小**。本项目在 `i7-11800H@2.30GHz` 的CPU下以Release模式测试，迭代计算了10^8^次，用时为 `0.28-0.29s` ，即每次运算开销约为 `3ns` 。

事实上，我们的算法是被高度优化的，因此才能做到如此小的开销。本文档末尾将给出**算法可行性**的证明，或者请看我的个人博客链接[https://blog.csdn.net/MamiyaHasaki/article/details/124789567](https://blog.csdn.net/MamiyaHasaki/article/details/124789567)



## 使用方法概述

本项目的代码实现均放在头文件中，故不需要链接源文件，**直接将头文件复制到您的工程项目即可使用**，注意本项目代码被包含在 `namespace Hasaki` 中。

**推荐使用** `seq_estimator.h` 而不是 `seq_estimator_optimized.h` ，因为后者通常不会带来很大的性能提升，然而后者使用了 `if constexpr` 语句，这要求编译器版本 `>=c++17` 。同时后者采用了模板，代码可读性较差，是我的练手之作。

### Hasaki::SeqEstimator

只需要 `#include "seq_estimator.h"` 即可使用，注意本项目代码被包含在 `namespace Hasaki` 中。

下面给出一个最小使用样例——

```c++
#include "seq_estimator.h"

#include <iostream>

int main() {
    Hasaki::SeqEstimator seqEst;
    std::cout << seqEst << std::endl;
    seqEst.Append (1.145);
    std::cout << seqEst << std::endl;
    seqEst.Append (1.4);
    std::cout << seqEst << std::endl;
    seqEst.Append (1.919);
    std::cout << seqEst << std::endl;
    seqEst.Append (0.8);
    std::cout << seqEst << std::endl;
    seqEst.Append (1.0);
    std::cout << seqEst << std::endl;

    return 0;
}
```

期望输出为——

```c++
(n = 0, avg = nan, var = nan, min = nan, max = nan)
(n = 1, avg = 1.145, var = nan, min = 1.145, max = 1.145)
(n = 2, avg = 1.2725, var = 0.0325125, min = 1.145, max = 1.4)
(n = 3, avg = 1.488, var = 0.155577, min = 1.145, max = 1.919)
(n = 4, avg = 1.316, var = 0.222054, min = 0.8, max = 1.919)
(n = 5, avg = 1.2528, var = 0.186512, min = 0.8, max = 1.919)
```

其中 `Hasaki::SeqEstimator seqEst` 是构造序列估计器，可以采用默认构造，也可在传入一个初始值，例如 `SeqEstimator seqEst(1.23)` 。

显然在没有数据（ `n=0` ）时，序列估计器的值是无意义的，此时用 `Get` 方法将会得到 `nan` 。在只有一个数据（ `n=1` ）时，方差是无意义的。在数据量大于等于2时（ `n>=2` ），所有数据均有意义。



### Hasaki::SeqEstimatorOptimized

这个模板类理论上是关于 `SeqEstimator` 的优化版本，如果只需要求解部分参量的估计（如只需要均值、最小值、最大值，则使用 `Hasaki::SeqEstimatorOptimized<SeqType::MEAN | SeqType::MIN | SeqType::MAX>` ），并采用了 `if constexpr` 的语法进行了编译期优化（C++17）。

但实际上，根据实际测试的结果，计算 `MIN/MAX` 的开销是非常小的，可以忽略不计，计算方差的开销同样不高。因此，如果你至少需要估计均值，那么**优化版本和普通版本在时间复杂度上并不存在显著的差异**，我推荐使用非优化版本 `Hasaki::SeqEstimator` 。

下面给出一个最小使用样例——

```c++
#include "seq_estimator_optimized.h"

#include <iostream>
using Hasaki::SeqType;

int main() {
    Hasaki::SeqEstimatorOptimized<SeqType::MEAN | SeqType::VAR | SeqType::MIN | SeqType::MAX> seqEst;
    std::cout << seqEst << std::endl;
    seqEst.Append (1.145);
    std::cout << seqEst << std::endl;
    seqEst.Append (1.4);
    std::cout << seqEst << std::endl;
    seqEst.Append (1.919);
    std::cout << seqEst << std::endl;
    seqEst.Append (0.8);
    std::cout << seqEst << std::endl;
    seqEst.Append (1.0);
    std::cout << seqEst << std::endl;

    return 0;
}
```



### main

 `main.cpp` 测试了整个工程项目的代码开销，对于10^8^个数字进行了迭代估计。结果如下——

-  `res1` ： `Hasaki::SeqEstimator` 
-  `res2` ： `Hasaki::SeqEstimatorOptimized<SeqType::MEAN | SeqType::VAR | SeqType::MIN | SeqType::MAX>` 
-  `res3` ： `Hasaki::SeqEstimatorOptimized<SeqType::MEAN>` 
-  `res4` ： `Hasaki::SeqEstimatorOptimized<SeqType::MAX>` 

```c++
res1 = (n = 1e+08, avg = 4.99989, var = 3.99983, min = -5.89269, max = 16.1132)
cost 0.286774 second
res2 = (n = 1e+08, avg = 4.99989, var = 3.99983, min = -5.89269, max = 16.1132)
cost 0.287764 second
res3 = (n = 1e+08, avg = 4.99989)
cost 0.274496 second
res4 = (n = 1e+08, max = 16.1132)
cost 0.093221 second
```

不难发现， `Hasaki::SeqEstimator` 和 `Hasaki::SeqEstimatorOptimized<SeqType::MEAN | SeqType::VAR | SeqType::MIN | SeqType::MAX>` 开销差不多。 `Hasaki::SeqEstimatorOptimized<SeqType::MEAN>` 开销略低，但可以忽略不计。只有 `Hasaki::SeqEstimatorOptimized<SeqType::MAX>` 开销才会显著降低，但这种场景过于平凡，不必使用我们的库函数。



## 可能的应用场景

### 1.网络流

在网络流中，每一个数据包对应了一个RTT，我们希望测量得RTT的均值、方差、最小值、最大值，同时不希望整个测量过程引入额外的时间复杂度、空间复杂度，否则会影响路由协议的性能。

在这种场景下，可以使用我们的 `Hasaki::SeqEstimator` ，它几乎不会引入额外的开销（ `O(1)` 的时空复杂度，一次迭代只需 `3ns` ）。



### 2.预测算法

在许多序列预测问题中，需要对某些参量进行迭代估计。此时，采用本算法，即可简单高效地实现对参量的迭代估计。



### 3.直接计算均值、方差

当然，这个代码和直接计算均值和方差的开销是很接近的，根据测试，下面这段代码的开销约为 `0.18-0.19s` （迭代计算的开销约为 `0.28-0.29s` ）。但后者将面临精度问题（大数吃小数），均值和方差的误差约为 `1e-16` 和 `1e-12` 。

```c++
double mean5 = 0;
double var5 = 0;
for (const auto num: randomNums)
    mean5 += num;
mean5 /= 1e8;
for (const auto num: randomNums)
    var5 += (num - mean5) * (num - mean5);
var5 /=(1e8 - 1);
```



## 理论证明

对于序列 $x_1,x_2,\cdots,x_n$ ，其样本均值、样本方差分别为：
$$
\hat{\mu}_n=\frac{1}{n}\sum_{k=1}^n{x_k} \tag{1}
$$

$$
\hat{\sigma}_{n}^{2}=\frac{1}{n-1}\sum_{k=1}^{n}{\left( x_k-\hat{\mu}_{n} \right) ^2} \tag{2}
$$

不妨令 $\beta_n=1/n$ ，则样本均值递推式为：
$$
\begin{aligned}
	\hat{\mu}_{n+1}&=\frac{1}{n+1}\sum_{k=1}^{n+1}{x_k}\\
	&=\frac{n}{n+1}\left(\frac{1}{n}\sum_{k=1}^{n}{x_k}\right)+\frac{1}{n+1}x_{n+1}\\
	&=\left( 1-\beta _{n+1} \right) \hat{\mu}_n+\beta _{n+1}x_{n+1}\\
	&=\hat{\mu}_n+\beta _{n+1}\left( x_{n+1}-\hat{\mu}_n \right)\\
\end{aligned} \tag{3}
$$
为了求解样本方差递推式（事实上，这个变形是为了提高计算机运算效率，这是为了复用表达式 $x_{n+1}-\hat{\mu}_n$ ），先证明一个恒等式：
$$
\begin{aligned}
	x_{n+1}-\hat{\mu}_{n+1}&=\left( x_{n+1}-\hat{\mu}_n \right) +\left( \hat{\mu}_n-\hat{\mu}_{n+1} \right)\\
	&=\left( x_{n+1}-\hat{\mu}_n \right) -\beta _{n+1}\left( x_{n+1}-\hat{\mu}_n \right)\\
	&=\left( 1-\beta _{n+1} \right) \left( x_{n+1}-\hat{\mu}_n \right)\\
\end{aligned} \tag{4}
$$

> 注：其中第二行等号用到了公式(3)的结论

自然得到：
$$
\begin{aligned}
	\beta _n\left( x_{n+1}-\hat{\mu}_{n+1} \right) ^2&=\beta _n\left( 1-\beta _{n+1} \right) ^2\left( x_{n+1}-\hat{\mu}_n \right) ^2\\
	&=\frac{1}{n}\frac{n}{n+1}\frac{n}{n+1}\left( x_{n+1}-\hat{\mu}_n \right) ^2\\
	&=\beta _{n+1}\left( 1-\beta _{n+1} \right) \left( x_{n+1}-\hat{\mu}_n \right) ^2\\
	&=\left[ \beta _{n+1}-\left( \beta _{n+1} \right) ^2 \right] \left( x_{n+1}-\hat{\mu}_n \right) ^2\\
\end{aligned} \tag{5}
$$
从而：
$$
\begin{aligned}
	\hat{\sigma}_{n+1}^{2}&=\frac{1}{n}\sum_{k=1}^{n+1}{\left( x_k-\hat{\mu}_{n+1} \right) ^2}\\
	&=\beta _n\left( x_{n+1}-\hat{\mu}_{n+1} \right) ^2+\left( 1-\beta _n \right) \frac{1}{n-1}\sum_{k=1}^n{\left[ \left( x_k-\hat{\mu}_n \right) +\left( \hat{\mu}_n-\hat{\mu}_{n+1} \right) \right] ^2}\\
	&=\beta _n\left( x_{n+1}-\hat{\mu}_{n+1} \right) ^2+\left( 1-\beta _n \right) \frac{1}{n-1}\sum_{k=1}^n{\left( x_k-\hat{\mu}_n \right) ^2}+\left( \hat{\mu}_n-\hat{\mu}_{n+1} \right) ^2\\
	&=\left[ \beta _{n+1}-\left( \beta _{n+1} \right) ^2 \right] \left( x_{n+1}-\hat{\mu}_n \right) ^2+\left( 1-\beta _n \right) \hat{\sigma}_{n}^{2}+\left( \beta _{n+1} \right) ^2\left( x_{n+1}-\hat{\mu}_n \right) ^2\\
	&=\left( 1-\beta _n \right) \hat{\sigma}_{n}^{2}+\beta _{n+1}\left( x_{n+1}-\hat{\mu}_n \right) ^2\\
\end{aligned} \tag{6}
$$

> 注：其中第四行等号用到了公式(3)(5)的结论

上式的推导第三行的等号是因为完全平方展开的交叉项为 $0$ ，具体推导可见式 $\left(7\right)$ 。
$$
\begin{aligned}
	\sum_{k=1}^n{\left( x_k-\hat{\mu}_n \right) \left( \hat{\mu}_n-\hat{\mu}_{n+1} \right)}&=\left( \hat{\mu}_n-\hat{\mu}_{n+1} \right) \sum_{k=1}^n{\left( x_k-\hat{\mu}_n \right)}\\
	&=\left( \hat{\mu}_n-\hat{\mu}_{n+1} \right) \left( \sum_{k=1}^n{x_k}-\sum_{k=1}^n{x_k} \right)\\
	&=0\\
\end{aligned}\tag{7}
$$
最终得到递推关系式（化简成这样为了保留 $\left( x_{n+1}-\hat{\mu}_n \right)$ 公共项，减少计算量）
$$
\begin{cases}
	\hat{\mu}_{n+1}=\hat{\mu}_n+\beta _{n+1}\left( x_{n+1}-\hat{\mu}_n \right)\\
	\hat{\sigma}_{n+1}^{2}=\left( 1-\beta _n \right) \hat{\sigma}_{n}^{2}+\beta _{n+1}\left( x_{n+1}-\hat{\mu}_n \right) ^2\\
\end{cases}\tag{8}
$$


### 要点阐释

- $n=0$ 时， $1-\beta_0$ 是 `nan` （分母为0），但由于此时方差无意义，所以令 $1-\beta_0=0$ 即可。
  - 不要使用 `if(n==0){n=1;return;}` 来避免这一情况，实测这样性能几乎折半。
- $n=1$ 时， $\sigma_1^2$ 算出来不是 $0$ ，但显然 $\sigma_1^2$ 也无意义。
- $n=2$ 时， $\sigma_2^2=0\times\sigma_1^2+\left( x_{2}-\hat{\mu}_1 \right) ^2/2$ 算出来不是 $0$ ，因此 $\sigma_1^2$ 不会影响到 $\sigma_2^2$ 。
- 那么自然地，后面的公式均正确。







