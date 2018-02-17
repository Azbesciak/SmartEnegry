from rest_framework import serializers

from api.models import Consumption, Device


class ConsumptionSerializer(serializers.ModelSerializer):
    id = serializers.IntegerField(read_only=True)
    measure_time = serializers.DateTimeField(read_only=True)
    value = serializers.DecimalField(read_only=True, max_digits=10, decimal_places=5)
    device_id = serializers.IntegerField(read_only=True)

    class Meta:
        model = Consumption
        fields = ('id', 'measure_time', 'value', 'device_id')


class DeviceSerializer(serializers.ModelSerializer):
    id = serializers.IntegerField(read_only=True)
    description = serializers.CharField(read_only=False)
    state = serializers.BooleanField(read_only=False)

    class Meta:
        model = Device
        fields = ('id', 'description', 'state')
