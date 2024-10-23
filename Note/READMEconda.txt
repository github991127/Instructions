⚫●◽★
——————————————————————

conda activate py307
⚫env
●C
conda create -n [name] python=3.7
●R查看当前环境的信息
conda info
●R查看已经创建的所有虚拟环境
conda info -e
-
base
jimboPySide2             C:\Users\wenjiabao\AppData\Local\anaconda3\envs\jimboPySide2
wenjiaba_py307  
●切换到xx虚拟环境
conda activate [name]
●U
conda create -n torch --clone py3  	# 将 py3 重命名为 torch
●D
conda remove -n env_name --all
conda env remove -n env_name
-
conda create -n py307 python=3.7
conda activate base

conda remove -n py307 --all
conda env remove -n wenjiaba_py307


⚫库
●C
conda install [pyside2]
pip install [pyside2]
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple [pyside2]
●R
conda list
●U
conda update gatk
conda update --all   	# 升级全部库
●D
conda remove --name env_name gatk
-
pip install pyside2
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple pyside2
⚫镜像源
●C增
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud//pytorch/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/conda-forge/
conda config --set show_channel_urls yes
●R查
conda config --get channels
●D删
conda config --remove-key channels
-
