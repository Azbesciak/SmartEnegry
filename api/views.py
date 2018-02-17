import datetime

from django.http import JsonResponse
from rest_framework import status
from rest_framework.decorators import api_view

from api.serializers import ConsumptionSerializer, DeviceSerializer
from .models import Consumption, Device


def get_consumptions(consumptions):
    serializer = ConsumptionSerializer(consumptions, many=True)
    return JsonResponse(serializer.data, safe=False)


def get_devices(dev, many=True):
    serializer = DeviceSerializer(dev, many=many)
    return JsonResponse(serializer.data, safe=False)


def tsTodate(ts):
    return datetime.datetime.utcfromtimestamp(int(ts)).strftime('%Y-%m-%d %H:%M:%S')


@api_view(['GET'])
def consumptions_list(request):
    return get_consumptions(Consumption.objects.all())


@api_view(["GET"])
def consumption_between(request, device_id, date_from, date_to):
    return get_consumptions(Consumption.objects.filter(
        device_id=device_id,
        measure_time__range=[tsTodate(date_from), tsTodate(date_to)]
    ))


@api_view(["POST"])
def change_state(request, device_id, state):
    try:
        dev = Device.objects.get(id=device_id)
    except Device.DoesNotExist:
        return JsonResponse(data=device_id, status=status.HTTP_404_NOT_FOUND)
    dev.state = state
    dev.save()
    return get_devices(dev, False)


@api_view(["GET"])
def devices(request):
    return get_devices(Device.objects.all())


