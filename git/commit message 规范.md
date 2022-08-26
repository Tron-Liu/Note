# commit message 规范 （Angular规范）

[来源](https://www.cnblogs.com/daysme/p/7722474.html)

## commit message 的格式

每次提交，commit message都包括三个部分：Header，Body和Footer

```text
<type>(<scope>):<subject>
// 空一行
<body>
// 空一行
<footer>
```

其中，Header是必需的，Body和Footer可以省略。
不管是哪一个部分，任何一行都不得超过72个字符（或100个字符）。这是为了避免自动换行影响美观

## Header

Header部分只有一行，包括三个字段：`type`(必需)、`scope`(可选)和`subject`(必需)

### type

`type` 用于说明commit的类别，只允许使用下面7个标识。

* feat：新功能（feature）
* fix：修补bug
* docs：文档（documentation）
* style：格式（不影响代码运行的变动）
* refactor：重构（既不是新增功能，也不是修改bug的代码变动）
* test：增加测试
* chore：构建过程或辅助工具的变动

如果type为feat或fix，则该commit将肯定出现在Change log中。其他情况（docs、chore、style、refactor、test）由你决定。
