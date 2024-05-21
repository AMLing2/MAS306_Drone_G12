# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: dronePosVec.proto

from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='dronePosVec.proto',
  package='dronePosVec',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x11\x64ronePosVec.proto\x12\x0b\x64ronePosVec\"\x85\x02\n\rdronePosition\x12,\n\ndeviceType\x18\x01 \x01(\x0e\x32\x18.dronePosVec.dataDevices\x12\x14\n\x08posShape\x18\x02 \x03(\rB\x02\x10\x01\x12\x14\n\x08position\x18\x03 \x03(\x02\x42\x02\x10\x01\x12\x14\n\x08rotShape\x18\x04 \x03(\rB\x02\x10\x01\x12\x14\n\x08rotation\x18\x05 \x03(\x02\x42\x02\x10\x01\x12\x14\n\x0ctimestamp_ns\x18\x06 \x01(\x04\x12\x14\n\x0c\x63\x61mIteration\x18\x07 \x01(\r\x12\x15\n\tcameraRaw\x18\x08 \x03(\rB\x02\x10\x01\x12\x15\n\trotation2\x18\t \x03(\x02\x42\x02\x10\x01\x12\x14\n\x0c\x64roneBattery\x18\n \x01(\x02\"f\n\x0c\x64roneControl\x12\x0f\n\x07motorFL\x18\x01 \x01(\x02\x12\x0f\n\x07motorFR\x18\x02 \x01(\x02\x12\x0f\n\x07motorBL\x18\x03 \x01(\x02\x12\x0f\n\x07motorBR\x18\x04 \x01(\x02\x12\x12\n\nkillswitch\x18\x05 \x01(\x08\"\xd1\x01\n\rdataTransfers\x12!\n\x02ID\x18\x01 \x01(\x0e\x32\x15.dronePosVec.progName\x12\'\n\x04type\x18\x02 \x01(\x0e\x32\x19.dronePosVec.transferType\x12\x0b\n\x03msg\x18\x03 \x01(\t\x12\x13\n\x0btimeSync_ns\x18\x04 \x01(\x03\x12\n\n\x02IP\x18\x05 \x01(\t\x12\x0c\n\x04port\x18\x06 \x01(\r\x12\x10\n\x08sockaddr\x18\x07 \x01(\x0c\x12\x13\n\x0bsockaddrlen\x18\x08 \x01(\r\x12\x11\n\tsa_family\x18\t \x01(\r*a\n\x0b\x64\x61taDevices\x12\x0b\n\x07IMUonly\x10\x00\x12\r\n\tCameraPos\x10\x01\x12\x10\n\x0cKalmanFilter\x10\x02\x12\x10\n\x0c\x43\x61meraImgRGB\x10\x03\x12\x12\n\x0e\x43\x61meraImgDepth\x10\x04*Q\n\x0ctransferType\x12\x0c\n\x08timeSync\x10\x00\x12\x0e\n\nsocketInfo\x10\x01\x12\x0f\n\x0bstateChange\x10\x02\x12\t\n\x05start\x10\x03\x12\x07\n\x03\x65nd\x10\x04*O\n\x08progName\x12\n\n\x06server\x10\x00\x12\t\n\x05\x64rone\x10\x01\x12\r\n\testimator\x10\x02\x12\t\n\x05\x61rena\x10\x03\x12\n\n\x06\x63\x61mera\x10\x04\x12\x06\n\x02rl\x10\x05\x62\x06proto3'
)

_DATADEVICES = _descriptor.EnumDescriptor(
  name='dataDevices',
  full_name='dronePosVec.dataDevices',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='IMUonly', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='CameraPos', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='KalmanFilter', index=2, number=2,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='CameraImgRGB', index=3, number=3,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='CameraImgDepth', index=4, number=4,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=614,
  serialized_end=711,
)
_sym_db.RegisterEnumDescriptor(_DATADEVICES)

dataDevices = enum_type_wrapper.EnumTypeWrapper(_DATADEVICES)
_TRANSFERTYPE = _descriptor.EnumDescriptor(
  name='transferType',
  full_name='dronePosVec.transferType',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='timeSync', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='socketInfo', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='stateChange', index=2, number=2,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='start', index=3, number=3,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='end', index=4, number=4,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=713,
  serialized_end=794,
)
_sym_db.RegisterEnumDescriptor(_TRANSFERTYPE)

transferType = enum_type_wrapper.EnumTypeWrapper(_TRANSFERTYPE)
_PROGNAME = _descriptor.EnumDescriptor(
  name='progName',
  full_name='dronePosVec.progName',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='server', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='drone', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='estimator', index=2, number=2,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='arena', index=3, number=3,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='camera', index=4, number=4,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='rl', index=5, number=5,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=796,
  serialized_end=875,
)
_sym_db.RegisterEnumDescriptor(_PROGNAME)

progName = enum_type_wrapper.EnumTypeWrapper(_PROGNAME)
IMUonly = 0
CameraPos = 1
KalmanFilter = 2
CameraImgRGB = 3
CameraImgDepth = 4
timeSync = 0
socketInfo = 1
stateChange = 2
start = 3
end = 4
server = 0
drone = 1
estimator = 2
arena = 3
camera = 4
rl = 5



