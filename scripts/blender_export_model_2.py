import os
import bmesh
os.chdir('C:\\src\\kmdreko\\exploration\\exploration\\models')

obj = bpy.context.selected_objects[0];

file = open('./temp_model.txt', 'w+')

mesh = obj.data;
alwaysDrawGroupId = obj.vertex_groups.find("always_draw");
orderGroupId = obj.vertex_groups.find("order");

# write version
print(2, file=file)

# write vertices
print(len(mesh.vertices), file=file)
for vertex in mesh.vertices:
  groups = [g for g in vertex.groups if g.group != alwaysDrawGroupId and g.group != orderGroupId]
  order = [g for g in vertex.groups if g.group == orderGroupId][0]
  group1, weight1 = (groups[0].group, groups[0].weight) if len(groups) > 0 else (0, 1.0)
  group2, weight2 = (groups[1].group, groups[1].weight) if len(groups) > 1 else (0, 0.0)
  group3, weight3 = (groups[2].group, groups[2].weight) if len(groups) > 2 else (0, 0.0)
  print(vertex.co[0], vertex.co[1], vertex.co[2], group1, group2, group3, weight1, weight2, weight3, order.weight, file=file)

# write faces
print(len(mesh.polygons), file=file)
for face in mesh.polygons:
  assert len(face.vertices) == 3
  print(face.vertices[0], face.vertices[1], face.vertices[2], file=file)

# write lines
bpy.ops.object.mode_set(mode='EDIT')
bm = bmesh.from_edit_mesh(mesh)
print(len(bm.edges), file=file)
for edge in bm.edges:
  vId0 = -1
  vId1 = edge.verts[0].index
  vId2 = edge.verts[1].index
  vId3 = -1
  faces = edge.link_faces
  if len(faces) == 0:
    continue
  if len(faces) == 1:
    faces = [faces[0], faces[0]]
  if alwaysDrawGroupId in [g.group for g in mesh.vertices[vId1].groups] and alwaysDrawGroupId in [g.group for g in mesh.vertices[vId2].groups]:
    vId0 = vId2
    vId3 = vId1
  else:
    vId0 = [v for v in faces[0].verts if v.index != vId1 and v.index != vId2][0].index
    vId3 = [v for v in faces[1].verts if v.index != vId1 and v.index != vId2][0].index
  print(vId0, vId1, vId2, vId3, file=file)

bpy.ops.object.mode_set(mode='OBJECT')

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
    parent_matrix, parent_vector, parent_head, parent_parent_id = parent
    location = head + Vector((0, parent_vector.length, 0))
    rotation = matrix.to_quaternion()
    print(parent_id, location[0], location[1], location[2], rotation[0], rotation[1], rotation[2], rotation[3], file=file)

file.close()
