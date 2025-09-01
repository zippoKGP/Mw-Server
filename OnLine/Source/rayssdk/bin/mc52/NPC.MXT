;=====================================================================
;《聖魔爭霸》人物設定檔(R4NP????)
;事件定義:
;			0~99		保留
;			100			對話入口事件
;			999			與玩家交互結束事件
;=====================================================================
;事件關係表
;100>101>110>999(105.10>105.11; 105.20>105.21)
;       >120>999
;       >130>999
;=====================================================================
;--------------------------------------------------------------------
;保留事件
;--------------------------------------------------------------------
PROC	0
ENDPROC
;
;--------------------------------------------------------------------
;閒聊對話入口事件
;--------------------------------------------------------------------
PROC	100
	TALK_CLEAR
	TALK_SAY 0 閒人：
	TALK_SAY 1 你好？
	TALK_SAY 2 要披風是嗎？
	TALK_SAY 3 想要你就說呀！
	TALK_SAY 4 你不說我怎麼知道呢？
	TALK_SAY 5 不可能你說了，我不給你的。
	TALK_SAY 6 只要你說了，我一定會給你，我不會不給你，我一定會給你。
	TALK_SAY 7 你需要嗎？
	TALK_NEXT 101
	TALK_SHOW
ENDPROC
;
PROC	101
	CHOICE_CLEAR
	CHOICE_TITLE 請做出您的選擇：
	CHOICE_OPTION 0 110 １要披風。
	CHOICE_OPTION 1 120 ２這麼簡單就有了？。
	CHOICE_OPTION 2 130 ３不要。
	CHOICE_SHOW
ENDPROC
;
PROC	110
	TASK_CHECK 105 101
	IF 101 = 10				;寶劍換披風
		TASK_PROC_VAR 105 11
	ELSEIF 101 = 20			;金錢換披風
		TASK_PROC_VAR 105 21
	ELSE
	ENDIF
	CALL 999
ENDPROC
;
PROC	120
	TALK_CLEAR
	TALK_SAY 0 閒人：
	TALK_SAY 1 不信算了。
	TALK_NEXT 999
	TALK_SHOW
ENDPROC
;
PROC	130
	TALK_CLEAR
	TALK_SAY 0 閒人：
	TALK_SAY 1 好，你有種，我喜歡。
	TALK_SAY 2 為此我將送給你一些東西。
	TALK_NEXT 140
	TALK_SHOW
ENDPROC
;
PROC	140
	CHOICE_CLEAR
;	CHOICE_TITLE 請做出您的選擇：
	CHOICE_OPTION 0 150 １獲得經驗值１０點。
	CHOICE_OPTION 1 160 ２獲得道具長劍。
	CHOICE_OPTION 3 170 ３獲得金錢３００。
	CHOICE_OPTION 4 180 ４獲得技術點２０點。
	CHOICE_OPTION 5 190 ５讓你走吧。
	CHOICE_OPTION 6 200 ６使用道具長劍。
	CHOICE_OPTION 7 210 ７使用金錢１００。
	CHOICE_SHOW
ENDPROC
;
PROC	150
	CHECK_EXP 100
	IF 100 >= 50
		TALK_CLEAR
		TALK_SAY 0 閒人：
		TALK_SAY 1 你的經驗值很多了。
		TALK_NEXT 999
		TALK_SHOW
	ELSE
	   	GET_EXP 10
	   	CALL 999
	ENDIF
ENDPROC
;
PROC	160
	CHECK_ITEM 0 100
	IF 100 >= 2
		TALK_CLEAR
		TALK_SAY 0 閒人：
		TALK_SAY 1 貪得無饜的傢伙。
		TALK_NEXT 999
		TALK_SHOW
	ELSE
;weapon	
;		GET_ITEM 0
;armor
;		GET_ITEM 33
;ornament
		GET_ITEM 164
		CALL 999
	ENDIF
ENDPROC
;
PROC	170
	CHECK_CASH 100
	IF 100 >= 900
		TALK_CLEAR
		TALK_SAY 0 閒人：
		TALK_SAY 1 讓錢壓死你。
		TALK_NEXT 999
		TALK_SHOW
	ELSE
	   	GET_CASH 300
		CALL 999
	ENDIF
ENDPROC
;
PROC	180
	CHECK_SKILL 100
	IF 100 >= 300
		TALK_CLEAR
		TALK_SAY 0 閒人：
		TALK_SAY 1 有志不在點高。
		TALK_NEXT 999
		TALK_SHOW
	ELSE
		GET_SKILL 20
		CALL 999
	ENDIF
ENDPROC
;
PROC	190
;error found here...
;	NPC_MOVE_TO 160 124 1856 1
	CALL 999
ENDPROC
;
PROC	200
	CHECK_ITEM 0 101
	IF 101 > 0
		USE_ITEM 0
		CALL 999
	ELSE
		TALK_CLEAR
		TALK_SAY 0 閒人：
		TALK_SAY 1 呵呵，你沒有長劍吧？
		TALK_NEXT 999
		TALK_SHOW
	ENDIF
ENDPROC
;
PROC	210
	CHECK_CASH 101
	IF 101 >= 100
		USE_CASH 100
		CALL 999
	ELSE
		TALK_CLEAR
		TALK_SAY 0 閒人：
		TALK_SAY 1 呵呵，好像你的錢不夠吧？
		TALK_NEXT 999
		TALK_SHOW
	ENDIF
ENDPROC
;
;--------------------------------------------------------------------
;與玩家交互結束事件
;--------------------------------------------------------------------
PROC	999
	DIALOG_FINISH
ENDPROC
;
;
