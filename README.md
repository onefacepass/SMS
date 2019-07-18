# SMS
Manage System of Student

## 开发环境

opencv 346
Qt 5.13.0 (msvc 2017 64bit)

## 工程导入

1. 导入前请确保 `$OPENCV` `$ARCFACE` 两个环境变量指向正确的opencv和arcsoft face library位置
2. 修改 `other/config.ini` 配置文件，其中`preload`应设置为 https://github.com/onefacepass/face_recognition 中的 preload 目录
3. QtCreator导入OneFacePass.pro文件

