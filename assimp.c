#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <lua.h>
#include <lauxlib.h>

#include <lextlib.h>
#include <lextlib_lua52.h>


#define ASSIMP_UPVALUE_VECTOR3D (lua_upvalueindex(1))
#define ASSIMP_UPVALUE_COLOR4D (lua_upvalueindex(2))


static const char *_vector3d__index_map[] = {"x", "y", "z", NULL};

static int _vector3d__index(lua_State *L) {
	const int table = 1, key = 2;

	int index = luaL_checkoption(L, key, NULL, _vector3d__index_map);
	lua_rawgeti(L, table, index+1);

	return 1;
}

static int _vector3d__newindex(lua_State *L) {
	const int table = 1, key = 2; /* value = 3 */

	int index = luaL_checkoption(L, key, NULL, _vector3d__index_map);
	lua_rawseti(L, table, index+1);

	return 0;
}

static struct luaL_Reg assimp_vector3d_lib[] = {
	LUAX_FUNCTION(_vector3d, __index),
	LUAX_FUNCTION(_vector3d, __newindex),

	{ NULL, NULL }
};

static const char *_color4d__index_map[] = {"r", "g", "b", "a", NULL};

static int _color4d__index(lua_State *L) {
	const int table = 1, key = 2;

	int index = luaL_checkoption(L, key, NULL, _color4d__index_map);
	lua_rawgeti(L, table, index+1);

	return 1;
}

static int _color4d__newindex(lua_State *L) {
	const int table = 1, key = 2; /* value = 3 */

	int index = luaL_checkoption(L, key, NULL, _color4d__index_map);
	lua_rawseti(L, table, index+1);

	return 0;
}

static struct luaL_Reg assimp_color4d_lib[] = {
	LUAX_FUNCTION(_color4d, __index),
	LUAX_FUNCTION(_color4d, __newindex),

	{ NULL, NULL }
};

/* [-0,+1,e] */
static int _make_vector3d(lua_State *L, struct aiVector3D *vector) {
	lua_createtable(L, 3, 0);                          // [-0,+1,e]
	int l_vector = lua_gettop(L);

	lua_pushnumber(L, vector->x);                      // [-0,+1,-]
	lua_rawseti(L, l_vector, 1);                       // [-1,+0,e]
	lua_pushnumber(L, vector->y);                      // [-0,+1,-]
	lua_rawseti(L, l_vector, 2);                       // [-1,+0,e]
	lua_pushnumber(L, vector->z);                      // [-0,+1,-]
	lua_rawseti(L, l_vector, 3);                       // [-1,+0,e]

	lua_pushvalue(L, ASSIMP_UPVALUE_VECTOR3D);
	lua_setmetatable(L, l_vector);

	assert(lua_gettop(L) == l_vector);
	return 1;
}


/* [-0,+1,e] */
static int _make_color4d(lua_State *L, struct aiColor4D *color) {
	lua_createtable(L, 4, 0);                          // [-0,+1,e]
	int l_color = lua_gettop(L);

	lua_pushnumber(L, color->r);                       // [-0,+1,-]
	lua_rawseti(L, l_color, 1);                        // [-1,+0,e]
	lua_pushnumber(L, color->g);                       // [-0,+1,-]
	lua_rawseti(L, l_color, 2);                        // [-1,+0,e]
	lua_pushnumber(L, color->b);                       // [-0,+1,-]
	lua_rawseti(L, l_color, 3);                        // [-1,+0,e]
	lua_pushnumber(L, color->a);                       // [-0,+1,-]
	lua_rawseti(L, l_color, 4);                        // [-1,+0,e]

	lua_pushvalue(L, ASSIMP_UPVALUE_COLOR4D);
	lua_setmetatable(L, l_color);

	assert(lua_gettop(L) == l_color);
	return 1;
}

