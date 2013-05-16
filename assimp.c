#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <lua.h>
#include <lauxlib.h>

#include "lextlib/lextlib.h"
#include "lextlib/lextlib_lua52.h"


static const char *_vector3d_map_index(int index) {
	static const char *map = "x\0y\0z\0";

	if (index < 1 || index > 3) {
		return NULL;
	}

	return &map[(index-1)*2];
}

static int _vector3d__index(lua_State *L) {
	const int table = 1, key = 2;

	int index = luaL_checkinteger(L, key);
	const char *mapped_index = _vector3d_map_index(index);
	if (mapped_index == NULL) {
		return luaL_error(L, "index out of range");
	}

	lua_getfield(L, table, mapped_index);

	return 1;
}

static int _vector3d__newindex(lua_State *L) {
	const int table = 1, key = 2; /* value = 3 */

	int index = luaL_checkinteger(L, key);
	const char *mapped_index = _vector3d_map_index(index);
	if (mapped_index == NULL) {
		return luaL_error(L, "index out of range");
	}

	lua_setfield(L, table, mapped_index);

	return 0;
}

struct luaL_Reg assimp_vector3d_lib[] = {
	LUAX_FUNCTION(_vector3d, __index),
	LUAX_FUNCTION(_vector3d, __newindex),

	{ NULL, NULL }
};

/* [-0,+1,e] */
static int _make_vector3d(lua_State *L, struct aiVector3D *vector) {
	lua_createtable(L, 3, 0);                          // [-0,+1,e]
	int l_vector = lua_gettop(L);

	lua_pushnumber(L, vector->x);
	lua_setfield(L, l_vector, "x");
	lua_pushnumber(L, vector->y);
	lua_setfield(L, l_vector, "y");
	lua_pushnumber(L, vector->z);
	lua_setfield(L, l_vector, "z");

	luaL_newlib(L, assimp_vector3d_lib);
	lua_setmetatable(L, l_vector);

	assert(lua_gettop(L) == l_vector);
	return 1;
}

/* [-0,+1,e] */
static int _make_face(lua_State *L, struct aiFace *face) {
	lua_newtable(L);                                    // [-0,+1,e]
	int l_face = lua_gettop(L);

	lua_createtable(L, face->mNumIndices, 0);           // [-0,+1,e]
	int l_indices = lua_gettop(L);

	for (int i = 0; i < face->mNumIndices; i++) {
		lua_pushinteger(L, face->mIndices[i]+1);          // [-0,+1,e]
		lua_rawseti(L, l_indices, i+1);                   // [-1,+0,e]
	}

	lua_setfield(L, l_face, "indices");                 // [-1,+0,e]

	assert(lua_gettop(L) == l_face);
	return 1;
}

/* [-0,+1,e] */
static int _make_mesh(lua_State *L, struct aiMesh *mesh) {
	lua_newtable(L);                                    // [-0,+1,e]
	int l_mesh = lua_gettop(L);

	lua_createtable(L, mesh->mNumFaces, 0);             // [-0,+1,e]
	int l_faces = lua_gettop(L);

	for (int i = 0; i < mesh->mNumFaces; i++) {
		_make_face(L, &mesh->mFaces[i]);                  // [-0,+1,e]
		lua_rawseti(L, l_faces, i+1);                     // [-1,+0,e]
	}

	lua_setfield(L, l_mesh, "faces");                   // [-1,+0,e]

	lua_createtable(L, mesh->mNumVertices, 0);          // [-0,+1,e]
	int l_vertices = lua_gettop(L);

	for (int i = 0; i < mesh->mNumVertices; i++) {
		_make_vector3d(L, &mesh->mVertices[i]);           // [-0,+1,e]
		lua_rawseti(L, l_vertices, i+1);                  // [-1,+0,e]
	}

	lua_setfield(L, l_mesh, "vertices");                // [-1,+0,e]

	lua_createtable(L, mesh->mNumVertices, 0);          // [-0,+1,e]
	int l_normals = lua_gettop(L);

	for (int i = 0; i < mesh->mNumVertices; i++) {
		_make_vector3d(L, &mesh->mNormals[i]);            // [-0,+1,e]
		lua_rawseti(L, l_normals, i+1);                   // [-1,+0,e]
	}

	lua_setfield(L, l_mesh, "normals");                 // [-1,+0,e]

	assert(lua_gettop(L) == l_mesh);
	return 1;
}

