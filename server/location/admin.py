from django.contrib import admin
from .models import Location, Device, Measurement, Calibration

admin.site.register(Location)
admin.site.register(Device)
admin.site.register(Measurement)
admin.site.register(Calibration)
