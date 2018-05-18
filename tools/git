#Author: persist
#Abstract: how to use git
#Note: Use Chinese instead of English for clear description 

# Git

	Git的工作原理,主要是三个概念:
> * 工作目录:	当前实际文件构成
> * 缓冲区:	记录add等命令追加的状态,为commit做准备
> * 提交历史:	commit最终提交的记录

## 基本

### 克隆

	克隆远程仓库到本地

``` bash
$ git clone Your_Repository_URL
```

### 查看

	查看当前状态,建议在更新和提交前查看,以进行确认
``` bash
$ git status
```

### 更新

	将你的文件加入缓冲区
``` bash
# git add Your_Files
```
	将你的文件从缓冲去删除
``` bash
$ git rm --cached Your_Files
```

### 提交

	将缓冲区进行本地提交
``` bash
$ git commit --author="Your Name <Your_Email>" -m "Your Message"
```

### 回滚

	回滚到commit_id, reset之后可以加入--soft, --mixed, --hard选项, 默认--mixed
> * --soft 	缓冲区和工作目录都不会改变
> * --mixed 	缓冲区和commit_id一致, 工作目录不会改变
> * --hard 	缓冲区和工作目录都会改变

``` bash
$ git reset commit_id
```

### 推送

	将本地当前分支推送到远端, 不加:Remote_Branch默认推送到远程存在追踪关系的分支(通常是同名的); 不加Local_Branch, 只有:Remote_Branch表示推送一个空的分支到远程主机,即等同于删除远程分支; 不加Local_Branch和:Remote_Branch则推送当前分支到与之存在追踪关系的分支
``` bash
$ git push Remote_Host Local_Branch:Remote_Branch
```

## 分支

### 查看

	查看本地分支, 加-r查看远程分支, 加-a查看所有分支
``` bash
$ git branch 
```

### 新建

	新建一个本地分支,不加后一个参数默认从当前分支新建?
``` bash
$ git checkout -b new_branch start_point
```

### 切换

	切换到另一个分支
``` bash
$ git checkout target_branch
```

### 删除
``` bash
$ git branch -d branch_name
```

### 取回

	取回远程主机最新分支,不指定分支默认取回所有分支
``` bash
$ git fetch Remote_Host Remote_Branch
```

### 合并

	将另一个分支合并到当前分支
``` bash
$ git merge another_branch
```

	当然,可以通过pull一步完成取回与合并, 不加Local_Branch默认与当前分支合并
``` bash
$ git pull Remote_Host Remote_Branch:Local_Branch
```
