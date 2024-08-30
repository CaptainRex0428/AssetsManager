# Assets Manager

!["MainUI"](./img/MainUI.png "MainUI")

当前版本`v1.1.5`

Assets Manager是基于`Unreal C++`书写的一款资产管理软件，以项目插件的方式应用到项目中

目前包含资产筛查、资产规范自定义检测等功能，集成了资产修改、删除、查找引用等操作，便于用户在一个界面内实现批量资产管理和修改，尤其对于大型项目来说，可以极大提升项目资产管理的效率



## 环境依赖
Assets Manager依赖虚幻的官方库实现，目前依赖的库包括

这些库被定义在[AssetsManager.Build.cs](https://github.com/CaptainRex0428/AssetsManager/blob/master/Source/AssetsManager/AssetsManager.Build.cs)文件中

```C#
PublicDependencyModuleNames.AddRange
(
	new string[]
	{
		"Core",
		"Blutility",
		"EditorScriptingUtilities",
		"UMG",
		"Niagara",
    		"UnrealEd",
   	 	"AssetRegistry",
    		"AssetTools",
    		"ContentBrowser",
		"InputCore",
		"Projects"
		
	}
);
	

PrivateDependencyModuleNames.AddRange
(
	new string[]
	{
		"CoreUObject",
		"Engine",
		"Slate",
		"SlateCore"
	}
);

```

&nbsp;

## 使用说明

### 安装说明

#### Release版本
1. 下载对应版本的[Release](https://github.com/CaptainRex0428/AssetsManager/releases)
2. 将解压文件放入引擎项目的`Plugins文件夹`中（没有的话自行创建）。参考地址`{%ProjectFolder}/Plugins/AssetsManager`
3. 打开引擎即可

!["Folder Template"](./img/FolderTemplate.png "Folder Template")


#### 源码版本
1. Clone源码到引擎项目的`Plugins文件夹`中（没有的话自行创建）。参考地址`{%ProjectFolder}/Plugins/AssetsManager`
2. 右键uproject文件生成新的`.sln文件`
3. 打开`.sln文件`编译
4. 编译成功后即可运行

!["CompileF5"](./img/CompileF5.png "CompileF5")


### 使用说明
在待检查资产的文件夹上右键，选择`AssetsManager`即可打开插件主界面\
!["MenuEntry"](./img/MenuEntry.png "MenuEntry")

&nbsp;

#### 界面说明
!["MainUIInfo"](./img/MainUIInfo.png "MainUIInfo")
1. 基础信息界面
2. 资产类型筛选
3. 错误条件筛选
4. 筛选后资产列表
5. 批量处理模块


## 目录结构描述
.\
│  .gitignore\
│  AssetsManager.uplugin\
│  ReadMe.md\
│  \
├─Resources\
│      Icon128.png\
│      LOGO.ai\
│      LOGO_GREY_256.png\
│      LOGO_RED_256.png\
│      LOGO_ORANGE_256.png\
│      LOGO_PURPLE_256.png\
│      LOGO_BLUE_256.png\
│      LOGO_GREEN_256.png\
│      TitleBar.ai\
│      TitleBar.png\
│      \
├─Source\
│  └─AssetsManager\
│      │  AssetsManager.Build.cs\
│      │  \
│      ├─Public\
│      │  │  ManagerLogger.h\
│      │  │  AssetsManager.h\
│      │  │  AssetsManagerConfig.h\
│      │  │  AssetsManagerStyle.h\
│      │  │  \
│      │  ├─AssetsChecker\
│      │  │      AssetsChecker.h\
│      │  │      \
│      │  ├─SlateWidgets\
│      │  │  │  ManagerSlate.h\
│      │  │  │  SCustomTable.h\
│      │  │  │  SCustomTableRow.h\
│      │  │  │  TCustomSlateDelegates.h\
│      │  │  │  SCustomListView.h\
│      │  │  │  SCustomEditableText.h\
│      │  │  │  SCommonSlate.h\
│      │  │  │  \
│      │  │  └─Material\
│      │  │          MaterialCreatorSlate.h\
│      │  │          \
│      │  └─StandardAsset\
│      │          FCustomStandardAssetData.h\
│      │          \
│      └─Private\
│          │  ManagerLogger.cpp\
│          │  AssetsManagerStyle.cpp\
│          │  AssetsManager.cpp\
│          │  \
│          ├─AssetsChecker\
│          │      AssetsChecker.cpp\
│          │      \
│          ├─SlateWidgets\
│          │  │  TCustomSlateDelegates.cpp\
│          │  │  SCommonSlate.cpp\
│          │  │  ManagerSlate.cpp\
│          │  │  \
│          │  └─Material\
│          │          MaterialCreatorSlate.cpp\
│          │          \
│          └─StandardAsset\
│                  FCustomStandardAssetData.cpp\
│          \
├─Content\
└─Config\
       AssetsManager.ini

&nbsp;

# 版本内容更新

&nbsp;

# 开发计划（v 1.2.0）

## Assets Manager

- [*]  代码整理与注释
- [ ]  界面迭代
- [ ]  骨骼网格体、骨骼、物理资产的标准化查验
- [ ]  添加游戏内贴图尺寸查验Strict模式
- [ ]  更新Config方式（添加ini作为配置文件）并更新相应读取方式

## Material Creator

- [ ]  构建界面
- [ ]  添加Uniform材质库
- [ ]  添加查验、更改和保存材质实例相关功能
