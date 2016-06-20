## @package urls
# This is the landing file of the API. Every URLs are defines here and will redirect to a specific view.

from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^calibrate/(?P<mac_address>[0-9a-f:]+)/(?P<x>[0-9.]+)/(?P<y>[0-9.]+)/(?P<z>[0-9.]+)$', views.calibrate, name='calibrate'),
    url(r'^findme/(?P<mac_address>[0-9a-f:]+)$', views.findme, name='findme'),
    url(r'^gotcha/(?P<ap_id>[0-9]+)/(?P<mac_address>[0-9a-f:]+)/(?P<RSSI>[^/]+)$', views.gotcha, name='gotcha'),
    url(r'^reset$', views.reset, name='reset'),
    url(r'^check$', views.check, name='check'),
]

"""
http://trips.loc/api/calibrate/68:5b:43:62:a0:eb/73/42/1
    {"code":"ACK"}
    {"code":"WAIT"}
    {"code":"DONE"}
http://trips.loc/api/findme/68:5b:43:62:a0:eb
    {"code":"ACK"}
    {"code":"WAIT"}
    {"code":"MAP", x=36,y=72;z=3}
http://trips.loc/api/gotcha/1/68:5b:43:62:a0:eb/123456
    {"code":"ACK"}
http://trips.loc/api/check
    {"code":"MACS", "macs":["68:5b:43:62:a0:eb", "ca:fe:42:ca:fe:42", "42:42:42:42:42:42"]}
"""
