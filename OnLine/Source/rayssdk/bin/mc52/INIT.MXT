;=====================================================================
;《聖魔爭霸》初始化劇本(INIT.MAC)
;事件定義:
;			0~99	保留事件
;			100		連線遊戲設置世界的入口
;			200		設置遊戲世界所有國家
;			201		設置遊戲世界所有組織
;			202		設置遊戲世界所有頭銜
;			203		設置遊戲世界所有種族
;			204		設置遊戲世界所有任務提示
;=====================================================================
PROC	0
ENDPROC
;
;------------------------------------------------------------------------------------
;連線遊戲設置世界的入口
;------------------------------------------------------------------------------------
PROC	100
	CALL	200
	CALL	201
	CALL	202
	CALL	203
	CALL	204
ENDPROC
;
;------------------------------------------------------------------------------------
;設置遊戲世界所有國家
;------------------------------------------------------------------------------------
PROC	200
;	GAME_COUNTRY_NAME 0 德古歐
;	GAME_COUNTRY_NAME 1 伊遠都斯
;	GAME_COUNTRY_NAME 2 貝特勒
ENDPROC
;
;------------------------------------------------------------------------------------
;設置遊戲世界所有組織
;------------------------------------------------------------------------------------
PROC	201
;	GAME_ORGANIZE_NAME 0 騎士團
;	GAME_ORGANIZE_NAME 1 角鬥士公會
;	GAME_ORGANIZE_NAME 2 賞金獵人公會
;	GAME_ORGANIZE_NAME 3 魔法師公會
;	GAME_ORGANIZE_NAME 4 盜賊公會
;	GAME_ORGANIZE_NAME 5 收藏家協會
ENDPROC
;
;
;------------------------------------------------------------------------------------
;設置遊戲世界所有頭銜
;------------------------------------------------------------------------------------
PROC	202
;
;	GAME_TITLE_NAME 0 學徒
;	GAME_TITLE_SET 0 0 0			;所屬組織
;
;	GAME_TITLE_NAME 1 學徒
;	GAME_TITLE_SET 1 0 0			;所屬組織
;
;	GAME_TITLE_NAME 2 學徒
;	GAME_TITLE_SET 2 0 0			;所屬組織
;
;	GAME_TITLE_NAME 3 學徒
;	GAME_TITLE_SET 3 0 0			;所屬組織
;
;	GAME_TITLE_NAME 4 學徒
;	GAME_TITLE_SET 4 0 0			;所屬組織
;
;	GAME_TITLE_NAME 5 學徒
;	GAME_TITLE_SET 5 0 0			;所屬組織
;
;	GAME_TITLE_NAME 6 學徒
;	GAME_TITLE_SET 6 0 0			;所屬組織
;
;	GAME_TITLE_NAME 7 學徒
;	GAME_TITLE_SET 7 0 0			;所屬組織
;
;	GAME_TITLE_NAME 8 學徒
;	GAME_TITLE_SET 8 0 0			;所屬組織
;
;	GAME_TITLE_NAME 9 學徒
;	GAME_TITLE_SET 9 0 0			;所屬組織
;
;	GAME_TITLE_NAME 10 學徒
;	GAME_TITLE_SET 10 0 0			;所屬組織
;
;	GAME_TITLE_NAME 11 學徒
;	GAME_TITLE_SET 11 0 0			;所屬組織
;
;	GAME_TITLE_NAME 12 學徒
;	GAME_TITLE_SET 12 0 0			;所屬組織
ENDPROC
;
;------------------------------------------------------------------------------------
;設置遊戲世界所有種族
;------------------------------------------------------------------------------------
PROC	203
;	GAME_PHYLE_NAME 0 古歐族		;名稱
;	GAME_PHYLE_SET 0 0 75			;力量--攻擊力關係
;	GAME_PHYLE_SET 0 1 75			;力量--防護力關係
;	GAME_PHYLE_SET 0 2 75			;反應力--防護力關係
;	GAME_PHYLE_SET 0 3 75			;反應力--敏捷度關係
;	GAME_PHYLE_SET 0 4 75			;活力--最大生命力關係
;	GAME_PHYLE_SET 0 5 75			;精力--智力關係
;	GAME_PHYLE_SET 0 6 75			;精力--最大法力關係
	;
