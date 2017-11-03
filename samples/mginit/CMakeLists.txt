add_executable (mginit
    mginit.c taskbar.c taskbar.h desktop.c desktop.h custom_minigui.c
    desktop_res_en.h desktop_res_cn.h desktop_res_tw.h
    )
target_link_libraries(mginit "${APP_LIBS}")
