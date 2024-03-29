# 第3章 字符界面操作基础

## 3.4 使用 bash

### 3.4.1 常用控制组合键

| 控制组合键  | 功能  |
|:--------- |:---- |
| Ctrl + l  | 清屏  |
| Ctrl + o  | 执行当前命令，并选择上一条命令 |
| Ctrl + s  | 阻止屏幕输出 |
| Ctrl + q  | 允许屏幕输出 |
| Ctrl + c  | 终止命令 |
| Ctrl + z  | 挂起命令 |
| Ctrl + m  | 相当于按回车键 |
| Ctrl + d  | 输入结束，即 EOF 的意思，或者注销 Linux 系统 |

### 3.4.2 光标操作

|      组合键      | 功能 |
|:--------------- |:--- |
|    Ctrl + a     | 移动光标到命令行首 |
|    Ctrl + e     | 移动光标到命令行尾 |
|    Ctrl + f     | 按字符前移（向右） |
|    Ctrl + b     | 按字符后移（向左） |
|    Ctrl + xx    | 在命令行首和光标之间移动 |
|    Ctrl + u     | 删除从光标到命令行首的部分，不包含光标处字符 |
|    Ctrl + k     | 删除从光标到命令行尾的部分，包含光标处字符 |
|    Ctrl + w     | 删除从光标到当前单词开头的部分，不包含光标处字符 |
|    Ctrl + d     | 删除光标处的字符 |
|    Ctrl + h     | 删除光标前的一个字符 |
|    Ctrl + y     | 插入最近删除的单词，插入单词位于光标所在位置之前 |
|    Ctrl + t     | 交换光标处字符和光标前面的字符 |
|     Alt + f     | 按单词前移（向右） |
|     Alt + b     | 按单词后移（向左） |
|     Alt + d     | 从光标处删除至单词尾，包含光标处字符 |
|     Alt + c     | 从光标处更改单词为首字母大写，并将光标移至命令行尾 |
|     Alt + u     | 从光标处更改单词为全部大写，并将光标移至命令行尾 |
|     Alt + l     | 从光标处更改单词为全部小写，并将光标移至命令行尾 |
|     Alt + t     | 交换光标处单词和光标前面的单词 |
| Alt + Backspace | 与 Ctrl+w 功能相似，分隔符有些差别 |

> 光标操作组合
>
> 1. `Ctrl + w` 与 `Ctrl + y` 组合可达到 **剪切+粘贴** 的效果

## 3.4.3 特殊字符

