import os
os.chdir('C:\\Users\\kmdre\\Downloads')

obj = bpy.data.objects['Character'];

file = open('./temp_model.txt', 'w+')

mesh = obj.data;
alwaysDrawGroupId = obj.vertex_groups.find("always_draw");

# write vertices
print(len(mesh.vertices), file=file)
for vertex in mesh.vertices:
  group, weight = (vertex.groups[0].group, vertex.groups[0].weight) if len(vertex.groups) > 0 else (0, 1.0)
  print(vertex.co[0], vertex.co[1], vertex.co[2], group, weight, file=file)

# write faces
print(len(mesh.polygons), file=file)
for face in mesh.polygons:
  assert len(face.vertices) == 3
  for vId, lId in zip(face.vertices, face.loop_indices):
    vertex = mesh.vertices[vId];
    uv = mesh.uv_layers.active.data[lId].uv
    group, weight = (vertex.groups[0].group, vertex.groups[0].weight) if len(vertex.groups) > 0 else (0, 1.0)
    print(vertex.co[0], vertex.co[1], vertex.co[2], group, weight, uv.x, uv.y, file=file)

# write lines
print(len(mesh.edges), file=file)
for edge in mesh.edges:
  vId0 = -1
  vId1 = edge.vertices[0]
  vId2 = edge.vertices[1]
  vId3 = -1
  faces = [face for face in mesh.polygons if (vId1, vId2) in face.edge_keys or (vId2, vId1) in face.edge_keys]
  if len(faces) == 0:
    continue
  if len(faces) == 1:
    faces = [faces[0], faces[0]]
  if alwaysDrawGroupId in [g.group for g in mesh.vertices[vId1].groups] and alwaysDrawGroupId in [g.group for g in mesh.vertices[vId2].groups]:
    vId0 = vId2
    vId3 = vId1
  else:
    vId0 = [v for v in faces[0].vertices if v != vId1 and v != vId2][0]
    vId3 = [v for v in faces[1].vertices if v != vId1 and v != vId2][0]
  print(vId0, vId1, vId2, vId3, file=file)

# write bones
arm = obj.parent

if arm is None:
  print(0, file=file)
else:
  bones = arm.data.bones
  print(len(bones), file=file)
  for i in range(0, len(bones)):
    bones[i].name = str(i)
  mybones = [(b.matrix, b.vector, b.head, int(b.parent.name) if not b.parent is None else -1) for b in bones]
  mybones[0] = (mybones[0][0] * Matrix(((1, 0, 0), (0, 0, 1), (0, -1, 0))), 
               (Matrix(((1, 0, 0), (0, 0, 1), (0, -1, 0))) * mybones[0][1]), 
               (Matrix(((1, 0, 0), (0, 0, 1), (0, -1, 0))) * mybones[0][2]), 
               mybones[0][3])
  for matrix, vector, head, parent_id in mybones:
    parent = mybones[parent_id] if parent_id != -1 else (Matrix(), Vector(), Vector(), -1)
    parent_matrix, parent_vector, parent_head, parent_parent = parent
    location = parent_matrix * (head + parent_vector)
    rotation = matrix.to_quaternion()
    print(parent_id, location[0], location[1], location[2], rotation[0], rotation[1], rotation[2], rotation[3], file=file)

file.close()




# ANIMATION EXPORT
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



# MODEL
# parent   location (x, y, z)   rotation (x, y, z, w)

# bones[i].parent.id?
# bones[i].parent.matrix * bones[i].head + bones[i].parent.vector
# bones[i].matrix.to_quaternion()


# animation 
# location (x, y, z)   rotation (x, y, z, w)

# pBones[i].location
# pBones[i].rotation_quaternion
