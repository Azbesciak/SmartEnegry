# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.apps import AppConfig
from api.client import init_client

class ApiConfig(AppConfig):
    name = 'api'
    
    def ready(self):
        init_client()
    