import datetime

from django.http import JsonResponse
from django.shortcuts import render
from rest_framework import status
from rest_framework.decorators import api_view

from api.serializers import ConsumptionSerializer, DeviceSerializer
from .models import Consumption, Device
from .client import Client


def get_consumptions(consumptions):
    serializer = ConsumptionSerializer(consumptions, many=True)
    return JsonResponse(serializer.data, safe=False)


def get_devices(dev, many=True):
    serializer = DeviceSerializer(dev, many=many)
    return JsonResponse(serializer.data, safe=False)


def tsTodate(ts):
    return datetime.datetime.utcfromtimestamp(int(ts)/1000).strftime('%Y-%m-%d %H:%M:%S')


def index(request, path=''):
    return render(request, template_name='index.html')


@api_view(['GET'])
def consumptions_list(request):
    return get_consumptions(Consumption.objects.all())


@api_view(["GET"])
def consumption_between(request, devices_ids, date_from, date_to):
    devices_ids = devices_ids.split(",")
    return get_consumptions(Consumption.objects.filter(
        device_id__in=devices_ids,
        measure_time__range=[tsTodate(date_from), tsTodate(date_to)]
    ))


@api_view(["POST"])
def change_state(request, device_id, state):
    dev = Device.update_state(device_id, state)
    if dev is None:
        return JsonResponse(data=device_id, status=status.HTTP_404_NOT_FOUND)
    Client.change_device_state(device_id, state)
    return get_devices(dev, False)


@api_view(["GET", "POST"])
def devices(request):
    if request.method == "GET":
        return get_devices(Device.objects.all())
    else:
        update_description(request)


def update_description(request):
    try:
        dev = Device.objects.get(id=request.body['id'])
    except Device.DoesNotExist:
        return JsonResponse(data=request.body, status=status.HTTP_404_NOT_FOUND)

    dev.description = request.body['description']
    dev.save()
    return get_devices(dev, False)

