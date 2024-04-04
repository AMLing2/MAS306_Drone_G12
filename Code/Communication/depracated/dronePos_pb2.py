# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: dronePos.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='dronePos.proto',
  package='dronePos',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x0e\x64ronePos.proto\x12\x08\x64ronePos\"+\n\x08position\x12\t\n\x01x\x18\x01 \x01(\x02\x12\t\n\x01y\x18\x02 \x01(\x02\x12\t\n\x01z\x18\x03 \x01(\x02\"1\n\x0bpositionDot\x12\n\n\x02\x64x\x18\x01 \x01(\x02\x12\n\n\x02\x64y\x18\x02 \x01(\x02\x12\n\n\x02\x64z\x18\x03 \x01(\x02\"3\n\x08rotation\x12\r\n\x05theta\x18\x01 \x01(\x02\x12\x0b\n\x03phi\x18\x02 \x01(\x02\x12\x0b\n\x03psi\x18\x03 \x01(\x02\"9\n\x0brotationDot\x12\x0e\n\x06\x64theta\x18\x01 \x01(\x02\x12\x0c\n\x04\x64phi\x18\x02 \x01(\x02\x12\x0c\n\x04\x64psi\x18\x03 \x01(\x02\x62\x06proto3'
)




_POSITION = _descriptor.Descriptor(
  name='position',
  full_name='dronePos.position',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='x', full_name='dronePos.position.x', index=0,
      number=1, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='y', full_name='dronePos.position.y', index=1,
      number=2, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='z', full_name='dronePos.position.z', index=2,
      number=3, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=28,
  serialized_end=71,
)


_POSITIONDOT = _descriptor.Descriptor(
  name='positionDot',
  full_name='dronePos.positionDot',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='dx', full_name='dronePos.positionDot.dx', index=0,
      number=1, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='dy', full_name='dronePos.positionDot.dy', index=1,
      number=2, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='dz', full_name='dronePos.positionDot.dz', index=2,
      number=3, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=73,
  serialized_end=122,
)


_ROTATION = _descriptor.Descriptor(
  name='rotation',
  full_name='dronePos.rotation',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='theta', full_name='dronePos.rotation.theta', index=0,
      number=1, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='phi', full_name='dronePos.rotation.phi', index=1,
      number=2, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='psi', full_name='dronePos.rotation.psi', index=2,
      number=3, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=124,
  serialized_end=175,
)


_ROTATIONDOT = _descriptor.Descriptor(
  name='rotationDot',
  full_name='dronePos.rotationDot',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='dtheta', full_name='dronePos.rotationDot.dtheta', index=0,
      number=1, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='dphi', full_name='dronePos.rotationDot.dphi', index=1,
      number=2, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='dpsi', full_name='dronePos.rotationDot.dpsi', index=2,
      number=3, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=177,
  serialized_end=234,
)

DESCRIPTOR.message_types_by_name['position'] = _POSITION
DESCRIPTOR.message_types_by_name['positionDot'] = _POSITIONDOT
DESCRIPTOR.message_types_by_name['rotation'] = _ROTATION
DESCRIPTOR.message_types_by_name['rotationDot'] = _ROTATIONDOT
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

position = _reflection.GeneratedProtocolMessageType('position', (_message.Message,), {
  'DESCRIPTOR' : _POSITION,
  '__module__' : 'dronePos_pb2'
  # @@protoc_insertion_point(class_scope:dronePos.position)
  })
_sym_db.RegisterMessage(position)

positionDot = _reflection.GeneratedProtocolMessageType('positionDot', (_message.Message,), {
  'DESCRIPTOR' : _POSITIONDOT,
  '__module__' : 'dronePos_pb2'
  # @@protoc_insertion_point(class_scope:dronePos.positionDot)
  })
_sym_db.RegisterMessage(positionDot)

rotation = _reflection.GeneratedProtocolMessageType('rotation', (_message.Message,), {
  'DESCRIPTOR' : _ROTATION,
  '__module__' : 'dronePos_pb2'
  # @@protoc_insertion_point(class_scope:dronePos.rotation)
  })
_sym_db.RegisterMessage(rotation)

rotationDot = _reflection.GeneratedProtocolMessageType('rotationDot', (_message.Message,), {
  'DESCRIPTOR' : _ROTATIONDOT,
  '__module__' : 'dronePos_pb2'
  # @@protoc_insertion_point(class_scope:dronePos.rotationDot)
  })
_sym_db.RegisterMessage(rotationDot)


# @@protoc_insertion_point(module_scope)
