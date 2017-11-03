add_executable (picview
    picview.c picview_res_en.h
    picview_res_cn.h picview_res_tw.h
    custom_minigui.c
)
target_link_libraries(picview "${APP_LIBS}")