/* [-0,+1,e] */
static int _make_scene(lua_State *L, struct aiScene *scene) {
	lua_newtable(L);                                    // [-0,+1,e]
	int l_scene = lua_gettop(L);

	lua_createtable(L, scene->mNumMeshes, 0);           // [-0,+1,e]
	int l_meshes = lua_gettop(L);

	for (int i = 0; i < scene->mNumMeshes; i++) {
		_make_mesh(L, scene->mMeshes[i]);                 // [-0,+1,e]
		lua_rawseti(L, l_meshes, i+1);                    // [-1,+0,e]
	}

	lua_setfield(L, l_scene, "meshes");                 // [-1,+0,e]

	assert(lua_gettop(L) == l_scene);
	return 1;
}


static int assimp_import_file(lua_State *L) {
	const char *filename = luaX_checkstring(L, 1, "filename");
	unsigned int flags = luaX_checkinteger(L, 2, "flags");

	struct aiScene *scene = (struct aiScene*)aiImportFile(filename, flags);
	if (scene == NULL) {
		lua_pushnil(L);                                    // [-0,+1,-]
		lua_pushstring(L, aiGetErrorString());             // [-0,+1,e]

		return 2;
	}

	_make_scene(L, scene);                               // [-0,+1,e]

	aiReleaseImport(scene);

	return 1;
}


struct luaL_Reg assimp_lib[] = {
	LUAX_FUNCTION(assimp_, import_file),

	{ NULL, NULL }
};


int luaopen_assimp(lua_State *L) {
	luaL_newlib(L, assimp_lib);                   // [-0,+1,e]
	int l_ai = lua_gettop(L);

	lua_newtable(L);                              // [-0,+1,e]
	int l_ai_process = lua_gettop(L);

	luaX_setconst(L, l_ai_process, aiProcess_, CalcTangentSpace);
	luaX_setconst(L, l_ai_process, aiProcess_, JoinIdenticalVertices);
	luaX_setconst(L, l_ai_process, aiProcess_, MakeLeftHanded);
	luaX_setconst(L, l_ai_process, aiProcess_, Triangulate);
	luaX_setconst(L, l_ai_process, aiProcess_, RemoveComponent);
	luaX_setconst(L, l_ai_process, aiProcess_, GenNormals);
	luaX_setconst(L, l_ai_process, aiProcess_, GenSmoothNormals);
	luaX_setconst(L, l_ai_process, aiProcess_, SplitLargeMeshes);
	luaX_setconst(L, l_ai_process, aiProcess_, PreTransformVertices);
	luaX_setconst(L, l_ai_process, aiProcess_, LimitBoneWeights);
	luaX_setconst(L, l_ai_process, aiProcess_, ValidateDataStructure);
	luaX_setconst(L, l_ai_process, aiProcess_, ImproveCacheLocality);
	luaX_setconst(L, l_ai_process, aiProcess_, RemoveRedundantMaterials);
	luaX_setconst(L, l_ai_process, aiProcess_, FixInfacingNormals);
	luaX_setconst(L, l_ai_process, aiProcess_, SortByPType);
	luaX_setconst(L, l_ai_process, aiProcess_, FindDegenerates);
	luaX_setconst(L, l_ai_process, aiProcess_, FindInvalidData);
	luaX_setconst(L, l_ai_process, aiProcess_, GenUVCoords);
	luaX_setconst(L, l_ai_process, aiProcess_, TransformUVCoords);
	luaX_setconst(L, l_ai_process, aiProcess_, FindInstances);
	luaX_setconst(L, l_ai_process, aiProcess_, OptimizeMeshes);
	luaX_setconst(L, l_ai_process, aiProcess_, OptimizeGraph);
	luaX_setconst(L, l_ai_process, aiProcess_, FlipUVs);
	luaX_setconst(L, l_ai_process, aiProcess_, FlipWindingOrder);
	luaX_setconst(L, l_ai_process, aiProcess_, SplitByBoneCount);
	luaX_setconst(L, l_ai_process, aiProcess_, Debone);

	lua_newtable(L);                          // [-0,+1,e]
	int l_ai_process_preset = lua_gettop(L);

	/* WARNING: Special case, inserted manually */
	luaX_setconst(L, l_ai_process_preset, aiProcess_, ConvertToLeftHanded);

	luaX_setconst(L, l_ai_process_preset, aiProcessPreset_, TargetRealtime_Fast);
	luaX_setconst(L, l_ai_process_preset, aiProcessPreset_, TargetRealtime_Quality);
	luaX_setconst(L, l_ai_process_preset, aiProcessPreset_, TargetRealtime_MaxQuality);

	lua_setfield(L, l_ai_process, "preset");  // [-1,+0,e]

	lua_setfield(L, l_ai, "process");         // [-1,+0,e]

	return 1;
}
