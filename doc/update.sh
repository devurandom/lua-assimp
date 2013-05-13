sed -nre 's/^\s*(aiProcess_)(\S+)\s*=.*$/luaX_setconst(L, l_aiprocess, \1, \2)\;/p' assimp/include/assimp/postprocess.h
sed -nre 's/^#define .*(aiProcessPreset_)(\S+).*$/luaX_setconst(L, l_ai_process_preset, \1, \2)\;/p' assimp/include/assimp/postprocess.h