;	GAME_PHYLE_NAME 1 亞斯提族		;名稱
;	GAME_PHYLE_SET 1 0 75			;力量--攻擊力關係
;	GAME_PHYLE_SET 1 1 75			;力量--防護力關係
;	GAME_PHYLE_SET 1 2 75			;反應力--防護力關係
;	GAME_PHYLE_SET 1 3 75			;反應力--敏捷度關係
;	GAME_PHYLE_SET 1 4 75			;活力--最大生命力關係
;	GAME_PHYLE_SET 1 5 75			;精力--智力關係
;	GAME_PHYLE_SET 1 6 75			;精力--最大法力關係
	;
;	GAME_PHYLE_NAME 2 加瓦族		;名稱
;	GAME_PHYLE_SET 2 0 75			;力量--攻擊力關係
;	GAME_PHYLE_SET 2 1 75			;力量--防護力關係
;	GAME_PHYLE_SET 2 2 75			;反應力--防護力關係
;	GAME_PHYLE_SET 2 3 75			;反應力--敏捷度關係
;	GAME_PHYLE_SET 2 4 75			;活力--最大生命力關係
;	GAME_PHYLE_SET 2 5 75			;精力--智力關係
;	GAME_PHYLE_SET 2 6 75			;精力--最大法力關係
ENDPROC
;
;------------------------------------------------------------------------------------
;設置遊戲世界所有任務提示
;------------------------------------------------------------------------------------
PROC	204
    TASKBAR_RESET
