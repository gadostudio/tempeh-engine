tempeh_add_shader_module(tri_vs VERTEX shaders/tri.vs)
tempeh_add_shader_module(tri_fs FRAGMENT shaders/tri.fs)

tempeh_add_dependency_group(tempeh_test_shaders
                            tri_vs
                            tri_fs)