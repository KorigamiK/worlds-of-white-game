obj = bpy.data.objects[0];

file = open('./temp_model.txt', 'w+')

mesh = obj.data;
alwaysDrawGroupId = obj.vertex_groups["always_draw"].index;

# write vertices
print(len(mesh.vertices), file=file)
for vertex in mesh.vertices:
  print(vertex.co[0], vertex.co[1], vertex.co[2], file=file)

# write faces
print(len(mesh.polygons), file=file)
for face in mesh.polygons:
  assert len(face.vertices) == 3
  vId0 = face.vertices[0]
  vId1 = face.vertices[1]
  vId2 = face.vertices[2]
  print(vId0, vId1, vId2, file=file)

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

file.close()