;	TASKBAR_PROMPT	100	0	１２３４５６７８９０１２３
;	TASKBAR_PROMPT	100	1	１２３４５６７８９０１２３
    TASKBAR_PROMPT	100	0	法比德兄弟
    TASKBAR_PROMPT	101	0	城里酒鬼的家
    TASKBAR_PROMPT	102	0	酒館老闆
    TASKBAR_PROMPT	103	0	慌張的小男(在城郊外的地方)
    TASKBAR_PROMPT	104	0	通輯的犯人
    TASKBAR_PROMPT	105	0	盜賊３４５６７８９０１２３
    TASKBAR_PROMPT	105	1	１２３４５６７８９０１２３
    TASKBAR_PROMPT	106	0	買弓箭(在城里的軍營附近)
    TASKBAR_PROMPT	107	0	生病的雞群（包青天的雞）
    TASKBAR_PROMPT	108	0	丟失的戒子（在城里）
    TASKBAR_PROMPT	109	0	尋找腰帶（在旅館客房裡）
    TASKBAR_PROMPT	110	0	買書（城里的民房）
    TASKBAR_PROMPT	111	0	作怪夢的人
    TASKBAR_PROMPT	112	0	靈魂
    TASKBAR_PROMPT	113	0	商隊（城里的旅館裡）
    TASKBAR_PROMPT	114	0	補給（回到旅館）
    TASKBAR_PROMPT	115	0	教會（城里的某處）
    TASKBAR_PROMPT	116	0	陰謀（在某城的酒館）
    TASKBAR_PROMPT	117	0	虛偽的老闆
    TASKBAR_PROMPT	118	0	？？？？？？
    TASKBAR_PROMPT	119	0	？？？？？？
    TASKBAR_PROMPT	120	0	？？？？？？
    TASKBAR_PROMPT	121	0	在民房內﹐找針線
    TASKBAR_PROMPT	122	0	？？？？？？
    TASKBAR_PROMPT	123	0	？？？？？？
    TASKBAR_PROMPT	124	0	？？？？？？
    TASKBAR_PROMPT	125	0	請牙醫
    TASKBAR_PROMPT	126	0	腦筋急轉彎
    TASKBAR_PROMPT	127	0	營救
    TASKBAR_PROMPT	128 0	猜謎
    TASKBAR_PROMPT	129	0	送貨
    TASKBAR_PROMPT	130	0	傳話
    TASKBAR_PROMPT	131	0	塞拉所等之人
    TASKBAR_PROMPT	132	0	找解藥
    TASKBAR_PROMPT	133	0	傳話
    TASKBAR_PROMPT	134	0	找風箏
    TASKBAR_PROMPT	135	0	幫忙買眼鏡
    TASKBAR_PROMPT	136	0	尋找制造眼鏡的材料
    TASKBAR_PROMPT	137	0	猜謎
    TASKBAR_PROMPT	138	0	找水喝
    TASKBAR_PROMPT	139	0	救人
    TASKBAR_PROMPT	140	0	找大夫
    TASKBAR_PROMPT	141	0	取回鋤頭
    TASKBAR_PROMPT	142	0	買殺虫劑
    TASKBAR_PROMPT	143	0	找包裹
    TASKBAR_PROMPT	144	0	送物品
    TASKBAR_PROMPT	145	0	找孩子
    TASKBAR_PROMPT	146	0	為道具店送貨
    TASKBAR_PROMPT	147	0	委託購物
    TASKBAR_PROMPT	148	0	救人
    TASKBAR_PROMPT	149	0	發傳單
    TASKBAR_PROMPT	150	0	送信
    TASKBAR_PROMPT	151	0	滿漢全席
    TASKBAR_PROMPT	152	0	？？？？？？
    TASKBAR_PROMPT	153	0	抱怨
    TASKBAR_PROMPT	154	0	配153
    TASKBAR_PROMPT	155	0	？？？？？？
    TASKBAR_PROMPT	156	0	腦筋急轉彎
    TASKBAR_PROMPT	157	0	？？？？？？
    TASKBAR_PROMPT	158	0	送物品
    TASKBAR_PROMPT	159	0	走私
    TASKBAR_PROMPT	160	0	解救
    TASKBAR_PROMPT	161	0	賣假肢
    TASKBAR_PROMPT	162	0	找石灰膏
    TASKBAR_PROMPT	163	0	送東西
    TASKBAR_PROMPT	164	0	送東西
    TASKBAR_PROMPT	165	0	？？？？？？
    TASKBAR_PROMPT	166	0	走私
    TASKBAR_PROMPT	167	0	傳話
    TASKBAR_PROMPT	168	0	傳話
    TASKBAR_PROMPT	169	0	有人向你借錢
    TASKBAR_PROMPT	170	0	找人
    TASKBAR_PROMPT	171	0	找藥
    TASKBAR_PROMPT	172	0	找圖
    TASKBAR_PROMPT	173	0	買東西
    TASKBAR_PROMPT	174	0	道具店送貨
    TASKBAR_PROMPT	175	0	腦筋急轉彎
    TASKBAR_PROMPT	176	0	作怪夢的人
    TASKBAR_PROMPT	177	0	夢的根源
    TASKBAR_PROMPT	178	0	保護
    TASKBAR_PROMPT	179	0	送信
    TASKBAR_PROMPT	180	0	道具店送東西
    TASKBAR_PROMPT	181	0	帶小孩找家人
    TASKBAR_PROMPT	182	0	？？？？？？
    TASKBAR_PROMPT	183	0	宣傳
    TASKBAR_PROMPT	184	0	找音樂盒
    TASKBAR_PROMPT	185	0	旋賞
    TASKBAR_PROMPT	186	0	找鞋子
    TASKBAR_PROMPT	187	0	懷孕的婦女
    TASKBAR_PROMPT	188	0	追殺
    TASKBAR_PROMPT	189	0	在碼頭
    TASKBAR_PROMPT	190	0	販賣
    TASKBAR_PROMPT	191	0	找老鼠干
    TASKBAR_PROMPT	192	0	收集毒蛇膽
    TASKBAR_PROMPT	193	0	狼皮
    TASKBAR_PROMPT	194	0	蜘蛛
    TASKBAR_PROMPT	195	0	牛頭怪的牛角
    TASKBAR_PROMPT	196	0	冰源魔女的魔杖
    TASKBAR_PROMPT	197	0	蝙蝠體
    TASKBAR_PROMPT	198	0	殺狼
    TASKBAR_PROMPT	199	0	洽談貿易
    TASKBAR_PROMPT	200	0	找東西
    TASKBAR_PROMPT	201	0	救人
    TASKBAR_PROMPT	202	0	送貨
    TASKBAR_PROMPT	203	0	除樹靈制療藥
    TASKBAR_PROMPT	204	0	村長怪物
    TASKBAR_PROMPT	205	0	解救村長
    TASKBAR_PROMPT	206	0	討債
    TASKBAR_PROMPT	207	0	腦筋急轉彎
    TASKBAR_PROMPT	108	0	找貓
    TASKBAR_PROMPT	209	0	找藥方
    TASKBAR_PROMPT	210	0	治療腦筋錯亂
    TASKBAR_PROMPT	211	0	尋找遺跡
    TASKBAR_PROMPT	212	0	尋找哥哥
    TASKBAR_PROMPT	213	0	翹鎖
    TASKBAR_PROMPT	214	0	收集
    TASKBAR_PROMPT	215	0	帶酒
    TASKBAR_PROMPT	216	0	找孩子
    TASKBAR_PROMPT	217	0	找食物
    TASKBAR_PROMPT	218	0	調查
    TASKBAR_PROMPT	219	0	找東西
    TASKBAR_PROMPT	220	0	拿東西
    TASKBAR_PROMPT	221	0	買礦石
    TASKBAR_PROMPT	222	0	解咒
    TASKBAR_PROMPT	223	0	找老婆
    TASKBAR_PROMPT	224	0	搬毒桶
    TASKBAR_PROMPT	225	0	找東西
    TASKBAR_PROMPT	226	0	找女兒
    TASKBAR_PROMPT	227	0	贖人
    TASKBAR_PROMPT	228	0	送家書
    TASKBAR_PROMPT	229	0	保護
    TASKBAR_PROMPT	230	0	送情書
    TASKBAR_PROMPT	231	0	撮合親事
    TASKBAR_PROMPT	232	0	找眼鏡
    TASKBAR_PROMPT	233	0	宣傳
    TASKBAR_PROMPT	234	0	送血書
    TASKBAR_PROMPT	235	0	購物
    TASKBAR_PROMPT	236	0	送禮物
    TASKBAR_PROMPT	237	0	送錢給別人
    TASKBAR_PROMPT	238	0	幫法蘭克大廳小蘭的生日
    TASKBAR_PROMPT	239	0	找忘記的密碼
    TASKBAR_PROMPT	240	0	嫌吵鬧的安布羅斯
    TASKBAR_PROMPT	241	0	？？？？？？
    TASKBAR_PROMPT	242	0	打賭
    TASKBAR_PROMPT	243	0	尋找一袋金筆
    TASKBAR_PROMPT	244	0	懸賞清潔垃圾
    TASKBAR_PROMPT	245	0	碼頭遺失船票
    TASKBAR_PROMPT	246	0	得到簽名
    TASKBAR_PROMPT	247	0	打聽誰是最富裕的人
    TASKBAR_PROMPT	248	0	送錢
    TASKBAR_PROMPT	249	0	買塗料
    TASKBAR_PROMPT	250	0	收集破罐子
    TASKBAR_PROMPT	251	0	？？？？？？
    TASKBAR_PROMPT	252	0	？？？？？？
    TASKBAR_PROMPT	253	0	？？？？？？
    TASKBAR_PROMPT	254	0	？？？？？？
    TASKBAR_PROMPT	255	0	？？？？？？
    TASKBAR_PROMPT	256	0	？？？？？？
    TASKBAR_PROMPT	257	0	？？？？？？
    TASKBAR_PROMPT	258	0	？？？？？？
    TASKBAR_PROMPT	259	0	？？？？？？
    TASKBAR_PROMPT	260	0	？？？？？？
    TASKBAR_PROMPT	261	0	尋找魔法書
    TASKBAR_PROMPT	262	0	買酒
    TASKBAR_PROMPT	263	0	找人
    TASKBAR_PROMPT	264	0	對聯
    TASKBAR_PROMPT	265	0	找東西
    TASKBAR_PROMPT	266	0	打聽
    TASKBAR_PROMPT	267	0	討書
    TASKBAR_PROMPT	268	0	送肉
    TASKBAR_PROMPT	269	0	讓無頭女鬼安息
    TASKBAR_PROMPT	270	0	解救
    TASKBAR_PROMPT	271	0	找娃娃
    TASKBAR_PROMPT	272	0	買犁
    TASKBAR_PROMPT	273	0	袤赽奻遜呁撓跦嶸羕
    TASKBAR_PROMPT	274	0	兄妹幾個
    TASKBAR_PROMPT	275	0	青蛙
    TASKBAR_PROMPT	276	0	小貓釣魚
    TASKBAR_PROMPT	277	0	吃餅
    TASKBAR_PROMPT	278	0	損失
    TASKBAR_PROMPT	279	0	雞蛋
    TASKBAR_PROMPT	280	0	買書
    TASKBAR_PROMPT	281	0	買美麗的拖鞋
    TASKBAR_PROMPT	282	0	送貨
    TASKBAR_PROMPT	283	0	找東西
    TASKBAR_PROMPT	284	0	轉告
    TASKBAR_PROMPT	285	0	？？？？？？
    TASKBAR_PROMPT	286	0	買咖啡
    TASKBAR_PROMPT	287	0	收購絲綢
    TASKBAR_PROMPT	288	0	收購銅鏡
    TASKBAR_PROMPT	289	0	販賣生鐵
    TASKBAR_PROMPT	290	0	販賣皮衣
    TASKBAR_PROMPT	291	0	幫一個老頭買珠寶
    TASKBAR_PROMPT	292	0	打聽侄子的消息
    TASKBAR_PROMPT	293	0	販賣
    TASKBAR_PROMPT	294	0	寶箱(有很多寶箱的房間)
    TASKBAR_PROMPT	295	0	營救
    TASKBAR_PROMPT	296	0	殲滅怪物
    TASKBAR_PROMPT	297	0	救人
    TASKBAR_PROMPT	298	0	送人(奄奄一吸的人)
    TASKBAR_PROMPT	299	0	拿東西
    TASKBAR_PROMPT	300	0	殺人
    TASKBAR_PROMPT	301	0	偷內衣
    TASKBAR_PROMPT	302	0	給偷歡的踐人看門
    TASKBAR_PROMPT	303	0	幫富婆約會情夫
    TASKBAR_PROMPT	304	0	幫人送手帕
    TASKBAR_PROMPT	305	0	送情書
    TASKBAR_PROMPT	306	0	？？？？？？
    TASKBAR_PROMPT	307	0	？？？？？？
    TASKBAR_PROMPT	308	0	？？？？？？
    TASKBAR_PROMPT	309	0	？？？？？？
    TASKBAR_PROMPT	310	0	？？？？？？
    TASKBAR_PROMPT	311	0	？？？？？？
    TASKBAR_PROMPT	312	0	？？？？？？
    TASKBAR_PROMPT	313	0	？？？？？？
    TASKBAR_PROMPT	314	0	？？？？？？
    TASKBAR_PROMPT	315	0	？？？？？？
    TASKBAR_PROMPT	316	0	？？？？？？
    TASKBAR_PROMPT	317	0	？？？？？？
    TASKBAR_PROMPT	318	0	？？？？？？
    TASKBAR_PROMPT	319	0	？？？？？？
    TASKBAR_PROMPT	320	0	？？？？？？
    TASKBAR_PROMPT	321	0	？？？？？？
    TASKBAR_PROMPT	322 0	？？？？？？
    TASKBAR_PROMPT	323 0	？？？？？？
    TASKBAR_PROMPT	324 0	？？？？？？
    TASKBAR_PROMPT	325 0	？？？？？？
    TASKBAR_PROMPT	326 0   ？？？？？？
    TASKBAR_PROMPT	327 0	？？？？？？
    TASKBAR_PROMPT	328 0	？？？？？？
    TASKBAR_PROMPT	329 0	？？？？？？
    TASKBAR_PROMPT	330 0	？？？？？？
    TASKBAR_PROMPT	331 0	？？？？？？
    TASKBAR_PROMPT	332 0	？？？？？？
    TASKBAR_PROMPT	333 0	？？？？？？
    TASKBAR_PROMPT	334 0	？？？？？？
    TASKBAR_PROMPT	335 0	？？？？？？
    TASKBAR_PROMPT	336	0	帶劍﹕
    TASKBAR_PROMPT	337	0	搶回戒指
    TASKBAR_PROMPT	338 0	？？？？？？
    TASKBAR_PROMPT	339 0	？？？？？？
    TASKBAR_PROMPT	340 0	？？？？？？
    TASKBAR_PROMPT	341 0	？？？？？？
    TASKBAR_PROMPT	342 0	？？？？？？
    TASKBAR_PROMPT	343 0	？？？？？？
    TASKBAR_PROMPT	344 0	？？？？？？
    TASKBAR_PROMPT	345 0	？？？？？？
    TASKBAR_PROMPT	346 0	？？？？？？
    TASKBAR_PROMPT	347 0	？？？？？？
    TASKBAR_PROMPT	348 0	？？？？？？
    TASKBAR_PROMPT	349 0	？？？？？？
    TASKBAR_PROMPT	350 0	？？？？？？
    TASKBAR_PROMPT	351	0	擔心
    TASKBAR_PROMPT	352	0	治病(351才觸發352)
    TASKBAR_PROMPT	353	0	磨利你的劍
    TASKBAR_PROMPT	354	0	加柴火(民房內)
    TASKBAR_PROMPT	355	0	破壞蒸酒器(破壞別人的蒸酒器)
    TASKBAR_PROMPT	356	0	打聽(幫黑暗聖堂的人打聽光明聖堂的人在教堂干什麼)
    TASKBAR_PROMPT	357	0	得到通行證
    TASKBAR_PROMPT	358	0	打探對手弱點
    TASKBAR_PROMPT	359	0	交換(島城)
    TASKBAR_PROMPT	360	0	智力題
    TASKBAR_PROMPT	361	0	找書
    TASKBAR_PROMPT	362	0	處理書(觸發361才出現362)
    TASKBAR_PROMPT	363	0	買零件
    TASKBAR_PROMPT	364	0	智力題
    TASKBAR_PROMPT	365	0	智力題
ENDPROC
;
;
PROC	205
;這是一個注釋
	ADD 100 10+20
ENDPROC
