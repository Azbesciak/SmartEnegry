# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.contrib import admin


from .models import Device, Consumption

admin.site.register(Device)
admin.site.register(Consumption)
# Register your models here.
