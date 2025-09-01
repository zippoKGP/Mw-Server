/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : TextMsg.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/29
******************************************************************************/ 

#ifndef _TEXTMSG_H_
#define _TEXTMSG_H_


#ifdef  GB_VERSION  //{




// -------- Error Message ---------------
// --- connect
#define MSG_CONNECT_SERVER_ERROR		"服务器忙碌中,请稍后再试..."
#define MSG_ACCOUND_ERROR				"帐号或者密码错误,请重新输入!"
#define MSG_CHAT_SERVER_ERROR			"聊天服务器错误!"
#define MSG_NICKNANE_ERROR				"昵称已经有人使用"
#define MSG_CREATE_CHARACTER_ERROR		"无法创建人物"
#define MSG_EXIST_SCENE_ERROR			"同一个帐号不能有玩一个以上的角色，程序将推出"
#define MSG_OTHER_ENTER_SCENE_ERROR		"这个帐号已经有其他玩家登录了"
// --- File
#define MSG_FILE_CREAT_ERROR			"无法创建 %s 档案"
#define MSG_FILE_OPEN_ERROR				"无法开启 %s 档案"
#define MSG_FILE_CHECK_ERROR			"%s 档案检查错误"
#define MSG_FILE_RANGE_ERROR			"%s 档案大小错误"
#define MSG_FILE_DECODE_ERROR			"%s 档案解码错误"
// --- Memory 
#define MSG_MEMORY_ALLOC_ERROR			"无法配置内存"
// --- Graph
#define MSG_IMG256_DECODE_ERROR			"IMG256 解码错误(%x)"
#define MSG_BUFFER_NOT_IMG256_FORMAT	"缓冲区不是 IMG256 格式"
// --- MDA dile
#define MSG_FILE_NOT_MDA_ERROR			"%s 不是 MDA 档案"
#define MSG_MDA_COMMAND_ERROR			"MDA 错误命令(%s)"
#define MSG_MDA_DATABASE_FRAME_ERROR	"MDA 帧数错误(%d)"
#define MSG_MDA_UNKNOW_COMMAND			"无法解读的 MDA 外部命令(%s)"
// --- MAP 
#define MSG_MAP_FILE_LOAD_ERROR			"无法载入地图档案(%s)"
#define MSG_MAP_VERSION_ERROR			"地图档案(%s)版本错误"
#define MSG_MAP_FORMAT_ERROR			"地图档案(%s)格式错误"
// --- Game rule
#define MSG_BAOBAO_FOLLOW_ERROR			"宠物跟随错误!"
#define MSG_BAOBAO_RELEASE_ERROR		"宠物目前正在观看，无法放生，请先设置不观看"
#define MSG_BAOBAO_RELEASE_ERROR1		"宠物目前正在参战，无法放生，请先设置不参战"

#define MSG_ADD_BAOBAO_ERROR			"增加宝宝数据错误"
#define MSG_DELETE_BAOBAO_ERROR			"删除宝宝出错，不存在这个宝宝"
#define MSG_ITEM_DETAIL_ERROR			"显示物品描述错误"
#define MSG_GIVE_ITEM_ERROR				"给予物品出错"
#define MSG_REQUEST_DEAL_ERROR			"请求交易失败"
#define MSG_DEAL_CANCEL_ERROR			"交易取消出错"
#define MSG_USE_EQUIP_ERROR				"使用装备出错"
#define MSG_BUY_ITEM_ERROR				"买物品错误"
#define MSG_SAVE_MONEY_ERROR			"存钱错误"
#define MSG_WITHDRAW_ERROR				"取钱错误"
#define MSG_PUSH_POPSHOP_ERROR			"向当铺存物品出错"
#define MSG_POP_POPSHOP_ERROR			"从当铺取物品出错"
#define MSG_MDA_ERROR					"MDA 资料库档案名称太长, system error"




// -------------- Message text -------------------
// --- system
#define MSG_GAME_NAME					"神界"
#define GAME_FONT_NAME                  "宋体"


