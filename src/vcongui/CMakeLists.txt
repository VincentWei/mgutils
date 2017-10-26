set (src_list
    child.c child.h error.c error.h
    key.c key.h paint.c paint.h resource.h terminal.c
    terminal.h vc.c vc.h vcongui.c vt.c vt.h defs.h defkeymap.c
    )

mg_add_source_files (${src_list})

mg_commit_source_files ()
