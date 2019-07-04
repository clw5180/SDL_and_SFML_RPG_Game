## A game called 'gluttonous snake' made by SDL2, easy to expand.

### 1、说明
贪吃蛇游戏，也是一个简单的游戏框架，主要功能包括：
- 多种游戏状态之间的转换，包括游戏开始状态、游戏进行状态、暂停状态以及游戏结束状态；
- 控制物体匀速或加速移动，碰撞检测，随机生成以及分数显示；
- 鼠标选中、点击按钮时不同的特效与音效，游戏进行时播放背景音乐；
- 支持源码中修改窗口大小同时保持从.xml读取的object可以根据窗口大小等比例缩放。


### 2、注意事项
- 新建VS2017项目，把snake文件夹内的.cpp和.h文件拖进来（不包括子目录内的文件）；
- 配置项目属性-VC++目录-包含目录添加tinyxml文件夹（另外还需要SDL2相关的头文件）；
- tinyxml文件夹内的四个tinyxml相关的.cpp文件拖到VS2017项目的资源文件下；
- 链接器-输入-附加依赖项要包含SDL2相关的lib文件：
  - SDL2.lib
  - SDL2main.lib
  - SDL2_image.lib
  - SDL2_mixer.lib
  - SDL2_ttf.lib
- 最后要把SDL2相关的dll文件放在Windows系统目录或exe同一目录下！


### 3、效果演示
![这里随便写文字](https://github.com/clw5180/SDL2.0-Tour-of-Game-Development/blob/master/SDL_snake/screenshot1.png)

![这里随便写文字](https://github.com/clw5180/SDL2.0-Tour-of-Game-Development/blob/master/SDL_snake/screenshot2.png)

![这里随便写文字](https://github.com/clw5180/SDL2.0-Tour-of-Game-Development/blob/master/SDL_snake/screenshot3.png)