// --- Game base
#define CHARACTER_SEX_MAN_TEXT			"男"			// 男性
#define CHARACTER_SEX_WOMAN_TEXT		"女"			// 女性

#define CHARACTER_PHYLE_HUMAN_TEXT		"人类"
#define CHARACTER_PHYLE_GENUS_TEXT		"灵兽"
#define CHARACTER_PHYLE_MAGIC_TEXT		"法师"
#define CHARACTER_PHYLE_ROBET_TEXT		"机械"

#define BAOBAO_PHYLE_HUMAN_TEXT			"人形系"
#define BAOBAO_PHYLE_FLY_TEXT			"飞行系"
#define BAOBAO_PHYLE_DEVIL_TEXT			"恶魔系"
#define BAOBAO_PHYLE_DEAD_TEXT			"不死系"
#define BAOBAO_PHYLE_DRAGON_TEXT		"龙系"
#define BAOBAO_PHYLE_SPECIAL_TEXT		"特殊系"


#define WEATHER_TEXT_NONE				"晴天"
#define WEATHER_TEXT_RANG				"雨"
#define WEATHER_TEXT_SNOW				"雪"
#define WEATHER_TEXT_CLOUD				"云"
#define WEATHER_TEXT_THUNDER			"雷"

#define WEATHER_TEXT_SMALL				"小"
#define WEATHER_TEXT_MIDDLE				"中"
#define WEATHER_TEXT_LARGE				"大"

#define WEATHER_TEXT_DOWN				"无"
#define WEATHER_TEXT_EN					"东北"
#define WEATHER_TEXT_WN					"西北"

#define WEATHER_TEXT_REPORT				"#Y%s#N区域,天气情况,目前#Y%s#N,风向#Y%s#N"
#define WEATHER_TEXT_SAMPLE_REPORT		"%s风向%s"

#define CHARACTER_INST_HUMAN_MAN		"男人类,擅长控制类技能#0"
#define CHARACTER_INST_HUMAN_WOMAN		"女人类,擅长控制类技能#0"
#define CHARACTER_INST_GENUS_MAN		"男灵兽,擅长多人攻击技能#0"
#define CHARACTER_INST_GENUS_WOMAN		"女灵兽,擅长恢复系技能#0"
#define CHARACTER_INST_MAGIC_MAN		"男法师,擅长攻击法术技能#0"
#define CHARACTER_INST_MAGIC_WOMAN		"女法师,擅长攻击法术技能#0"
#define CHARACTER_INST_ROBET_MAN		"男机甲,擅长吸收系技能#0"
#define CHARACTER_INST_ROBET_WOMAN		"女机甲,擅长吸收系技能#0"


// --- Connect
#define MSG_SERVER_NOT_FIND				"无法连接到指定的服务器"
#define MSG_CONNECT_WAITTING			"连接服务器!请稍待......"
#define MSG_ACCOUNT_EMERY				"请输入正确的帐号名与密码"
#define MSG_CONNECT_SERVER				"正在连接服务器"
#define MSG_LAN_DISCONNECT				"网络断开连接!!!"
#define MSG_SERVER_SHUTDOWN				"服务器正在维护当中,请稍后再试!"

// --- Game data
#define MSG_CHAR_LEVEL					"%d 级"
#define MSG_CHARACTER_CREATE_FULL		"~C2你已经创建5个人物,无法再创建新的人物~C0"
#define MSG_NOT_SELECT_PHYLE			"请选择一个种族!"
#define MSG_NOT_NICK_NAME				"请输入昵称!"

#define MSG_MAP_MAIN_MENU_INST_ITEM			"物品(I)"
#define MSG_MAP_MAIN_MENU_INST_TEAM			"组队(T)"
#define MSG_MAP_MAIN_MENU_INST_ATTACK		"攻击(A)"
#define MSG_MAP_MAIN_MENU_INST_GIVE			"给予(G)"
#define MSG_MAP_MAIN_MENU_INST_DEAL			"交易(X)"
#define MSG_MAP_MAIN_MENU_INST_MISSION		"任务(Q)"
#define MSG_MAP_MAIN_MENU_INST_GROUP		"公会(B)"
#define MSG_MAP_MAIN_MENU_INST_SYSTEM		"系统(S)"
#define MSG_MAP_MAIN_MENU_INST_CHAT			"聊天器(F)"

