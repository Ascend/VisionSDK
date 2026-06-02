## 准备

* 字体文件： ttc. 可以从`C:\Windows\Fonts`中获取
* 字符表：需要渲染的字符，格式为每行一个字符
* 安装依赖： `pip install -r requirements.txt`

## 运行

```python
# 需要提前创建output目录，否则会报错
 python gen_font_textures.py --font simsun.ttc --font_size_pixel 60 --char_table "vocab.txt" --output=../PutText/vocab/
```

## 输出

默认在output目录，会产生以下文件：
* png: 对字体进行渲染后得到的png图片，用于预览
* bin: 对字体进行渲染后得到的二进制文件
* txt： 字符表中的每个字符、对应字符的前进宽度(advance width)信息和字体高度信息
