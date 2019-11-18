#!/bin/bash

# 配置文件路径
config_file="/usr/local/etc/ssh_ip.conf"
# 拷贝文件到target_dir
target_dir="~/cx"

# 读取配置文件
function read_config_file()
{
    section=$1
    key=$2
    value=`awk -F '=' '/\['$section'\]/{a=1}a==1&&$1~/'$key'/{print $2;exit}' $config_file`
    echo "$value"
}

# 打印IP地址列表
function print_ip_address()
{
    item_count=100
    echo "IP地址列表:"
    for ((i=1; i<=${item_count}; i++))
    do
        section="item""$i"
        ip=`read_config_file "$section" "ip"`
        if [[ ${ip} != "" ]]; then
            echo "[""$i""]:"${ip}
        fi
    done
}

# 选择IP地址
function choice_ip_address()
{
    read -p "上传到:" input
    echo ${input}
}

# 拷贝文件
function copy_file()
{
    section="item""$ip_index"
    ip=`read_config_file "$section" "ip"`
    port=`read_config_file "$section" "port"`
    user_name=`read_config_file "$section" "user_name"`

    if [[ ${ip} != "" && ${port} != "" && ${user_name} != "" ]]; then
        scp -P $port -r $file_names "$user_name"@"$ip":$target_dir
        echo "$file_names""已经上传到""$target_dir"
    else
        echo "无效的IP地址或用户名！"
    fi
}

# 脚本启动点
if [ $# -lt 1 ]; then
    echo "请输入参数！"
    exit 1
fi

file_names=""
for arg in $@
do
    file_names="$file_names"" ""$arg"
done

print_ip_address
ip_index=`choice_ip_address`

copy_file "$ip_index" "$file_names"
