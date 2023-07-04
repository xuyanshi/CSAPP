#!/bin/bash
# 将此脚本置于/fslab-handout文件夹下

# 初始化得分
score=0
errors=""

# 进行16次测试
for ((i=0; i<=15; i++))
do
    echo "Running Test $i"

    # 挂载fs，通常会显示Invalid argument
    # 因为在循环结束执行make umount会解挂
    # 在make mount命令中会先执行一次解挂，此次解挂就会失败
    # 不能解挂没有挂载的fs
    echo "Running 'make mount'"
    make mount

    # 开始测试
    script="./traces/$i.sh"
    echo "Running '$script'"
    output=$(./$script)

    # 比对输出
    expected_output=$(< "./traces/$i.ans")
    if [ "$output" == "$expected_output" ]; then
        echo "Test $i passed"
        score=$((score + 1))
    else
        echo "Test $i failed"
        errors+=" $i"
    fi

    # 解挂fs
    echo "Running 'make umount'"
    make umount

    echo "Test $i completed"
    echo
done

# 输出分数和错误编号
echo "Final Score: $score / 16"
echo "Errors: $errors"