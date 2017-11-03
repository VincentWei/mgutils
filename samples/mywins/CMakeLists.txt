add_executable (mywins
    mywins.c mywins_res_cn.h mywins_res_en.h mywins_res_tw.h
    )
target_link_libraries(mywins "${APP_LIBS}")