#define MSG_MAP_MAIN_MENU_INST_CHATWIN		"窗口切换"
#define MSG_MAP_MAIN_MENI_INST_HISTORY		"历史纪录"
#define MSG_MAP_MAIN_MENU_INST_DISCARE		"屏蔽"
#define MSG_MAP_MAIN_MENU_INST_PHIZ			"表情符号(0)"
#define MSG_MAP_MAIN_MENU_INST_SCROLL		"滚屏开关"
#define MSG_MAP_MAIN_MENU_INST_VIEW			"窗口加大"
#define MSG_MAP_MAIN_MENU_INST_UP			"上翻"
#define MSG_MAP_MAIN_MENU_INST_DOWN			"下翻"

#define MSG_MAP_MAIN_MENU_INST_SWITCH		"窗口切换"
#define MSG_MAP_MAIN_MENU_INST_WORLD		"世界地图"
#define MSG_MAP_MAIN_MENU_INST_SMALL		"缩略地图(Tab)"


#define MSG_WINDOW_CLOSE					"关闭"

#define MSG_VOLUME_SUB						"减低音量"
#define MSG_VOLUME_ADD						"加大音量"


#define MSG_CHANNEL_NAME_SCREEN				"当前"
#define MSG_CHANNEL_NAME_TEAM				"队伍"
#define MSG_CHANNEL_NAME_PERSON				"私聊"
#define MSG_CHANNEL_NAME_GROUP				"公会"
#define MSG_CHANNEL_NAME_SELLBUY			"经济"
#define MSG_CHANNEL_NAME_WORLD				"世界"
#define MSG_CHANNEL_NAME_SYSTEM				"系统"
#define MSG_CHANNEL_NAME_MESSAGE			"讯息"
			


#define MSG_CHAT_TIMER_ERROR				"#R#S请不要太快说话#N"
#define MSG_SET_PERSON_ID					"#R私聊对象设定为#N(#Y%s#N)#G%d"

#define MSG_PERSON_SEND						"你悄悄的对#Y%s#N说："
#define MSG_PERSON_RECEIVE					"#Y%s#N悄悄的对你说："


#define MSG_PLAYER_INFO_ID					" ID ：%d"
#define MSG_PLAYER_INFO_SEX					"性别：%s"
#define MSG_PLAYER_INFO_PHYLE				"种族：%s"
#define MSG_PLAYER_INFO_TITLE				"头衔：%s"
#define MSG_PLAYER_INFO_NAME				"姓名：%s"
#define MSG_PLAYER_INFO_LEVEL				"等级：%d"


#define MSG_MAX_CHAT_FRIEND					"你的好友栏已经满了,无法执行这个操作!"
#define MSG_ADD_FRIEND						"#8你已经将#Y%s#N加为好友了!#21"
#define MSG_ADD_SAME_FRIEND					"#Y%s#N已经是你的好友了!#47"
#define MSG_OTHER_ADD_FRIEND				"%s 把你加为好友"

#define MSG_SYSTEM_ADD_FRIEND				"你已经将%s加为好友了!"
#define MSG_SYSTEM_ADD_SAME_FRIEND			"%s已经是你的好友了!"
#define MSG_FRIEND_ONLINE					"#G你的好友#Y%s#G已经进入游戏#9#N"
#define MSG_FRIEND_OFFLINE					"#R你的好友#Y%s#R已经离开游戏#N#8"



#define MSG_NEXUS_FRIEND0					"萍水相逢"				// 一般朋友关系
#define MSG_NEXUS_FRIEND1					"平淡之交"				
#define MSG_NEXUS_FRIEND2					"普通朋友"				
#define MSG_NEXUS_FRIEND3					"深情厚意"				
#define MSG_NEXUS_FRIEND4					"生死至交"				

