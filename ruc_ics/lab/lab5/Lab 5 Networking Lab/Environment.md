# 准备工作：实验环境配置

为了方便同学们配置实验环境，也为了方便进行作业的自动化验收评分，所有的实验作业都将在提供给大家的统一的虚拟机环境中进行。本文件将指导你如何安装相关的程序，建立虚拟机。

## 安装 Vagrant

Vagrant 是一个用来创建和部署虚拟开发环境的工具，可以根据 Vagrantfile 配置文件中的指令内容自动完成虚拟机的创建和配置。

* Windows & macOS & Linux：使用[此链接](https://www.vagrantup.com/downloads)下载和安装适配你的计算机的 Vagrant

  > **Windows**: 安装结束时将要求重新启动计算机，单击Yes立即执行此操作，或稍后手动重新启动，否则Vagrant将无法工作！

## 安装 VirtualBox

VirtualBox 是一个开源的虚拟机软件，并且已经支持了 M1/M2 芯片的 macOS 系统。

* Windows & macOS & Linux：使用[此链接](https://www.virtualbox.org/wiki/Downloads)下载或根据网页指示通过包管理工具安装适配你的计算机的 VirtualBox

## 使用 Vagrant 配置 VirtualBox 虚拟机

1. 下载老师提供的课程资料：NetRUC 文件夹
2. 在 NetRUC 文件夹路径下打开一个终端，用于给计算机输入命令（以 Windows10 系统为例：按住 Shift 键，在文件夹中右击，选择在此处打开 PowerShell）
3. 在终端中输入命令 ``` vagrant up```。之后 Vagrant 会根据当前路径下 Vagrantfile 中的内容自动创建 VirtualBox 虚拟机，完成所有的配置。

## 登录/登出虚拟机

* ```vagrant up``` 会启动虚拟机，之后可以通过 ```vagrant ssh``` 登录到虚拟机中。用户名和密码都是 vagrant 。如果成功登录，你的终端提示符会更改为 ```vagrant@netruc:/vagrant$```，此时你已经登入虚拟机（guest）中了，之后你运行的所有命令将在虚拟机中运行，而不是你的主机（host）。
* 使用 ```logout``` 命令来断开与虚拟机的连接，此时终端的提示符将变回你的主机，此时你在终端中输入的命令将在主机中执行。
* ```vagrant suspend``` 将保存虚拟机的状态并停止它。
* ```vagrant halt``` 将正常关闭虚拟机。
* ```vagrant destroy``` 将从您的系统中删除虚拟机的所有痕迹。 如果你在虚拟机上保存了重要文件（例如你的作业），请不要使用此命令。
* 此外，命令 ```vagrant status``` 将允许你检查机器的状态以防您不确定（例如，正在运行、关闭电源、已保存...）。 
* 你必须在包含 Vagrantfile 的目录的某个子目录中才能使用上述任何命令，否则 Vagrant 将不知道你指的是哪个虚拟机。

## 一些额外说明

* 当你通过 ```vagrant ssh``` 登录到虚拟机中时，你会被自动定位到 /vagrant 目录下，该目录已经做了虚拟机与主机之间的文件共享，主机中的 NetRUC 文件夹下的任何改动都会同步到虚拟机中的 /vagrant 目录。这非常方便，你可以在虚拟机的外部，也就是你的主机上使用任何你喜欢的编辑器对 NetRUC 目录下的代码文件进行编写，但所有的代码文件仍然需要在虚拟机内编译和运行。
* 在 DOS (Windows) 和 Unix (Linux/MacOS) 中，行尾的符号不同。在前者中，它们由回车和换行符（CRLF，或“\r\n”）表示，而在后者中，仅由换行符（LF，或“\n”）表示。Vagrantfile 已经设置为自动将所有文件转换回 Unix 格式，因此你不必担心这一点。但是，如果你想编写/编辑 shell 脚本（.sh文件）来帮助自己进行测试，或者如果你在使用其他类型的文件时遇到此问题，请使用预安装的程序 dos2unix。运行 dos2unix [file] 将其转换为 Unix 格式（在 VM 中编辑/运行之前），并运行 unix2dos [file] 将其转换为 DOS 格式（在 Windows 上编辑之前）。请记住，只有在你想编辑 shell 脚本时才需要这样做。
* 如果你在运行 ```vagrant up``` 初始化虚拟机时产生了错误，那大概率是遇到了网络问题导致某些软件无法正常安装，你可以尝试
  * 彻底删除当前的虚拟机，更换一个网络环境再次 ```vagrant up```。
  * 你可以尝试阅读 Vagrantfile 里的内容，找到安装相关软件的具体命令，在虚拟机中手动安装相应的软件。（推荐）

