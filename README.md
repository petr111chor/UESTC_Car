# git 上传工程流程
在项目文件夹中打开控制台或者git bash，依次运行下列指令：
- git init # 初始化 Git 仓库
- git remote add origin https://github.com/petr111chor/UESTC_Car.git # 连接到远程仓库(前面这两条指令只用执行一次，后面再上传时只需执行add后续指令)
- git add .
- git commit -m "在这里写都有啥改动"  # 提交文件到本地仓库
- git push -u origin master
# git 下载工程流程
git pull origin master

## 更新日志
**建议每天修改代码之后都在这写一下自己干了啥**
### 2025.3.15
创建GitHub项目，将初始版本上传至GitHub，并配置好git
