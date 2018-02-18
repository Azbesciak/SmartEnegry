from django.conf.urls import url
from rest_framework.urlpatterns import format_suffix_patterns

from api import views

# Create a router and register our viewsets with it.


urlpatterns = [
    url(r'^consumptions/$', views.consumptions_list),
    url(r'^consumptions/(?P<devices_ids>[\d]+(,[\d]+)*)/(?P<date_from>\d+)/(?P<date_to>\d+)/$', views.consumption_between),
    url(r'^device/(?P<device_id>\d+)/(?P<state>[01])/$', views.change_state),
    url(r'^device/$', views.devices),
]

urlpatterns = format_suffix_patterns(urlpatterns)
