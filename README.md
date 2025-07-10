# RGPDemo

一个简单的 RGP 游戏 Demo。

学习使用 UE5 蓝图与 C++ 实现以下功能：

🏃 基础移动。

⚔️ 武器拾取。

🧌 攻击。

🧨 物品击碎。

🧍‍♂️ IK 绑定。

## 基础移动

鼠标控制摄像机， WASD 控制移动方向，并可使用 Space 进行跳跃。

使用增强输入更好地管理输入映射。

学习动画蓝图，通过状态机控制角色移动动画、跳跃动画等。

![基础移动](./Examples/基础移动.gif)

## 武器拾取

检测物品拾取范围与人物碰撞，并在碰撞时拾取武器。

设置武器槽，学习使用 IK Rig 和 IK Retargeter 使用不同的骨骼动画。

使用委托（Delegate）通过 C++ 代码控制 `Component Begin Overlap` 和 `Component End Overlap`。

![碰撞检测拾取武器与动画](./Examples/碰撞检测拾取武器与动画.gif)

## 攻击

对 Sword 实现盒体碰撞与追踪，当盒体碰撞到怪物时触发受击动画。

学习使用 Animation Montage，在蓝图和 C++ 代码中控制动画播放。

![怪物打击](./Examples/怪物打击.gif)

## 物品击碎

使用接口（Interface）负责处理受击效果。

实现 Destructible Mesh 和 Field System，在武器上创建场（Field）以实现物品击碎。

![物品击碎](./Examples/物品击碎.gif)

## IK 绑定

学习使用 Control Rig 实现逆运动学（Inverse Kinematics），蓝图编程实现双脚高度检测并进行人物骨骼适应。

![IK绑定](./Examples/IK绑定.gif)

## 备注

### 忽略项目

这些项目太大，没有通过 Git 管理并推送到远程仓库，需要本地添加。

- ParagonMinions
- AcientTreasures
