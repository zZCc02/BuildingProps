# BuildingProps
开发UE智慧城市的工具

SingletonControl作为单例用来存储点击选择的操作，Explode用来配置楼层分层的基础变量


DatasmithSceneRoot 这里用来设置你的DatasmithSceneActor，通过他来获取你的将被移动的Actor数组并排序。注意每一层楼的绑定必须得有嵌套两层Actor,
默认的使用情景是，你有一个室内Actor下挂载的室内的staticmeshActor,一个室外Actor用来挂载外墙的staticmeshActor,一个总的Actor下面添加的是室内Actor
和室外Actor.如果你不是按照这样制作的模型，点击会崩溃。


Floor LName这是用来分割每层楼总的Actor名字以便获取索引的，传入的是例如"A1栋-11F",这个时候你需要传入的是"A1栋-",此时得到索引11用来对数组进行排序。
Curve和PullCurve分别代表的是楼层向上拆分的CurveFloat和抽出的CurveFloat，他相当于蓝图中的timeline内的时间线。你可以两个都使用同一个CurveFloat也可以不同。


ExplodeOffset用来设置楼层分层时每层楼的间隔。

PullOffset用来设置每层楼的抽出偏移值，这是根据你的总Actor的朝向来计算的。