| 符号 | 功能 |
|:--- |:---- |
|  ~  | 用户主目录 |
|  `  | 反引号，用来命令代替 |
|  #  | 注释 |
|  $  | 变量取值 |
|  &  | 后台进程工作 |
|  (  | 子 Shell 开始 |
|  )  | 子 Shell 结束 |
|  \  | 使命令持续到下一行 |
|  \| | 管道 |
|  <  | 输入重定向 |
|  >  | 输出重定向 |
|  >> | 追加输出重定向 |
|  '  | 单引号（不具有变数置换功能） |
|  "  | 双引号（具有置换功能） |
|  /  | 路径分隔符 |
|  ;  | 命令分隔符 |

## 3.4.4 通配符

|       符号   | 功能 |
|:------------ |:--- |
|       ?      | 代表任何单一字符 |
|      *       | 代表任何字符 |
|  [ 字符组合 ] | 在中括号中的字符都符合，比如 [a-z] 代表所有的小写字母 |
| [ !字符组合 ] | 不在中括号中的字符都符合，比如 [!1-9]代表非数字的都符合 |

## 3.5 Shell 实用功能

### 3.5.2 命令历史记录

#### 表 3-11 history 命令选项含义

| 选项 | 选项含义 ( n 代表数字 ) |
|:--- |:-------------------- |
|  -c | 清除命令历史记录 |
|  -w | 将当前的历史命令写到 .bash_history 文件，覆盖 .bash_history 文件的内容 |
|  -a | 将目前新增的 history 历史命令写入 .bash_history 文件 |
|   n | 显示最近 n 个命令历史记录 |
|  -r | 读取历史文件的内容，并把它们作为当前历史 |

#### 表 3-12 使用历史命令

|     举例      | 描述 |
|:------------ |:---- |
|  !!          | 运行上一个命令 |
|  !6          | 运行第 6 个命令|
|  !8 /test    | 运行第 8 个命令并在命令后面加上 \test |
|  !?CF?       | 运行上一个包含 CF 字符串的命令 |
|  !ls         | 运行上一个 ls 命令（或以 ls 开头的历史命令）|
|  !ls:s/CF/G  | 运行上一个 ls 命令，其中把 CF 替换成 G |
|  fc          | 编辑并运行上一个历史命令 |
|  fc 6        | 编辑并运行第 6 条历史命令 |
|  ^boot^root^ | 快速替换。将最后一条命令中的 boot 替换为 root 后运行 |
|  !-5         | 运行倒数第 5 个命令 |
|  !$          | 运行前一个命令最后的参数 |

#### 表 3-13 搜索历史命令

| 快捷键或组合键   | 描述 |
|:------------- |:---- |
|   向上方向键    | 查看上一个命令 |
|   向下方向键    | 查看下一个命令 |
|   Ctrl + p    | 查看历史列表中的上一个命令 |
|   Ctrl + n    | 查看历史列表中的下一条命令 |
|   Ctrl + r    | 向上搜索历史命令 |
|   Alt + p     | 向上搜索历史命令 |
|   Alt + >     | 移动到历史列表末尾 |

### 3.5.3 命令排列

1. 使用 ";" 命令时，先执行命令 1，不管命令 1 是否出错，接下来就执行命令 2
    > 命令语法：
    >> 命令 1 ; 命令 2
2. 使用 "&&" 命令时，只有当命令 1 正确运行完毕后，才能执行命令 2
    > 命令语法：
    >> 命令 1 && 命令 2

### 3.5.6 文件名匹配

1. 通配符 "\*"，"\*" 可匹配一个或多个字符。
2. 通配符 "?", "?" 只能匹配一个字符。

### 3.5.7 管道

1. Linux 系统的理念是是汇集许多小程序，每个程序都有特殊的专长。复杂的任务不是由大型软件完成的，而是运用 shell 的机制，组合许多小程序共同完成的。管道在其中发挥着重要的作用，它可以将某个命令的输出信息当作某个命令的输入，由管道符号 "|" 来标识。

    ```bash
    命令语法 :
    >> [ 命令 1] | [命令 2] | [命令 3]
    ```

## 3.7 vi 编译器

### 3.7.2 进入插入模式

| 命令 | 功能 |
|:--- |:---- |
|  i  | 从光标当前所在位置之前开始插入 |
|  a  | 从光标当前所在位置之后开始插入 |
|  I  | 在光标所在行的行首插入 |
|  A  | 在光标所在行的行末尾插入 |
|  o  | 在光标所在行的下面新开一行插入 |
|  O  | 在光标所在行的上面新开一行插入 |
|  s  | 删除光标位置的一个字符，然后进入插入模式 |
|  S  | 删除光标所在的行，然后进入插入模式 |

### 3.7.3 光标移动

|    命令  | 功能 ( n 代表数字 ) |
|:-------- |:---------------- |
|  向上箭头 | 使光标向上移动一行 |
|  向下箭头 | 使光标向下移动一行 |
|  向左箭头 | 使光标向左移动一个字符 |
|  向右箭头 | 使光标向右移动一个字符 |
|    k     | 使光标向上移动一行 |
|    j     | 使光标向下移动一行 |
|    h     | 使光标向左移动一个字符 |
|    l     | 使光标向右移动一个字符 |
|    nk    | 使光标向上移动 n 行 |
|    nj    | 使光标向下移动 n 行 |
|    nh    | 使光标向左移动 n 个字符 |
|    nl    | 使光标向右移动 n 个字符 |
|    H     | 使光标移动到屏幕的顶部 |
|    M     | 使光标移动到屏幕的之间 |
|    L     | 使光标移动到屏幕的底部 |
| Ctrl + b | 使光标往上移动一页屏幕 |
| Ctrl + f | 使光标往下移动一页屏幕 |
| Ctrl + u | 使光标往上移动半页屏幕 |
| Ctrl + d | 使光标往下移动半页屏幕 |
| Ctrl + O | 使光标回到上次编辑的位置 |
|    0     | 使光标移动到所在行的行首 |
|    $     | 使光标移动到光标所在行的行尾 |
|    ^     | 使光标移动到光标所在行的行首 |
|    w     | 使光标跳到下一个字的开头 |
|    W     | 使光标跳到下一个字的开头，但会忽略一些标点符号 |
|    e     | 使光标跳到下一个字的字尾 |
|    E     | 使光标跳到下一个字的字尾，但会忽略一些标点符号 |
|    b     | 使光标回到上一个字的开头 |
|    B     | 使光标回到上一个字的开头，但会忽略一些标点符号 |
|    (     | 使光标移动到上一个句首 |
|    )     | 使光标移动到下一个句首 |
|    {     | 使光标移动到上一个段落首 |
|    }     | 使光标移动到下一个段落首 |
|    G     | 使光标移动到文件尾（最后一行的第一个非空白字符处）|
|    gg    | 使光标移动到文件头（第一个第一个非空白字符处）|
|  space   | 使光标向右移动一个字符 |
| Backspace | 使光标向左移动一个字符 |
|  Enter   | 使光标向下移动一行 |
| Ctrl + p | 使光标向上移动一行 |
| Ctrl + n | 使光标向下移动一行 |
|    n\|   | 使光标移动到第 n 个字符处 |
|    nG    | 使光标移动到第 n 行行首 |
|    n+    | 使光标向下移动 n 行 |
|    n-    | 使光标向上移动 n 行 |
|    n$    | 使光标向下移动到以当前行算起的第 n 行尾 |

### 3.7.4 命令模式操作

|   类型    | 命令      | 功能 (n 代表数字 ) |
|:-------- |:-------- |:----------------- |
| 删除      |  x       | 删除光标所在位置的字符 |
| 删除      |  X       | 删除光标所在位置的前面一个字符 |
| 删除      |  nx      | 删除光标所在位置开始的 n 个字符 |
| 删除      |  nX      | 删除光标所在位置前面 n 个字符 |
| 删除      |  dd      | 删除光标所在行 |
| 删除      | ndd      | 从光标所在行开始删除 n 行 |
| 删除      |  db      | 删除光标所在位置的前面一个单词 |
| 删除      | ndb      | 删除光标所在位置的前面 n 个单词 |
| 删除      |  dw      | 从光标所在位置开始删除一个单词 |
| 删除      | ndw      | 从光标所在位置开始删除 n 个单词 |
| 删除      |  d$      | 删除光标到行尾的内容，包含光标所在处字符 |
| 删除      |  D       | 删除光标到行尾的内容，包含光标所在处字符 |
| 删除      |  dG      | 从光标位置所在行一直删除到文件尾 |
| 复制和粘贴 |  yw      | 复制光标所在位置到单词尾的字符 |
| 复制和粘贴 | nyw      | 复制光标所在位置开始的 n 个单词 |
| 复制和粘贴 |  yy      | 复制光标所在行 |
| 复制和粘贴 | nyy      | 复制从光标所在位置开始的 n 行 |
| 复制和粘贴 |  y$      | 复制光标所在位置到行尾内容或缓存区 |
| 复制和粘贴 |  y^      | 复制光标前面所在位置到行首内容或缓存区 |
| 复制和粘贴 |  YY      | 将当前行复制到缓冲区 |
| 复制和粘贴 | nYY      | 将当前开始的 n 行复制到缓冲区 |
| 复制和粘贴 |  p       | 将缓冲区内的内容写到光标所在的位置 |
| 替换      |  r       | 替换光标所在处的字符，按 r 键之后输入要替换的字符 |
| 替换      |  R       | 替换光标所到之处的字符，直到按下 esc 键为止，按 R 键之后输入要替换的字符 |
| 撤销和重复 |  u       | 撤销上一个操作。按多次 u 可以执行多次撤销 |
| 撤销和重复 |  U       | 取消所有操作 |
| 撤销和重复 |  .       | 再执行一次前面刚完成的操作 |
| 列出行号   | Ctrl + g | 列出光标所在行的行号 |
| 保存和退出 | ZZ       | 保存退出 |
| 保存和退出 | ZQ       | 不保存退出 |
| 查找字符   | /关键字  | 先按 / 键，再输入想查找的字符，按 n 键往下查找下一个关键字，按 N 键往上查找上一个关键字 |
| 查找字符   | ?关键字  | 先按 ? 键，再输入想查找的字符，按 n 键往下查找下一个关键字，按 N 键往上查找上一个关键字 |
| 合并      |  nJ      | 将当前行开始的 n 行进行合并 |
| 合并      |   J      | 清除光标所在行与下一行之间的换行，行尾没有空格的话会自动添加一个空格 |

### 3.7.5 末行模式操作

| 类型           | 命令 | 功能 (command 代表命令; stri 代表字符串; ni 代表数字)|
|:------------- |:---- |:---------------------------------------------- |
| 运行 Shell 命令 | :!command                | 运行 Shell 命令 |
| 运行 Shell 命令 | :r!command               | 将命令运行的结果信息输入到当前行的位置 |
| 运行 Shell 命令 | :n1,n2 w !command        | 将 n1 到 n2 行的内容作为命令的输入 |
| 查找字符        | :/str/                   | 从当前光标开始往右移动到有 str 的地方 |
| 查找字符        | :?str?                   | 从当前光标开始往左移动到有 str 的地方 |
| 替换字符        | : s/str1/str2/           | 将光标所在行第一个字符 str1 替换为 str2 |
| 替换字符        | : s/str1/str2/g          | 将光标所在行所有的字符 str1 替换为 str2 |
| 替换字符        | :n1,n2s/str1/str2/g      | 用 str2 替换从 n1 行到 n2 行中出现的 str1 |
| 替换字符        | :%s/str1/str2/g          | 用 str2 替换文件中所有的 str1 |
| 替换字符        | :.,$s/str1/str2/g        | 将从当前位置到结尾的所有的 str1 替换为 str2 |
| 保存和退出      | :w                       | 保存文件 |
| 保存和退出      | :w filename              | 将文件另存为 filename |
| 保存和退出      | :wq                      | 保存文件并退出 vi 编辑器 |
| 保存和退出      | :wq filename             | 将文件另存为 filename 后退出 vi 编辑器 |
| 保存和退出      | :wq!                     | 保存文件并强制退出 vi 编辑器 |
| 保存和退出      | :wq! filename            | 将文件另存为 filename 后强制退出 vi 编辑器 |
| 保存和退出      | :x                       | 保存文件并强制退出 vi 编辑器，其功能与 :wq! 相同 |
| 保存和退出      | :q                       | 退出 vi 编辑器 |
| 保存和退出      | :q!                      | 如果无法离开 vi，强制退出 vi 编辑器 |
| 保存和退出      | :n1,n2w filename         | 将从 n1 行开始到 n2 行结束的内容保存到文件 filename 中 |
| 保存和退出      | :nw filename             | 将第 n 行内容保存到文件 filename 中 |
| 保存和退出      | :1,.w filename           | 将从第一行开始到光标当前位置的所有内容保存到文件 filename 中 |
| 保存和退出      | :.,$w filename           | 将从光标当前位置开始到文件末尾的所有内容保存到文件 filename 中 |
| 保存和退出      | :r filename              | 打开另外一个已经存在的文件 filename |
| 保存和退出      | :e filename              | 新建名为 filename 的文件 |
| 保存和退出      | :f filename              | 把当前文件名改为 filename 文件 |
| 保存和退出      | :/str/w filename         | 将包含有 str 的行写到文件 filename 中 |
| 保存和退出      | :/str1/,/str2/w filename | 将从包含有 str1 开始到 str2 结束的行内容写入到文件 filename 中 |
| 删除           | :d                       | 删除当前行 |
| 删除           | :nd                      | 删除第 n 行 |
| 删除           | :n1,n2 d                 | 删除从 n1 行开始到 n2行为止的所有内容 |
| 删除           | :.,$d                    | 删除从当前行开始到文件末尾的所有内容 |
| 删除           | :/str1/ , /str2/d        | 删除从 str1 开始到 str2 为止的所在行的所有内容 |
| 复制和移动     | :n1,n2 co n3              | 将从 n1 行开始到 n2 行为止的所有内容复制到 n3 行后面 |
| 复制和移动     | :n1,n2 m n3               | 将从 n1 行开始到 n2 行为止的所有内容移动到 n3 行后面 |
| 跳到某一行     | :n                        | 在冒号后输入一个数字，再按回车键就会跳到改行 |
| 设置 vi 环境   | :set number               | 在文件中的每一行前面列出行号 |
| 设置 vi 环境   | :set nonumber             | 取消在文件中的每一行前面列出行号 |
| 设置 vi 环境   | :set readonly             | 设置文件为只读状态 |
