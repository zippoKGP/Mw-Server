;==========================================================================================
;《聖魔爭霸》地圖設定檔	(R4MP100)
;事件定義:
;			0			保留
;			1~63		地圖事件
;			100			地圖場景聲源和背景音樂信息
;			110			地圖人物設置
;==========================================================================================
;------------------------------------------------------------------------------------------
;保留事件
;------------------------------------------------------------------------------------------
PROC	0
ENDPROC
;
;------------------------------------------------------------------------------------------
;地圖事件設定(需要與地圖檔案上的事件點對應)
;------------------------------------------------------------------------------------------
PROC	1
ENDPROC
;
PROC	2
ENDPROC
;
PROC	3
ENDPROC
;
PROC	4
ENDPROC
;
PROC	5
ENDPROC
;
PROC	6
ENDPROC
;
PROC	7
ENDPROC
;
PROC	8
ENDPROC
;
PROC	9
ENDPROC
;
PROC	10
ENDPROC
;
PROC	11
ENDPROC
;
PROC	12
ENDPROC
;
PROC	13
ENDPROC
;
PROC	14
ENDPROC
;
PROC	15
ENDPROC
;
PROC	16
ENDPROC
;
PROC	17
ENDPROC
;
PROC	18
ENDPROC
;
PROC	19
ENDPROC
;
PROC	20
ENDPROC
;
PROC	21
ENDPROC
;
PROC	22
ENDPROC
;
PROC	23
ENDPROC
;
PROC	24
ENDPROC
;
PROC	25
ENDPROC
;
PROC	26
ENDPROC
;
PROC	27
ENDPROC
;
PROC	28
ENDPROC
;
PROC	29
ENDPROC
;
PROC	30
ENDPROC
;
PROC	31
ENDPROC
;
PROC	32
ENDPROC
;
PROC	33
ENDPROC
;
PROC	34
ENDPROC
;
PROC	35
ENDPROC
;
PROC	36
ENDPROC
;
PROC	37
ENDPROC
;
PROC	38
ENDPROC
;
PROC	39
ENDPROC
;
PROC	40
ENDPROC
;
PROC	41
ENDPROC
;
PROC	42
ENDPROC
;
PROC	43
ENDPROC
;
PROC	44
ENDPROC
;
PROC	45
ENDPROC
;
PROC	46
ENDPROC
;
PROC	47
ENDPROC
;
PROC	48
ENDPROC
;
PROC	49
ENDPROC
;
PROC	50
ENDPROC
;
PROC	51
ENDPROC
;
PROC	52
ENDPROC
;
PROC	53
ENDPROC
;
PROC	54
ENDPROC
;
PROC	55
ENDPROC
;
PROC	56
ENDPROC
;
PROC	57
ENDPROC
;
PROC	58
ENDPROC
;
PROC	59
ENDPROC
;
PROC	60
ENDPROC
;
PROC	61
ENDPROC
;
PROC	62
ENDPROC
;
PROC	63
ENDPROC
;
;------------------------------------------------------------------------------------------
;地圖場景音源和背景音樂信息
;------------------------------------------------------------------------------------------
PROC	100
;設定地圖場景聲源
	SNDSRC_CLEAR
	SNDSRC_SET 2322 175 1 10 100 008.WAV
	SNDSRC_SET 1293 316 1 10 100 013.WAV
	SNDSRC_SET 521 291 1 10 100 012.WAV
	SNDSRC_SET 2485 1398 1 10 100 001.WAV
	SNDSRC_SET 188 755 1 10 100 002.WAV
;設定地圖背景音樂
	;STOP_MUSIC
	PLAY_MUSIC 1 1
ENDPROC
;
;------------------------------------------------------------------------------------------
;地圖人物設置
;------------------------------------------------------------------------------------------
PROC	110
	NPC_LOCATE 106 100
	NPC_DATA 106 0
	NPC_FILE 106 42
	NPC_ACTION 106 0
	NPC_KIND 106 0
	NPC_POSITION 106 237+40 1844 0
	NPC_DIR 106 3
	NPC_FLAG 106 1
	NPC_MOVE_SPEED 106 8
	;
	;
	NPC_LOCATE 160 100
	NPC_DATA 160 0
	NPC_FILE 160 42
	NPC_ACTION 160 0
	NPC_KIND 160 0
	NPC_POSITION 160 237 1844 0
	NPC_DIR 160 3
	NPC_FLAG 160 1
	NPC_MOVE_SPEED 160 8
	;
	NPC_LOCATE 161 100
	NPC_DATA 161 0
	NPC_FILE 161 10
	NPC_ACTION 161 0
	NPC_KIND 161 0
	NPC_POSITION 161 834     719     0       
	NPC_DIR 161 3
	NPC_FLAG 161 1
	NPC_MOVE_SPEED 161 8
	;
	NPC_LOCATE 162 100
	NPC_DATA 162 0
	NPC_FILE 162 10
	NPC_ACTION 162 0
	NPC_KIND 162 0
	NPC_POSITION 162 1404    1326    0       
	NPC_DIR 162 3
	NPC_FLAG 162 1
	NPC_MOVE_SPEED 162 8
	;
	NPC_LOCATE 163 100
	NPC_DATA 163 0
	NPC_FILE 163 10
	NPC_ACTION 163 0
	NPC_KIND 163 0
	NPC_POSITION 163 936     1266    0       
	NPC_DIR 163 3
	NPC_FLAG 163 1
	NPC_MOVE_SPEED 163 8
	;
	NPC_LOCATE 164 100
	NPC_DATA 164 0
	NPC_FILE 164 10
	NPC_ACTION 164 0
	NPC_KIND 164 0
	NPC_POSITION 164 2040    1252    0       
	NPC_DIR 164 3
	NPC_FLAG 164 1
	NPC_MOVE_SPEED 164 8
	;
	NPC_LOCATE 165 100
	NPC_DATA 165 0
	NPC_FILE 165 10
	NPC_ACTION 165 0
	NPC_KIND 165 0
	NPC_POSITION 165 2059    1813    0       
	NPC_DIR 165 3
	NPC_FLAG 165 1
	NPC_MOVE_SPEED 165 8
	;
	NPC_LOCATE 166 100
	NPC_DATA 166 0
	NPC_FILE 166 10
	NPC_ACTION 166 0
	NPC_KIND 166 0
	NPC_POSITION 166 1682    1769    0       
	NPC_DIR 166 3
	NPC_FLAG 166 1
	NPC_MOVE_SPEED 166 8
	;
	;
ENDPROC
;
;=======================================================================================================
