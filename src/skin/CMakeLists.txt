
set (src_list
    skin.c
    cmdbutton.c
    chkbutton.c
    label.c
    bmplabel.c
    slider.c
    rotslider.c
    mgcontrol.c
    )

set (hdr_list
    item_comm.h 
	cmdbutton.h
    chkbutton.h
    label.h
    bmplabel.h
    slider.h
    rotslider.h
    mgcontrol.h
    )

mg_add_source_files (${src_list} ${hdr_list})

mg_commit_source_files ()
