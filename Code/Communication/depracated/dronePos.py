from protobuf3.message import Message
from protobuf3.fields import FloatField


class position(Message):
    pass


class positionDot(Message):
    pass


class rotation(Message):
    pass


class rotationDot(Message):
    pass

position.add_field('x', FloatField(field_number=1, optional=True))
position.add_field('y', FloatField(field_number=2, optional=True))
position.add_field('z', FloatField(field_number=3, optional=True))
positionDot.add_field('dx', FloatField(field_number=1, optional=True))
positionDot.add_field('dy', FloatField(field_number=2, optional=True))
positionDot.add_field('dz', FloatField(field_number=3, optional=True))
rotation.add_field('theta', FloatField(field_number=1, optional=True))
rotation.add_field('phi', FloatField(field_number=2, optional=True))
rotation.add_field('psi', FloatField(field_number=3, optional=True))
rotationDot.add_field('dtheta', FloatField(field_number=1, optional=True))
rotationDot.add_field('dphi', FloatField(field_number=2, optional=True))
rotationDot.add_field('dpsi', FloatField(field_number=3, optional=True))
