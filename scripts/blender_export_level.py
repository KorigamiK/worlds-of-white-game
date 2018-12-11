import os
os.chdir('C:\\src\\kmdreko\\exploration\\exploration\\levels')

file = open('./temp_level.txt', 'w+')

print('level', 1, file=file)

objects = bpy.data.objects
print(len(objects), file=file)
for object in objects:
  name     = object.name.partition('.')[0] # name.001 => name
  location = object.location       # XYZ
  rotation = object.rotation_euler # XYZ
  scale    = object.scale          # XYZ
  print(name, location[0], location[1], location[2], rotation[0], rotation[1], rotation[2], scale[0], scale[1], scale[2], file=file)

file.close()
