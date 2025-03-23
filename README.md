# git 上传工程流程
在项目文件夹中打开控制台或者git bash，依次运行下列指令：
- git init # 初始化 Git 仓库
- git remote add origin https://github.com/petr111chor/UESTC_Car.git # 连接到远程仓库(前面这两条指令只用执行一次，后面再上传时只需执行add后续指令)
- git checkout -b **分支名** #创建一个名为**分支名**的分支
- git add . # 将文件添加至当前分支中
- git commit -m "在这里写都有啥改动"  # 提交文件到本地仓库
- git push -u origin master # 将文件提交至名为master的分支中
# git 下载工程流程
git pull origin master # 拉取远程仓库中名为master的分支并与本地分支合并

## 更新日志
**建议每天修改代码之后都在这写一下自己干了啥**
### 2025.3.15
创建GitHub项目，将初始版本上传至GitHub，并配置好git
### 2025.3.16
完成中线误差调试，图像处理正常，等待硬件完成可下地调试
### 2025.3.23
添加显示边线与中线函数，可以在显示屏上显示边线与中线，同时打印中线误差，以便调试。无法巡线感觉是摄像头位置的问题，学长说我们的图像处理是正常的，可能是电机参数的问题。