#define MSG_NEXUS_MARRIAGE0					"老鼠大米"			
#define MSG_NEXUS_MARRIAGE1					"新婚夫妻"				// 婚姻
#define MSG_NEXUS_MARRIAGE2					"蜜月之爱"						
#define MSG_NEXUS_MARRIAGE3					"甜甜蜜蜜"			
#define MSG_NEXUS_MARRIAGE4					"老夫老妻"		

#define MSG_NEXUS_BROTHER0					"白瓷兄弟"				// 结拜
#define MSG_NEXUS_BROTHER1					"钢之兄弟"		
#define MSG_NEXUS_BROTHER2					"银之兄弟"		
#define MSG_NEXUS_BROTHER3					"金之兄弟"
#define MSG_NEXUS_BROTHER4					"钻石兄弟"

#define MSG_NEXUS_MASTER0					"露水师徒"				// 师徒
#define MSG_NEXUS_MASTER1					"烈火师徒"			
#define MSG_NEXUS_MASTER2					"名师高徒"
#define MSG_NEXUS_MASTER3					"良师益友"
#define MSG_NEXUS_MASTER4					"再造之恩"
		
#define MSG_MAX_FRIEND_GROUP_SEND			"群发好友人数上限"
#define MSG_NO_MESSAGE						"请输入讯息内容!"


#define MSG_ADD_FRIEND_ERROR				"对方没有打开好友开关"	

#define MSG_CHAT_SEARCH_ERROR				"没有这个玩家，或者玩家不在线"

#define MSG_TEAM_LEAVE						"#Y%s#R离开队伍"

#define MSG_ADD_TEAM_NOT_LEADER_ERROR		"对方不是队长"
#define MSG_ADD_TEAM_OK						"已经向队长提出申请，请等待"
#define MSG_ADD_TEAM_FULL					"对方申请列表已满"

#define MSG_ADD_TEAM_READY					"%s 加入队伍了"
#define MSG_LEAVE_TEAM						"请注意！%s 离开队伍"
#define MSG_BREAK_TEAM						"请注意！您解散队伍了"
#define MSG_ALREADY_IN_TEAM					"你已在申请列中了，请稍等"

#define MSG_NO_MISSION						"#Y您目前没有接受任何任务#32"


#define MSG_HP_TEXT							"生命"
#define MSG_MP_TEXT							"魔法"
#define MSG_EXP_TEXT						"经验"


#define MSG_BAOBAO_TITLE					"%s的宠物"



// ----- 战斗数值部分
#define MSG_ADD_EXP							"       ~C2恭喜！您获得 ~C4%u~C2 经验~C0       "
#define	MSG_LEVEL_UP						"       ~C1恭喜！你升到 ~C4%u~C1 级了~C0       "


#define MSG_SUB_EXP							"       ~C1您被扣了 ~C4%d~C1 经验~C0       "
#define	MSG_LEVEL_DOWN						"       ~C1您被降到 ~C4%d~C1 级了~C0       "

#define MSG_RELEASE_BAOBAO					"#G您确定要#R放生#G宠物#Y %s #G吗？#83"

#define MSG_POINT_ERROR						"~C1系统内部点数计算错误,请跟 GM 联络!~C0"


#define MSG_ITEM_DROP						"您确定丢弃? #83"

#define MSG_NO_GIVE_DATA					" 没有给与的东西或者金钱! "


#define MSG_FIGHT_IMAGE_ERROR				"战斗使用档案遗失, 无法继续进行游戏, 系统将退出, 请与 GM 联络,或重新安装"


