# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models


class Device(models.Model):
    description = models.CharField(max_length=200, null=True, blank=True)
    state = models.BooleanField(default=False)

    def __str__(self):
        return "id=" + str(self.id) + ", des=" + self.description


class Consumption(models.Model):
    measure_time = models.DateTimeField('date of measurement')
    value = models.DecimalField(max_digits=10, decimal_places=5)
    device = models.ForeignKey(Device, on_delete=models.CASCADE)

    def __str__(self):
        return "time=" + str(self.measure_time) +\
                        str(self.value) +\
                        ", dev=" + str(self.device)