/* [-0,+1,e] */
static int _make_face(lua_State *L, struct aiFace *face) {
	lua_newtable(L);                                    // [-0,+1,e]
	int l_face = lua_gettop(L);

	lua_createtable(L, face->mNumIndices, 0);           // [-0,+1,e]
	int l_indices = lua_gettop(L);

	for (int i = 0; i < face->mNumIndices; i++) {
		lua_pushinteger(L, face->mIndices[i]+1);          // [-0,+1,-]
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

	lua_pushlstring(L, mesh->mName.data, mesh->mName.length);// [-0,+1,-]
	lua_setfield(L, l_mesh, "name");                    // [-1,+0,e]

	lua_pushinteger(L, mesh->mMaterialIndex+1);         // [-0,+1,-]
	lua_setfield(L, l_mesh, "material_index");          // [-1,+0,e]

	lua_createtable(L, AI_MAX_NUMBER_OF_TEXTURECOORDS, 0);// [-0,+1,e]
	int l_num_uv_components = lua_gettop(L);

	for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++) {
		lua_pushinteger(L, mesh->mNumUVComponents[i]);       // [-0,+1,-]
		lua_rawseti(L, l_num_uv_components, i+1);         // [-1,+0,e]
	}

	lua_setfield(L, l_mesh, "num_uv_components");       // [-1,+0,e]

	lua_createtable(L, 0, 4);                           // [-0,+1,e]
	int l_primitive_types = lua_gettop(L);

	if (mesh->mPrimitiveTypes & aiPrimitiveType_POINT) {
		lua_pushboolean(L, true);                         // [-0,+1,-]
		lua_setfield(L, l_primitive_types, "point");      // [-1,+0,e]
	}
	if (mesh->mPrimitiveTypes & aiPrimitiveType_LINE) {
		lua_pushboolean(L, true);                         // [-0,+1,-]
		lua_setfield(L, l_primitive_types, "line");       // [-1,+0,e]
	}
	if (mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) {
		lua_pushboolean(L, true);                         // [-0,+1,-]
		lua_setfield(L, l_primitive_types, "triangle");   // [-1,+0,e]
	}
	if (mesh->mPrimitiveTypes & aiPrimitiveType_POLYGON) {
		lua_pushboolean(L, true);                         // [-0,+1,-]
		lua_setfield(L, l_primitive_types, "polygon");    // [-1,+0,e]
	}

	lua_setfield(L, l_mesh, "primitive_types");         // [-1,+0,e]

	if (mesh->mFaces != NULL) {
		lua_createtable(L, mesh->mNumFaces, 0);           // [-0,+1,e]
		int l_faces = lua_gettop(L);

		for (int i = 0; i < mesh->mNumFaces; i++) {
			_make_face(L, &mesh->mFaces[i]);                // [-0,+1,e]
			lua_rawseti(L, l_faces, i+1);                   // [-1,+0,e]
		}

		lua_setfield(L, l_mesh, "faces");                 // [-1,+0,e]
	}

	if (mesh->mVertices != NULL) {
		lua_createtable(L, mesh->mNumVertices, 0);        // [-0,+1,e]
		int l_vertices = lua_gettop(L);

		for (int i = 0; i < mesh->mNumVertices; i++) {
			_make_vector3d(L, &mesh->mVertices[i]);         // [-0,+1,e]
			lua_rawseti(L, l_vertices, i+1);                // [-1,+0,e]
		}

		lua_setfield(L, l_mesh, "vertices");              // [-1,+0,e]
	}

	if (mesh->mNormals != NULL) {
		lua_createtable(L, mesh->mNumVertices, 0);        // [-0,+1,e]
		int l_normals = lua_gettop(L);

		for (int i = 0; i < mesh->mNumVertices; i++) {
			_make_vector3d(L, &mesh->mNormals[i]);          // [-0,+1,e]
			lua_rawseti(L, l_normals, i+1);                 // [-1,+0,e]
		}

		lua_setfield(L, l_mesh, "normals");               // [-1,+0,e]
	}

	if (mesh->mTangents != NULL) {
		lua_createtable(L, mesh->mNumVertices, 0);        // [-0,+1,e]
		int l_tangents = lua_gettop(L);

		for (int i = 0; i < mesh->mNumVertices; i++) {
			_make_vector3d(L, &mesh->mTangents[i]);         // [-0,+1,e]
			lua_rawseti(L, l_tangents, i+1);                // [-1,+0,e]
		}

		lua_setfield(L, l_mesh, "tangents");              // [-1,+0,e]
	}

	if (mesh->mBitangents != NULL) {
		lua_createtable(L, mesh->mNumVertices, 0);        // [-0,+1,e]
		int l_bitangents = lua_gettop(L);

		for (int i = 0; i < mesh->mNumVertices; i++) {
			_make_vector3d(L, &mesh->mBitangents[i]);       // [-0,+1,e]
			lua_rawseti(L, l_bitangents, i+1);              // [-1,+0,e]
		}

		lua_setfield(L, l_mesh, "bitangents");            // [-1,+0,e]
	}

	lua_createtable(L, AI_MAX_NUMBER_OF_COLOR_SETS, 0); // [-0,+1,e]
	int l_colors = lua_gettop(L);

	for (int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++) {
		if (mesh->mColors[j] != NULL) {
			lua_createtable(L, mesh->mNumVertices, 0);      // [-0,+1,e]
			int l_colors_j = lua_gettop(L);

			for (int i = 0; i < mesh->mNumVertices; i++) {
				_make_color4d(L, &mesh->mColors[j][i]);       // [-0,+1,e]
				lua_rawseti(L, l_colors_j, i+1);              // [-1,+0,e]
			}

			lua_rawseti(L, l_colors, j+1);                  // [-1,+0,e]
		}
	}

	lua_setfield(L, l_mesh, "colors");                  // [-1,+0,e]

	lua_createtable(L, AI_MAX_NUMBER_OF_TEXTURECOORDS, 0);// [-0,+1,e]
	int l_texture_coords = lua_gettop(L);

	for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++) {
		if (mesh->mTextureCoords[j] != NULL) {
			lua_createtable(L, mesh->mNumVertices, 0);      // [-0,+1,e]
			int l_texture_coords_j = lua_gettop(L);

			for (int i = 0; i < mesh->mNumVertices; i++) {
				_make_vector3d(L, &mesh->mTextureCoords[j][i]);// [-0,+1,e]
				lua_rawseti(L, l_texture_coords_j, i+1);      // [-1,+0,e]
			}

			lua_rawseti(L, l_texture_coords, j+1);          // [-1,+0,e]
		}
	}

	lua_setfield(L, l_mesh, "texture_coords");          // [-1,+0,e]

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
	luaL_newlib(L, assimp_color4d_lib);           // [-0,+1,e]
	luaL_newlib(L, assimp_vector3d_lib);          // [-0,+1,e]

	luaL_newlibtable(L, assimp_lib);              // [-0,+1,e]
	luaL_setfuncs(L, assimp_lib, 2);              // [-2,+0,e]
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