// --- show 抗性资料 ----
#define MSG_FASTNESS_PROTECT				"~C4防御~C0：%d"
#define MSG_FASTNESS_SHOOT					"~C4命中~C0：%d"
#define MSG_FASTNESS_DODGE					"~C4躲避~C0：%d"
#define MSG_FASTNESS_SUCK					"~C4抗吸~C0：%d"
#define MSG_FASTNESS_CRAZY					"~C4狂暴~C0：%d%% %d"
#define MSG_FASTNESS_DEADLINESS				"~C4致命~C0：%d%% %d"
#define MSG_FASTNESS_REDEADLINESS			"~C4抗致命~C0：%d%%"
#define MSG_FASTNESS_DOUBLEATT				"~C4连击~C0：%d%% %d"
#define MSG_FASTNESS_BACKATT				"~C4反击~C0：%d%% %d"
#define MSG_FASTNESS_BREAK_PHYSICS			"~C4破物理~C0：%d%% %d"
#define MSG_FASTNESS_MAGIC_FEEDBACK			"~C4法术反馈~C0：%d%% %d"
#define MSG_FASTNESS_PHYSICS_FEEDBACK		"~C4物理反馈~C0：%d%% %d"
#define MSG_FASTNESS_DEAD					"~C4抗死亡宣告~C0：%d%%"
#define MSG_FASTNESS_MAGIC_DARK				"~C4暗法~C0：%d"
#define MSG_FASTNESS_MAGIC_LIGHT			"~C4光法~C0：%d"
#define MSG_FASTNESS_MAGIC_WATER			"~C4水法~C0：%d"
#define MSG_FASTNESS_MAGIC_FIRE				"~C4火法~C0：%d"
#define MSG_FASTNESS_PHYSICS				"~C4物理~C0：%d"
#define MSG_FASTNESS_VIRUS					"~C4中毒~C0：%d"
#define MSG_FASTNESS_CHAOS					"~C4混乱~C0：%d"
#define MSG_FASTNESS_STONE					"~C4石化~C0：%d"
#define MSG_FASTNESS_SLEEP					"~C4昏睡~C0：%d"
#define MSG_FASTNESS_WEEK					"~C4衰老~C0：%d"



#define MSG_BAOBAO_DETAIL_INST				"#Y%s#N#E等级：%d#E生命：%d/%d#E魔法：%d/%d#E攻击：%d#E速度：%d#E转生次数：%d#E成长：%.3f#E"

#define MSG_SKILL_DETAIL_INST				"#Y%s#N#E%s#E"



#define MSG_FIGHT_COMMAND_NONE			"~C1  请下达命令~C0"
#define MSG_FIGHT_COMMAND_ATTACK		"使用~C5物理~C0攻击"

#define MSG_FIGHT_COMMAND_MAGIC_SEL		"请选择~C1技能~C0..."
#define MSG_FIGHT_COMMAND_MAGIC			"使用~C5%s~C0"

#define MSG_FIGHT_COMMAND_ITEM_SEL		"请选择~C1物品~C0..."
#define MSG_FIGHT_COMMAND_ITEM			"使用~C5物品~C0"

#define MSG_FIGHT_COMMAND_PROTECT		"使用~C5保护~C0命令"

#define MSG_FIGHT_COMMAND_RECOVER		"使用~C5防御~C0命令"

#define MSG_FIGHT_COMMAND_CHANGE_SEL	"请选择~C1宠物~C0..."
#define MSG_FIGHT_COMMAND_CHANGE		"使用~C5更换宠物~C0"

#define MSG_FIGHT_COMMAND_RUN			"使用~C5逃跑~C0命令"
#define MSG_FIGHT_COMMAND_AUTO_HP		"使用~C5自动补~C1血~C0"
#define MSG_FIGHT_COMMAND_AUTO_MP		"使用~C5自动补~C2法~C0"
#define MSG_FIGHT_COMMAND_AUTO_ATTACK	"使用~C5自动~C0攻击"


#define MSG_FIGHT_TARGET				"对象~C5%s~C0"
#define MSG_FIGHT_LEFT_TARGET_NONE		"~C1请~C4左键~C1选择对象~C0"
#define MSG_FIGHT_RIGHT_TARGET_NONE		"~C1请~C4右键~C1选择对象~C0"



#define MSG_FIGHT_FIRST_INST			"第一回合战斗,延迟~C4%d~C0秒,请下达命令"








#endif		// GB Version






#endif	// TEXTMSG_H