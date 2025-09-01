def_visible_primitive( get_MAX_version,			"maxVersion");	
def_visible_primitive( get_INI_setting,			"getINISetting");
def_visible_primitive( set_INI_setting,			"setINISetting");
def_visible_primitive( del_INI_setting,			"delINISetting");
def_visible_primitive( get_file_version,		"getFileVersion");	

def_visible_primitive( gen_class_id,			"genClassID");

def_visible_primitive( add_note_track,			"addNoteTrack");
def_visible_primitive( delete_note_track,		"deleteNoteTrack");
def_visible_primitive( has_note_tracks,			"hasNoteTracks");
def_visible_primitive( num_note_tracks,			"numNoteTracks");
def_visible_primitive( get_note_track,			"getNoteTrack");

//mcr_func_def

// UI stuff - moved to MAXScript core
//def_visible_primitive( registerRightClickMenu,			"registerRightClickMenu");
//def_visible_primitive( unregisterRightClickMenu,		"unregisterRightClickMenu");
//def_visible_primitive( unregisterAllRightClickMenus,	"unregisterAllRightClickMenus");
//def_visible_primitive( registerRedrawViewsCallback,		"registerRedrawViewsCallback");
//def_visible_primitive( unregisterRedrawViewsCallback,	"unregisterRedrawViewsCallback");
	
// moved from MAXScript
def_struct_primitive( tvw_pickTrackDlg,		trackView,	"pickTrackDlg");
