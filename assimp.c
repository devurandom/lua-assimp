#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <lua.h>
#include <lauxlib.h>

#include <lextlib.h>
#include <lextlib_lua52.h>


#define ASSIMP_UPVALUE_VECTOR3D (lua_upvalueindex(1))
#define ASSIMP_UPVALUE_COLOR4D (lua_upvalueindex(2))


#define AI_V3 sane_vector3f
#define AI_M4X4 sane_matrix44f


static inline float sane_vector3f(struct aiVector3D *v, int i) {
	assert(i >= 0 && i < 3);
	switch (i) {
		case 0: return v->x;
		case 1: return v->y;
		case 2: return v->z;
	}
	return 0.0f;
}

static inline float sane_matrix44f(struct aiMatrix4x4 *m, int i, int j) {
	assert(i >= 0 && i < 4);
	assert(j >= 0 && j < 4);
	switch (i) {
		case 0:
			switch (j) {
				case 0: return m->a1;
				case 1: return m->a2;
				case 2: return m->a3;
				case 3: return m->a4;
			}
		case 1:
			switch (j) {
				case 0: return m->b1;
				case 1: return m->b2;
				case 2: return m->b3;
				case 3: return m->b4;
			}
		case 2:
			switch (j) {
				case 0: return m->c1;
				case 1: return m->c2;
				case 2: return m->c3;
				case 3: return m->c4;
			}
		case 3:
			switch (j) {
				case 0: return m->d1;
				case 1: return m->d2;
				case 2: return m->d3;
				case 3: return m->d4;
			}
	}
	return 0.0f;
}


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

	for (int i = 0; i < 3; i++) {
		lua_pushnumber(L, AI_V3(vector, i));             // [-0,+1,-]
		lua_rawseti(L, l_vector, i+1);                   // [-1,+0,e]
	}

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
static int _make_matrix44f(lua_State *L, struct aiMatrix4x4 *matrix) {
	lua_createtable(L, 4*4, 0);                         // [-0,+1,e]
	int l_matrix = lua_gettop(L);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			lua_pushnumber(L, AI_M4X4(matrix, i, j));       // [-0,+1,-]
			lua_rawseti(L, l_matrix, i*4+j+1);              // [-1,+0,e]
		}
	}

	assert(lua_gettop(L) == l_matrix);
	return 1;
}

/* [-0,+1,e] */
static int _make_vertex_weight(lua_State *L, struct aiVertexWeight *weight) {
	lua_newtable(L);                                    // [-0,+1,e]
	int l_weight = lua_gettop(L);

	lua_pushinteger(L, weight->mVertexId+1);            // [-0,+1,-]
	lua_setfield(L, l_weight, "vertex_index");          // [-1,+0,e]

	lua_pushnumber(L, weight->mWeight);                 // [-0,+1,-]
	lua_setfield(L, l_weight, "weight");                // [-1,+0,e]

	assert(lua_gettop(L) == l_weight);
	return 1;
}

/* [-0,+1,e] */
static int _make_bone(lua_State *L, struct aiBone *bone) {
	lua_newtable(L);                                    // [-0,+1,e]
	int l_bone = lua_gettop(L);

	lua_pushlstring(L, bone->mName.data, bone->mName.length);// [-0,+1,-]
	lua_setfield(L, l_bone, "name");                    // [-1,+0,e]

	_make_matrix44f(L, &bone->mOffsetMatrix);           // [-0,+1,e]
	lua_setfield(L, l_bone, "offset_matrix");           // [-1,+0,e]

	lua_createtable(L, bone->mNumWeights, 0);           // [-0,+1,e]
	int l_weights = lua_gettop(L);

	for (int i = 0; i < bone->mNumWeights; i++) {
		_make_vertex_weight(L, &bone->mWeights[i]);       // [-0,+1,-]
		lua_rawseti(L, l_weights, i+1);                   // [-1,+0,e]
	}

	lua_setfield(L, l_bone, "weights");                 // [-1,+0,e]

	assert(lua_gettop(L) == l_bone);
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

	if (mesh->mBones != NULL) {
		lua_createtable(L, mesh->mNumBones, 0);           // [-0,+1,e]
		int l_bones = lua_gettop(L);

		for (int i = 0; i < mesh->mNumBones; i++) {
			_make_bone(L, mesh->mBones[i]);                 // [-0,+1,e]
			lua_rawseti(L, l_bones, i+1);                   // [-1,+0,e]
		}

		lua_setfield(L, l_mesh, "bones");                 // [-1,+0,e]
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
