add_executable (notebook
    main.c notebook.h notebook_res_cn.h
    notebook_res_en.h notebook_res_tw.h
    custom_minigui.c
)
target_link_libraries(notebook "${APP_LIBS}")
