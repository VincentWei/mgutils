add_executable (painter
    painter.c painter.h painter_res_en.h
    painter_res_cn.h painter_res_tw.h
    custom_minigui.c
)
target_link_libraries(painter "${APP_LIBS}")
