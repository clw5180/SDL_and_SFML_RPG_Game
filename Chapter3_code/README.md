### 1、说明
利用SDL事件处理与绘图功能实现**角色行走**的一个小例子。我自己用Mov Gear软件做的Sprite Sheet，里面包含了八个方向的静止、行走和跑动图；目前很容易就可以加入跑动效果，只需要在事件处理类CInputHandler中加一个shift键，然后把相应的坐标步进值调大即可。。
 
### 2、效果演示
这里使用到了仙剑2的地图场景和幽城幻剑录的夏侯仪。测试了一下角色在地图行走的效果，除了地图没加遮挡层，然后人物在45°方向行走之后停下来的朝向有点问题，其他都还算OK

![这里随便写文字](https://github.com/clw5180/SDL2.0-Tour-of-Game-Development/blob/master/Chapter3_code/screenshot.png)  
  

