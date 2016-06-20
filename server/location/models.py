## @package models
# Contain all the model-class that will allow us an easy access from the rest of the code to the tables in our database

from django.db import models
from django.db.models import Avg

## Class linked to the Location table in the database
class Location(models.Model):
    x = models.FloatField(default=0)
    y = models.FloatField(default=0)
    z = models.FloatField(default=0)

    def __str__(self):
        return "(" + str(self.x) + "," + str(self.y) + "," + str(self.z) + ")"

## Class linked to the Device table in the database
class Device(models.Model):
    mac = models.CharField(max_length=255)
    calibration = models.BooleanField(default=0)
    
    @staticmethod
    def get_macs():
        return list(Device.objects.values_list('mac', flat=True))

## Class linked to the Measurment table in the database
class Measurement(models.Model):
    ap_id = models.IntegerField(default=0)
    dv_id = models.ForeignKey(Device, on_delete=models.CASCADE)
    ss_value = models.FloatField(default=0)
    
    ## Compute the average of all the measurments for a given device and a given access-point
    # @param ap_id Identifier of the access point
    # @param dv_id Identifier of the device
    @staticmethod
    def ss_average(ap_id, dv_id):
        return Measurement.objects.filter(dv_id=dv_id, ap_id=ap_id).aggregate(Avg('ss_value')).values()[0]

## Class linked to the Calibration table in the database
class Calibration(models.Model):
    loc_id = models.ForeignKey(Location, on_delete=models.CASCADE)
    ss_value = models.FloatField(default=0)
    ap_id = models.IntegerField(default=0)

