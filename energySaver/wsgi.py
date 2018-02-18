"""
WSGI config for energySaver project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/1.11/howto/deployment/wsgi/
"""

import os

from api.client import init_client
started = False
if not started:
    started = True
    init_client()

from django.core.wsgi import get_wsgi_application

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "energySaver.settings")


application = get_wsgi_application()
