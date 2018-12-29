import os
os.chdir('C:\\Users\\kmdre\\Downloads')

scene = bpy.context.scene
obj = bpy.data.objects['Armature']

file = open('./animation.txt', 'w+')

print(scene.frame_end - scene.frame_start + 1, len(obj.pose.bones), file=file)
for frame in range(scene.frame_start, scene.frame_end+1):
  scene.frame_set(frame)
  for bone in obj.pose.bones:
    # m = obj.convert_space(pose_bone=bone, matrix=bone.matrix, from_space='POSE', to_space='LOCAL')
    # m = bone.matrix_basis
    # print(m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3], file=file)
    location = bone.location
    rotation = bone.rotation_quaternion
    print(location[0], location[1], location[2], rotation[0], rotation[1], rotation[2], rotation[3], file=file)

file.close()