_DRONEPOSITION = _descriptor.Descriptor(
  name='dronePosition',
  full_name='dronePosVec.dronePosition',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='deviceType', full_name='dronePosVec.dronePosition.deviceType', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='posShape', full_name='dronePosVec.dronePosition.posShape', index=1,
      number=2, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\020\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='position', full_name='dronePosVec.dronePosition.position', index=2,
      number=3, type=2, cpp_type=6, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\020\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='rotShape', full_name='dronePosVec.dronePosition.rotShape', index=3,
      number=4, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\020\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='rotation', full_name='dronePosVec.dronePosition.rotation', index=4,
      number=5, type=2, cpp_type=6, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\020\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='timestamp_ns', full_name='dronePosVec.dronePosition.timestamp_ns', index=5,
      number=6, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='camIteration', full_name='dronePosVec.dronePosition.camIteration', index=6,
      number=7, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='cameraRaw', full_name='dronePosVec.dronePosition.cameraRaw', index=7,
      number=8, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\020\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='rotation2', full_name='dronePosVec.dronePosition.rotation2', index=8,
      number=9, type=2, cpp_type=6, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\020\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='droneBattery', full_name='dronePosVec.dronePosition.droneBattery', index=9,
      number=10, type=2, cpp_type=6, label=1,
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
  serialized_start=35,
  serialized_end=296,
)


_DRONECONTROL = _descriptor.Descriptor(
  name='droneControl',
  full_name='dronePosVec.droneControl',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='motorFL', full_name='dronePosVec.droneControl.motorFL', index=0,
      number=1, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='motorFR', full_name='dronePosVec.droneControl.motorFR', index=1,
      number=2, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='motorBL', full_name='dronePosVec.droneControl.motorBL', index=2,
      number=3, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='motorBR', full_name='dronePosVec.droneControl.motorBR', index=3,
      number=4, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='killswitch', full_name='dronePosVec.droneControl.killswitch', index=4,
      number=5, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
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
  serialized_start=298,
  serialized_end=400,
)


_DATATRANSFERS = _descriptor.Descriptor(
  name='dataTransfers',
  full_name='dronePosVec.dataTransfers',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='ID', full_name='dronePosVec.dataTransfers.ID', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='type', full_name='dronePosVec.dataTransfers.type', index=1,
      number=2, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='msg', full_name='dronePosVec.dataTransfers.msg', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='timeSync_ns', full_name='dronePosVec.dataTransfers.timeSync_ns', index=3,
      number=4, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='IP', full_name='dronePosVec.dataTransfers.IP', index=4,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='port', full_name='dronePosVec.dataTransfers.port', index=5,
      number=6, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sockaddr', full_name='dronePosVec.dataTransfers.sockaddr', index=6,
      number=7, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sockaddrlen', full_name='dronePosVec.dataTransfers.sockaddrlen', index=7,
      number=8, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sa_family', full_name='dronePosVec.dataTransfers.sa_family', index=8,
      number=9, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
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
  serialized_start=403,
  serialized_end=612,
)

_DRONEPOSITION.fields_by_name['deviceType'].enum_type = _DATADEVICES
_DATATRANSFERS.fields_by_name['ID'].enum_type = _PROGNAME
_DATATRANSFERS.fields_by_name['type'].enum_type = _TRANSFERTYPE
DESCRIPTOR.message_types_by_name['dronePosition'] = _DRONEPOSITION
DESCRIPTOR.message_types_by_name['droneControl'] = _DRONECONTROL
DESCRIPTOR.message_types_by_name['dataTransfers'] = _DATATRANSFERS
DESCRIPTOR.enum_types_by_name['dataDevices'] = _DATADEVICES
DESCRIPTOR.enum_types_by_name['transferType'] = _TRANSFERTYPE
DESCRIPTOR.enum_types_by_name['progName'] = _PROGNAME
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

dronePosition = _reflection.GeneratedProtocolMessageType('dronePosition', (_message.Message,), {
  'DESCRIPTOR' : _DRONEPOSITION,
  '__module__' : 'dronePosVec_pb2'
  # @@protoc_insertion_point(class_scope:dronePosVec.dronePosition)
  })
_sym_db.RegisterMessage(dronePosition)

droneControl = _reflection.GeneratedProtocolMessageType('droneControl', (_message.Message,), {
  'DESCRIPTOR' : _DRONECONTROL,
  '__module__' : 'dronePosVec_pb2'
  # @@protoc_insertion_point(class_scope:dronePosVec.droneControl)
  })
_sym_db.RegisterMessage(droneControl)

dataTransfers = _reflection.GeneratedProtocolMessageType('dataTransfers', (_message.Message,), {
  'DESCRIPTOR' : _DATATRANSFERS,
  '__module__' : 'dronePosVec_pb2'
  # @@protoc_insertion_point(class_scope:dronePosVec.dataTransfers)
  })
_sym_db.RegisterMessage(dataTransfers)


_DRONEPOSITION.fields_by_name['posShape']._options = None
_DRONEPOSITION.fields_by_name['position']._options = None
_DRONEPOSITION.fields_by_name['rotShape']._options = None
_DRONEPOSITION.fields_by_name['rotation']._options = None
_DRONEPOSITION.fields_by_name['cameraRaw']._options = None
_DRONEPOSITION.fields_by_name['rotation2']._options = None
# @@protoc_insertion_point(module_scope)
