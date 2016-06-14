from django.db import models
from django.db.models import Avg

class Location(models.Model):
    x = models.FloatField(default=0)
    y = models.FloatField(default=0)
    z = models.FloatField(default=0)

    def __str__(self):
        return "(" + str(self.x) + "," + str(self.y) + "," + str(self.z) + ")"

class Device(models.Model):
    mac = models.CharField(max_length=255)
    calibration = models.BooleanField(default=0)
    
    @staticmethod
    def get_macs():
        return list(Device.objects.values_list('mac', flat=True))

class Measurement(models.Model):
    ap_id = models.IntegerField(default=0)
    dv_id = models.ForeignKey(Device, on_delete=models.CASCADE)
    ss_value = models.FloatField(default=0)
    
    @staticmethod
    def ss_average(ap_id, dv_id):
        return Measurement.objects.filter(dv_id=dv_id, ap_id=ap_id).aggregate(Avg('ss_value')).values()[0]


class Calibration(models.Model):
    loc_id = models.ForeignKey(Location, on_delete=models.CASCADE)
    ss_value = models.FloatField(default=0)
    ap_id = models.IntegerField(default=0)

