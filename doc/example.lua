ai = require "assimp"

scene, err = ai.import_file("assimp/test/models/OBJ/box.obj", ai.process.preset.TargetRealtime_Fast)

function print_table(t,i)
	local string = require "string"
	local i = i or 0
	for k,v in pairs(t) do
		if type(v) == "table" then
			print(string.rep(" ",i) .. k .. ":")
			print_table(v, i+1)
		else
			print(string.rep(" ", i) .. k .. ":", v)
		end
	end
end

print_table(scene)
print(scene.meshes[1].vertices[1][2])
